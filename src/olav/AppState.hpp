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

#ifndef APP_STATE_HPP
#define APP_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFramework.hpp"
#include "AbstractCamera.hpp"
#include "globale_defines.h"

class AppState;

enum QueryFlags
{
	OGRE_HEAD_MASK	= 1<<0,
    CUBE_MASK		= 1<<1
};

//|||||||||||||||||||||||||||||||||||||||||||||||


class AppStateListener
{
public:
	AppStateListener(){};
	virtual ~AppStateListener(){};

	virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;

	virtual AppState*	findByName(Ogre::String stateName) = 0;
	virtual void		changeAppState(AppState *state) = 0;
	virtual bool		pushAppState(AppState* state) = 0;
	virtual void		popAppState() = 0;
	virtual void		pauseAppState(AppState* state) = 0;
	virtual void		shutdown() = 0;
	virtual void        popAllAndPushAppState(AppState* state) = 0;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

//!This is the base class for all AppStates in LMC. So this is not used when Radiation3D runs as stand-alone application.
class AppState : public OIS::KeyListener, public OIS::MouseListener, public OgreBites::SdkTrayListener
{
public:

	static void create(AppStateListener* parent, const Ogre::String name){}; //!<Creates a state

	void destroy(){delete this;}

	virtual void enter() = 0;//!<Called when a scene is entered. Should call the createscene()-functions of the corresponding state.
	virtual void exit() = 0;//!<Called when a state is left. Deletes the cameras and other scene content.
	virtual bool pause(){return true;}//!<Pauses the state
	virtual void resume(){};//!<Resumes the state
	virtual void update(double timeSinceLastFrame) = 0;//!<Updates the scene
	bool flag;//!<Use this for whatever you want. In the RadiationState it is used for enabling GL_LINE_SMOOTH.

protected:
	AppState(){flag=true;};//!<Constructor

	AppState*	findByName(Ogre::String stateName){return m_pParent->findByName(stateName);}//!<Finds a state by its name
	void		changeAppState(AppState* state){m_pParent->changeAppState(state);}//!<Changes the Appstate
	bool		pushAppState(AppState* state){return m_pParent->pushAppState(state);}//!<Pushes an AppState on the stack
	void		popAppState(){m_pParent->popAppState();}//!<Pops an AppState from the stack
	void		shutdown(){m_pParent->shutdown();}//!<Closes the whole Program
	void        popAllAndPushAppState(AppState* state){m_pParent->popAllAndPushAppState(state);}//!<Empties the stack and then puts the given state on it

	//!Assign a camera
	void setCamera(AbstractCamera* camera)
	{
		m_pSceneMgr->destroyCamera(m_pCamera->getOgreCamera());
		m_pCamera = camera;
		OgreFramework::getSingletonPtr()->m_pViewport[0]->setCamera(m_pCamera->getOgreCamera());
		if (OgreFramework::getSingletonPtr()->m_bStereo)
		{
			OgreFramework::getSingletonPtr()->m_pViewport[1]->setCamera(m_pCamera->getOgreCamera());
			OgreFramework::getSingletonPtr()->m_stereo.shutdown();
			OgreFramework::getSingletonPtr()->m_stereo.init(OgreFramework::getSingletonPtr()->m_pViewport[0], OgreFramework::getSingletonPtr()->m_pViewport[1], OgreFramework::getSingletonPtr()->m_sm);
		}
	}
	//!Assign a camera without destroying the currently assigned camera
	void setCameraWithOutDestroy(AbstractCamera* camera)
	{
		m_pCamera = camera;
		OgreFramework::getSingletonPtr()->m_pViewport[0]->setCamera(m_pCamera->getOgreCamera());
		if (OgreFramework::getSingletonPtr()->m_bStereo)
		{
			OgreFramework::getSingletonPtr()->m_pViewport[1]->setCamera(m_pCamera->getOgreCamera());
			OgreFramework::getSingletonPtr()->m_stereo.shutdown();
			OgreFramework::getSingletonPtr()->m_stereo.init(OgreFramework::getSingletonPtr()->m_pViewport[0], OgreFramework::getSingletonPtr()->m_pViewport[1], OgreFramework::getSingletonPtr()->m_sm);
		}
	}

	void showHelp(Ogre::String name)
	{
		if (OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("help"))
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
			if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Hilfe (F1)");
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Help (F1)");
		}
		else
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_TOPLEFT);
			if (OgreBites::TL_LEFT == HELP_POSITION)
			{
				std::string help = OgreFramework::getSingletonPtr()->getHelp(name);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
				if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
				{
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(HELP_POSITION, "help", "Hilfe", INFOBOX_WIDTH,OgreFramework::getSingletonPtr()->m_pRenderWnd->getHeight()-48)->appendText(help);
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(HELP_POSITION, "helpOkay",OgreFramework::getSingletonPtr()->ConvertToUTF("Schließen"));
				}
				else
				{
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(HELP_POSITION, "help", "Help", INFOBOX_WIDTH,OgreFramework::getSingletonPtr()->m_pRenderWnd->getHeight()-48)->appendText(help);
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(HELP_POSITION, "helpOkay","Close");
				}
			}
			else
			{
				std::string help = OgreFramework::getSingletonPtr()->getHelp(name);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
				if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
				{
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(HELP_POSITION, "help", "Hilfe", 600, 500)->appendText(help);
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(HELP_POSITION, "helpOkay",OgreFramework::getSingletonPtr()->ConvertToUTF("Schließen"));				
				}
				else
				{
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(HELP_POSITION, "help", "Help", 600, 500)->appendText(help);
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(HELP_POSITION, "helpOkay","Close");				
				}
			}
		}
	}

	AppStateListener*			m_pParent;//!<A listener to manage updates of the AppStates

	AbstractCamera*				m_pCamera;//!<A camera
	Ogre::SceneManager*			m_pSceneMgr;//!<A scene manager
	Ogre::FrameEvent            m_FrameEvent;//!<A frame event
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#define DECLARE_APPSTATE_CLASS(T)										\
	static void create(AppStateListener* parent, const Ogre::String name)	\
{																		\
	T* myAppState = new T();											\
	myAppState->m_pParent = parent;										\
	parent->manageAppState(name, myAppState);							\
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
