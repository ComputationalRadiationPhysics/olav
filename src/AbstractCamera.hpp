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
#include "OgreFramework.hpp"

class AbstractCamera
{
public:
	AbstractCamera(Ogre::Camera* camera)
	{
		this->m_pCamera = camera;
		//camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
		//camera->setOrthoWindow(0.5,0.5);
	}

	Ogre::Camera* getOgreCamera()
	{
		return m_pCamera;
	}

	Ogre::Real getDistanceToPoint(Ogre::Vector3 point)
	{
		return m_pCamera->getDerivedPosition().distance(point);
	}

	Ogre::Real getDistanceToPointSq(Ogre::Vector3 point)
	{
		return m_pCamera->getDerivedPosition().squaredDistance(point);
	}

	virtual void rotate(Ogre::Vector3 rotation) = 0;
	virtual void translate(Ogre::Vector3 translation) = 0;

protected:
	Ogre::SceneManager* m_pOgreSceneManager;
	Ogre::Camera* m_pCamera;
	static const int ZOOM_SPEED_DIVISOR = 2;
};
