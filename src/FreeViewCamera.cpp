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

#include "FreeViewCamera.hpp"


void FreeViewCamera::rotate(Ogre::Vector3 rotation)
{
	m_pCamera->yaw(Ogre::Degree(rotation.x));
	m_pCamera->pitch(Ogre::Degree(rotation.y));

	// mouse wheel for high control
	this->fFixedHeight += rotation.z * 0.05f;
	while (this->fFixedHeight < 1.0f)
		this->fFixedHeight += 12.0f * 0.05f;
	while (this->fFixedHeight > 44.0f)
		this->fFixedHeight -= 12.0f * 0.05f;
}


void FreeViewCamera::translate(Ogre::Vector3 translation)
{
	m_pCamera->moveRelative(translation);

	if(bIsHeightFixed)
	{
		m_pCamera->setPosition(m_pCamera->getDerivedPosition().x, Ogre::Real(fFixedHeight), m_pCamera->getDerivedPosition().z);
	}
}

Ogre::Real FreeViewCamera::getDistanceToPoint(Ogre::Vector3 point)
{
	return (m_pCamera->getPosition().distance(point));
}

void FreeViewCamera::enableFixedHeight()
{
	bIsHeightFixed = true;
}

void FreeViewCamera::disableFixedHeight()
{
	bIsHeightFixed = false;
}

void FreeViewCamera::setFixedHeight(float height)
{
	fFixedHeight = height;
}

float FreeViewCamera::getFixedHeight()
{
	return fFixedHeight;
}

bool FreeViewCamera::isHeightFixed()
{
	return bIsHeightFixed;
}
