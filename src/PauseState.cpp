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

#include "PauseState.hpp"
#include "FreeViewCamera.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

bool PauseState::closeInstant = false;

//|||||||||||||||||||||||||||||||||||||||||||||||

PauseState::PauseState()
{
    m_bQuit         = false;
    m_FrameEvent    = Ogre::FrameEvent();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PauseState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering PauseState...");

    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "PauseSceneMgr");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	m_pCamera = new FreeViewCamera(m_pSceneMgr->createCamera("PauseCamera"));
	m_pCamera->getOgreCamera()->setPosition(Vector3(0, 25, -50));
    m_pCamera->getOgreCamera()->lookAt(Vector3(0, 0, 0));
    m_pCamera->getOgreCamera()->setNearClipDistance(1);

    m_pCamera->getOgreCamera()->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport[0]->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->m_pViewport[0]->getActualHeight()));

    OgreFramework::getSingletonPtr()->m_pViewport[0]->setCamera(m_pCamera->getOgreCamera());
    if (OgreFramework::getSingletonPtr()->m_bStereo)
	{
		OgreFramework::getSingletonPtr()->m_pViewport[1]->setCamera(m_pCamera->getOgreCamera());
		OgreFramework::getSingletonPtr()->m_stereo.shutdown();
		OgreFramework::getSingletonPtr()->m_stereo.init(OgreFramework::getSingletonPtr()->m_pViewport[0], OgreFramework::getSingletonPtr()->m_pViewport[1], OgreFramework::getSingletonPtr()->m_sm);
	}

    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();
    #ifndef ELBE_ONLY
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackToGameBtn", "Return to State", 250);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackToMenuBtn", "Return to Menu", 250);
    #else
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackToGameBtn", "Return to OLAV", 250);
    #endif
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "ExitBtn", "Exit OLAV", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "PauseLbl", "Pause mode", 250);

    m_bQuit = false;

    createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PauseState::createScene()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PauseState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving PauseState...");

    m_pSceneMgr->destroyCamera(m_pCamera->getOgreCamera());
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(0);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PauseState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PauseState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PauseState::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PauseState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool PauseState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PauseState::update(double timeSinceLastFrame)
{
	OgreFramework::getSingletonPtr()->updateEmulatedMousePosition(timeSinceLastFrame);
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        popAppState();
        return;
    }
	if (closeInstant)
        m_bQuit = true;
	closeInstant = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PauseState::buttonHit(OgreBites::Button *button)
{
    if(button->getName() == "ExitBtn")
        OgreFramework::getSingletonPtr()->m_pTrayMgr->showYesNoDialog("Sure?", "Really leave?");
    else if(button->getName() == "BackToGameBtn")
        m_bQuit = true;
    else if(button->getName() == "BackToMenuBtn")
        popAllAndPushAppState(findByName("MenuState"));
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void PauseState::yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit)
{
    if(yesHit == true)
        shutdown();
    else
        OgreFramework::getSingletonPtr()->m_pTrayMgr->closeDialog();
}

//|||||||||||||||||||||||||||||||||||||||||||||||
