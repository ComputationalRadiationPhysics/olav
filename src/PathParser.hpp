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

#pragma once
#include "PathPoint.hpp"
#include "TimeManager.hpp"

// Allround class for parsing a path from a given mesh
class PathParser
{
public:
	static const int radius = 10;

	/// constructor : needs to set the mesh
	/// if endofPath is bigger than 0, this is the pathPointIndex (!) at which the path ends (for booster / oszillator)
	/// reverse is needed for oszillator path -> after the end of that path the already contained points are added in 
	/// reverse order, so that the path goes backward to its beginning (in order to perfrom a perfect loop)
	PathParser(const Ogre::MeshPtr mesh, int endOfPath = 0, bool reverse = false);
	~PathParser(void);

	/// the pointList where the points are stored in
	std::vector<PathPoint*>* pointList;

	/// the interpolation interval
	static const double INTERPOLATION_INTERVAL;

	// initialize the pointlist 
	void initPointList(const Ogre::MeshPtr mesh);

	/// total length of the path; if endOfPath > 0 then thers is a partial distance till this end point
	Ogre::Real totalDistance;
	Ogre::Real partialDistance;
	/// method for manually inserting an end and start point (needed for stretcher)
	void insertStartAnEnd(PathPoint* start, PathPoint* end);

	/// computes the time values of the path points regarding to the distance on the path in comparison to the 
	/// total time for the whole animation
	void updateTimeValues(Ogre::Real modificator = 1.0, Ogre::Real start = 0, Ogre::Real dist = 0);
	std::vector<PathPoint*>* getPointList();

	/// returns the time for the next Point on the path
	double getTimeForNearestPoint(Ogre::Vector3 sphereCenter, double offset = 0.0);

	/// checks whether another point is within a specified range (e.g. for determining whether 
	/// the pulse has passed a transformation point)
	bool isPointWithinRange(Ogre::Vector3 pathPoint, Ogre::Vector3 pointToTest);

private:
	const Ogre::MeshPtr mesh;
	unsigned int pathEnd;
	bool reverse;
	
};
