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

#include "ChaseCamera.hpp"


void ChaseCamera::rotate(Ogre::Vector3 rotation)
{
	float zoomAmount = rotation.z / AbstractCamera::ZOOM_SPEED_DIVISOR;
	m_pCameraNode->translate(0, 0, zoomAmount);

	// stay between min and max distance
	float distance = this->m_pCameraNode->getPosition().z;
	
	if(distance < m_fMinDistance)
	{
		// move back
		m_pCameraNode->setPosition(0, 0, m_fMinDistance);
	}

	if(distance > m_fMaxDistance)
	{
		m_pCameraNode->setPosition(0, 0, m_fMaxDistance);
	}
}

void ChaseCamera::setMinDistance(float dist)
{
	m_fMinDistance = dist;
}

void ChaseCamera::setMaxDistance(float dist)
{
	m_fMaxDistance = dist;
}
