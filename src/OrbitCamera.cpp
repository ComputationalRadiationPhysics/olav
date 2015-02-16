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

#include "OrbitCamera.hpp"


void OrbitCamera::rotate(Ogre::Vector3 rotation)
{
	float zoomAmount = rotation.z / AbstractCamera::ZOOM_SPEED_DIVISOR * (this->m_pCameraNode->getPosition().z-m_fMinDistance)/(m_fMaxDistance-m_fMinDistance);

	// use x and y to yaw and pitch
	rotX+=rotation.x;
	rotY+=rotation.y;
	if (rotY>89.9f)
		rotY=89.9f;
	if (rotY<-89.9f)
		rotY=-89.9f;
	//m_pCameraTargetNode->yaw(Ogre::Degree(rotation.x), Ogre::Node::TS_WORLD);
	//m_pCameraTargetNode->pitch(Ogre::Degree(rotation.y));
	m_pCameraTargetNode->resetOrientation();
	m_pCameraTargetNode->rotate(Ogre::Vector3(0.0f,1.0f,0.0f),Ogre::Radian(Ogre::Degree(rotX)));
	m_pCameraTargetNode->rotate(Ogre::Vector3(1.0f,0.0f,0.0f),Ogre::Radian(Ogre::Degree(rotY)));
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
	#ifdef BELEG_DEMO
		printf("X: %f Y:%f D: %f\n",rotX,rotY,distance);
	#endif
}

void OrbitCamera::absoluteRotate(Ogre::Vector3 rotation)
{
	float zoomAmount = rotation.z;

	// use x and y to yaw and pitch
	rotX = rotation.x;
	rotY = rotation.y;
	if (rotY>89.9f)
		rotY=89.9f;
	if (rotY<-89.9f)
		rotY=-89.9f;
	//m_pCameraTargetNode->yaw(Ogre::Degree(rotation.x), Ogre::Node::TS_WORLD);
	//m_pCameraTargetNode->pitch(Ogre::Degree(rotation.y));
	m_pCameraTargetNode->resetOrientation();
	m_pCameraTargetNode->rotate(Ogre::Vector3(0.0f,1.0f,0.0f),Ogre::Radian(Ogre::Degree(rotX)));
	m_pCameraTargetNode->rotate(Ogre::Vector3(1.0f,0.0f,0.0f),Ogre::Radian(Ogre::Degree(rotY)));
	m_pCameraNode->resetToInitialState();
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
void OrbitCamera::translate(Ogre::Vector3 translation)
{}

void OrbitCamera::setMinDistance(float dist)
{
	m_fMinDistance = dist;
}

void OrbitCamera::setMaxDistance(float dist)
{
	m_fMaxDistance = dist;
}

Ogre::SceneNode* OrbitCamera::getCameraTargetNode()
{
	return m_pCameraTargetNode;
}

Ogre::SceneNode* OrbitCamera::getCameraNode()
{
	return m_pCameraNode;
}
