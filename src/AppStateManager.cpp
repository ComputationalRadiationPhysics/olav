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

#include "AppStateManager.hpp"

#include <OgreWindowEventUtilities.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

AppStateManager::AppStateManager()
{
	//this logo we need in every state
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("HZDR_logo", "General");
	material->getTechnique(0)->getPass(0)->createTextureUnitState("hzdr_small.png");
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
	Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(overlayManager.createOverlayElement("Panel", "HZDR_logo", true));
	panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setDimensions(100,20);
	panel->setMaterialName("HZDR_logo");
	m_bShutdown = false;
	performance.keyboard_capture_time = 0;
	performance.mouse_capture_time = 0;
	performance.private_update_time = 0;
	performance.ogre_update_time = 0;
	performance.render_time = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

AppStateManager::~AppStateManager()
{
	state_info si;

    while(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	while(!m_States.empty())
	{
		si = m_States.back();
        si.state->destroy();
        m_States.pop_back();
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
	try
	{
		state_info new_state_info;
		new_state_info.name = stateName;
		new_state_info.state = state;
		m_States.push_back(new_state_info);
	}
	catch(std::exception& e)
	{
		delete state;
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new AppState\n" + Ogre::String(e.what()), "AppStateManager.cpp (39)");
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

AppState* AppStateManager::findByName(Ogre::String stateName)
{
	std::vector<state_info>::iterator itr;

	for(itr=m_States.begin();itr!=m_States.end();itr++)
	{
		if(itr->name==stateName)
			return itr->state;
	}

	return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::start(AppState* state)
{
	changeAppState(state);

	int timeSinceLastFrame = 1;
	int startTime = 0;

	while(!m_bShutdown)
	{
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isClosed()) m_bShutdown = true;

		Ogre::WindowEventUtilities::messagePump();

		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isActive())
		{
			startTime = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU();
			
			unsigned long t1 = OgreFramework::getSingletonPtr()->m_pTimer->getMicroseconds();
			OgreFramework::getSingletonPtr()->m_pKeyboard->capture();
			unsigned long t2 = OgreFramework::getSingletonPtr()->m_pTimer->getMicroseconds();
			performance.keyboard_capture_time = t2 - t1;
			
			t1 = t2;
			OgreFramework::getSingletonPtr()->m_pMouse->capture();
			t2 = OgreFramework::getSingletonPtr()->m_pTimer->getMicroseconds();
			performance.mouse_capture_time = t2 - t1;
			
			t1 = t2;
			m_ActiveStateStack.back()->update(timeSinceLastFrame);
			t2 = OgreFramework::getSingletonPtr()->m_pTimer->getMicroseconds();
			performance.private_update_time = t2 - t1;

			t1 = t2;
			OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			t2 = OgreFramework::getSingletonPtr()->m_pTimer->getMicroseconds();
			performance.ogre_update_time = t2 - t1;

			t1 = t2;
			OgreFramework::getSingletonPtr()->m_pRoot->renderOneFrame();
			t2 = OgreFramework::getSingletonPtr()->m_pTimer->getMicroseconds();
			performance.render_time = t2 - t1;

			timeSinceLastFrame = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
		}
		else
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
		}
	}
	// End MainLoop //

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::changeAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	m_ActiveStateStack.push_back(state);
	init(state);
	m_ActiveStateStack.back()->enter();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool AppStateManager::pushAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty())
	{
		if(!m_ActiveStateStack.back()->pause())
			return false;
	}

	m_ActiveStateStack.push_back(state);
	init(state);
	m_ActiveStateStack.back()->enter();

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::popAppState(void)
{
	if(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	if(!m_ActiveStateStack.empty())
	{
		init(m_ActiveStateStack.back());
		m_ActiveStateStack.back()->resume();
	}
    else
		shutdown();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::pauseAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty())
	{
		m_ActiveStateStack.back()->pause();
	}

	if(m_ActiveStateStack.size() > 2)
	{
		init(m_ActiveStateStack.back());
		m_ActiveStateStack.back()->resume();
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::shutdown()
{
	m_bShutdown = true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::init(AppState* state)
{
    OgreFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(state);
	OgreFramework::getSingletonPtr()->m_pMouse->setEventCallback(state);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(state);

	OgreFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();
	OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void AppStateManager::popAllAndPushAppState(AppState* state)
{
    while(!m_ActiveStateStack.empty())
    {
        m_ActiveStateStack.back()->exit();
        m_ActiveStateStack.pop_back();
    }

    pushAppState(state);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

AppStateManagerPerformance AppStateManager::getPerformance()
{
	return performance;
}
