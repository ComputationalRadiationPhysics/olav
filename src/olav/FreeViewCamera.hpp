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

#include "AbstractCamera.hpp"
#include "CollisionTools.hpp"

class FreeViewCamera : public AbstractCamera
{
public:
	FreeViewCamera(Ogre::Camera* camera) : AbstractCamera(camera) 
	{
		bIsHeightFixed = false;
		fFixedHeight = 0.0f;
		m_pCamera->setNearClipDistance(1);
	}
	Ogre::Real getDistanceToPoint(Ogre::Vector3 point);

	void rotate(Ogre::Vector3 rotation);
	void translate(Ogre::Vector3 translation);
	void setFixedHeight(float height);
	float getFixedHeight();
	void enableFixedHeight();
	void disableFixedHeight();

	bool isHeightFixed();

private:
	MOC::CollisionTools* mCollisionTools;
	Ogre::Vector3 oldPos;
	bool bIsHeightFixed;
	float fFixedHeight;
};
