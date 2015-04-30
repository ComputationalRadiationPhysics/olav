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

#include "OgreFramework.hpp"
#include "tinyxml.hpp"
#include "globale_defines.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "resource.h"
#endif
#include "LMCApp.hpp"
#include <GL/gl.h>
#include "CmakeDefines.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||
#if (OGRE_VERSION_MAJOR == 1) && (OGRE_VERSION_MINOR < 8)
	template<> OgreFramework* Ogre::Singleton<OgreFramework>::ms_Singleton = 0;
#else
	template<> OgreFramework* Ogre::Singleton<OgreFramework>::msSingleton = 0;
#endif

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFramework::OgreFramework()
{
	m_pRoot				= 0;
	m_pRenderWnd		= 0;
	m_pViewport[0]		= 0;
	m_pViewport[1]		= 0;
	m_pLog				= 0;
	m_pTimer			= 0;

	m_pTrayMgr			= 0;
	m_pInputMgr			= 0;
	m_pKeyboard			= 0;
	m_pMouse			= 0;
	m_bMouseEmulation 	= false;
	explanationMode		= expert;
	m_bDemo				= false;
	m_bDemoChange		= false;
	m_bDontFadeIn		= false;

	#ifdef WIN32
	char buffer[1024];
	GetEnvironmentVariable("APPDATA",buffer,1024);
	appdata = buffer;
	appdata += "\\olav\\";
	CreateDirectory(appdata.c_str(),NULL);
	#else
	char *buffer = getenv("HOME");
	appdata = buffer;
	appdata += "/.config/olav/";
	#endif

	Ogre::LogManager* logMgr = new Ogre::LogManager();

	m_pLog = Ogre::LogManager::getSingleton().createLog(appdata+"OgreLogfile.log", true, true, false);
	m_pLog->setDebugOutputEnabled(false);
	//Okay, lets' open the ogre.cfg and let's search for Fixed...
	FILE* f = fopen((appdata+"ogre.cfg").c_str(),"r+");
	if (f)
	{
		int f_pos = 0;
		char buffer[512];
		int i = 0;
		while (fread(&(buffer[i]),1,1,f))
		{
			if (buffer[i] == '\n' || (feof(f) && i++))
			{
				buffer[i] = 0;
				if (strstr(buffer,"Fixed Pipeline Enabled") == buffer)
				{
					fseek(f,f_pos,SEEK_SET);
					char c = '#';
					fwrite(&c,1,1,f);
				}
				f_pos += i+1;
				fseek(f,f_pos,SEEK_SET);
				i = 0;
			}
			else
				i++;
		}
		fclose(f);
	}
	
	m_pRoot = new Ogre::Root( PLUGIN_CONFIG, appdata+"ogre.cfg",appdata+"Ogre.log");
	m_pRoot->restoreConfig();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

OgreFramework::~OgreFramework()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
	if(m_pTrayMgr)	  delete m_pTrayMgr;
	if(m_pInputMgr)		OIS::InputManager::destroyInputSystem(m_pInputMgr);
}

bool OgreFramework::initStereoViewports(bool force)
{
	int w = m_pViewport[1]->getActualWidth();
	int h = m_pViewport[1]->getActualHeight();
	if(m_sm == Ogre::StereoManager::SM_DUALOUTPUT) {
		if(m_vSideBySide) {
			if((w == 0) || (w < h) || force) {
				m_pViewport[0]->setDimensions( 0,   0, 1, 0.5);
				m_pViewport[1]->setDimensions(0, 0.5, 1, 0.5);
			}
		} else {
			if((w > h) || (w == 0) || force) {
				m_pViewport[0]->setDimensions(   0, 0, 0.5, 1);
				m_pViewport[1]->setDimensions(0.5, 0, 0.5, 1);
			}
		}
	} else {
		if(w != 0 || h != 0 || force) {
			m_pViewport[0]->setDimensions(0, 0, 1, 1);
			m_pViewport[1]->setDimensions(1, 1, 1, 1);
		}
	}
	return true;
}
//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener,bool show_settings,bool stereo,Ogre::StereoManager::StereoMode mode,bool mouse_emulation,int language, int demo, int change)
{	
	m_bMouseEmulation = mouse_emulation;
	m_fEmulatedMouseRelX = 0.0f;
	m_fEmulatedMouseRelY = 0.0f;
	m_fEmulatedMouseRelZ = 0.0f;
	m_sm = mode;
	m_vSideBySide = false;
	m_bStereo = stereo;
	m_iLanguage = language;
	m_bDemo = (demo == 1);
	m_bDemoChange = (change == 1);
	if(show_settings && !(m_pRoot->showConfigDialog()))
		return false;
	if (m_pRoot->getRenderSystem()->getName().compare("OpenGL Rendering Subsystem") == 0)
		m_pRoot->getRenderSystem()->setConfigOption("RTT Preferred Mode", "PBuffer");
	m_pRenderWnd = m_pRoot->initialise(true, wndTitle);
	glGetIntegerv(0x821B,&m_iMajor); //MAJOR
	glGetIntegerv(0x821C,&m_iMinor); //MINOR
	printf("OpenGL Version: %i.%i\n",m_iMajor,m_iMinor);
	/*if (m_iMajor >= 3)
	{
		delete m_pRoot;
		m_pRoot = new Ogre::Root(
		#ifdef WIN32
				"plugins.win32",
		#elif defined __amd64__
				"plugins.amd64",
		#else
				"plugins.i386",
		#endif
			appdata+"ogre.cfg",appdata+"Ogre.log");
		m_pRoot->restoreConfig();
		m_pRoot->getRenderSystem()->setConfigOption("RTT Preferred Mode", "FBO"); //I need the FBO to do... things!
		m_pRenderWnd = m_pRoot->initialise(true, wndTitle);
	}*/

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	  HWND hwnd;
	  m_pRenderWnd->getCustomAttribute("WINDOW", (void*)&hwnd);
	  LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1) );
	  SetClassLong( hwnd, GCL_HICON, iconID );
#endif
	
	//Adding Viewport(s)
	if (stereo)
	{
		m_pLog->logMessage("Using Stereo");
		m_pViewport[0] = m_pRenderWnd->addViewport(0);
		m_pViewport[0]->setBackgroundColour(ColourValue(0.5f, 0.5f, 0.5f, 1.0f));
		m_pViewport[0]->setDimensions(   0, 0, 0.5, 1);

		m_pViewport[1] = m_pRenderWnd->addViewport(0,1);
		m_pViewport[1]->setBackgroundColour(ColourValue(0.5f, 0.5f, 0.5f, 1.0f));
		m_pViewport[1]->setDimensions(0.5, 0, 0.5, 1);
		initStereoViewports(true);
	}
	else
	{
		m_pViewport[0] = m_pRenderWnd->addViewport(0);
		m_pViewport[0]->setBackgroundColour(ColourValue(0.5f, 0.5f, 0.5f, 1.0f));

		m_pViewport[0]->setCamera(0);
	}

	size_t hWnd = 0;
	OIS::ParamList paramList;
	m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);

	paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
	m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

	m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
	m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();

	if(pKeyListener == 0)
		m_pKeyboard->setEventCallback(this);
	else
		m_pKeyboard->setEventCallback(pKeyListener);

	if(pMouseListener == 0)
		m_pMouse->setEventCallback(this);
	else
		m_pMouse->setEventCallback(pMouseListener);

	Ogre::String secName, typeName, archName;
	Ogre::ConfigFile cf;
	cf.load(RESOURCES_CONFIG);
	
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	  
	m_pTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", m_pRenderWnd, m_pMouse, 0);

	m_pTimer = new Ogre::Timer();
	m_pTimer->reset();

	m_pRenderWnd->setActive(true);

	//parsing the help
	#ifdef WIN32
	TiXmlDocument file;
	if(!file.LoadFile("media\\Documentation\\general_help.xml",TIXML_ENCODING_UTF8))
	{
		OgreFramework::getSingletonPtr()->m_pLog->logMessage("Did not find helpfile in first try");
		if(!file.LoadFile(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation\\general_help.xml",TIXML_ENCODING_UTF8))
		{
			OgreFramework::getSingletonPtr()->m_pLog->logMessage("Did not find helpfile in second try");
			if(!file.LoadFile("..\\..\\media\\Documentation\\general_help.xml",TIXML_ENCODING_UTF8))
		{
			OgreFramework::getSingletonPtr()->m_pLog->logMessage("Did not find helpfile in third try");
			throw 1;
		}
		}
	}
	#else
	TiXmlDocument file;
	if(!file.LoadFile(RESOURCES_DIR"/media/Documentation/general_help.xml",TIXML_ENCODING_UTF8))
	{
		OgreFramework::getSingletonPtr()->m_pLog->logMessage("Did not find helpfile in first try");
		if(!file.LoadFile(DEBUG_MEDIA_FOLDER "media/Documentation/general_help.xml",TIXML_ENCODING_UTF8))
		{
			OgreFramework::getSingletonPtr()->m_pLog->logMessage("Did not find helpfile in second try");
			throw 1;
		}
	}
	#endif

	TiXmlElement * help = file.FirstChildElement()->FirstChildElement();
	while(help)
	{
		std::string name = help->FirstChildElement("name")->GetText();
		std::string text = help->FirstChildElement("text")->GetText();
		m_helps.insert(std::pair<std::string,std::string>(name,text));
		help = help->NextSiblingElement();
	}

	m_pLog->logMessage("Setting some dirty fixes because of known bad cards");
	Ogre::RenderSystemCapabilities *cap = m_pRoot->getAvailableRenderers().at(0)->createRenderSystemCapabilities();
	if (cap!=NULL){
	if (cap->getDeviceName().find("Quadro FX 540") != cap->getDeviceName().npos)
	{
		m_pLog->logMessage("Found "+cap->getDeviceName()+", will run in circles and cry");
		m_noshadow = true;
	}
	else
		m_noshadow = false;
	m_performance = cap->getDriverVersion().major;
	}else m_performance=4;
	m_stereo.shutdown();
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFramework::updateEmulatedMousePosition(float timeSinceLastFrame)
{
	if (m_bMouseEmulation)
	{
		OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(OgreFramework::getSingletonPtr()->m_pMouse->getMouseState());
		mutableMouseState.X.abs += OgreFramework::getSingletonPtr()->m_fEmulatedMouseRelX*timeSinceLastFrame;
		mutableMouseState.Y.abs += OgreFramework::getSingletonPtr()->m_fEmulatedMouseRelY*timeSinceLastFrame;
		mutableMouseState.Z.abs += OgreFramework::getSingletonPtr()->m_fEmulatedMouseRelZ*timeSinceLastFrame;
		OgreFramework::getSingletonPtr()->m_pTrayMgr->getCursorContainer()->setPosition(mutableMouseState.X.abs,mutableMouseState.Y.abs);
	}
}

bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(m_pKeyboard->isKeyDown(OIS::KC_P))
		m_pRenderWnd->writeContentsToTimestampedFile("OLAV_Screenshot_",".png");
	#ifndef WIN32
	if(m_pKeyboard->isModifierDown(OIS::Keyboard::Alt) && m_pKeyboard->isKeyDown(OIS::KC_F4))
	{
		LMCApp::m_pAppStateManager->shutdown();
	}
	#endif
	if(m_pKeyboard->isModifierDown(OIS::Keyboard::Ctrl) && m_pKeyboard->isKeyDown(OIS::KC_F))
	{
		if (OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget(OgreFramework::getSingletonPtr()->m_pTrayMgr->mName+"/FpsLabel"))
			OgreFramework::getSingletonPtr()->m_pTrayMgr->hideFrameStats();
		else
			OgreFramework::getSingletonPtr()->m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	}
	if(m_pKeyboard->isModifierDown(OIS::Keyboard::Ctrl) && m_pKeyboard->isKeyDown(OIS::KC_P))
	{
		OgreBites::ParamsPanel* panel;
		if (panel = (OgreBites::ParamsPanel*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget(OgreFramework::getSingletonPtr()->m_pTrayMgr->mName+"/PerformanceLabel"))
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyWidget(panel);
		}
		else
		{
			Ogre::StringVector my_vector;
			my_vector.push_back("Keyboard Capture:");
			my_vector.push_back("Mouse Capture:");
			my_vector.push_back("Private Update:");
			my_vector.push_back("Ogre Update:");
			my_vector.push_back("Render:");
			panel = OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_BOTTOMLEFT,OgreFramework::getSingletonPtr()->m_pTrayMgr->mName+"/PerformanceLabel",200,my_vector);
			panel->setParamValue(0,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().keyboard_capture_time) + " ms");
			panel->setParamValue(1,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().mouse_capture_time) + " ms");
			panel->setParamValue(2,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().private_update_time) + " ms");
			panel->setParamValue(3,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().ogre_update_time) + " ms");
			panel->setParamValue(4,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().render_time) + " ms");
		}
	}
	if (m_bStereo)
	{
		if(m_pKeyboard->isKeyDown(OIS::KC_L))
		{
			m_stereo.setEyesSpacing(m_stereo.getEyesSpacing()+0.01);
			OgreFramework::getSingletonPtr()->m_pLog->logMessage("Set eye spacing to "+Ogre::StringConverter::toString(m_stereo.getEyesSpacing()));
		}
		if(m_pKeyboard->isKeyDown(OIS::KC_K))
		{
			m_stereo.setEyesSpacing(m_stereo.getEyesSpacing()-0.01);
			OgreFramework::getSingletonPtr()->m_pLog->logMessage("Set eye spacing to "+Ogre::StringConverter::toString(m_stereo.getEyesSpacing()));
		}
	}

	if (m_bMouseEmulation)
	{
		OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(OgreFramework::getSingletonPtr()->m_pMouse->getMouseState());
		OIS::MouseEvent evt(m_pMouse,mutableMouseState);
		switch (keyEventRef.key)
		{
			case OIS::KC_NUMPAD4: m_fEmulatedMouseRelX = -0.2f; break;
			case OIS::KC_NUMPAD6: m_fEmulatedMouseRelX =  0.2f; break;
			case OIS::KC_NUMPAD8: m_fEmulatedMouseRelY = -0.2f; break;
			case OIS::KC_NUMPAD2: m_fEmulatedMouseRelY =  0.2f; break;
			case OIS::KC_NUMPAD7: m_fEmulatedMouseRelZ = -0.2f; break;
			case OIS::KC_NUMPAD1: m_fEmulatedMouseRelZ =  0.2f; break;
			case OIS::KC_NUMPAD5:
				evt.state.buttonDown(OIS::MB_Left);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt,OIS::MB_Left);
				break;
			case OIS::KC_NUMPAD3:
			case OIS::KC_NUMPAD9:
				evt.state.buttonDown(OIS::MB_Right);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt,OIS::MB_Right);
				break;
		}
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
	{
		m_pRenderWnd->writeContentsToTimestampedFile("OLAV_Screenshot_", ".bmp");
		return true;
	}

	if(m_pKeyboard->isKeyDown(OIS::KC_O))
	{
		if(m_pTrayMgr->isLogoVisible())
		{
			m_pTrayMgr->hideFrameStats();
			m_pTrayMgr->hideLogo();
		}
		else
		{
			m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
			m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
		}
	}

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	if (m_bMouseEmulation)
	{
		OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(OgreFramework::getSingletonPtr()->m_pMouse->getMouseState());
		OIS::MouseEvent evt(m_pMouse,mutableMouseState);
		switch (keyEventRef.key)
		{
			case OIS::KC_NUMPAD4:
			case OIS::KC_NUMPAD6: m_fEmulatedMouseRelX = 0.0f; break;
			case OIS::KC_NUMPAD8:
			case OIS::KC_NUMPAD2: m_fEmulatedMouseRelY = 0.0f; break;
			case OIS::KC_NUMPAD7:
			case OIS::KC_NUMPAD1: m_fEmulatedMouseRelZ = 0.0f; break;
			case OIS::KC_NUMPAD5:
				evt.state.buttonDown(OIS::MB_Left);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt,OIS::MB_Left);
				break;
			case OIS::KC_NUMPAD3:
			case OIS::KC_NUMPAD9:
				evt.state.buttonDown(OIS::MB_Right);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt,OIS::MB_Right);
				break;
		}
	}
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
	
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void OgreFramework::updateOgre(double timeSinceLastFrame)
{
		OgreBites::ParamsPanel* panel;
		if (panel = (OgreBites::ParamsPanel*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget(OgreFramework::getSingletonPtr()->m_pTrayMgr->mName+"/PerformanceLabel"))
		{
			panel->setParamValue(0,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().keyboard_capture_time) + " µs");
			panel->setParamValue(1,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().mouse_capture_time) + " µs");
			panel->setParamValue(2,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().private_update_time) + " µs");
			panel->setParamValue(3,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().ogre_update_time) + " µs");
			panel->setParamValue(4,Ogre::StringConverter::toString(LMCApp::m_pAppStateManager->getPerformance().render_time) + " µs");
		}	
}

//|||||||||||||||||||||||||||||||||||||||||||||||

Ogre::UTFString OgreFramework::ConvertToUTF(Ogre::String String)
{
   Ogre::UTFString UTFString;
   int i;
   Ogre::UTFString::code_point cp;
   for (i=0; i<(int)String.size(); ++i)
   {
	  cp = String[i];
	  cp &= 0xFF;
	  UTFString.append(1, cp);
   }
   return UTFString;
}

std::string OgreFramework::getHelp(std::string name)
{
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
		return m_helps.find(name + "_d")->second;
	return m_helps.find(name)->second;
}
