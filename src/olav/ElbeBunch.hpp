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
#include "MoveablePathElement.hpp"
#include "ElbeConstants.hpp"
#include "ElbeElectron.hpp"
#include "AbstractCamera.hpp"

#ifndef ELBE_ONLY
	#include "ElectronVis.h"
#endif

#define TRACK_COUNT 10
#define MAX_VOLUME_BIT 21
#define RENDER_TEXTURE_SIZE 1024	

class ElbeBunch : public MoveablePathElement,public Ogre::RenderTargetListener
{
public:
	ElbeBunch(Ogre::SceneManager* mSceneMgr, Ogre::SceneNode* node,double time__ = 0,bool followed = false, bool is_in_collision = false,int electronCount = MAX_PARTICLE_COUNT,bool show_billboard = true,bool show_volume = true,int shader = 0,bool show_border = true);
	~ElbeBunch();

	void visualize(AbstractCamera* camera,float speed);	// make cloud- or bunch-entity visible
	bool update(double timeSinceLastFrame,bool explosion,bool update_particles = true,int update_tracks = TRACK_COUNT);	// call this method every fram
	void reset();
	bool isInKryostat();
	bool isEndKryostat();
	void updateParticle(int update_tracks);
	void setTreeVisible(bool value);
	void set_not_visible(Ogre::SceneNode* sn);
	virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);

	// returns the current radius_x, radius_y and length of the bunch in [mm]
	Ogre::Vector3 getCurrentDimensions();

	ElbeElectron* getElectron();

	double getBeta();
	double getGamma();
	Ogre::ManualObject*			singleTrack[TRACK_COUNT][2];
	int singleTrackCount;
	Ogre::Vector3				oldTrackPosition[TRACK_COUNT][4];
	Ogre::Billboard*			billboard[MAX_PARTICLE_COUNT];
	float lastTrackUpdate;
	bool show_billboard;
	bool show_volume;
	bool show_border;
	float stepwidth;

private:
	int volume_x,volume_y,volume_z;
	Ogre::uint32 *volume_lookup; //RGBA * three dimensions
	Ogre::TexturePtr volume_texture;

	Ogre::SceneNode* ogreNode;
	Ogre::Entity* m_pBunchEntity;
	Ogre::Entity* m_pBunchEntity_background;
	Ogre::Entity* m_pBunchEntity_border;
	int entityCount;
	Ogre::Entity* entityList[10000]; //in fact 3655 should be enough
	Ogre::Entity* m_pCloudEntity;
	Ogre::BillboardSet* m_pBillboards;

	Ogre::MaterialPtr m_pMatBunch;

	Ogre::Vector3 m_Scale;
	ElbeElectron* electron;
	int zeroCounter;
	double brightness;
	static int counter;
	bool followed;
	Ogre::SceneManager* mSceneMgr;
	bool is_in_collision;
	Ogre::SceneNode* lightNode;
	Ogre::Light* light;
	float electronSize;
	Ogre::Vector3 oldPosition;
	int shader;
	Ogre::TexturePtr rtt_texture;
	Ogre::RenderTexture *renderTexture;
	Ogre::SceneNode* extraNode;
#ifndef RELEASE
	ElectronVis*	   m_pElectronVis;
	Ogre::ManualObject* blackPlane;
	Ogre::SceneNode*   electronNode;
	tpoint             electron_position;
	tpoint             mapped_position;
	float a,olda;
#endif
};
