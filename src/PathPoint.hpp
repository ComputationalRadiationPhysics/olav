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
#include <Ogre.h>

/// helper class for the use of paths (e.g. Pulse, PathCamera)
/// stores position, time in animation, distance to last point and the direction of the vector to the last point
class PathPoint
{
public:
	/// constructor - just initialize values
	PathPoint(Ogre::Vector3 pos, Ogre::Real t, Ogre::Real dist = 0, Ogre::Vector3 direction = Ogre::Vector3::ZERO)
	{
		position = pos;
		time = t;
		distanceToLastPoint = dist;
		this->direction = direction;
	}

	~PathPoint(void)
	{
	};

	/// the position of the PathPoint
	Ogre::Vector3 position;
	/// the distance to the last point
	Ogre::Real distanceToLastPoint;
	/// the time in the animation
	Ogre::Real time;
	/// the direction from the last vector
	Ogre::Vector3 direction;
};
