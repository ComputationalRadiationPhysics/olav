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
#include "MoveablePathElement.hpp"


class ChaseCamera : public AbstractCamera
{
public:
	ChaseCamera(Ogre::Camera* camera, MoveablePathElement* mpe) : AbstractCamera(camera)
	{
		m_pTargetNode = mpe->getOgreNode();
		m_fInitialDistance = 10.0f;

		m_pCameraTargetNode = m_pTargetNode->createChildSceneNode();
		m_pCameraNode = m_pCameraTargetNode->createChildSceneNode(m_pTargetNode->getOrientation().xAxis().normalisedCopy() * -m_fInitialDistance);
		m_pCameraNode->attachObject(m_pCamera);
		m_pCamera->setAutoTracking(true, m_pCameraTargetNode);
		m_pCamera->setFixedYawAxis(true);
		m_pCamera->setNearClipDistance(0.1f);
		m_pCamera->setFarClipDistance(1000);
	}

	void rotate(Ogre::Vector3 rotation);
	void translate(Ogre::Vector3 translation){}

	void setMinDistance(float dist);
	void setMaxDistance(float dist);

private:
	Ogre::SceneNode* m_pCameraNode;
	Ogre::SceneNode* m_pCameraTargetNode;
	Ogre::SceneNode* m_pTargetNode;
	float m_fInitialDistance;

	float m_fMinDistance;
	float m_fMaxDistance;
};
