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

class OrbitCamera : public AbstractCamera
{
public:
	OrbitCamera(Ogre::Camera* camera, Ogre::SceneNode* targetNode) : AbstractCamera(camera)
	{ 
		m_pTargetNode = targetNode;
		#ifndef BELEG_DEMO
			m_fInitialDistance = 10.0f;
			rotX = 0.0f;
			rotY = 0.0f;
		#else
			m_fInitialDistance = 0.05f;
			rotX = -75.0f;
			rotY = 0.0f;
		#endif

		m_pCameraTargetNode = m_pTargetNode->createChildSceneNode();
		m_pCameraTargetNode->setInheritOrientation(false);
		m_pCameraNode = m_pCameraTargetNode->createChildSceneNode(Ogre::Vector3(0, 0, Ogre::Real(m_fInitialDistance)));
		m_pCameraNode->attachObject(m_pCamera);
		m_pCamera->setAutoTracking(true, m_pCameraTargetNode);
		//m_pCamera->setFixedYawAxis(true);
		m_pCamera->setNearClipDistance(0.001f);
		m_pCamera->setFarClipDistance(1000);
	}

	void rotate(Ogre::Vector3 rotation);
	void absoluteRotate(Ogre::Vector3 rotation);
	void translate(Ogre::Vector3 translation);

	void setMinDistance(float dist);
	void setMaxDistance(float dist);

	Ogre::SceneNode* getCameraTargetNode();
	Ogre::SceneNode* getCameraNode();

private:
	Ogre::SceneNode* m_pCameraNode;
	Ogre::SceneNode* m_pCameraTargetNode;
	Ogre::SceneNode* m_pTargetNode;
	float m_fInitialDistance;

	float m_fMinDistance;
	float m_fMaxDistance;
	float rotX;
	float rotY;
};
