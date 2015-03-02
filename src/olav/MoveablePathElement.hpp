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

#ifndef MOVEABLE_HPP
#define MOVEABLE_HPP

#include <Ogre.h>
#include "PathParser.hpp"

class MoveablePathElement
{
public:
	MoveablePathElement(Ogre::SceneNode* node, Ogre::MeshPtr newPath);

	virtual void updatePosition(bool setDir = true);
	float updatePositionTo(int goal,bool setDir = true);
	void updatePositionOneMS(bool setDir = true);
	void updatePositionWay(double way);

	bool nextLocation();
	virtual void reloadPath();
	virtual void reloadFirstPosition();

	Ogre::Real getWalkSpeed();
	void setWalkSpeed(Ogre::Real newWalkSpeed);
	std::deque<Ogre::Vector3>* getWalkList();
	Ogre::Vector3 getDirection();
	Ogre::SceneNode* getOgreNode();
	int getNumPathPoint();
	void setNumPathPoint(int num);
	float getPathPosition();

	static Ogre::Quaternion getOrientationFromDirection(Ogre::Vector3 truncDirection);
	int getNumPoint();
	Ogre::Real getDistance();
	Ogre::Real getFullDistance();
	Ogre::Vector3 getOldDirection();
	Ogre::Real getOldFullDistance();
	Ogre::Vector3 getDestination();
	Ogre::Vector3 getOldDestination();

//private:
	virtual void numPointChangeFeedBack();
	virtual void updateFeedBack(bool numPointChange,float elapsedTime);

//protected:
	Ogre::MeshPtr				path;
	std::deque<Ogre::Vector3>   mWalkList;				// The list of points
	std::vector<Ogre::Real>		mPathLength;			
	std::vector<Ogre::Real>		mKumPathLength;			
	Ogre::SceneNode*			elementNode;			// The PulsNode
	Ogre::Real					mDistance;              // The distance the object has left to travel
	Ogre::Real					mFullDistance;          // Dead Kittenz...
	Ogre::Vector3				mOldDirection;             // The direction the object is moving
	Ogre::Real					mOldFullDistance;      // They told me I could be anything... So I became a fucking long variable
	Ogre::Vector3				mDirection;             // The direction the object is moving
	Ogre::Vector3				mDestination;           // The destination the object is moving towards
	Ogre::Vector3				mOldDestination;        // The last destination the object was moving towards
	Ogre::Real					mWalkSpeed;             // The speed at which the object is moving
	Ogre::Real					move;
	int							numPoint;				// Number of Path Point
	float						position;				// Position on the Path
	bool						finished;
};

#endif
