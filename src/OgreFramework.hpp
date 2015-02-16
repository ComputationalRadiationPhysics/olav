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

#ifndef OGRE_FRAMEWORK_HPP
#define OGRE_FRAMEWORK_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

//#define BELEG_DEMO

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "SdkTrays.h"
#include "StereoManager.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

typedef enum {standard = 0,expert = 1} explanationModeType;

class OgreFramework : public Ogre::Singleton<OgreFramework>, OIS::KeyListener, OIS::MouseListener
{
public:
	OgreFramework();
	~OgreFramework();

	bool initStereoViewports(bool force);
	bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0,bool show_settings = true,bool stereo = false,Ogre::StereoManager::StereoMode mode = Ogre::StereoManager::SM_DUALOUTPUT,bool mouse_emulation = false,int language = 0, int demo = 0, int change = 0);
	void updateOgre(double timeSinceLastFrame);

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	Ogre::Root*					m_pRoot;
	Ogre::RenderWindow*			m_pRenderWnd;
	Ogre::Viewport*				m_pViewport[2];
	bool						m_bStereo;
	Ogre::Log*					m_pLog;
	Ogre::Timer*				m_pTimer;

	OIS::InputManager*			m_pInputMgr;
	OIS::Keyboard*				m_pKeyboard;
	OIS::Mouse*					m_pMouse;
	int							m_radialBlur;
	int							m_performance;
	bool						m_noshadow;
    Ogre::StereoManager::StereoMode m_sm;     ///< Stereoscopic mode
    bool                   		m_vSideBySide;    ///< For side by side (dual output), is the view vertical ?
    bool						m_bMouseEmulation;
	float						m_fEmulatedMouseRelX;
	float						m_fEmulatedMouseRelY;
	float						m_fEmulatedMouseRelZ; //Wheel
	int							m_iLanguage;
	bool						m_bDontFadeIn;
	bool						m_bDemo;
	bool						m_bDemoChange;
	int 						m_iMajor;
	int 						m_iMinor;
	explanationModeType explanationMode;
    OgreBites::SdkTrayManager*	m_pTrayMgr;

	void setSpeed(int s) {m_radialBlur=s;};
	int	 getSpeed(){return m_radialBlur;};
	Ogre::UTFString ConvertToUTF(Ogre::String String);

	std::string getHelp(std::string name);

	std::string appdata;

    // Stereoscopie
    Ogre::StereoManager     m_stereo;         ///< Handle stereoscopie
	void updateEmulatedMousePosition(float timeSinceLastFrame);

private:
	OgreFramework(const OgreFramework&);
	OgreFramework& operator= (const OgreFramework&);
	std::map<std::string, std::string> m_helps;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
