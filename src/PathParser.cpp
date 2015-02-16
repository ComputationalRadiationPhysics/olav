/**
* Copyright 2015 Alexander Matthes
*
* This file is part of OLAV.
*
* OLAV is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* OLAV is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with OLAV.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include "PathParser.hpp"

const double PathParser::INTERPOLATION_INTERVAL = 0.1;

PathParser::PathParser(const Ogre::MeshPtr mesh, int endOfPath, bool reverse) : pathEnd(endOfPath), partialDistance(0), reverse(reverse)
{
	initPointList(mesh);
}

PathParser::~PathParser(void)
{

}

void PathParser::initPointList(const Ogre::MeshPtr mesh)
{
    bool added_shared = false;
    int current_offset = 0;
    int shared_offset = 0;
    int next_offset = 0;
    int index_offset = 0;

	int vertex_count = 0;

    // Calculate how many vertices and indices we're going to need
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }
    }

    // Allocate space for the vertices and indices
	pointList = new std::vector<PathPoint*>();
	pointList->resize(vertex_count/2);
    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            float* pReal;
			totalDistance = 0;
			Ogre::Real dist = 0;
			Ogre::Vector3 lastPt = Ogre::Vector3::ZERO;

			// the mesh is a spline therefore every second point is omitted
			for(unsigned int j = 0; j < vertex_data->vertexCount; j += 2, vertex += vbuf->getVertexSize()*2)
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				// calculate distance on the fly
				if (j>0) 
				{						
					dist = pt.distance(lastPt);
					totalDistance += dist;
					if (j/2 <= pathEnd)
					{
						partialDistance += dist;
					}
				}
				// create new PathPoint and set the distance to the lastPt if possible
				(*pointList)[current_offset + (j/2)] = new PathPoint(pt, (current_offset + (j/2))*INTERPOLATION_INTERVAL, dist);
				if (j>0) 
				{
					(*pointList)[current_offset + (j/2)]->direction = lastPt-pt;					
				}
				lastPt = pt;
            }

			// set partial distance being the total distance on short paths
			if (pathEnd == 0)
			{
				partialDistance = totalDistance;
			} 

			// insert "gap" for stretcher animation
			if ((pathEnd == 0 && pointList->size() > TimeManager::STRETCHER_NODE) || pathEnd > TimeManager::STRETCHER_NODE)
			{
				std::vector<PathPoint*>::iterator it = pointList->begin();
				it+= TimeManager::STRETCHER_NODE + 1;
				pointList->insert(it, 1, (*pointList)[TimeManager::STRETCHER_NODE]);
				(*pointList)[TimeManager::STRETCHER_NODE+1]->distanceToLastPoint = 0.0;
			}
            
            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }
	}
	
	if (pathEnd != 0)
	{
		pointList->resize(pathEnd+1);
	}

	// calculate reverse path
	if (reverse)
	{	
		int c = pointList->size()-1;
		Ogre::Vector3 lastPt = (*pointList)[c]->position;

		while (c > 0)
		{
			c--;
			PathPoint* p = (*pointList)[c];
			double distanceToLastPoint = p->position.distance(lastPt);
			Ogre::Vector3 direction = lastPt - p->position;
			pointList->push_back(new PathPoint(p->position, 0, distanceToLastPoint, direction));
			partialDistance += p->distanceToLastPoint;
			lastPt = p->position;
		}
	}
}

// calculate the time values based on the position on the path fr each point
void PathParser::updateTimeValues(Ogre::Real modificator, Ogre::Real start, Ogre::Real dist)
{
	double tComplete = TimeManager::TOTAL_ANIMATION_LENGTH * modificator;
	double t = 0;

	// for stretcher path : adapt calculation (offset, shorter distance, shorter total time)
	if (start != 0 && dist != 0)
	{	
		Ogre::Real quot = totalDistance / dist;
		tComplete *= quot;
		TimeManager::TOTAL_STRETCHER_LENGTH = tComplete;
		t = start;
		(*pointList)[0]->time = start;
	}

	// now calculate times
	for (unsigned int i = 1; i < pointList->size(); ++i)
	{
		double d = (*pointList)[i]->distanceToLastPoint;		
		
		// calculate time ragrding to whole time
		if ( d > 0)
		{
			double lFactor = d / totalDistance;
			t += tComplete * lFactor;
		} 
		else if (pathEnd != 0)
		{
			t = (*pointList)[i]->time;
		}
		
		// compute gap for tour path
		if (start == 0 && i == TimeManager::STRETCHER_NODE+1)
		{
			t += TimeManager::TOTAL_STRETCHER_LENGTH * modificator;
		}
		(*pointList)[i]->time = t;
	}	
	// for tour path (or every path that goes from start to end) set the end time to the total time to avoid numeric problems
	if (start == 0 && dist == 0 && !reverse && pathEnd == 0)
	{
		(*pointList)[pointList->size()-1]->time = TimeManager::getTotalAnimationLength();
	}
}

// retruns the time for the given point 
double PathParser::getTimeForNearestPoint(Ogre::Vector3 sphereCenter, double offset)
{
	for (unsigned int i = 0; i < pointList->size(); i++)
	{
		PathPoint* p = (*pointList)[i];
		double sqrRad = (p->position).squaredDistance(sphereCenter);
		if (sqrRad <= radius && p->time > offset)
		{
			return p->time;
		}
	}

	return 0;
}
/// simply tests whether a point is within the range specified by the radius
bool PathParser::isPointWithinRange(Ogre::Vector3 pathPoint, Ogre::Vector3 pointToTest)
{
	double sqrRad = (pathPoint).squaredDistance(pointToTest);
	if (sqrRad <= radius)
	{
		return true;
	}

	return false;
}

std::vector<PathPoint*>* PathParser::getPointList()
{
	return pointList;
}

/// inserts satrt and end points and calculates the distance
void PathParser::insertStartAnEnd(PathPoint *start, PathPoint *end)
{
	PathPoint* pStart = new PathPoint(start->position, 0);
	pointList->insert(pointList->begin(), pStart);
	Ogre::Real distStart = ((*pointList)[1]->position - (*pointList)[0]->position).length();
	(*pointList)[1]->distanceToLastPoint = distStart;
	
	Ogre::Real distEnd = (end->position - (*pointList)[pointList->size() -1]->position).length();
	totalDistance += distStart + distEnd;
	PathPoint* pEnd = new PathPoint(end->position, 0, distEnd);
	pointList->push_back(pEnd);
}
