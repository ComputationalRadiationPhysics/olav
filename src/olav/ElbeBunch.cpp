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

#include "ElbeBunch.hpp"
#include "LMCApp.hpp"
#include "OrbitCamera.hpp"
#include "ElbePipe.hpp"
#include "ElbeState.hpp"
#include "OgreFramework.hpp"
#include <OgreGpuProgramManager.h>
#include <RenderSystems/GL/OgreGLFBORenderTexture.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>
#define _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
//	#include <crtdbg.h>
#endif
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define VOLUME_TYPE Ogre::TU_WRITE_ONLY

ElbeBunch::ElbeBunch(Ogre::SceneManager* mSceneMgr, Ogre::SceneNode* node,double time__,bool followed,bool is_in_collision,int electronCount,bool show_billboard,bool show_volume, int shader)
	: MoveablePathElement(node, is_in_collision?mSceneMgr->getEntity("ElectronPath")->getMesh():mSceneMgr->getEntity("BunchPath")->getMesh())
{
	stepwidth = 0.001;
	if (OgreFramework::getSingletonPtr()->m_iMajor == 1 &&
		OgreFramework::getSingletonPtr()->m_iMinor <= 4)
		shader = -1;
	renderTexture = NULL;
	this->shader = shader;
	this->show_billboard = show_billboard;
	this->show_volume = show_volume;
	if (followed && shader!=-1)
	{
		volume_x = 1 << MAX_VOLUME_BIT/3;
		volume_y = 1 << MAX_VOLUME_BIT/3;
		volume_z = 1 << MAX_VOLUME_BIT/3;
		volume_lookup = (Ogre::uint32*)malloc(4*volume_x*volume_y*volume_z);
	}
	electronSize = 1.0f;
	for (int i = 0; i < TRACK_COUNT; i++)
		singleTrack[i][0] = NULL;
	this->mSceneMgr = mSceneMgr;
	this->is_in_collision = is_in_collision;
	zeroCounter = 0;
	ogreNode = elementNode->createChildSceneNode();
	
	ogreNode->setScale(Ogre::Vector3(Ogre::Real(ElbeConstants::MODEL_SCALE), Ogre::Real(ElbeConstants::MODEL_SCALE), Ogre::Real(ElbeConstants::MODEL_SCALE)));

	m_pBunchEntity = mSceneMgr->createEntity("eBunch_elbe.mesh");
	m_pBunchEntity->setCastShadows(false);
	m_pBunchEntity->setVisible(true);

	m_pBunchEntity_background = NULL;
	
	if (followed && shader != -1)
	{
		switch (shader)
		{
			case 0:
				#if USE_HACKY_WORKAROUND
				if (OgreFramework::getSingletonPtr()->m_iMajor >= 3)
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudFineHacky");
				else
				#endif
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudFine");
				break;
			case 1:
			{
				#if USE_HACKY_WORKAROUND
				if (OgreFramework::getSingletonPtr()->m_iMajor >= 3)
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudRawHacky");
				else
				#endif
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudRaw");
				//Creating transfer function:
				
				#if USE_HACKY_WORKAROUND
					Ogre::TexturePtr transfer_texture = m_pMatBunch->getTechnique(0)->getPass((OgreFramework::getSingletonPtr()->m_iMajor>=3)?1:0)->getTextureUnitState(0)->_getTexturePtr();
				#else
					Ogre::TexturePtr transfer_texture = m_pMatBunch->getTechnique(0)->getPass(0)->getTextureUnitState(0)->_getTexturePtr();
				#endif
				Ogre::HardwarePixelBufferSharedPtr pixelBuffer = transfer_texture->getBuffer();
				pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL);
				const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
				Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
				Ogre::uint8 old_values[256][256][4];
				int new_values[256][256][4];
				
				memcpy(old_values,pDest,256*256*4);
				int s1,s2;
				float d = stepwidth;
				for (s1 = 0; s1 < 255; s1++)
					for (s2 = 0; s2 < 255; s2++)
					{
						int n = abs(s1-s2)+1;
						int i;
						new_values[s2][s1][0] = 0;
						new_values[s2][s1][1] = 0;
						new_values[s2][s1][2] = 0;
						new_values[s2][s1][3] = 0;
						for (i = 0; i < n; i++)
						{
							new_values[s2][s1][0] += (int)old_values[0][(s1*(n-i)+s2*i)/n][0];
							new_values[s2][s1][1] += (int)old_values[0][(s1*(n-i)+s2*i)/n][1];
							new_values[s2][s1][2] += (int)old_values[0][(s1*(n-i)+s2*i)/n][2];
							new_values[s2][s1][3] += (int)old_values[0][(s1*(n-i)+s2*i)/n][3];
						}
						//alpha
						float alpha = 1.0f-exp((float)(-new_values[s2][s1][3] / n)/255.0f*0.001f);
						//printf("%.9f %.9f\n",(float)(new_values[s2][s1][3] / n)/255.0f*0.001f,alpha);
						//new_values[s2][s1][3] = 255-exp_255[-new_values[s2][s1][3] / n]; //alpha
						new_values[s2][s1][3] = (int)(alpha/0.001*255.0f);
						//RGB
						new_values[s2][s1][0] = new_values[s2][s1][0] / n;
						new_values[s2][s1][1] = new_values[s2][s1][1] / n;
						new_values[s2][s1][2] = new_values[s2][s1][2] / n;
					}
				for (s1 = 0; s1 < 255; s1++)
					for (s2 = 0; s2 < 255; s2++)
					{
						old_values[s2][s1][0] = new_values[s2][s1][0];
						old_values[s2][s1][1] = new_values[s2][s1][1];
						old_values[s2][s1][2] = new_values[s2][s1][2];
						old_values[s2][s1][3] = new_values[s2][s1][3];
					}
				memcpy(pDest,old_values,256*256*4);	
				/*int stem_function[256][4];
				int x,y;
				int y_pos = 0;
				//Stemfunction
				stem_function[0][0] = old_values[y_pos][0][0];
				stem_function[0][1] = old_values[y_pos][0][1];
				stem_function[0][2] = old_values[y_pos][0][2];
				stem_function[0][3] = old_values[y_pos][0][3];
				for (x = 1; x < 256; x++)
				{
					stem_function[x][0] = stem_function[x-1][0] + old_values[y_pos][x][0];
					stem_function[x][1] = stem_function[x-1][1] + old_values[y_pos][x][1];
					stem_function[x][2] = stem_function[x-1][2] + old_values[y_pos][x][2];
					stem_function[x][3] = stem_function[x-1][3] + old_values[y_pos][x][3];
				}
				//Preintegration
				for (x=0; x < 256; x++)
					for (y=0; y<256; y++)
						if (x!=y)
						{
							new_values[y][x][0] = (stem_function[y][0]-stem_function[x][0])/(y-x);
							new_values[y][x][1] = (stem_function[y][1]-stem_function[x][1])/(y-x);
							new_values[y][x][2] = (stem_function[y][2]-stem_function[x][2])/(y-x);
							new_values[y][x][3] = (stem_function[y][3]-stem_function[x][3])/(y-x);
						}
						else
						{
							new_values[y][x][0] = 0;
							new_values[y][x][1] = 0;
							new_values[y][x][2] = 0;
							new_values[y][x][3] = 0;
						}
				memcpy(pDest,new_values,256*256*4);*/
				pixelBuffer->unlock();
			}
				break;
			case 2:
				#if USE_HACKY_WORKAROUND
				if (OgreFramework::getSingletonPtr()->m_iMajor >= 3)
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudAnalHacky");
				else
				#endif
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudAnal");
				break;
			case 3:
				#if USE_HACKY_WORKAROUND
				if (OgreFramework::getSingletonPtr()->m_iMajor >= 3)
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudAnalApproxHacky");
				else
				#endif
					m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudAnalApprox");
				break;
		}
		Ogre::ResourceGroupManager::getSingleton().createResourceGroup("volume_texture_group");
		volume_texture = Ogre::TextureManager::getSingleton().createManual(
			"volume_texture", // name
			"volume_texture_group",
			Ogre::TEX_TYPE_3D,      // type
			volume_x, volume_y, volume_z,
			0,                // number of mipmaps
			Ogre::PF_BYTE_RGBA,     // pixel format
			VOLUME_TYPE);      // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
							  // textures updated very often (e.g. each frame)
		#if USE_HACKY_WORKAROUND
			m_pMatBunch->getTechnique(0)->getPass(OgreFramework::getSingletonPtr()->m_iMajor>=3?1:0)->createTextureUnitState("volume_texture");//->setTextureFiltering(Ogre::TFO_NONE);
		#else
			m_pMatBunch->getTechnique(0)->getPass(0)->createTextureUnitState("volume_texture");//->setTextureFiltering(Ogre::TFO_NONE);
		#endif
		
		Ogre::ResourceGroupManager::getSingleton().createResourceGroup("rtt_texture_group");
		rtt_texture = Ogre::TextureManager::getSingleton().createManual(
			"rtt_texture",
			"rtt_texture_group",
			Ogre::TEX_TYPE_2D,
			RENDER_TEXTURE_SIZE,
			RENDER_TEXTURE_SIZE,
			0,
			Ogre::PF_X8R8G8B8,
			Ogre::TU_RENDERTARGET);
		#if USE_HACKY_WORKAROUND
			m_pMatBunch->getTechnique(0)->getPass(OgreFramework::getSingletonPtr()->m_iMajor>=3?1:0)->createTextureUnitState("rtt_texture")->setTextureFiltering(Ogre::TFO_NONE);
		#else
			m_pMatBunch->getTechnique(0)->getPass(0)->createTextureUnitState("rtt_texture")->setTextureFiltering(Ogre::TFO_NONE);
		#endif
		renderTexture = rtt_texture->getBuffer()->getRenderTarget();
		#if USE_HACKY_WORKAROUND		 
		if ( OgreFramework::getSingletonPtr()->m_iMajor >= 3)
			OgreFramework::getSingletonPtr()->m_pRoot->getRenderSystem()->setFBOWorkAroundFBO(((Ogre::GLFBORenderTexture*)renderTexture)->mFB.getGLFBOID());
		else
		#endif
		{
			renderTexture->addViewport(NULL);
			renderTexture->getViewport(0)->setClearEveryFrame(true);
			renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue(0.5f,0.5f,0.5f));
			renderTexture->getViewport(0)->setOverlaysEnabled(false);
			m_pBunchEntity_background = mSceneMgr->createEntity("eBunch_elbe.mesh");
			m_pBunchEntity_background->setCastShadows(false);
			m_pBunchEntity_background->setVisible(false);
			m_pBunchEntity_background->setMaterial(Ogre::MaterialManager::getSingletonPtr()->getByName("Examples/ElectronCloudFirst"));
			ogreNode->attachObject(m_pBunchEntity_background);
			renderTexture->addListener(this);
		}
	}
	else
	{
		// Materialien erzeugen
		m_pMatBunch = Ogre::MaterialManager::getSingletonPtr()->create("ElbeBunchMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		m_pMatBunch->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		m_pMatBunch->getTechnique(0)->getPass(0)->setTransparentSortingEnabled(true);
		m_pMatBunch->getTechnique(0)->getPass(0)->setTransparentSortingForced(true);
		m_pMatBunch->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);
		m_pMatBunch->setAmbient(0, 1, 0);
		m_pMatBunch->setDiffuse(0, 1, 0, 0.5);
		m_pMatBunch->setSpecular(0, 1, 0, 0.5);
	}
	m_pBunchEntity->setMaterial(m_pMatBunch);
	ogreNode->attachObject(m_pBunchEntity);


	ElbePipe::counter++;

	m_Scale = Ogre::Vector3(2, 2, 50);

	if (followed)
	{
		srand(ElbeState::followed_random_seed);
		electron = new ElbeElectron(time__,ElbeConstants::MAX_CALC_TIME,electronCount);
	}
	else
	{
		srand(time(NULL));
		electron = new ElbeElectron(time__,ElbeConstants::MAX_CALC_TIME,0);
	}


	brightness = 0.0f;
	this->followed = followed;
	if (followed)
	{
		light=mSceneMgr->createLight();
		light->setDiffuseColour(0.0f, 0.05f, 0.0f);
		light->setAttenuation(100, 0.0f, 0.0f, 0.5);
		light->setType(Ogre::Light::LT_POINT);
		light->setCastShadows(false);
		light->setRenderingDistance(0);
		elementNode->attachObject(light);
	}
	//ElectronVis stuff
#ifndef RELEASE
	if (is_in_collision)
	{
		ogreNode->rotate(Ogre::Vector3(0.0f,1.0f,0.0f),Ogre::Radian(M_PI/2.0f));

		//Electron from Radiation3D
		m_pElectronVis = new ElectronVis(8, mSceneMgr, 1.0f, 33,true);
		m_pElectronVis->electron->MAX_FIELDLINE=80;
		m_pElectronVis->slow_motion=33;
		m_pElectronVis->cutspheres=false;
		electronNode=ogreNode->createChildSceneNode();
		electronNode->scale(8.0f,8.0f,8.0f);
		electronNode->attachObject(m_pElectronVis->getElectron());
		m_pElectronVis->texmap=true;
		m_pElectronVis->electron->calc_wavefront=true;
		m_pElectronVis->showWavefronts=true;
		electronNode->attachObject(m_pElectronVis->getWaveSpheres());
		electron_position.x = this->getOgreNode()->getPosition().x;//-m_pElectronVis->electron->oldElectronPosition.x;
		electron_position.y = this->getOgreNode()->getPosition().y;//-m_pElectronVis->electron->oldElectronPosition.y;
		electron_position.z = this->getOgreNode()->getPosition().z;//-m_pElectronVis->electron->oldElectronPosition.z;
		electron_position.kx = 0;
		electron_position.ky = 0;
		electron_position.kz = 0;
		mapped_position.x = electronNode->getPosition().x;
		mapped_position.y = electronNode->getPosition().x;
		mapped_position.z = electronNode->getPosition().x;
		mapped_position.kx = 0;
		mapped_position.ky = 0;
		mapped_position.kz = 0;
		a=0.0f;
		olda=0.0f;

		//skyplane
		float size=5.0f;
		mesh sphere=m_pElectronVis->electron->createInitialSphere("initial_sphere",size,m_pElectronVis->electron->ci,m_pElectronVis->electron->ci,false);
		blackPlane=mSceneMgr->createManualObject("blackplane");
		blackPlane->setDynamic(true);
		blackPlane->begin("skysphere",Ogre::RenderOperation::OT_TRIANGLE_LIST);
	
		for (int it=0;it<sphere.size();it++)
			{
				blackPlane->position((Ogre::Real)sphere[it].position.x,(Ogre::Real)sphere[it].position.y,(Ogre::Real)sphere[it].position.z);
				for (int iz=0;iz<sphere[it].indices.size();iz++)
						blackPlane->index(sphere[it].indices[iz]);
				blackPlane->normal(-sphere[it].normal.x,-sphere[it].normal.y,-sphere[it].normal.z);
				blackPlane->textureCoord(sphere[it].normal.x/2.0f+0.5f,sphere[it].normal.y/2.0f+0.5f);
			}
		blackPlane->end();
		ogreNode->attachObject(blackPlane);
	}
#endif
	reloadFirstPosition();
	m_pBillboards = NULL;
	update(0,false,false,0);
	if (followed)
	{
		m_pBillboards = mSceneMgr->createBillboardSet();
		m_pBillboards->setBillboardType(Ogre::BBT_POINT);
		//m_pBillboards->setSortingEnabled(true);
		m_pBillboards->setMaterialName("Examples/ElectronParticle");

		for (int i = 0; i < electron->shown_particles; i++)
		{
			Ogre::Vector3 pos;
			pos.x = electron->Xraum.particle[0][i]*ElbeConstants::MODEL_SCALE_2;
			pos.y = electron->Yraum.particle[0][i]*ElbeConstants::MODEL_SCALE_2;
			pos.z = electron->Zraum.particle[0][i]*ElbeConstants::MODEL_SCALE_2;
			if (i < TRACK_COUNT)
				billboard[i] = m_pBillboards->createBillboard(pos, Ogre::ColourValue(1, 0, 0, 1));
			else
				billboard[i] = m_pBillboards->createBillboard(pos, Ogre::ColourValue(0, 1, 0, 1));
			billboard[i]->setDimensions(0.005f, 0.005f);
		}

		elementNode->attachObject(m_pBillboards);

		m_pBillboards->setVisible(show_billboard);
		lastTrackUpdate = electron->pos;

		for (int i = 0; i < TRACK_COUNT; i++)
		{
			//m_pBillboards->getBillboard(i)->_getDerivedPosition();
			singleTrack[i][0] = mSceneMgr->createManualObject();
			singleTrack[i][0]->setDynamic(true);
			singleTrack[i][1] = mSceneMgr->createManualObject();
			singleTrack[i][1]->setDynamic(true);
			oldTrackPosition[i][0] = Ogre::Vector3(
				getOgreNode()->_getDerivedPosition().x+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				getOgreNode()->_getDerivedPosition().y-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				getOgreNode()->_getDerivedPosition().z+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
			oldTrackPosition[i][1] = oldTrackPosition[i][0];
			oldTrackPosition[i][2] = oldTrackPosition[i][0];
			oldTrackPosition[i][3] = oldTrackPosition[i][0];
			Ogre::SceneNode* meowNode = mSceneMgr->createSceneNode("singletracknode"+Ogre::StringConverter::toString(i));
			mSceneMgr->getRootSceneNode()->addChild(meowNode);
			meowNode->attachObject(singleTrack[i][0]);
			singleTrack[i][0]->setVisible(true);
			meowNode->attachObject(singleTrack[i][1]);
			singleTrack[i][1]->setVisible(true);
			
			singleTrack[i][0]->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
			singleTrack[i][0]->position(oldTrackPosition[i][0]);
			singleTrack[i][0]->colour(1,0,0,1);
			singleTrack[i][0]->position(oldTrackPosition[i][1]);
			singleTrack[i][0]->colour(1,0,0,1);

			singleTrack[i][1]->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
			singleTrack[i][1]->position(oldTrackPosition[i][2]);
			singleTrack[i][1]->colour(1,0,0,1);
			singleTrack[i][1]->position(oldTrackPosition[i][3]);
			singleTrack[i][1]->colour(1,0,0,1);

			Ogre::Vector3 point[4];
			point[0] = Ogre::Vector3(
				+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
			point[1] = Ogre::Vector3(
				+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
			point[2] = Ogre::Vector3(
				+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
			point[3] = Ogre::Vector3(
				+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
				+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
			
			Ogre::Quaternion fix = Ogre::Quaternion(0.0f,-sqrt(2.0f)/2.0f,0.0f,-sqrt(2.0f)/2.0f);
			for (int j = 0; j < 4; j++)
			{
				point[j] = getOgreNode()->getOrientation()*fix*point[j];
				oldTrackPosition[i][j] =  getOgreNode()->_getDerivedPosition() + point[j];
			}

			singleTrack[i][0]->position(oldTrackPosition[i][0]);
			singleTrack[i][0]->colour(1,0,0,1);
			singleTrack[i][0]->position(oldTrackPosition[i][1]);
			singleTrack[i][0]->colour(1,0,0,1);
			singleTrack[i][0]->end();

			singleTrack[i][1]->position(oldTrackPosition[i][2]);
			singleTrack[i][1]->colour(1,0,0,1);
			singleTrack[i][1]->position(oldTrackPosition[i][3]);
			singleTrack[i][1]->colour(1,0,0,1);
			singleTrack[i][1]->end();
		}
		singleTrackCount = 0;
	}
}

ElbeBunch::~ElbeBunch()
{
	if (followed && shader!=-1)
		free(volume_lookup);
	elementNode->removeChild(ogreNode);
	mSceneMgr->destroySceneNode(ogreNode);
	mSceneMgr->destroyEntity(m_pBunchEntity);
	if (m_pBunchEntity_background)
		mSceneMgr->destroyEntity(m_pBunchEntity_background);

	if (followed)
	{
		mSceneMgr->destroyBillboardSet(m_pBillboards);
		for (int i = 0; i < TRACK_COUNT; i++)
		{
			mSceneMgr->destroySceneNode(singleTrack[i][0]->getParentSceneNode());
		}
		if (shader!=-1)
		{
			Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("volume_texture_group");
			Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("rtt_texture_group");
		}
	}
	elementNode->getParentSceneNode()->removeChild(elementNode);
	if (followed)
		mSceneMgr->destroyLight(light);
	delete electron;
}

inline void draw_sphere(Ogre::uint32 *volume_lookup,float x,float y,float z,float r,Ogre::uint32 alpha,int volume_x,int volume_y,int volume_z,float m_x,float m_y,float m_z)
{
	int i_pos_x = (int)((1.0f+x)/2.0f*(float)volume_x);
	if (i_pos_x < 0 || i_pos_x >= volume_x)
		return;
	int i_pos_y = (int)((1.0f+y)/2.0f*(float)volume_y);
	if (i_pos_y < 0 || i_pos_y >= volume_y)
		return;
	int i_pos_z = (int)((1.0f+z)/2.0f*(float)volume_z);
	if (i_pos_z < 0 || i_pos_z >= volume_z)
		return;
	/*int rx = (int)(r/m_x*(float)volume_x);
	int ry = (int)(r/m_y*(float)volume_y);
	int rz = (int)(r/m_z*(float)volume_z);
	int xl = i_pos_x - rx;
	if (xl < 0)
		xl = 0;
	int yl = i_pos_y - ry;
	if (yl < 0)
		yl = 0;
	int zl = i_pos_z - rz;
	if (zl < 0)
		zl = 0;
	int xr = i_pos_x + rx +1;
	if (xr > volume_x)
		xr = volume_x;
	int yr = i_pos_y + ry +1;
	if (yr > volume_y)
		yr = volume_y;
	int zr = i_pos_z + rz +1;
	if (zr > volume_z)
		zr = volume_z;
	for (int X = xl; X < xr; X++)
		for (int Y = yl; Y < yr; Y++)
			for (int Z = zl; Z < zr; Z++)*/
	volume_lookup[(i_pos_x + volume_x*(i_pos_y + volume_y*i_pos_z))] += alpha;
}

void set_if_changed(int *dest,int value,int *changed)
{
	if (value != *dest)
	{
		*dest = value;
		*changed = 1;
	}
}

void ElbeBunch::visualize(AbstractCamera* camera,float speed)
{
	float alpha = 1.0f;
	float bill_alpha = 1.0f;
	float plane_alpha = 0.0f;
	float camera_float = camera->getOgreCamera()->getDerivedPosition().normalisedCopy().dotProduct(ogreNode->_getDerivedPosition().normalisedCopy());
	camera_float = 0.0f;
	float biggest = m_Scale.x;
	if (m_Scale.y > biggest)
		biggest = m_Scale.y;
	if (m_Scale.z > biggest)
		biggest = m_Scale.z;
	float smallest = m_Scale.x;
	if (m_Scale.y < smallest)
		smallest = m_Scale.y;
	if (m_Scale.z < smallest)
		smallest = m_Scale.z;
	if (followed)
	{
#ifndef RELEASE
		if (is_in_collision)
		{
			if (camera->getDistanceToPoint(ogreNode->_getDerivedPosition())<10.0f)
			{
				alpha = 0.0f;
				if (camera->getDistanceToPoint(ogreNode->_getDerivedPosition())<3.0f)
				{
					bill_alpha = 0.0f;
					plane_alpha = 1.0f;
				}
				else
				{
					bill_alpha = ((camera->getDistanceToPoint(ogreNode->_getDerivedPosition())-3.0f)/7.0f);
					plane_alpha = 1.0f-((camera->getDistanceToPoint(ogreNode->_getDerivedPosition())-3.0f)/7.0f);
				}
				plane_alpha *= 1.0f;//camera_float;
				electronNode->setVisible(true);
			}
			else
			{
				if (camera->getDistanceToPoint(ogreNode->_getDerivedPosition())<20.0f)
					alpha = ((camera->getDistanceToPoint(ogreNode->_getDerivedPosition())-10.0f)/10.0f);
				electronNode->setVisible(false);
			}
			Ogre::MaterialPtr materialPtr = ((Ogre::MaterialPtr) (Ogre::MaterialManager::getSingleton().getByName("skysphere")));
			Ogre::TextureUnitState* ptus = materialPtr->getTechnique(0)->getPass(0)->getTextureUnitState(0);
			ptus->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, plane_alpha);
			ptus = materialPtr->getTechnique(0)->getPass(0)->getTextureUnitState(1);
			if (!finished)
				ptus->setTextureScroll(speed,0.0f);
			if (position < 903.3f)
				m_pElectronVis->getWaveSpheres()->setVisible(false);
			else
				m_pElectronVis->getWaveSpheres()->setVisible(true);
		}
		else
#endif
		{
			if (camera->getDistanceToPoint(ogreNode->_getDerivedPosition())/*biggest/50.0f*/<0.3f)
				alpha = 0.0f;
			else
			if (camera->getDistanceToPoint(ogreNode->_getDerivedPosition())/*biggest/50.0f*/<1.0f)
				alpha = ((camera->getDistanceToPoint(ogreNode->_getDerivedPosition())/*biggest/50.0f*/-0.3f)/0.7f);
		}
	}
	electronSize = camera->getDistanceToPoint(ogreNode->_getDerivedPosition())*5.0f;
	if (electronSize > 1.0f)
		electronSize = 1.0f;
	if (electronSize < 0.2f)
		electronSize = 0.2f;
	alpha = (electronSize-0.2f)/0.8f;
	if (followed)
	{
		m_pBillboards->setVisible(show_billboard);
		if (alpha == 0.0f)
			m_pBunchEntity->setVisible(false);
		else
			m_pBunchEntity->setVisible(true);
	}
	if (followed && shader!=-1)
	{
		if (show_volume)
		{
			float divisor = 50.0f;
			if (biggest > divisor)
				divisor = biggest;
			m_pBunchEntity->setVisible(true);
			Ogre::Vector3 v = camera->getOgreCamera()->getDerivedPosition()-ogreNode->_getDerivedPosition();
			#if USE_HACKY_WORKAROUND
			if (OgreFramework::getSingletonPtr()->m_iMajor >= 3)
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("windowsize",Ogre::Vector4(
				camera->getOgreCamera()->getViewport()->getActualWidth()/2,
				camera->getOgreCamera()->getViewport()->getActualHeight()/2,
				camera->getOgreCamera()->getViewport()->getActualWidth()*65/64,
				camera->getOgreCamera()->getViewport()->getActualHeight()*65/64));
			else
			#endif
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("windowsize",Ogre::Vector4(
				OgreFramework::getSingletonPtr()->m_pRenderWnd->getViewport(0)->getActualWidth()/2,
				OgreFramework::getSingletonPtr()->m_pRenderWnd->getViewport(0)->getActualHeight()/2,
				OgreFramework::getSingletonPtr()->m_pRenderWnd->getViewport(0)->getActualWidth()*65/64,
				OgreFramework::getSingletonPtr()->m_pRenderWnd->getViewport(0)->getActualHeight()*65/64));
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("kolor",Ogre::Vector4(brightness, 1.0f, brightness, alpha));
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("skale",Ogre::Vector4(m_Scale.x,m_Scale.y,m_Scale.z, biggest));

			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("ankle",Ogre::Vector4(v));
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("kamera",Ogre::Vector4(camera->getOgreCamera()->getDerivedPosition().x,camera->getOgreCamera()->getDerivedPosition().y,camera->getOgreCamera()->getDerivedPosition().z,show_billboard?1.0f:0.0f));
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("kenter",Ogre::Vector4(ogreNode->_getDerivedPosition().x,ogreNode->_getDerivedPosition().y,ogreNode->_getDerivedPosition().z,divisor));
			
			float render_time = LMCApp::m_pAppStateManager->getPerformance().render_time;
			if (render_time > 30000.0f)
				stepwidth *= 1.1;
			if (render_time < 20000.0f)
				stepwidth /= 1.1;
			
			if (stepwidth > 0.02)
				stepwidth = 0.02;
			if (stepwidth < 0.001)
				stepwidth = 0.001;
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("stepwidth",Ogre::Vector4(stepwidth,0.0,0.0,0.0));
			
			if (shader >= 2)
			{
				float phi = electron->calc_phi(electron->Xraum.alpha,electron->Xraum.beta,electron->Xraum.gamma);
				float SIN = -sin(phi);
				float COS = cos(phi);
				float A=sqrt(electron->Xraum.epsilon/(electron->Xraum.gamma*COS*COS-2.0f*electron->Xraum.alpha*COS*SIN+electron->Xraum.beta*SIN*SIN));
				float B=sqrt(electron->Xraum.epsilon/(electron->Xraum.gamma*SIN*SIN+2.0f*electron->Xraum.alpha*COS*SIN+electron->Xraum.beta*COS*COS));
				float R11 = +COS/A;
				float R12 = +SIN/B;
				float R21 = -SIN/A;
				float R22 = +COS/B;
				float a = (R11*R11+R12*R12)/2.0f;
				float b =  R12*R22+R11*R21;
				float c = (R21*R21+R22*R22)/2.0f;
				Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("prekalkulated_x",
					Ogre::Vector4(sqrt(1.0f/(16.0f*M_PI*c)),
								  b*b/(4*c)-a,
								  (b*electron->Xraum.beta-2.0f*c*electron->Xraum.alpha)/(2.0f*sqrt(c)*electron->Xraum.beta),
								  c));
				float part = sqrt(c*(electron->Xraum.beta*electron->Xraum.epsilon))/electron->Xraum.beta;
				float biggest_value = fabs(sqrt(1.0f/(16.0f*M_PI*c))*(erf(+part)-erf(-part)));
				Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("greek_x",Ogre::Vector4(electron->Xraum.alpha,electron->Xraum.beta,/*electron->Xraum.gamma*/biggest_value,electron->Xraum.epsilon));
				phi = electron->calc_phi(electron->Yraum.alpha,electron->Yraum.beta,electron->Yraum.gamma);
				SIN = -sin(phi);
				COS = cos(phi);
				A=sqrt(electron->Yraum.epsilon/(electron->Yraum.gamma*COS*COS-2.0f*electron->Yraum.alpha*COS*SIN+electron->Yraum.beta*SIN*SIN));
				B=sqrt(electron->Yraum.epsilon/(electron->Yraum.gamma*SIN*SIN+2.0f*electron->Yraum.alpha*COS*SIN+electron->Yraum.beta*COS*COS));
				R11 = +COS/A;
				R12 = +SIN/B;
				R21 = -SIN/A;
				R22 = +COS/B;
				a = (R11*R11+R12*R12)/2.0f;
				b =  R12*R22+R11*R21;
				c = (R21*R21+R22*R22)/2.0f;
				Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("prekalkulated_y",
					Ogre::Vector4(sqrt(1.0f/(16.0f*M_PI*c)),
								  b*b/(4*c)-a,
								  (b*electron->Yraum.beta-2.0f*c*electron->Yraum.alpha)/(2.0f*sqrt(c)*electron->Yraum.beta),
								  c));
				part = sqrt(c*(electron->Yraum.beta*electron->Yraum.epsilon))/electron->Yraum.beta;
				biggest_value = fabs(sqrt(1.0f/(16.0f*M_PI*c))*(erf(+part)-erf(-part)));
				Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("greek_y",Ogre::Vector4(electron->Yraum.alpha,electron->Yraum.beta,/*electron->Yraum.gamma*/biggest_value,electron->Yraum.epsilon));
				phi = electron->calc_phi(electron->Zraum.alpha,electron->Zraum.beta,electron->Zraum.gamma);
				SIN = -sin(phi);
				COS = cos(phi);
				A=sqrt(electron->Zraum.epsilon/(electron->Zraum.gamma*COS*COS-2.0f*electron->Zraum.alpha*COS*SIN+electron->Zraum.beta*SIN*SIN));
				B=sqrt(electron->Zraum.epsilon/(electron->Zraum.gamma*SIN*SIN+2.0f*electron->Zraum.alpha*COS*SIN+electron->Zraum.beta*COS*COS));
				R11 = +COS/A;
				R12 = +SIN/B;
				R21 = -SIN/A;
				R22 = +COS/B;
				a = (R11*R11+R12*R12)/2.0f;
				b =  R12*R22+R11*R21;
				c = (R21*R21+R22*R22)/2.0f;
				Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("prekalkulated_z",
					Ogre::Vector4(sqrt(1.0f/(16.0f*M_PI*c)),
								  b*b/(4*c)-a,
								  (b*electron->Zraum.beta-2.0f*c*electron->Zraum.alpha)/(2.0f*sqrt(c)*electron->Zraum.beta),
								  c));
				part = sqrt(c*(electron->Zraum.beta*electron->Zraum.epsilon))/electron->Zraum.beta;
				biggest_value = fabs(sqrt(1.0f/(16.0f*M_PI*c))*(erf(+part)-erf(-part)));
				Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("greek_z",Ogre::Vector4(electron->Zraum.alpha,electron->Zraum.beta,/*electron->Zraum.gamma*/biggest_value,electron->Zraum.epsilon));
			}
			if (shader == 0)
			{
				int bits_x = ceil(log(m_Scale.x)/log(2.0f));
				int bits_y = ceil(log(m_Scale.y)/log(2.0f));
				int bits_z = ceil(log(m_Scale.z)/log(2.0f));
				int extra_bits = ceil((float)(bits_x+bits_y+bits_z - MAX_VOLUME_BIT)/3.0f);
				bits_x -= extra_bits;
				bits_y -= extra_bits;
				bits_z -= extra_bits;
				int changed = 0;
				set_if_changed(&volume_x,1 << bits_x,&changed);
				set_if_changed(&volume_y,1 << bits_y,&changed);
				set_if_changed(&volume_z,1 << bits_z,&changed);
				if (changed)
				{
					Ogre::ResourceGroupManager::getSingleton().clearResourceGroup("volume_texture_group");
					volume_texture = Ogre::TextureManager::getSingleton().createManual(
						"volume_texture", // name
						"volume_texture_group",
						Ogre::TEX_TYPE_3D,      // type
						volume_x, volume_y, volume_z,
						0,                // number of mipmaps
						Ogre::PF_BYTE_RGBA,     // pixel format
						VOLUME_TYPE);
					#if (OGRE_VERSION_MAJOR == 1) && (OGRE_VERSION_MINOR < 8)
						m_pMatBunch->getTechnique(0)->getPass(OgreFramework::getSingletonPtr()->m_iMajor>=3?1:0)->getTextureUnitState(1)->_setTexturePtr(volume_texture);//->setTextureFiltering(Ogre::TFO_NONE);
					#else
						m_pMatBunch->getTechnique(0)->getPass(OgreFramework::getSingletonPtr()->m_iMajor>=3?1:0)->getTextureUnitState(1)->setTexture(volume_texture);//->setTextureFiltering(Ogre::TFO_NONE);
					#endif
				}
			}
			if (shader < 2)
			{
				memset(volume_lookup,0,volume_x*volume_y*volume_z*4);
				if (shader == 0) //fine
				{
					int sphere_alpha = (int)(10000.0f*(float)(volume_x*volume_y*volume_z)/(float)(1 << MAX_VOLUME_BIT))*10000/ElbePipe::particle_count;
					for (int i = 0; i < electron->shown_particles; i++)
						draw_sphere(
							volume_lookup,
							electron->Xraum.particle[0][i]/m_Scale.x,
							electron->Yraum.particle[0][i]/m_Scale.y,
							electron->Zraum.particle[0][i]/m_Scale.z,
							0.1,
							sphere_alpha,
							volume_x,
							volume_y,
							volume_z,
							m_Scale.x,
							m_Scale.y,
							m_Scale.z);
				}
				else // shader == 1, raw
				{
					int sphere_alpha = 400*10000/ElbePipe::particle_count;
					for (int i = 0; i < electron->shown_particles; i++)
						draw_sphere(
							volume_lookup,
							electron->Xraum.particle[0][i]/divisor,
							electron->Yraum.particle[0][i]/divisor,
							electron->Zraum.particle[0][i]/divisor,
							0.1,
							sphere_alpha,
							volume_x,
							volume_y,
							volume_z,
							m_Scale.x,
							m_Scale.y,
							m_Scale.z);
					/*for (int x = 0; x < volume_x; x++)
						for (int y = 0; y < volume_y; y++)
							for (int z = 0; z < volume_z; z++)
							draw_sphere(
								volume_lookup,
								(float)x/(float)volume_x*2.0f-1.0f,
								(float)y/(float)volume_y*2.0f-1.0f,
								(float)z/(float)volume_z*2.0f-1.0f,
								0.1,
								(int)(
								electron->GAUSS(&(electron->Xraum),((float)x/(float)volume_x*2.0f-1.0f)*divisor/m_Scale.x)*
								electron->GAUSS(&(electron->Yraum),((float)y/(float)volume_y*2.0f-1.0f)*divisor/m_Scale.y)*
								electron->GAUSS(&(electron->Zraum),((float)z/(float)volume_z*2.0f-1.0f)*divisor/m_Scale.z)*
								100000000000000.0f),
								volume_x,
								volume_y,
								volume_z,
								m_Scale.x,
								m_Scale.y,
								m_Scale.z);*/					
				}
				//printf("%i %i %i\n",volume_x,volume_y,volume_z);
				Ogre::HardwarePixelBufferSharedPtr pixelBuffer = volume_texture->getBuffer();
				pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
				const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
				Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
				int c = 0;
				for (int i = 0; i < volume_x*volume_y*volume_z; i++)
					if (volume_lookup[i] != 0)
						c++;
				//printf("%i %i %f\n",c,volume_x*volume_y*volume_z,(float)c/(float)(volume_x*volume_y*volume_z));
				memcpy(pDest,volume_lookup,volume_x*volume_y*volume_z*4);
				pixelBuffer->unlock();
				#if USE_HACKY_WORKAROUND
					Ogre::GpuProgramParametersSharedPtr pParams = m_pMatBunch->getTechnique(0)->getPass(OgreFramework::getSingletonPtr()->m_iMajor>=3?1:0)->getFragmentProgramParameters();
					if (OgreFramework::getSingletonPtr()->m_iMajor < 3 && renderTexture)
				#else
						renderTexture->getViewport(0)->setCamera(camera->getOgreCamera());
					Ogre::GpuProgramParametersSharedPtr pParams = m_pMatBunch->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
				#endif
				pParams->setNamedConstant("volume_texture", 1);
				pParams->setNamedConstant("rtt_texture", 2);
			}
		}
		else
			Ogre::GpuProgramManager::getSingletonPtr()->getSharedParameters("ElectronCloudParams")->setNamedConstant("kolor",Ogre::Vector4(0.0,0.0,0.0,0.0));
	}
	else
	if (show_volume)
		m_pBunchEntity->setVisible(true);
	else
		m_pBunchEntity->setVisible(false);
	this->m_pMatBunch->setDiffuse(brightness, 1.0f, brightness, alpha);
	/*if (followed)
	{
		for (int i = 0; i < m_pBillboards->getNumBillboards(); i++)
			m_pBillboards->getBillboard(i)->setColour(Ogre::ColourValue(brightness, 1.0f, brightness, bill_alpha));
		//m_pBillboards->notifyBillboardDataChanged();
	}*/
	double old_pos = electron->pos;
	float size = electronSize/1000.0f;
	//size = 0.0002f;
	if (m_pBillboards)
	{
		float greatest = 0.0f;
		#ifdef WIN32
			float* len = (float*)malloc(sizeof(float)*electron->shown_particles);
		#else
			float len[electron->shown_particles];
		#endif
		for (int i = 0; i < electron->shown_particles; i++)
		{
			len[i] = sqrt(electron->Xraum.particle[1][i]*electron->Xraum.particle[1][i]+
			              electron->Yraum.particle[1][i]*electron->Yraum.particle[1][i]+
			              electron->Zraum.particle[1][i]*electron->Zraum.particle[1][i]);
			if (len[i] > greatest)
				greatest = len[i];
		}
		greatest/=2.0f;
		for (int i = 0; i < electron->shown_particles; i++)
		{
			Ogre::Vector3 pos;
			pos.x = electron->Xraum.particle[0][i]*ElbeConstants::MODEL_SCALE_2;
			pos.y = electron->Yraum.particle[0][i]*ElbeConstants::MODEL_SCALE_2;
			pos.z = electron->Zraum.particle[0][i]*ElbeConstants::MODEL_SCALE_2;
			billboard[i]->setPosition(pos);
			billboard[i]->setDimensions(size,size);
			len[i] /= greatest;
			if (len[i] > 1.0f)
				len[i] = 1.0f;
			billboard[i]->setColour(Ogre::ColourValue(0,len[i],0));
		}
		#ifdef WIN32
			free(len);
		#endif
	}
}

bool ElbeBunch::update(double timeSinceLastFrame,bool explosion,bool update_particles,int update_tracks)
{
#ifndef RELEASE
	if (is_in_collision)
	{
		this->m_pBunchEntity->getParentSceneNode()->setScale(0.3f,0.2f,0.2f);
		updatePosition();
		float dt=1.0f/2000000000.0f;
		float speed=0.99*299792458.0f;
		for (int n=0;n<timeSinceLastFrame;n++)
			if (this->m_pElectronVis->updateElectron_Calc(1, dt/*0.0000000025*/ /*s*/, mapped_position)>0)
			{
				tpoint move_vector;
				move_vector.x=0.0f;
				move_vector.y=0.0f;
				move_vector.z=0.0f;
				move_vector.x = -this->getOgreNode()->getPosition().x + electron_position.x;
				move_vector.y = -this->getOgreNode()->getPosition().y + electron_position.y;
				move_vector.z = -this->getOgreNode()->getPosition().z + electron_position.z;
				if (position>=1102.3f && position<=1104.3f && mWalkSpeed > 0)
				{
					move_vector.x=-(a-olda);
					move_vector.y=-(sin(a*2.0f*M_PI/8.0f)*0.04f);
				}
				float length = sqrt(move_vector.x*move_vector.x+
									move_vector.y*move_vector.y+
									move_vector.z*move_vector.z);
				if (length==0)
					length=1;

				move_vector.x = move_vector.x/length*speed*dt;
				move_vector.y = move_vector.y/length*speed*dt;
				move_vector.z = move_vector.z/length*speed*dt;

			
				mapped_position.x+=move_vector.x;
				mapped_position.y+=move_vector.y;
				mapped_position.z+=move_vector.z;
				if (getPathPosition()>=1102.3f && getPathPosition()<=1104.3f && mWalkSpeed > 0){

					olda=a;
					a+=helper::SineSamplingBisection(speed*dt,8.0f,0.04f,mapped_position.x);
				}
			this->m_pElectronVis->updateElectron_Draw(true,true);
			this->m_pElectronVis->updateSpheres_Draw(true,true);
		}
		return true;
	}
#endif

	//m_pBillboards->notifyBillboardDataChanged();

	// move on
	if (!electron->calc(timeSinceLastFrame,ElbeConstants::MAX_CALC_TIME))
		return false;
	if (electron->driftnumber < 31)
	{
		double relative_position = electron->L/electron->getDriftLength(electron->driftnumber);
		double now_position = electron->getRealDriftPosition(electron->driftnumber) + relative_position*electron->getRealDriftLength(electron->driftnumber);
		double real_way = now_position - position;
		updatePositionWay(real_way);
	}
	else
		updatePositionWay(timeSinceLastFrame*0.003); //nearly C

	if (m_pBillboards)
	{
		float size = electronSize/1000.0f;
		size /= 4.0f;
		if (followed && fabs(electron->pos-lastTrackUpdate) > 200.0f)
		{
			for (int i = 0; i < update_tracks; i++)
			{
				singleTrack[i][0]->beginUpdate(singleTrackCount);
				singleTrack[i][0]->position(oldTrackPosition[i][0]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->position(oldTrackPosition[i][1]);
				singleTrack[i][0]->colour(1,0,0,1);

				singleTrack[i][1]->beginUpdate(singleTrackCount);
				singleTrack[i][1]->position(oldTrackPosition[i][2]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->position(oldTrackPosition[i][3]);
				singleTrack[i][1]->colour(1,0,0,1);

				Ogre::Vector3 point[4];
				point[0] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2-size,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
				point[1] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2+size,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
				point[2] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2-size);
				point[3] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2+size);
				
				Ogre::Quaternion fix = Ogre::Quaternion(0.0f,-sqrt(2.0f)/2.0f,0.0f,-sqrt(2.0f)/2.0f);
				for (int j = 0; j < 4; j++)
				{
					point[j] = getOgreNode()->getOrientation()*fix*point[j];
					oldTrackPosition[i][j] =  getOgreNode()->_getDerivedPosition() + point[j];
				}

				singleTrack[i][0]->position(oldTrackPosition[i][0]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->position(oldTrackPosition[i][1]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->end();

				singleTrack[i][1]->position(oldTrackPosition[i][2]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->position(oldTrackPosition[i][3]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->end();
				
				singleTrack[i][0]->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
				singleTrack[i][0]->position(oldTrackPosition[i][0]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->position(oldTrackPosition[i][1]);
				singleTrack[i][0]->colour(1,0,0,1);

				singleTrack[i][1]->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
				singleTrack[i][1]->position(oldTrackPosition[i][2]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->position(oldTrackPosition[i][3]);
				singleTrack[i][1]->colour(1,0,0,1);

				point[0] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2-size,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
				point[1] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2+size,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
				point[2] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2-size);
				point[3] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2+size);
				
				singleTrack[i][0]->position(oldTrackPosition[i][0]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->position(oldTrackPosition[i][1]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->end();

				singleTrack[i][1]->position(oldTrackPosition[i][2]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->position(oldTrackPosition[i][3]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->end();
			}
			lastTrackUpdate = electron->pos;
			singleTrackCount++;
		}
		else
		{
			for (int i = 0; i < update_tracks; i++)
			{
				singleTrack[i][0]->beginUpdate(singleTrackCount);
				singleTrack[i][0]->position(oldTrackPosition[i][0]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->position(oldTrackPosition[i][1]);
				singleTrack[i][0]->colour(1,0,0,1);

				singleTrack[i][1]->beginUpdate(singleTrackCount);
				singleTrack[i][1]->position(oldTrackPosition[i][2]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->position(oldTrackPosition[i][3]);
				singleTrack[i][1]->colour(1,0,0,1);

				Ogre::Vector3 point[4];
				point[0] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2-size,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
				point[1] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2+size,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
				point[2] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2-size);
				point[3] = Ogre::Vector3(
					+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
					+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2+size);
				
				Ogre::Quaternion fix = Ogre::Quaternion(0.0f,-sqrt(2.0f)/2.0f,0.0f,-sqrt(2.0f)/2.0f);
				Ogre::Vector3 newPosition[4];
				for (int j = 0; j < 4; j++)
				{
					point[j] = getOgreNode()->getOrientation()*fix*point[j];
					newPosition[j] =  getOgreNode()->_getDerivedPosition() + point[j];
				}

				singleTrack[i][0]->position(newPosition[0]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->position(newPosition[1]);
				singleTrack[i][0]->colour(1,0,0,1);
				singleTrack[i][0]->end();

				singleTrack[i][1]->position(newPosition[2]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->position(newPosition[3]);
				singleTrack[i][1]->colour(1,0,0,1);
				singleTrack[i][1]->end();
			}
		}
	}
	m_Scale = electron->getBunchScale();
	// update the visualization
	this->m_pBunchEntity->getParentSceneNode()->setScale(m_Scale * ElbeConstants::MODEL_SCALE);

	if (electron->driftnumber != electron->previousDriftnumber)
		brightness = 1.0;
	if (brightness>0.0) {
		brightness -= timeSinceLastFrame*0.002;
		if (brightness < 0.0)
			brightness = 0.0;
	}

	if (mDirection == Ogre::Vector3::ZERO)
		zeroCounter ++;
	else
		zeroCounter = 0;
	if (zeroCounter>3)
		return false;
	return true;
}

Ogre::Vector3 ElbeBunch::getCurrentDimensions()
{
	return m_Scale;
}

void ElbeBunch::reset()
{
	MoveablePathElement::reloadFirstPosition();
	mWalkList.push_back(Ogre::Vector3(-90.55f, 13.81f, 28.33f)-Ogre::Vector3(80.6301f,13.7751f,-17.3819f));

	if (followed)
	{
		m_pBillboards->setVisible(show_billboard);
		m_pBunchEntity->setVisible(true);
	}

	ogreNode->setScale(Ogre::Vector3(Ogre::Real(ElbeConstants::MODEL_SCALE), Ogre::Real(ElbeConstants::MODEL_SCALE), Ogre::Real(ElbeConstants::MODEL_SCALE)));

	ogreNode->setVisible(false);
}

bool ElbeBunch::isInKryostat()
{
	return ((elementNode->_getDerivedPosition()-Ogre::Vector3(102.562f, 13.7751f, -17.3819f)).length() < mWalkSpeed*TimeManager::getTimeSinceLastFrame());
}

ElbeElectron* ElbeBunch::getElectron() {
	return electron;
}

double ElbeBunch::getBeta()
{
	return electron->beta;
}

double ElbeBunch::getGamma()
{
	return electron->gamma;
}

void ElbeBunch::updateParticle(int update_tracks)
{
	for (int i = update_tracks; i < TRACK_COUNT; i++)
	if (singleTrack[i][0])
	{
		singleTrack[i][0]->clear();
		singleTrack[i][1]->clear();
		oldTrackPosition[i][0] = Ogre::Vector3(
			getOgreNode()->_getDerivedPosition().x+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			getOgreNode()->_getDerivedPosition().y-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			getOgreNode()->_getDerivedPosition().z+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
		oldTrackPosition[i][1] = oldTrackPosition[i][0];
		oldTrackPosition[i][2] = oldTrackPosition[i][0];
		oldTrackPosition[i][3] = oldTrackPosition[i][0];
		
		singleTrack[i][0]->begin("Examples/ElectronParticleTrack", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
		singleTrack[i][0]->position(oldTrackPosition[i][0]);
		singleTrack[i][0]->colour(1,1,1,1);
		singleTrack[i][0]->position(oldTrackPosition[i][1]);
		singleTrack[i][0]->colour(1,1,1,1);

		singleTrack[i][1]->begin("Examples/ElectronParticleTrack", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
		singleTrack[i][1]->position(oldTrackPosition[i][2]);
		singleTrack[i][1]->colour(1,1,1,1);
		singleTrack[i][1]->position(oldTrackPosition[i][3]);
		singleTrack[i][1]->colour(1,1,1,1);

		Ogre::Vector3 point[4];
		point[0] = Ogre::Vector3(
			+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
		point[1] = Ogre::Vector3(
			+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
		point[2] = Ogre::Vector3(
			+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
		point[3] = Ogre::Vector3(
			+getElectron()->Zraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			-getElectron()->Yraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2,
			+getElectron()->Xraum.particle[0][i]* ElbeConstants::MODEL_SCALE_2);
		
		Ogre::Quaternion fix = Ogre::Quaternion(0.0f,-sqrt(2.0f)/2.0f,0.0f,-sqrt(2.0f)/2.0f);
		for (int j = 0; j < 4; j++)
		{
			point[j] = getOgreNode()->getOrientation()*fix*point[j];
			oldTrackPosition[i][j] =  getOgreNode()->_getDerivedPosition() + point[j];
		}

		singleTrack[i][0]->position(oldTrackPosition[i][0]);
		singleTrack[i][0]->colour(1,1,1,1);
		singleTrack[i][0]->position(oldTrackPosition[i][1]);
		singleTrack[i][0]->colour(1,1,1,1);
		singleTrack[i][0]->end();

		singleTrack[i][1]->position(oldTrackPosition[i][2]);
		singleTrack[i][1]->colour(1,1,1,1);
		singleTrack[i][1]->position(oldTrackPosition[i][3]);
		singleTrack[i][1]->colour(1,1,1,1);
		singleTrack[i][1]->end();
		singleTrackCount = 0;
	}
	if (m_pBillboards)
		for (int i = 0; i < TRACK_COUNT; i++)
			if (i < update_tracks)
				m_pBillboards->getBillboard(i)->setColour(Ogre::ColourValue(1, 0, 0, 1));
			else
				m_pBillboards->getBillboard(i)->setColour(Ogre::ColourValue(0, 1, 0, 1));
}

void ElbeBunch::set_not_visible(Ogre::SceneNode* sn)
{
	//Iterating over the Child Scene Nodes
	Ogre::SceneNode::ChildNodeIterator cni = sn->getChildIterator();
	while (cni.hasMoreElements())
	{
		Ogre::SceneNode* cn = static_cast<Ogre::SceneNode*>(cni.getNext());
		set_not_visible(cn);
	}	
	//Iterating over the Child Objects
	Ogre::SceneNode::ObjectIterator coi = sn->getAttachedObjectIterator();
	while (coi.hasMoreElements())
	{
		Ogre::Entity* co = static_cast<Ogre::Entity*>(coi.getNext());
		if (co->isVisible())
		{
			co->setVisible(false);
			entityList[entityCount] = co;
			entityCount++;
		}
	}
}

void ElbeBunch::setTreeVisible(bool value)
{
	if (value == false)
	{
		entityCount = 0;
		set_not_visible(mSceneMgr->getRootSceneNode());
	}
	else
	{
		int i;
		for (i = 0; i < entityCount; i++)
			entityList[i]->setVisible(true);
	}
}

void ElbeBunch::preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
	setTreeVisible(false);
	m_pBunchEntity->setVisible(false);
	m_pBunchEntity_background->setVisible(true);
}
 
void ElbeBunch::postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt)
{
	setTreeVisible(true);
	m_pBunchEntity->setVisible(true);
	m_pBunchEntity_background->setVisible(false);
}
