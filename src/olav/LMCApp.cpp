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

//|||||||||||||||||||||||||||||||||||||||||||||||

#ifdef WIN32
	#include <windows.h>
#endif
//#include <gl/gl.h>

#include "LMCApp.hpp"

#ifndef ELBE_ONLY
	#include "MenuState.hpp"
	#include "DracoState.hpp"
	#include "Oger.h"
#endif
#include "ElbeState.hpp"
#include "PauseState.hpp"
#ifndef RELEASE
	#include "CollisionState.hpp"
#endif

AppStateManager*	LMCApp::m_pAppStateManager = NULL;

//|||||||||||||||||||||||||||||||||||||||||||||||

LMCApp::LMCApp()
{
	m_pAppStateManager = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

LMCApp::~LMCApp()
{
	delete m_pAppStateManager;
    delete OgreFramework::getSingletonPtr();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void LMCApp::startDemo(int enter_state,bool show_settings,bool stereo,Ogre::StereoManager::StereoMode mode,bool mouse_emulation,float eye_spacing,int language,bool simple, int demo,int change)
{	
	new OgreFramework();
	if(!(OgreFramework::getSingletonPtr()->initOgre("OLAV", 0, 0,show_settings,stereo,mode,mouse_emulation,language,demo,change)))
		return;
	OgreFramework::getSingletonPtr()->m_stereo.setEyesSpacing(eye_spacing);
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("LMC initialized!");
	OgreFramework::getSingletonPtr()->explanationMode = simple?standard:expert;

	m_pAppStateManager = new AppStateManager();

#ifndef ELBE_ONLY
	MenuState::create(m_pAppStateManager, "MenuState");
	DracoState::create(m_pAppStateManager, "DracoState");
	Oger::create(m_pAppStateManager,"Oger");
#endif
	ElbeState::create(m_pAppStateManager, "ElbeState");
#ifndef RELEASE
	CollisionState::create(m_pAppStateManager, "CollisionState");
#endif
    PauseState::create(m_pAppStateManager, "PauseState");
	switch (enter_state)
	{
#ifndef RELEASE
		case 1: 
			m_pAppStateManager->start(m_pAppStateManager->findByName("CollisionState"));
			break;
#endif
		case 2: 
			m_pAppStateManager->start(m_pAppStateManager->findByName("DracoState"));
			break;
		case 3: 
			m_pAppStateManager->start(m_pAppStateManager->findByName("ElbeState"));
			break;
		case 4:
			m_pAppStateManager->start(m_pAppStateManager->findByName("Oger"));
			break;
		default:
			m_pAppStateManager->start(m_pAppStateManager->findByName("MenuState"));
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||
