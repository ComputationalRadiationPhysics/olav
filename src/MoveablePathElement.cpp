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

#include "MoveablePathElement.hpp"
#include "OgreFramework.hpp"

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

MoveablePathElement::MoveablePathElement(Ogre::SceneNode* node, Ogre::MeshPtr newPath)
{
	elementNode = node;
	path = newPath;
	reloadFirstPosition();
	mWalkSpeed = 0.02f;
}

//This function reloads the Path with the pathParser
void MoveablePathElement::reloadPath()
{
	if(!path.isNull()){
		PathParser* pp = new PathParser(path);
			
		mWalkList.clear();
		mPathLength.clear();
		mKumPathLength.clear();

		//Fill walk list
		for(unsigned int i = 0; i < pp->pointList->size(); i++)
		{
			mWalkList.push_back(pp->pointList->at(i)->position);
			if (i+1 < pp->pointList->size())
			{
				float meow = (pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length();
				mPathLength.push_back(meow);
				float oldKum = 0;
				if (i!=0)
					oldKum = mKumPathLength[i-1];
				mKumPathLength.push_back(oldKum+meow);
			}
		}
		delete pp;
	}
}

//use this function, if you need to stop the object
void MoveablePathElement::reloadFirstPosition()
{
	numPoint=1;
	reloadPath();
	if (!mWalkList.empty())
	{
		elementNode->setPosition(mWalkList.front());	// this gets the front of the deque
		mDestination = mWalkList.front();
		mWalkList.pop_front();							// this removes the front of the deque
	}
	mDirection = Ogre::Vector3::ZERO;
	mOldDirection = Ogre::Vector3::ZERO;
	position = 0.0;
	finished = false;
}

///call this function every frame, to update the position of the object
void MoveablePathElement::updatePosition(bool setDir)
{
	int oldNumpoint = numPoint;
	double timeSinceLastFrame = TimeManager::getTimeSinceLastFrame();
    
	if (finished)
		return;
	while (mDirection == Ogre::Vector3::ZERO) 
		if (!nextLocation())
			break;
	position += mWalkSpeed * (Ogre::Real)timeSinceLastFrame;
	move = mWalkSpeed * (Ogre::Real)timeSinceLastFrame;
	Ogre::Real movement = move;
	while (mDirection != Ogre::Vector3::ZERO && mDistance < movement)
	{
		movement -= mDistance;
		elementNode->setPosition(mDestination);
		do {
			if (!nextLocation())
				break;
		} while (mDirection == Ogre::Vector3::ZERO);
		if (finished)
			return;
	}
	move = movement;
	mDistance -= move;
	elementNode->translate(mDirection * move);
	if (!mKumPathLength.empty())
		position = mKumPathLength[numPoint-2]-mDistance;

	updateFeedBack(oldNumpoint != numPoint,timeSinceLastFrame);
}

float MoveablePathElement::updatePositionTo(int goal,bool setDir)
{
	int oldNumpoint = numPoint;
	if (goal <= numPoint)
		return 0.0f;
	if (finished)
		return 0.0f;
	float gone_way = 0.0f;
	while (mDirection == Ogre::Vector3::ZERO) 
		if (!nextLocation())
			break;
	while (mDirection != Ogre::Vector3::ZERO && numPoint < goal)
	{
		position += mDistance;
		gone_way += mDistance;
		elementNode->setPosition(mDestination);
		do {
			if (!nextLocation())
				break;
		} while (mDirection == Ogre::Vector3::ZERO);
		if (finished)
			return gone_way;
	}
	updateFeedBack(oldNumpoint != numPoint,gone_way/mWalkSpeed);
	return gone_way;
}

///same like updatePosition, but the time ist 1ms
void MoveablePathElement::updatePositionOneMS(bool setDir)
{
	int oldNumpoint = numPoint;
	double timeSinceLastFrame=1.0f;

	if (mDirection == Ogre::Vector3::ZERO) 
		nextLocation();
	position += mWalkSpeed * (Ogre::Real)timeSinceLastFrame;
	move = mWalkSpeed * (Ogre::Real)timeSinceLastFrame;
	mDistance -= move;
	if (mDistance <= 0.0f)
	{
		elementNode->setPosition(mDestination);
		mDirection = Ogre::Vector3::ZERO;				
		nextLocation();
	}
	else
		elementNode->translate(mDirection * move);
	updateFeedBack(oldNumpoint != numPoint,1.0f);
}

void MoveablePathElement::updatePositionWay(double way)
{
	int oldNumpoint = numPoint;
	if (finished)
		return;
	while (mDirection == Ogre::Vector3::ZERO) 
		if (!nextLocation())
			break;
	position += way;
	move = way;
	Ogre::Real movement = move;
	while (mDirection != Ogre::Vector3::ZERO && mDistance < movement)
	{
		movement -= mDistance;
		elementNode->setPosition(mDestination);
		do {
			if (!nextLocation())
				break;
		} while (mDirection == Ogre::Vector3::ZERO);
		if (finished)
			return;
	}
	move = movement;
	mDistance -= move;
	elementNode->translate(mDirection * move);
	updateFeedBack(oldNumpoint != numPoint,way/mWalkSpeed);
}

Ogre::Quaternion MoveablePathElement::getOrientationFromDirection(Ogre::Vector3 truncDirection)
{
	Ogre::Quaternion orientation = truncDirection.getRotationTo(Ogre::Vector3(1.0f,0.0f,0.0f));
	orientation = orientation * Ogre::Vector3(1.0f,0.0f,0.0f).getRotationTo(Ogre::Vector3(0.0f,0.0f,1.0f));
	return orientation.Inverse();
}

///get the next location, if the object has past the last point
bool MoveablePathElement::nextLocation()
{
	mOldDestination = mDestination;
	if (mWalkList.empty())
		finished = true;
	if (finished)
		return false;
    mDestination = mWalkList.front();	// this gets the front of the deque
    mWalkList.pop_front();				// this removes the front of the deque
	mOldFullDistance = mFullDistance;
	mOldDirection = mDirection;
    mDirection = mDestination - elementNode->getPosition();
    mDistance = mDirection.normalise();
	mFullDistance = mDistance;
	numPoint+=1;

	elementNode->setOrientation(getOrientationFromDirection(mDirection));

	numPointChangeFeedBack();
    return true;
}

///return animationspeed
Ogre::Real MoveablePathElement::getWalkSpeed()
{
	return mWalkSpeed;
}

///set a new animationspeed
void MoveablePathElement::setWalkSpeed(Ogre::Real newWalkSpeed)
{
	mWalkSpeed=newWalkSpeed;
}

//return a pointer to the walklist to get or set new points
std::deque<Ogre::Vector3>* MoveablePathElement::getWalkList()
{
	return &mWalkList;
}

//return the direction
Ogre::Vector3 MoveablePathElement::getDirection()
{
	return mDirection;
}

///get the node
Ogre::SceneNode* MoveablePathElement::getOgreNode()
{
	return elementNode;
}

//get the current Pathsection, where the object is
int MoveablePathElement::getNumPathPoint(){ return numPoint; }

void MoveablePathElement::setNumPathPoint(int num){ numPoint=num; }

float MoveablePathElement::getPathPosition()
{
	return position;
}

void MoveablePathElement::numPointChangeFeedBack() {}

void MoveablePathElement::updateFeedBack(bool numPointChange,float elapsedTime) {}

int MoveablePathElement::getNumPoint()
{
	return numPoint;
}

Ogre::Real MoveablePathElement::getDistance()
{
	return mDistance;
}

Ogre::Real MoveablePathElement::getFullDistance()
{
	return mFullDistance;
}

Ogre::Vector3 MoveablePathElement::getOldDirection()
{
	return mOldDirection;
}

Ogre::Real MoveablePathElement::getOldFullDistance()
{
	return mOldFullDistance;
}

Ogre::Vector3 MoveablePathElement::getDestination()
{
	return mDestination;
}

Ogre::Vector3 MoveablePathElement::getOldDestination()
{
	return mOldDestination;
}
