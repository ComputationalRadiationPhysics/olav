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


#include "ElbeState.hpp"
#include "FreeViewCamera.hpp"
#include "OrbitCamera.hpp"
#include "ChaseCamera.hpp"
#include "ElbePipe.hpp"
#include "globale_defines.h"
#include "PauseState.hpp"
#include "CmakeDefines.h"
#include <GL/gl.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

#define SLIDER_SLIDER_WIDTH 140
#define SLIDER_VALUE_WIDTH 70

//|||||||||||||||||||||||||||||||||||||||||||||||
#define ZOOM_LEVEL_1 3
#define ZOOM_LEVEL_2 6
#define ZOOM_LEVEL_3 20

#define SLOW_TIME 1600
//Use this formula for the speed: SLOW_SPEED = (0.98 - SLOW_TIME/2000) / (13 - SLOW_TIME/1000)
#define SLOW_SPEED 0.0157895

#define demoShowFirst(time,file) \
		doc = mDemoDoc->GetDocumentation(file); \
		caption = doc->GetTitle(); \
		text = doc->GetText(); \
		demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_2, time     ,caption,text,0.5f,true)); \
		demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, time+SLOW_TIME,caption,text,SLOW_SPEED,true)); \
		demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, time+13000-SLOW_TIME,caption,text,SLOW_SPEED,true)); \
		demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_2, time+13000,"","",0.5f,true));

#define demoShowAgain(time,file) \
		doc = mDemoDoc->GetDocumentation(file); \
		caption = doc->GetTitle(); \
		demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_2, time,caption,"",0.5f,true)); \
		demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_2, time+1500,"","",0.5f,true));

#define demoShowAgain2(time,file) \
		doc = mDemoDoc->GetDocumentation(file); \
		caption = doc->GetTitle(); \
		demoList.push_back(tElbeDemoType( 50.0f,-20.0f,ZOOM_LEVEL_3, time,caption,"",0.5f,false)); \
		demoList.push_back(tElbeDemoType( 50.0f,-20.0f,ZOOM_LEVEL_3, time+1500,"","",0.5f,false));

#define demoShowFirst3(time,file) \
		doc = mDemoDoc->GetDocumentation(file); \
		caption = doc->GetTitle(); \
		text = doc->GetText(); \
		demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, time     ,caption,text,0.5f,false)); \
		demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, time+1000,caption,text,0.1f,false)); \
		demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, time+12000,caption,text,0.1f,false)); \
		demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, time+13000,"","",0.5f,false));

#define demoShowAgain3(time,file) \
		doc = mDemoDoc->GetDocumentation(file); \
		caption = doc->GetTitle(); \
		demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, time,caption,"",0.5f,false)); \
		demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, time+1500,"","",0.5f,false));

void ElbeState::refillDemoList()
{
	FILE *test;
	if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
	#ifdef WIN32
		test = fopen(RESOURCES_DIR"\\media\\Documentation\\elbe_demo_german.xml", "r");
	else
		test = fopen(RESOURCES_DIR"\\media\\Documentation\\elbe_demo_english.xml", "r");
	#else
		test = fopen(RESOURCES_DIR"/media/Documentation/elbe_demo_german.xml", "r");
	else
		test = fopen(RESOURCES_DIR"/media/Documentation/elbe_demo_english.xml", "r");
	#endif
	if (test)
	{
		fclose(test);
		if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
		#ifdef WIN32
			mDemoDoc = new DocumentationManager(RESOURCES_DIR"\\media\\Documentation", "elbe_demo_german.xml");
		else
			mDemoDoc = new DocumentationManager(RESOURCES_DIR"\\media\\Documentation", "elbe_demo_english.xml");
		#else
			mDemoDoc = new DocumentationManager(RESOURCES_DIR"/media/Documentation", "elbe_demo_german.xml");
		else
			mDemoDoc = new DocumentationManager(RESOURCES_DIR"/media/Documentation", "elbe_demo_english.xml");
		#endif
	}
	else
	{
		if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
			test = fopen(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation\\elbe_demo_german.xml", "r");
		else
			test = fopen(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation\\elbe_demo_english.xml", "r");
		if (test)
		{
			fclose(test);
			if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
				mDemoDoc = new DocumentationManager(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation", "elbe_demo_german.xml");
			else
				mDemoDoc = new DocumentationManager(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation", "elbe_demo_english.xml");
		}
	}
	m_Pause = false;
	animationSpeed = 0.0f;
	demoTime = 0;
	demoPoint = 0;
	demoList.clear();
	std::string caption;
	std::string text;
	const Documentation * doc;
	demoLast = tElbeDemoType(65.0f,-20.0f,30,0,"black","",0.0f,true);
	demoList.push_back(tElbeDemoType( 65.0f,-20.0f,30, 2000,"black","",0.0f,true));
	demoList.push_back(tElbeDemoType( 65.0f,-20.0f,30, 4000,"internalOverlay","",0.0f,true));
	demoList.push_back(tElbeDemoType( 65.0f,-20.0f,30, 9000,"internalOverlay","",0.0f,true));
	demoList.push_back(tElbeDemoType( 65.0f,-20.0f,30, 10000,"","",0.0f,true));
	demoList.push_back(tElbeDemoType( 65.0f,-20.0f,30, 10000,"","",0.0f,true));
	demoList.push_back(tElbeDemoType( 65.0f,-20.0f,30, 15000,"","",0.0f,true));
	doc = mDemoDoc->GetDocumentation("gun");
	caption = doc->GetTitle();
	text = doc->GetText();
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f,10, 23000,caption,text,0.0f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 30000,caption,text,0.0f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 32000,caption,text,0.5f,true));
	doc = mDemoDoc->GetDocumentation("steerer");
	caption = doc->GetTitle();
	text = doc->GetText();
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 34000,caption,text,0.5f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 34700,caption,text,0.03f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 38000,caption,text,0.03f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 39000,"","",0.5f,true));
	doc = mDemoDoc->GetDocumentation("solenoid");
	caption = doc->GetTitle();
	text = doc->GetText();
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 41000      ,caption,text,0.5f,true)); \
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 41000+ 1000,caption,text,0.04f,true)); \
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1, 41000+12000,caption,text,0.04f,true)); \
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_2, 41000+13000,"","",0.5f,true));
	demoShowFirst( 56200,"buncher260");
	demoShowAgain( 72500,"steerer");
	demoShowAgain( 75000,"solenoid");
	demoShowFirst( 83300,"macropulser");
	demoShowAgain( 98500,"solenoid");
	demoShowAgain(100000,"steerer");
	demoShowAgain(107000,"solenoid_und_steerer");
	demoShowFirst(108500,"buncher1300");
	demoShowAgain(125000,"solenoid");
	demoShowAgain(126500,"steerer");
	doc = mDemoDoc->GetDocumentation("linac");
	caption = doc->GetTitle();
	text = doc->GetText();
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1,130000,caption,text,0.5f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1,133000,caption,text,0.05f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1,146000,caption,text,0.05f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_1,150000,caption,text,0.5f,true));
	demoList.push_back(tElbeDemoType(  0.0f,  0.0f, ZOOM_LEVEL_2,163000,"","",0.5f,true));
	int extra = 10651;
	demoShowFirst(extra+157500,"quadrupol");
	demoShowAgain(extra+170500,"steerer");
	demoShowAgain(extra+172000,"quadrupol");
	demoShowAgain(extra+173500,"quadrupol");
	demoList.push_back(tElbeDemoType( 25.0f,-10.0f,10.5,extra+181750,"","",0.5f,false));
	demoList.push_back(tElbeDemoType( 50.0f,-20.0f,ZOOM_LEVEL_3,extra+190000,"","",0.5f,false));
	demoShowAgain2(extra+190500,"quadrupol");
	demoShowAgain2(extra+192000,"quadrupol");
	demoShowAgain2(extra+194000,"steerer");
	doc = mDemoDoc->GetDocumentation("schikane");
	caption = doc->GetTitle();
	text = doc->GetText();
	demoList.push_back(tElbeDemoType( 50.0f,-20.0f,ZOOM_LEVEL_3,extra+198000,caption,text,0.5f,false));
	demoList.push_back(tElbeDemoType( 50.0f,-20.0f,ZOOM_LEVEL_3,extra+199000,caption,text,0.4f,false));
	demoList.push_back(tElbeDemoType( 50.0f,-20.0f,ZOOM_LEVEL_3,extra+219000,caption,text,0.4f,false));
	demoList.push_back(tElbeDemoType( 50.0f,-20.0f,ZOOM_LEVEL_3,extra+218000,"","",0.5f,false));
	demoShowAgain2(extra+222500,"steerer");
	demoShowAgain2(extra+225000,"quadrupol");
	demoShowAgain2(extra+226500,"quadrupol");
	demoList.push_back(tElbeDemoType( 90.0f,  0.0f, 1,extra+232000,"ELBE-LINAC","",0.5f,false));
	demoList.push_back(tElbeDemoType( 90.0f,  0.0f, 1,extra+253000,"","",0.5f,false));
	demoList.push_back(tElbeDemoType( 90.0f,  0.0f, 1,extra+255000,"Quadrupol-Magnet","",0.5f,false));
	demoList.push_back(tElbeDemoType( 90.0f,  0.0f, 1,extra+258000,"","",0.5f,false));
	demoList.push_back(tElbeDemoType( 90.0f,  0.0f, 1,extra+259000,"","",0.5f,false));
	demoShowAgain3(extra+265500,"quadrupol");
	demoShowFirst3(extra+269000,"dipol");
	demoShowAgain3(extra+286000,"quadrupol");
	demoShowAgain3(extra+292000,"dipol");
	demoShowAgain3(extra+296500,"quadrupol");
	demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, extra+300000,"","",0.5f,false));
	demoList.push_back(tElbeDemoType(-60.0f,-20.0f,18, extra+302000,"black","",0.0f,false));
}

ElbeState::ElbeState()
{
	followed_random_seed = rand();
}

float ElbeState::MIN_CAMERA_DISTANCE = 0.01f;
float ElbeState::MAX_CAMERA_DISTANCE = 50.0f;
float ElbeState::FIXED_CAMERA_HEIGHT = 14.0f;
int ElbeState::followed_random_seed = 0;

const float ElbeState::BUNCH_SPEED = 0.002f;

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering ElbeState...");
	track_count = 0;
	demoMode = false;
    m_MoveSpeed			= 0.03f;
    m_RotateSpeed		= 0.3f;

    m_bLMouseDown       = false;
    m_bRMouseDown       = false;
    m_bQuit             = false;
    m_bSettingsMode     = false;
	m_bHidden			= false;

    m_pCurrentObject	= 0;
    m_pDetailsPanel		= 0;

	m_Pause				= true;
	once = 5;
	mDocMgr = 0;
	mDemoDoc = 0;
	savedName = "";

	animationSpeed		= 1.024;
	trackState = 0;
	justGUICreation = false;
	details = true;
	fixedHeight = true;
	coverVisible = false;
	explosion =	false;
	show_billboard = true;
	show_volume = true;
	show_border = true;
	mouseX = 0;
	mouseY = 0;
	mousepressed = false;
	showArrows = true;
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
	{
		defaultString = "Voreinstellung";
		defaultsString = "Voreinstellungen";
		closeString = OgreFramework::getSingletonPtr()->ConvertToUTF("Schließen");
	}
	else
	{
		defaultString = "Default";
		defaultsString = "Defaults";
		closeString = "Close";
	}
	if (OgreFramework::getSingletonPtr()->m_bDemo)
	{
		OgreFramework::getSingletonPtr()->m_pLog->logMessage("Loading Demo Path...");
		refillDemoList();
	}
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Loading Documentation...");
	FILE *test;
	if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
	#ifdef WIN32
		test = fopen(RESOURCES_DIR"\\media\\Documentation\\elbe_german.xml", "r");
	else
		test = fopen(RESOURCES_DIR"\\media\\Documentation\\elbe_english.xml", "r");
	#else
		test = fopen(RESOURCES_DIR"/media/Documentation/elbe_german.xml", "r");
	else
		test = fopen(RESOURCES_DIR"/media/Documentation/elbe_english.xml", "r");
	#endif
	if (test)
	{
		fclose(test);
		if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
		#ifdef WIN32
			mDocMgr = new DocumentationManager(RESOURCES_DIR"\\media\\Documentation", "elbe_german.xml");
		else
			mDocMgr = new DocumentationManager(RESOURCES_DIR"\\media\\Documentation", "elbe_english.xml");
		#else
			mDocMgr = new DocumentationManager(RESOURCES_DIR"/media/Documentation", "elbe_german.xml");
		else
			mDocMgr = new DocumentationManager(RESOURCES_DIR"/media/Documentation", "elbe_english.xml");
		#endif
	}
	else
	{
		if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
			test = fopen(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation\\elbe_german.xml", "r");
		else
			test = fopen(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation\\elbe_english.xml", "r");
		if (test)
		{
			fclose(test);
			if (OgreFramework::getSingletonPtr()->m_iLanguage) //german
				mDocMgr = new DocumentationManager(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation", "elbe_german.xml");
			else
				mDocMgr = new DocumentationManager(DEBUG_MEDIA_FOLDER_WINDOWS"media\\Documentation", "elbe_english.xml");
		}
	}



    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC/*, "GameSceneMgr"*/);

	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pRSQ = m_pSceneMgr->createRayQuery(Ray());
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Create Scene...");
	createScene();
	m_pPipe = new ElbePipe(m_pSceneMgr);

	if (OgreFramework::getSingletonPtr()->m_bDemo)
	{
		demoMode = true;
		OrbitCamera* cam = new OrbitCamera(m_pSceneMgr->createCamera("ElbeCameraOrbit"), m_pPipe->getCameraBunch()->getOgreNode());
		cam->setMinDistance(ElbeState::MIN_CAMERA_DISTANCE);
		cam->setMaxDistance(ElbeState::MAX_CAMERA_DISTANCE);
		setCameraWithOutDestroy(cam);
		m_pPipe->refreshTrack(1);
		m_pSceneMgr->getSceneNode("Cryos_02")->flipVisibility();
		m_pSceneMgr->getSceneNode("Cryos_02_H")->flipVisibility();
	}
	else
	{
		FreeViewCamera* camera = new FreeViewCamera(m_pSceneMgr->createCamera("ElbeCameraFree"));
		camera->setFixedHeight(ElbeState::FIXED_CAMERA_HEIGHT);
		m_pCamera = camera;
		if (fixedHeight)
			camera->enableFixedHeight();
		else
			camera->disableFixedHeight();
		m_pCamera->getOgreCamera()->setPosition(Vector3(177, 21, -6.3f));
		m_pCamera->getOgreCamera()->lookAt(Vector3(-0.13f, -0.6f, -50.1f));
		m_pCamera->getOgreCamera()->setNearClipDistance(1);
		m_pCamera->getOgreCamera()->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport[0]->getActualWidth()) /
			Real(OgreFramework::getSingletonPtr()->m_pViewport[0]->getActualHeight()));
	}

    OgreFramework::getSingletonPtr()->m_pViewport[0]->setCamera(m_pCamera->getOgreCamera());
    if (OgreFramework::getSingletonPtr()->m_bStereo)
    {
		OgreFramework::getSingletonPtr()->m_pViewport[1]->setCamera(m_pCamera->getOgreCamera());
		OgreFramework::getSingletonPtr()->m_stereo.shutdown();
		OgreFramework::getSingletonPtr()->m_stereo.init(OgreFramework::getSingletonPtr()->m_pViewport[0], OgreFramework::getSingletonPtr()->m_pViewport[1], OgreFramework::getSingletonPtr()->m_sm);
	}
	m_pPipe->updateScreenPosition();
    buildGUI();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ElbeState::pause()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing ElbeState...");
	Ogre::OverlayManager::getSingleton().getByName("elbe_intro_overlay")->hide();
	Ogre::OverlayManager::getSingleton().getByName("black_intro_overlay")->hide();
	OgreFramework::getSingletonPtr()->m_pTrayMgr->showTrays();
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::resume()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming ElbeState...");

    buildGUI();

    OgreFramework::getSingletonPtr()->m_pViewport[0]->setCamera(m_pCamera->getOgreCamera());
    if (OgreFramework::getSingletonPtr()->m_bStereo)
	{
		OgreFramework::getSingletonPtr()->m_pViewport[1]->setCamera(m_pCamera->getOgreCamera());
		OgreFramework::getSingletonPtr()->m_stereo.shutdown();
		OgreFramework::getSingletonPtr()->m_stereo.init(OgreFramework::getSingletonPtr()->m_pViewport[0], OgreFramework::getSingletonPtr()->m_pViewport[1], OgreFramework::getSingletonPtr()->m_sm);
	}
    m_bQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving ElbeState...");
	Ogre::OverlayManager::getSingleton().getByName("elbe_intro_overlay")->hide();
	if (!OgreFramework::getSingletonPtr()->m_bDontFadeIn)
		Ogre::OverlayManager::getSingleton().getByName("black_intro_overlay")->hide();
	OgreFramework::getSingletonPtr()->m_bDontFadeIn = false;
	if(m_pPipe)
	{
		delete m_pPipe;
		m_pPipe = NULL;
	}

	m_pCurrentObject =0;
	m_pCurrentEntity =0;

	OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(0);

    m_pSceneMgr->destroyCamera(m_pCamera->getOgreCamera());
	
    m_pSceneMgr->destroyQuery(m_pRSQ);
    
	//if(m_pSceneMgr)
    //    OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving ElbeState done...");
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::createScene()
{
    Ogre::Light* light = m_pSceneMgr->createLight("Light0");
	light->setPosition( 75, 75, 75);
	light->setDiffuseColour(0.7,0.7,0.7);
	light = m_pSceneMgr->createLight("Light1");
	light->setPosition(150, 75, 75);
	light->setDiffuseColour(0.7,0.7,0.7);

    DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
    pDotSceneLoader->parseDotScene("elbe_max2010.scene", "General", m_pSceneMgr, m_pSceneMgr->getRootSceneNode());
    delete pDotSceneLoader;

	
	Ogre::SceneNode::ChildNodeIterator node = m_pSceneMgr->getRootSceneNode()->getChildIterator();
	while (node.hasMoreElements())
	{
		Ogre::SceneNode* parent = static_cast<SceneNode*>(node.getNext());
		if (parent->getName().find("Cavity_Field")!=parent->getName().npos)
		{
			parent->setPosition(parent->getPosition()+Ogre::Vector3(0.02f,0.0f,-0.4f));
			parent->setVisible(0);
		}
		Ogre::SceneNode::ChildNodeIterator child = parent->getChildIterator();
		while (child.hasMoreElements())
		{
			SceneNode* childNode = static_cast<SceneNode*>(child.getNext());
			Ogre::SceneNode::ObjectIterator it = childNode->getAttachedObjectIterator();
			while (it.hasMoreElements())
			{
				Entity* entity = static_cast<Entity*>(it.getNext());
				if (entity->getName().find("Line_")!=entity->getName().npos)
					entity->setLightMask(0);
			}
		}
	}
	m_pSceneMgr->getSceneNode("sliceObjects")->setVisible(false);
	m_pSceneMgr->getSceneNode("sliceObjects_H")->setVisible(true);

	m_pSceneMgr->getSceneNode("Cryos_02")->setVisible(false);
	m_pSceneMgr->getSceneNode("Cryos_02_H")->setVisible(true);

	m_pSceneMgr->getSceneNode("Solenoid_01_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Buncher_B_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_01_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Buncher_B_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_02_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_02_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_03_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_03_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_04_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_04_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_05_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Solenoid_05_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Macropulser_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Macropulser_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Buncher_A_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Buncher_A_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Cryos_01_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Cryos_01_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Cryos_02_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Cryos_02_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Gun_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Gun_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("DiPol_01_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("DiPol_02_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_01_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_01_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_02_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_02_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_03_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_03_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_04_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_04_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_05_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_05_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_06_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_06_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_07_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_07_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_08_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_08_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_09_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_09_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_10_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_10_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_11_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_11_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_12_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_12_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_13_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_13_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_14_H_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("QuadroPol_14_BB")->setVisible(false);
	m_pSceneMgr->getSceneNode("Chikane_BB")->setVisible(false);

	collisionBox.push_back(m_pSceneMgr->getEntity("VBox159")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox192")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox220")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox286")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox285")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox284")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox283")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox161")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox162")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox171")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox282")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox281")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox280")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox275")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox271")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox181")->getWorldBoundingBox(true));
	collisionBox.push_back(m_pSceneMgr->getEntity("VBox01")->getWorldBoundingBox(true));


}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ElbeState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	#ifdef BELEG_DEMO
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F2))
	{
		if (m_pPipe->getCameraBunch())
			//((OrbitCamera*)m_pCamera)->absoluteRotate(Ogre::Vector3(-57.0,-24.0,0.20));
			((OrbitCamera*)m_pCamera)->absoluteRotate(Ogre::Vector3(0.0,0.0,0.13));
	}
	#endif
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isModifierDown(OIS::Keyboard::Ctrl) && OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_L))
	{
		if (m_pSceneMgr->getLight("Light0")->getVisible())
		{
			m_pSceneMgr->getLight("Light0")->setVisible(false);
			m_pSceneMgr->getLight("Light1")->setVisible(false);
		}
		else
		{
			m_pSceneMgr->getLight("Light0")->setVisible(true);
			m_pSceneMgr->getLight("Light1")->setVisible(true);
		}
	}
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_SPACE))
    {
		OgreBites::Button* button = (OgreBites::Button*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("pauseButton");
		if (button)
			buttonHit(button);
		else
			m_Pause = !m_Pause;
		return true;
	}
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O))
    {
		OgreBites::Button* button = (OgreBites::Button*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("freeViewButton");
		if (button)
			buttonHit(button);
		return true;
	}
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RETURN))
    {
		if (animationSpeed > 0.036)
			animationSpeed = 0.036;
		else
			animationSpeed = 1.024;
		OgreBites::Slider *slider=(OgreBites::Slider*)(OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("animationSlider"));
		slider->setValue(sqrt(animationSpeed*1000.0));
		sliderMoved(slider);
		return true;
	}
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        pushAppState(findByName("PauseState"));
        return true;
    }

	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F3))
	{
		if(m_bHidden)
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->showTrays();
			m_bHidden = false;
		}
		else
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->hideTrays();
			m_bHidden = true;
		}
	}

	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_F1))
	{
		showHelp("Elbe");
	}

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ElbeState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
    return true;
}

void ElbeState::triggerElbeMap(bool mousepressed)
{
	if (mousepressed && OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("elbeMap") && mouseY>=409 && mouseY<409+67)
	{
		int pos;
		if (mouseX>=393)
			pos = 385;
		else
		if (mouseX>=48)
			pos = mouseX-8;
		else
		{
			if (mouseY <= 440)
			{
				pos = (447-mouseY)*10/7;
			}
			else
			{
				pos = 447-mouseY;
			}
		}
		
		ElbePipe::screenPosition = float(385-pos)/400.0f*27000.0f;

		OgreBites::Slider *slider = (OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("screenSlider");
		slider->setValue(int(ElbePipe::screenPosition/1000.0f),false);
		slider = (OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("screenDetailSlider");
		slider->setValue(int(ElbePipe::screenPosition)%1000);
		m_pPipe->updateScreenPosition();
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ElbeState::mouseMoved(const OIS::MouseEvent &evt)
{
	mouseX = evt.state.X.abs;
	mouseY = evt.state.Y.abs;
	triggerElbeMap(mousepressed);
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;


	if (!demoMode)
	{
		if(m_bRMouseDown)
		{
			m_pCamera->rotate(Ogre::Vector3(evt.state.X.rel * -0.1f, evt.state.Y.rel * -0.1f, 0));
		}

		m_pCamera->rotate(Ogre::Vector3(0, 0, evt.state.Z.rel * -0.1f));
	}
	OgreBites::WidgetIterator widget = OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidgetIterator(OgreBites::TL_LEFT);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ElbeState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	mouseX = evt.state.X.abs;
	mouseY = evt.state.Y.abs;
	if (id == OIS::MB_Left)
		mousepressed = true;
	triggerElbeMap(mousepressed);
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;

    if(id == OIS::MB_Left)
    {
        onLeftPressed(evt);
        m_bLMouseDown = true;
    }
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
    }
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ElbeState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left)
		mousepressed = false;
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;

    if(id == OIS::MB_Left)
    {
        m_bLMouseDown = false;
    }
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = false;
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::onLeftPressed(const OIS::MouseEvent &evt)
{
    if(m_pCurrentObject)
    {
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
		if (OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("helpButton") == NULL)
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Help (F1)");
        m_pCurrentObject->showBoundingBox(false);
    }

	Ogre::Ray mouseRay = m_pCamera->getOgreCamera()->getCameraToViewportRay(OgreFramework::getSingletonPtr()->m_pMouse->getMouseState().X.abs / float(evt.state.width),
        OgreFramework::getSingletonPtr()->m_pMouse->getMouseState().Y.abs / float(evt.state.height));
    m_pRSQ->setRay(mouseRay);
    m_pRSQ->setSortByDistance(true);
	m_pRSQ->setQueryMask(~1<<0);

    Ogre::RaySceneQueryResult &result = m_pRSQ->execute();
    Ogre::RaySceneQueryResult::iterator itr;

    for(itr = result.begin(); itr != result.end(); itr++)
    {
		OgreFramework::getSingletonPtr()->m_pLog->logMessage("You clicked on MovableName: " + itr->movable->getName());
        if(itr->movable && itr->movable->getName().rfind("_BB") == itr->movable->getName().size() - 3)
        {
            m_pCurrentObject = m_pSceneMgr->getEntity(itr->movable->getName())->getParentSceneNode();
            OgreFramework::getSingletonPtr()->m_pLog->logMessage("ObjName " + m_pCurrentObject->getName());
            m_pCurrentObject->showBoundingBox(true);
            m_pCurrentEntity = m_pSceneMgr->getEntity(itr->movable->getName());

			showDescription(m_pCurrentEntity);

            break;
        }
    }
	
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::showDescription(Entity* current)
{
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_TOPLEFT);
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();
	unsigned int height = OgreFramework::getSingletonPtr()->m_pRenderWnd->getHeight();
	
	std::string currentname = current->getParentNode()->getName().c_str();

	std::string cleanedNodeName(currentname);
	
	int firstblank = currentname.find_first_of("_");

	cleanedNodeName = currentname.substr(0,firstblank);

	if(cleanedNodeName=="Buncher")
	{
		cleanedNodeName = currentname.substr(0,currentname.find_last_of("_"));
	}

	const Documentation * doc = mDocMgr->GetDocumentation(cleanedNodeName);

	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);

	int slidercount = 0;
	if (OgreFramework::getSingletonPtr()->explanationMode == expert)
	{
		if (current->getParentNode()->getName().find("Solenoid")!=current->getParentNode()->getName().npos ||
			current->getParentNode()->getName().find("Gun")!=current->getParentNode()->getName().npos ||
			current->getParentNode()->getName().find("QuadroPol")!=current->getParentNode()->getName().npos)
			slidercount = 2;
		else
		if (current->getParentNode()->getName().find("Buncher")!=current->getParentNode()->getName().npos)
			slidercount = 3;
		else
		if (current->getParentNode()->getName().find("Cryos")!=current->getParentNode()->getName().npos)
			slidercount = 5;
	}
	else
	{
		if (current->getParentNode()->getName().find("Solenoid")!=current->getParentNode()->getName().npos ||
			current->getParentNode()->getName().find("Gun")!=current->getParentNode()->getName().npos)
			slidercount = 2;
		else
		if (current->getParentNode()->getName().find("Cryos")!=current->getParentNode()->getName().npos)
			slidercount = 5;
	}
	if (OgreFramework::getSingletonPtr()->explanationMode == expert)
		description = OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_LEFT,"description",current->getName(),INFOBOX_WIDTH,height+PICTURE_HEIGHT-450-slidercount*36/*height*2/5*/);
	else
		description = OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_LEFT,"description",current->getName(),INFOBOX_WIDTH+60,height+PICTURE_HEIGHT-450-slidercount*36/*height*2/5*/,1);
	if(doc)
	{
		description->setCaption(doc->GetTitle());
		description->setText(doc->GetText());
		
		if(doc->GetNumPictures() > 0)
		{
			OgreBites::DecorWidget* meow = OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_LEFT, "descrPic",doc->GetIthPictureFileName(0));
		}

	}
	OgreFramework::getSingletonPtr()->m_pLog->logMessage(current->getParentNode()->getName());
	OgreFramework::getSingletonPtr()->m_pTrayMgr->moveWidgetToTray("description",OgreBites::TL_LEFT);
	savedName = current->getParentNode()->getName();
	if (current->getParentNode()->getName() == "Gun_BB") {
		if (OgreFramework::getSingletonPtr()->explanationMode == expert)
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUGun1", "U in [V]: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 500000, 401)->setValue(ElbePipe::U_G1,false);
		else
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUGun1", "Energie: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH-40, SLIDER_VALUE_WIDTH+80, 0, 500000, 401)->setValue(ElbePipe::U_G1,true);
		else
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUGun1", "Energy: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH-40, SLIDER_VALUE_WIDTH+80, 0, 500000, 401)->setValue(ElbePipe::U_G1,true);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrDefault",defaultString);
	}
	for (int i = 1; i <=5; i++)
	{
		Ogre::String name = "Solenoid_0"+Ogre::StringConverter::toString(i)+"_BB";
		if (current->getParentNode()->getName() == name)
		{
			description->setCaption(description->getCaption()+" "+Ogre::StringConverter::toString(i));
			Ogre::Real value = ElbePipe::I_S1;
			switch (i)
			{
				case 2: value = ElbePipe::I_S2; break;
				case 3: value = ElbePipe::I_S3; break;
				case 4: value = ElbePipe::I_S4; break;
				case 5: value = ElbePipe::I_S5; break;
			}
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol"+Ogre::StringConverter::toString(i), "I in [A]: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(value,false);
			else
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol"+Ogre::StringConverter::toString(i), "Fokussierung: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(value,false);
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol"+Ogre::StringConverter::toString(i), "Focussing: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(value,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrDefault",defaultString);
		}
	}

	if (OgreFramework::getSingletonPtr()->explanationMode == expert)
	{
		if (current->getParentNode()->getName() == "Buncher_B_BB") {
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUBun1", "Potential in [V]: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 30000, 301)->setValue(ElbePipe::U_B1,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldPBun1", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase in [°]: "), INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -90, 90, 181)->setValue(ElbePipe::P_B1,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrDefault",defaultString);
		}

		if (current->getParentNode()->getName() == "Buncher_A_BB") {
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUBun2", "Potential in [V]: ", INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 30000, 301)->setValue(ElbePipe::U_B2,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldPBun2", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase in [°]: "), INFOBOX_WIDTH+(OgreFramework::getSingletonPtr()->explanationMode == expert?0:60), SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -90, 90, 181)->setValue(ElbePipe::P_B2,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrDefault",defaultString);
		}
	}

	if (current->getParentNode()->getName() == "Cryos_01_BB") {
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
			description->setCaption(description->getCaption()+" mit Cavity 1 & 2");
		else
			description->setCaption(description->getCaption()+" with Cavity 1 & 2");
		if (OgreFramework::getSingletonPtr()->explanationMode == expert)
		{
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 1 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "E-Feld 1 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 2 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "E-Feld 2 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1,false);
			}
			else
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 1 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "E-field 1 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 2 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "E-field 2 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1,false);
			}
		}
		else
		{
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("rechte Phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "rechte Beschleunigung: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("linke Phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "linke Beschleunigung: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1,false);
			}
			else
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("right phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "right acceleration: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("left phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "left acceleration: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1,false);
			}
		}
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrDefault",defaultString);
	}

	if (current->getParentNode()->getName() == "Cryos_02_BB") {
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
			description->setCaption(description->getCaption()+" with Cavity 3 & 4");
		else
			description->setCaption(description->getCaption()+" with Cavity 3 & 4");
		if (OgreFramework::getSingletonPtr()->explanationMode == expert)
		{
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 1 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "E-Feld 1 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 2 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "E-Feld 2 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2,false);
			}
			else
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 1 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "E-field 1 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Phase 2 [°]: "), INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "E-field 2 [MV/m]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2,false);
			}
		}
		else
		{
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("rechte Phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "rechte Beschleunigung: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("linke Phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "linke Beschleunigung: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2,false);
			}
			else
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("right phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "right acceleration: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("left phase: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "left acceleration: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH-20, SLIDER_VALUE_WIDTH-25, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2,false);
			}
		}
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrDefault",defaultString);
	}

	if (OgreFramework::getSingletonPtr()->explanationMode == expert)
		for (int i = 1; i <=14; i++)
		{
			Ogre::String name = "QuadroPol_"+Ogre::StringConverter::toString(i,2,'0')+"_BB";
			if (current->getParentNode()->getName() == name)
			{
				description->setCaption(description->getCaption()+" "+Ogre::StringConverter::toString(i));
				Ogre::Real value = ElbePipe::I_Q1;
				switch (i)
				{
					case  2: value = ElbePipe::I_Q2; break;
					case  3: value = ElbePipe::I_Q3; break;
					case  4: value = ElbePipe::I_Q4; break;
					case  5: value = ElbePipe::I_Q5; break;
					case  6: value = ElbePipe::I_Q6; break;
					case  7: value = ElbePipe::I_Q7; break;
					case  8: value = ElbePipe::I_Q8; break;
					case  9: value = ElbePipe::I_Q9; break;
					case 10: value = ElbePipe::I_Q10; break;
					case 11: value = ElbePipe::I_Q11; break;
					case 12: value = ElbePipe::I_Q12; break;
					case 13: value = ElbePipe::I_Q13; break;
					case 14: value = ElbePipe::I_Q14; break;
				}
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole"+Ogre::StringConverter::toString(i)+"Slider", "I in [A]: ", INFOBOX_WIDTH, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(value,false);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrDefault",defaultString);
			}
		}

	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrOkay",closeString);
}

void ElbeState::showScreens()
{
	OgreBites::Widget* helpButton = OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("helpButton");
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_TOPLEFT);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();
	unsigned int height = OgreFramework::getSingletonPtr()->m_pRenderWnd->getHeight();
	int slidercount = 2;
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
	{
		description = OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_LEFT,"screenDescription","Screen Beschreibung",SCREEN_WIDTH,height-SCREEN_WIDTH-slidercount*36-185);
		description->setCaption(OgreFramework::getSingletonPtr()->ConvertToUTF("Screens"));
		description->setText(OgreFramework::getSingletonPtr()->ConvertToUTF("Nutzen Sie die Schiebereglner oben oder klicken Sie in die Karte, um die Position des Screenes zu setzen. An der eingestellten Position wird ein transparentes Viereck gezeichnet. If irgendein Bunch dieses Viereck passiert, wird ein Bild gemacht und angezeigt werden."));
	}
	else
	{
		description = OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_LEFT,"screenDescription","Screen Description",SCREEN_WIDTH,height-SCREEN_WIDTH-slidercount*36-185);
		description->setCaption(OgreFramework::getSingletonPtr()->ConvertToUTF("Screens"));
		description->setText(OgreFramework::getSingletonPtr()->ConvertToUTF("Move the sliders above or click in the map picture to set the position of the screen. You will see a transparent rectangle at this position. If some bunch arrives at this rectangle a picture of it will be made and shown."));
	}
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_LEFT, "descrPic","screenTexturePanel");
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_LEFT, "elbeMap","elbeMapPanel");
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "screenSlider", OgreFramework::getSingletonPtr()->ConvertToUTF("Position [mm]: "), SCREEN_WIDTH, SCREEN_WIDTH-SLIDER_VALUE_WIDTH-105, SLIDER_VALUE_WIDTH, 0, 27, 28)->setValue(ElbePipe::screenPosition/1000.0f);
	OgreBites::Slider* slider = OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "screenDetailSlider", OgreFramework::getSingletonPtr()->ConvertToUTF("Detail Position [mm]: "), SCREEN_WIDTH, SCREEN_WIDTH-150-15, 15, 0, 999, 1000);
	slider->setValue(int(ElbePipe::screenPosition)%1000);
	slider->setValueCaption("");

	Ogre::StringVector my_vector;
	my_vector.push_back("Screen Radius X");
	my_vector.push_back("Screen Radius Y");
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_LEFT,"screenparampanel",SCREEN_WIDTH,my_vector);

	OgreFramework::getSingletonPtr()->m_pTrayMgr->moveWidgetToTray("screenDescription",OgreBites::TL_LEFT);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT,"descrOkay",closeString);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::showOptions()
{
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
	justGUICreation = true;
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();

	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);

	// show options dialog
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "lblOptions", "Optionen", INFOBOX_WIDTH+60);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUGun1", "Gun Spannung in [V]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 500000, 301)->setValue(ElbePipe::U_G1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUBun1", "Buncher 1 Spannung [V]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 30000, 301)->setValue(ElbePipe::U_B1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldPBun1", OgreFramework::getSingletonPtr()->ConvertToUTF("Buncher 1 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -90, 90, 181)->setValue(ElbePipe::P_B1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUBun2", "Buncher 2 Spannung [V]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 30000, 301)->setValue(ElbePipe::U_B2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldPBun2", OgreFramework::getSingletonPtr()->ConvertToUTF("Buncher 2 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -90, 90, 181)->setValue(ElbePipe::P_B2);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 1 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "Cavity 1 E-Feld in [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 2 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "Cavity 2 E-Feld in [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 3 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "Cavity 3 E-Feld in [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 4 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "Cavity 4 E-Feld in [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2);
	}
	else
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "lblOptions", "Options", INFOBOX_WIDTH+60);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUGun1", "Gun Potential in [V]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 500000, 301)->setValue(ElbePipe::U_G1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUBun1", "Buncher 1 Potential in [V]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 30000, 301)->setValue(ElbePipe::U_B1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldPBun1", OgreFramework::getSingletonPtr()->ConvertToUTF("Buncher 1 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -90, 90, 181)->setValue(ElbePipe::P_B1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUBun2", "Buncher 2 Potential in [V]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 30000, 301)->setValue(ElbePipe::U_B2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldPBun2", OgreFramework::getSingletonPtr()->ConvertToUTF("Buncher 2 Phase in [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -90, 90, 181)->setValue(ElbePipe::P_B2);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 1 Phase Shift [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "Cavity 1 E-field [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 2 Phase Shift [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "Cavity 2 E-field [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 3 Phase Shift [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "Cavity 3 E-field [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", OgreFramework::getSingletonPtr()->ConvertToUTF("Cavity 4 Phase Shift [°]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "Cavity 4 E-field [MV/m]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2);
	}
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "defaultOptionsButton", defaultsString);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "cancelOptionsButton", closeString);
	justGUICreation = false;
}

void ElbeState::showSimpleOptions()
{
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
	justGUICreation = true;
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();

	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);

	// show options dialog
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "lblOptions", "Einstellungen", INFOBOX_WIDTH+40);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUGun1", " Gun Energie: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH-40, SLIDER_VALUE_WIDTH+80, 0, 500000, 301)->setValue(ElbePipe::U_G1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "optionsLabel2", "Beschleuniger 1", INFOBOX_WIDTH+40);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", "Rechte Phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "Rechte Beschleunigung: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", "Linke Phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "Linke Beschleunigung: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "optionsLabel1", "Solenoiden", INFOBOX_WIDTH+40);
		for (int i = 1; i <=5; i++)
		{
			Ogre::String name = "Solenoid_0"+Ogre::StringConverter::toString(i)+"_BB";
			Ogre::Real value = ElbePipe::I_S1;
			switch (i)
			{
				case 2: value = ElbePipe::I_S2; break;
				case 3: value = ElbePipe::I_S3; break;
				case 4: value = ElbePipe::I_S4; break;
				case 5: value = ElbePipe::I_S5; break;
			}
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol"+Ogre::StringConverter::toString(i), Ogre::StringConverter::toString(i)+".: Fokussierung: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH+40, SLIDER_VALUE_WIDTH-20, 0, 4, 401)->setValue(value,false);
		}
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "optionsLabel3", "Beschleuniger 2", INFOBOX_WIDTH+40);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", "Rechte Phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "Rechte Beschleunigung: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", "Linke Phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "Linke Beschleunigung: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2);
	}
	else
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "lblOptions", "Options", INFOBOX_WIDTH+40);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldUGun1", " Gun Energy: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH-40, SLIDER_VALUE_WIDTH+80, 0, 500000, 301)->setValue(ElbePipe::U_G1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "optionsLabel2", "Accelerator 1", INFOBOX_WIDTH+40);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider1", "Right phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider1", "Right acceleration: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1PhaseSlider2", "Left phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos1EpeakSlider2", "Left acceleration: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS1);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "optionsLabel1", "Solenoids", INFOBOX_WIDTH+40);
		for (int i = 1; i <=5; i++)
		{
			Ogre::String name = "Solenoid_0"+Ogre::StringConverter::toString(i)+"_BB";
			Ogre::Real value = ElbePipe::I_S1;
			switch (i)
			{
				case 2: value = ElbePipe::I_S2; break;
				case 3: value = ElbePipe::I_S3; break;
				case 4: value = ElbePipe::I_S4; break;
				case 5: value = ElbePipe::I_S5; break;
			}
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol"+Ogre::StringConverter::toString(i), Ogre::StringConverter::toString(i)+".: Focussing: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH+40, SLIDER_VALUE_WIDTH-20, 0, 4, 401)->setValue(value,false);
		}
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "optionsLabel3", "Accelerator 2", INFOBOX_WIDTH+40);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider1", "Right phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider1", "Right acceleration: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E1_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2PhaseSlider2", "Left phase: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, -180, 180, 361)->setValue(ElbePipe::P2_CRYOS2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "cryos2EpeakSlider2", "Left acceleration: ", INFOBOX_WIDTH+40, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH-20, 0, 20, 81)->setValue(ElbePipe::E2_CRYOS2);
	}
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "defaultSimpleOptionsButton", defaultsString);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "cancelOptionsButton", closeString);
	justGUICreation = false;
}

void ElbeState::showOptions_Q()
{
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);

	justGUICreation = true;
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();

	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);

	// show options dialog
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "lblOptions", "Magnet Optionen", INFOBOX_WIDTH);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol1", "Solenoid 1: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol2", "Solenoid 2: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol3", "Solenoid 3: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S3);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol4", "Solenoid 4: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S4);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol5", "Solenoid 5: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S5);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole1Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 1: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole2Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 2: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole3Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 3: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q3);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole4Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 4: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q4);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole5Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 5: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q5);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole6Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 6: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q6);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole7Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 7: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q7);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole8Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 8: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q8);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole9Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 9: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q9);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole10Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 10: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q10);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole11Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 11: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q11);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole12Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 12: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q12);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole13Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 13: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q13);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole14Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupol 14: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q14);
	}
	else
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_LEFT, "lblOptions", "Magnet Options", INFOBOX_WIDTH);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol1", "Solenoid 1: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol2", "Solenoid 2: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol3", "Solenoid 3: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S3);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol4", "Solenoid 4: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S4);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "sldISol5", "Solenoid 5: I in [A]: ", INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, 0, 4, 401)->setValue(ElbePipe::I_S5);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole1Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 1: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q1);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole2Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 2: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q2);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole3Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 3: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q3);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole4Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 4: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q4);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole5Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 5: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q5);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole6Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 6: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q6);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole7Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 7: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q7);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole8Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 8: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q8);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole9Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 9: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q9);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole10Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 10: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q10);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole11Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 11: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q11);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole12Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 12: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q12);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole13Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 13: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q13);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_LEFT, "quadrupole14Slider", OgreFramework::getSingletonPtr()->ConvertToUTF("Quadrupole 14: I in [A]: "), INFOBOX_WIDTH+60, SLIDER_SLIDER_WIDTH, SLIDER_VALUE_WIDTH, -10, 10, 101)->setValue(ElbePipe::I_Q14);
	}
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "defaultOptionsButton_Q", defaultsString);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "cancelOptionsButton_Q", closeString);
	justGUICreation = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::showSaveStates()
{
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();

	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);

	// show save states dialog
	Ogre::StringVector states;
	states.push_back(defaultString);
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
	{
		states.push_back("Benutzerdefiniert 1");
		states.push_back("Benutzerdefiniert 2");
		states.push_back("Benutzerdefiniert 3");
		states.push_back("Benutzerdefiniert 4");
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createThickSelectMenu(OgreBites::TL_LEFT,"savestateswitch","Einstellung",200,states.size(),states);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "loadStateButton", "Einstellung laden");
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "saveStateButton", "Einstellung speichern");
		OgreBites::Button* b; 
		b = (OgreBites::Button*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("saveStateButton"); 
		b->hide();
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "restartStateButton",OgreFramework::getSingletonPtr()->ConvertToUTF("Schließen & Zurücksetzen"));
	}
	else
	{
		states.push_back("Custom 1");
		states.push_back("Custom 2");
		states.push_back("Custom 3");
		states.push_back("Custom 4");
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createThickSelectMenu(OgreBites::TL_LEFT,"savestateswitch","State",200,states.size(),states);

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "loadStateButton", "Load State");
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "saveStateButton", "Save State");
		OgreBites::Button* b; 
		b = (OgreBites::Button*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("saveStateButton"); 
		b->hide();
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "restartStateButton", "Close & Reset");
	}
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_LEFT, "cancelStateButton", closeString);
}

//|||||||||||||||||||||||||||||||||||||||||||||||
void ElbeState::itemSelected(OgreBites::SelectMenu *selectMenu)
{
	if (selectMenu->getName() == "savestateswitch")
	{
		OgreBites::Button* b; 
		b = (OgreBites::Button*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("saveStateButton"); 
		if (selectMenu->getSelectedItem().find("Custom") == std::string::npos && selectMenu->getSelectedItem().find("Benutzerdefiniert") == std::string::npos)
			b->hide();
		else
			b->show();
	}
}

void ElbeState::moveCamera()
{
    if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
        m_pCamera->translate(5.0*m_TranslateVector);
	else
		m_pCamera->translate(5.0*m_TranslateVector / 4);
	//prevent camera from going inside table and laser elements
	Ogre::Vector3 pos_new = m_pCamera->getOgreCamera()->_getParentNodeFullTransform().transformAffine(m_pCamera->getOgreCamera()->getPosition());
	for (std::list<Ogre::AxisAlignedBox>::iterator it = collisionBox.begin(); it != collisionBox.end();++it)
	{
		if ((*it).contains(pos_new))
		{
			if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
				m_pCamera->translate(-m_TranslateVector);
			else
				m_pCamera->translate(-m_TranslateVector / 4);
			break;
		}
	}
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
        m_pCamera->translate(-4.0*m_TranslateVector);
	else
		m_pCamera->translate(-4.0*m_TranslateVector / 4);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::getInput()
{
    if(m_bSettingsMode == false)
    {
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A) ||
		   OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LEFT))
            m_TranslateVector.x = -m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D) ||
		   OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
            m_TranslateVector.x = m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W) ||
		   OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_UP))
            m_TranslateVector.z = -m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S) ||
		   OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_DOWN))
            m_TranslateVector.z = m_MoveScale;
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ElbeState::updateDemo()
{
	if (!demoMode)
		return false;
	if (demoList.empty())
		return false;
	while (demoList.front().time < demoTime && !(demoList.empty()))
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
		buildGUI();
		trackState = demoList.front().envelope?1:0;
		if (demoList.front().envelope)
		{
			m_pPipe->track->setVisible(true);
			m_pPipe->track2->setVisible(true);
		}
		else
		{
			m_pPipe->track->setVisible(false);
			m_pPipe->track2->setVisible(false);
		}
		demoLast = demoList.front();
		demoList.pop_front();
	}
	if (demoList.empty())
	{
		OgreFramework::getSingletonPtr()->m_bDontFadeIn = true;
		if (OgreFramework::getSingletonPtr()->m_bDemoChange)
			changeAppState(findByName("DracoState"));
		else
			changeAppState(findByName("ElbeState"));
		return true;
	}
	int t_max = demoList.front().time - demoLast.time;
	int t = demoTime - demoLast.time;
	float l = float(t)/float(t_max);
	((OrbitCamera*)m_pCamera)->absoluteRotate(Ogre::Vector3(l*demoList.front().x+(1.0f-l)*demoLast.x,l*demoList.front().y+(1.0f-l)*demoLast.y,l*demoList.front().z+(1.0f-l)*demoLast.z));	
	animationSpeed = l*demoList.front().speed+(1.0f-l)*demoLast.speed;

	if (demoLast.caption != "internalOverlay" && demoList.front().caption == "internalOverlay")
		((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName("elbe_intro_material"))->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, l);
	else
	if (demoLast.caption == "internalOverlay" && demoList.front().caption != "internalOverlay")
		((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName("elbe_intro_material"))->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, 1.0f-l);
	else
	if (demoList.front().caption == "internalOverlay")
		((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName("elbe_intro_material"))->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, 1.0f);

	if (demoLast.caption != "black" && demoList.front().caption == "black")
	{
		Ogre::OverlayManager::getSingleton().getByName("black_intro_overlay")->show();
		((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName("black_intro_material"))->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, l);
	}
	else
	if (demoLast.caption == "black" && demoList.front().caption != "black")
	{
		Ogre::OverlayManager::getSingleton().getByName("black_intro_overlay")->show();
		((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName("black_intro_material"))->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, 1.0f);
	}
	else
	if (demoList.front().caption == "black")
	{
		Ogre::OverlayManager::getSingleton().getByName("black_intro_overlay")->show();
		((Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName("black_intro_material"))->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, 1.0f);
	}
	else
		Ogre::OverlayManager::getSingleton().getByName("black_intro_overlay")->hide();


	return false;
}

void ElbeState::update(double timeSinceLastFrame)
{
	if (!m_Pause)
	{
		demoTime += (int)timeSinceLastFrame;
		if (updateDemo())
			return;
	}
	OgreFramework::getSingletonPtr()->updateEmulatedMousePosition(timeSinceLastFrame);
	if (timeSinceLastFrame < 50.0)
	{
		ElbeConstants::MAX_CALC_TIME /= pow(1.02,timeSinceLastFrame);
		if (ElbeConstants::MAX_CALC_TIME < ElbeConstants::MIN_MAX_CALC_TIME)
			ElbeConstants::MAX_CALC_TIME = ElbeConstants::MIN_MAX_CALC_TIME;
	}
	else
	{
		ElbeConstants::MAX_CALC_TIME *= pow(1.02,timeSinceLastFrame);
		if (ElbeConstants::MAX_CALC_TIME > ElbeConstants::MAX_MAX_CALC_TIME)
			ElbeConstants::MAX_CALC_TIME = ElbeConstants::MAX_MAX_CALC_TIME;
	}
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
		TimeManager::resetTime();
        popAppState();
        return;
    }

    m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
    m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

    m_TranslateVector = Vector3::ZERO;
	if (OgreFramework::getSingletonPtr()->m_bDemo)
		m_pPipe->visualize(m_pCamera,0.0f);
	else
		m_pPipe->visualize(m_pCamera,0.0f);
	if (showArrows)
		m_pPipe->updateFields();

	if(!m_Pause)
	{
		#ifdef BELEG_DEMO
			double old_pos;
			if (m_pPipe->getCameraBunch())
				old_pos = m_pPipe->getCameraBunch()->getElectron()->pos;
		#endif
		ElbeBunch* oldCameraBunch = m_pPipe->getCameraBunch();
		float factor=animationSpeed;
		if (!m_pPipe->update(timeSinceLastFrame,factor,explosion,show_billboard,show_volume,track_count,show_border) && m_pPipe->newCameraBunch())
		{
			if (m_pCamera->getOgreCamera()->getName() == "ElbeCameraOrbit")
			{
				// Recreate to OrbitCamera
				Ogre::Quaternion orientation = ((OrbitCamera*)(m_pCamera))->getCameraTargetNode()->getOrientation();
				double zoom = ((OrbitCamera*)(m_pCamera))->getCameraNode()->getPosition().z;
				m_pCamera->getOgreCamera()->detachFromParent();
				m_pSceneMgr->destroyCamera(m_pCamera->getOgreCamera());
				OrbitCamera* cam = new OrbitCamera(m_pSceneMgr->createCamera("ElbeCameraOrbit"), m_pPipe->getCameraBunch()->getOgreNode());
				cam->setMinDistance(ElbeState::MIN_CAMERA_DISTANCE);
				cam->setMaxDistance(ElbeState::MAX_CAMERA_DISTANCE);
				setCameraWithOutDestroy(cam);
				((OrbitCamera*)(m_pCamera))->getCameraTargetNode()->setOrientation(orientation);
				((OrbitCamera*)(m_pCamera))->getCameraNode()->setPosition(
					((OrbitCamera*)(m_pCamera))->getCameraNode()->getPosition().x,
					((OrbitCamera*)(m_pCamera))->getCameraNode()->getPosition().y,zoom);
			}
		}
		#ifdef BELEG_DEMO
			if (m_pPipe->getCameraBunch())
			{
				//if (m_pPipe->getCameraBunch()->getElectron()->pos > 2990.0f && old_pos <= 2990.0f)
				if (m_pPipe->getCameraBunch()->getElectron()->pos > 3480.0f && old_pos <= 3480.0f)
					m_Pause = true;
			}
		#endif
	

		if (m_pPipe->getBunchCount()>0 &&  oldCameraBunch == m_pPipe->getFakeBunch() && oldCameraBunch != m_pPipe->getCameraBunch())
		{
			if (m_pCamera->getOgreCamera()->getName() == "ElbeCameraOrbit") {
				// Recreate to OrbitCamera
				Ogre::Quaternion orientation = ((OrbitCamera*)(m_pCamera))->getCameraTargetNode()->getOrientation();
				double zoom = ((OrbitCamera*)(m_pCamera))->getCameraNode()->getPosition().z;
				m_pCamera->getOgreCamera()->detachFromParent();
				m_pSceneMgr->destroyCamera(m_pCamera->getOgreCamera());
				OrbitCamera* cam = new OrbitCamera(m_pSceneMgr->createCamera("ElbeCameraOrbit"), m_pPipe->getCameraBunch()->getOgreNode());
				cam->setMinDistance(ElbeState::MIN_CAMERA_DISTANCE);
				cam->setMaxDistance(ElbeState::MAX_CAMERA_DISTANCE);
				setCameraWithOutDestroy(cam);
				((OrbitCamera*)(m_pCamera))->getCameraTargetNode()->setOrientation(orientation);
				((OrbitCamera*)(m_pCamera))->getCameraNode()->setPosition(
					((OrbitCamera*)(m_pCamera))->getCameraNode()->getPosition().x,
					((OrbitCamera*)(m_pCamera))->getCameraNode()->getPosition().y,zoom);
			}
		}

	}

	// update GUI - show current dimensions of the bunch
	Ogre::String rx = Ogre::StringConverter::toString(m_pPipe->getCameraBunch()->getCurrentDimensions().x, 2);
	Ogre::String ry = Ogre::StringConverter::toString(m_pPipe->getCameraBunch()->getCurrentDimensions().y, 2);
	Ogre::String l = Ogre::StringConverter::toString(m_pPipe->getCameraBunch()->getCurrentDimensions().z, 2);

	char buffer[256];

	OgreBites::ParamsPanel* panel = (OgreBites::ParamsPanel*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("parampanel");
	if (panel)
	{
		int add = 0;
		if (demoMode)
		{
			sprintf(buffer,"%.2f %%",animationSpeed/0.5f*100.0f);
			panel->setParamValue(0,buffer);
			add = 1;
		}
		sprintf(buffer,"%i ps",int(3846.1538461538461538461538461538-m_pPipe->getGlobalTime()));
		panel->setParamValue(0+add,buffer);
		if (OgreFramework::getSingletonPtr()->explanationMode == expert)
		{
			sprintf(buffer,"%.2f mm",m_pPipe->getCameraBunch()->getElectron()->pos);
			panel->setParamValue(1+add,buffer);
			sprintf(buffer,"%.4f MeV",m_pPipe->getCameraBunch()->getElectron()->energy*0.6242); //with rest engery!
			panel->setParamValue(2+add,buffer);
			sprintf(buffer,"%.4f%% c",m_pPipe->getCameraBunch()->getBeta()*100.0);
			panel->setParamValue(3+add,buffer);
			sprintf(buffer,"%.4f",m_pPipe->getCameraBunch()->getGamma());
			panel->setParamValue(4+add,buffer);
			Ogre::Vector3 scale = m_pPipe->getCameraBunch()->getElectron()->getBunchScale();
			sprintf(buffer,"%.3f x %.3f x %.3f",scale.x,scale.y,scale.z);
			panel->setParamValue(5+add,buffer);
			if (m_pPipe->getCameraBunch()->getElectron()->driftnumber==9) //in fist cavity
				sprintf(buffer,"E(z,t):%.4f",m_pPipe->cavityGetE11ZTmm(m_pPipe->getCameraBunch()->getElectron()->L,m_pPipe->globalTime));
			else
			if (m_pPipe->getCameraBunch()->getElectron()->driftnumber==11) //in second cavity
				sprintf(buffer,"E(z,t):%.6f",m_pPipe->cavityGetE12ZTmm(m_pPipe->getCameraBunch()->getElectron()->L,m_pPipe->globalTime));	
			else
			if (m_pPipe->getCameraBunch()->getElectron()->driftnumber==20) //in third cavity
				sprintf(buffer,"E(z,t):%.6f",m_pPipe->cavityGetE21ZTmm(m_pPipe->getCameraBunch()->getElectron()->L,m_pPipe->globalTime));	
			else
			if (m_pPipe->getCameraBunch()->getElectron()->driftnumber==22) //in fourth cavity
				sprintf(buffer,"E(z,t):%.6f",m_pPipe->cavityGetE22ZTmm(m_pPipe->getCameraBunch()->getElectron()->L,m_pPipe->globalTime));	
			else
				sprintf(buffer,"n.A.");
			panel->setParamValue(6+add,buffer);
		}
		else
		{
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
				sprintf(buffer,"%.2f%% der Lichtgeschw.",m_pPipe->getCameraBunch()->getBeta()*100.0);
			else
				sprintf(buffer,"%.2f%% of the lightspeed",m_pPipe->getCameraBunch()->getBeta()*100.0);
			//sprintf(buffer,"%i",demoTime);
			panel->setParamValue(1+add,buffer);
			sprintf(buffer,"%.4f MeV",m_pPipe->getCameraBunch()->getElectron()->energy*0.6242); //with rest engery!
			//sprintf(buffer,"%f",m_pPipe->getCameraBunch()->getElectron()->pos);
			panel->setParamValue(2+add,buffer);
			Ogre::Vector3 scale = m_pPipe->getCameraBunch()->getElectron()->getBunchScale();
			sprintf(buffer,"%.0f mm x %.0f mm x %.0f mm",scale.x,scale.y,scale.z);
			panel->setParamValue(3+add,buffer);
		}
	}
	getInput();
    moveCamera();
	if (OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("screenDescription"))
	{
		m_pPipe->refreshScreenTexture();
		panel = (OgreBites::ParamsPanel*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("screenparampanel");
		panel->setParamValue(0,Ogre::StringConverter::toString(m_pPipe->screenW)+" mm");
		panel->setParamValue(1,Ogre::StringConverter::toString(m_pPipe->screenH)+" mm");
	}
	if (once > 0)
		once--;
	if (once == 0)
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
		buildGUI();
		//PauseState::closeInstant = true;
		//pushAppState(findByName("PauseState"));
		once = -1;
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::buildGUI()
{
	justGUICreation = true;
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();

	if (demoMode)
	{
		if (demoLast.caption == "internalOverlay" || demoList.front().caption == "internalOverlay")
		{
			Ogre::OverlayManager::getSingleton().getByName("elbe_intro_overlay")->show();
			return;
		}
		else
		{
			Ogre::OverlayManager::getSingleton().getByName("elbe_intro_overlay")->hide();
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_TOPRIGHT, "HZDR_logo","HZDR_logo");
			if (demoList.front().caption != "")
			{
				OgreBites::TextBox* box;
				if (demoList.front().text != "")
					box = OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_TOP,"demoDescription",demoList.front().caption,850,285,2);
				else
					box = OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_TOP,"demoDescription",demoList.front().caption,850,54,2);
				box->setText(demoList.front().text);
			}
		}
	}
	else
	{
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Hilfe (F1)");
		else
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Help (F1)");
		#ifndef ELBE_ONLY
			OgreBites::DecorWidget* meow = OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_TOPRIGHT, "HZDR_logo","HZDR_logo");
		#else
			OgreBites::DecorWidget* meow = OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_BOTTOMRIGHT, "HZDR_logo","HZDR_logo");
		#endif
		OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

		if (OgreFramework::getSingletonPtr()->m_iLanguage)
		{
			if (m_Pause)
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "pauseButton", "Start", 100);
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "pauseButton", "Pause", 100);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "stopButton", "Stop", 100);
		}
		else
		{
			if (m_Pause)
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "pauseButton", "Play", 100);
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "pauseButton", "Pause", 100);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "stopButton", "Stop", 100);
		}

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createSeparator(OgreBites::TL_BOTTOMRIGHT, "seperator1", 140);

	
	#ifndef ELBE_ONLY
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPRIGHT, "menuButton", "Menu", 100);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPRIGHT, "dracoButton", "Draco", 100);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOPRIGHT, "GameLbl", "ELBE", 100);
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPRIGHT, "radiButton", "Radiation3D", 100);
	#endif
	#ifndef RELEASE
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPRIGHT, "collButton", "Collision", 100);
	#endif
	}
	if (OgreFramework::getSingletonPtr()->m_iLanguage)
	{
		if (!demoMode)
		{
			if (m_pCamera->getOgreCamera()->getName() == "ElbeCameraFree")
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "freeViewButton", "Orbitsicht", 140);
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "freeViewButton", "Freie Sicht", 140);
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "fixedHeightButton",OgreFramework::getSingletonPtr()->ConvertToUTF("fixe Höhe"), 140)->setChecked(fixedHeight,false);
				if (m_pCamera->getOgreCamera()->getName() != "ElbeCameraFree")
					OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("fixedHeightButton")->hide();
			}


			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "coverButton", OgreFramework::getSingletonPtr()->ConvertToUTF("Hülle"), 140)->setChecked(coverVisible,false);
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "optionsButton", "RF Einst.", 140);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "optionsButton_Q", "Magnet Einst.", 140);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "savestatesButton", "Sichern", 140);
			}
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "simpleOptionsButton", "Einstellungen", 140);
			switch (trackState)
			{
				case 0:
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "trackButton", "Envelope An", 140);
					break;
				case 1:
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "trackButton", "Envelope Aus", 140);
					break;
				case 2:
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "trackButton", "Envelope Dyn.", 140);
					break;
			}
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "screenButton", "Zeige Screen", 140);

				OgreFramework::getSingletonPtr()->m_pTrayMgr->createSeparator(OgreBites::TL_BOTTOMRIGHT, "seperator3", 140);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "logButton", "log.txt sichern", 140);
			}
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "arrowButton", "Feldpfeile", 140)->setChecked(showArrows,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "billboardButton", "Einzelelektronen", 140)->setChecked(show_billboard,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "volumeButton", "Bunchvolumen", 140)->setChecked(show_volume,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "borderButton", "Bunchbegrenzung", 140)->setChecked(show_border,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createThickSlider(OgreBites::TL_BOTTOMRIGHT, "tracksSlider","Spuren",140,30,0,TRACK_COUNT,TRACK_COUNT+1)->setValue(track_count,false);
		}
		Ogre::StringVector my_vector;
		if (demoMode)
			my_vector.push_back("Animationsgeschwindigkeit");
		my_vector.push_back(OgreFramework::getSingletonPtr()->ConvertToUTF("Nächster Elektronenbunch in"));
		if (OgreFramework::getSingletonPtr()->explanationMode == expert)
		{
			my_vector.push_back("  Position");
			my_vector.push_back("  Energie");
			my_vector.push_back("  Beta");
			my_vector.push_back("  Gamma");
			my_vector.push_back("  Dimension (mm)");
			my_vector.push_back("Cavity Feld");
		}
		else
		{
			my_vector.push_back("Geschwindigkeit");
			my_vector.push_back("Energie");
			my_vector.push_back("Dimension");
		}
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_BOTTOM,"parampanel",350,my_vector);

		if (!demoMode)
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_BOTTOM, "animationSlider", "Animationsgeschw.", 130, 100, 1, 100, 100)->setValue(sqrt(animationSpeed*1000.0));
			OgreBites::Slider *slider=(OgreBites::Slider*)(OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("animationSlider"));
		}
	}
	else
	{
		if (!demoMode)
		{
			if (m_pCamera->getOgreCamera()->getName() == "ElbeCameraFree")
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "freeViewButton", "Orbit View", 140);
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "freeViewButton", "Free View", 140);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "fixedHeightButton", "Fixed Height", 140)->setChecked(fixedHeight,false);
			if (m_pCamera->getOgreCamera()->getName() != "ElbeCameraFree")
				OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("fixedHeightButton")->hide();


			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "coverButton", "Cover", 140)->setChecked(coverVisible,false);
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "optionsButton", "RF Config", 140);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "optionsButton_Q", "Magnet Config", 140);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "savestatesButton", "Savestates", 140);
			}
			else
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "simpleOptionsButton", "Config", 140);
			switch (trackState)
			{
				case 0:
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "trackButton", "Envelope Off", 140);
					break;
				case 1:
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "trackButton", "Envelope Off", 140);
					break;
				case 2:
					OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "trackButton", "Envelope Dynamic", 140);
					break;
			}
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
			{
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "screenButton", "Show Screen", 140);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createSeparator(OgreBites::TL_BOTTOMRIGHT, "seperator3", 140);
				OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "logButton", "Save as log.txt", 140);
			}
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "arrowButton", "Field Arrows", 140)->setChecked(showArrows,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "billboardButton", "Each Electron", 140)->setChecked(show_billboard,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "volumeButton", "Bunch volume", 140)->setChecked(show_volume,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createCheckBox(OgreBites::TL_BOTTOMRIGHT, "borderButton", "Bunch border", 140)->setChecked(show_border,false);
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createThickSlider(OgreBites::TL_BOTTOMRIGHT, "tracksSlider","Tracks",140,30,0,TRACK_COUNT,TRACK_COUNT+1)->setValue(track_count,false);
		}
		Ogre::StringVector my_vector;
		if (demoMode)
			my_vector.push_back("Animation speed");
		my_vector.push_back("Next Electron in");
		if (OgreFramework::getSingletonPtr()->explanationMode == expert)
		{
			my_vector.push_back("  Position");
			my_vector.push_back("  Energy");
			my_vector.push_back("  Beta");
			my_vector.push_back("  Gamma");
			my_vector.push_back("  Dimension (mm)");
			my_vector.push_back("Cavity Field");
		}
		else
		{
			my_vector.push_back("Speed");
			my_vector.push_back("Energy");
			my_vector.push_back("Dimension");
		}

		OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_BOTTOM,"parampanel",350,my_vector);
		if (!demoMode)
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSlider(OgreBites::TL_BOTTOM, "animationSlider", "Animation Speed", 130, 100, 1, 100, 100)->setValue(sqrt(animationSpeed*1000.0));
			OgreBites::Slider *slider=(OgreBites::Slider*)(OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("animationSlider"));
		}
	}
	if (OgreFramework::getSingletonPtr()->m_bDemo)
	{
		if (demoMode)
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "demoButton", "", 16);
		else
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "demoButton", OgreFramework::getSingletonPtr()->ConvertToUTF("Demo fortführen"), 150);
	}
	justGUICreation = false;
}

void ElbeState::checkBoxToggled(OgreBites::CheckBox *checkBox)
{
	if (checkBox == NULL)
		return;
	if(checkBox->getName() == "explosionButton")
		explosion = !explosion;
	else if(checkBox->getName() == "billboardButton")
	{
		show_billboard = !show_billboard;
		if (m_pPipe->getCameraBunch())
			m_pPipe->getCameraBunch()->show_billboard = show_billboard;
	}
	else if(checkBox->getName() == "volumeButton")
	{
		show_volume = !show_volume;
		if (m_pPipe->getCameraBunch())
			m_pPipe->getCameraBunch()->show_volume = show_volume;
	}
	else if(checkBox->getName() == "borderButton")
	{
		show_border = !show_border;
		if (m_pPipe->getCameraBunch())
			m_pPipe->getCameraBunch()->show_border = show_border;
	}
	else if(checkBox->getName() == "fixedHeightButton" && m_pCamera->getOgreCamera()->getName() == "ElbeCameraFree")
	{
		FreeViewCamera* cam = static_cast<FreeViewCamera*>(m_pCamera);
		fixedHeight = checkBox->isChecked();
		if (fixedHeight)
			cam->enableFixedHeight();
		else
			cam->disableFixedHeight();
	}
	else if(checkBox->getName() == "chaseViewButton")
	{
		details = !details;
	}
	else if(checkBox->getName() == "coverButton")
	{
		m_pSceneMgr->getSceneNode("sliceObjects")->flipVisibility();
		m_pSceneMgr->getSceneNode("sliceObjects_H")->flipVisibility();

		m_pSceneMgr->getSceneNode("Cryos_02")->flipVisibility();
		m_pSceneMgr->getSceneNode("Cryos_02_H")->flipVisibility();
		coverVisible = !coverVisible;
	}
	else if(checkBox->getName() == "arrowButton")
	{
		showArrows = !showArrows;
		m_pSceneMgr->getSceneNode("Emitter")->getAttachedObject(1)->setVisible(showArrows);
		if (showArrows)
		{
			Ogre::Node::ChildNodeIterator it = m_pSceneMgr->getSceneNode("Buncher_B_Field")->getChildIterator();
			while (it.hasMoreElements())
			{
				Ogre::Node::ChildNodeIterator it2 = it.getNext()->getChildIterator();
				while (it2.hasMoreElements())
				{
					Ogre::SceneNode* node = (Ogre::SceneNode*)(it2.getNext());
					node->setVisible(true);
				}
			}
			it = m_pSceneMgr->getSceneNode("Buncher_A_Field")->getChildIterator();
			while (it.hasMoreElements())
			{
				Ogre::Node::ChildNodeIterator it2 = it.getNext()->getChildIterator();
				while (it2.hasMoreElements())
				{
					Ogre::SceneNode* node = (Ogre::SceneNode*)(it2.getNext());
					node->setVisible(true);
				}
			}
			it = m_pSceneMgr->getSceneNode("Cavity_Field")->getChildIterator();
			while (it.hasMoreElements())
			{
				Ogre::Node::ChildNodeIterator it2 = it.getNext()->getChildIterator();
				while (it2.hasMoreElements())
				{
					Ogre::SceneNode* node = (Ogre::SceneNode*)(it2.getNext());
					node->setVisible(true);
				}
			}
		}
		else
		{
			Ogre::Node::ChildNodeIterator it = m_pSceneMgr->getSceneNode("Buncher_B_Field")->getChildIterator();
			while (it.hasMoreElements())
			{
				Ogre::Node::ChildNodeIterator it2 = it.getNext()->getChildIterator();
				while (it2.hasMoreElements())
				{
					Ogre::SceneNode* node = (Ogre::SceneNode*)(it2.getNext());
					node->setVisible(false);
				}
			}
			it = m_pSceneMgr->getSceneNode("Buncher_A_Field")->getChildIterator();
			while (it.hasMoreElements())
			{
				Ogre::Node::ChildNodeIterator it2 = it.getNext()->getChildIterator();
				while (it2.hasMoreElements())
				{
					Ogre::SceneNode* node = (Ogre::SceneNode*)(it2.getNext());
					node->setVisible(false);
				}
			}
			it = m_pSceneMgr->getSceneNode("Cavity_Field")->getChildIterator();
			while (it.hasMoreElements())
			{
				Ogre::Node::ChildNodeIterator it2 = it.getNext()->getChildIterator();
				while (it2.hasMoreElements())
				{
					Ogre::SceneNode* node = (Ogre::SceneNode*)(it2.getNext());
					node->setVisible(false);
				}
			}
		}
	}
}


void ElbeState::buttonHit(OgreBites::Button *button)
{
	if (button == NULL)
		return;
	if (button->getName() == "demoButton")
	{
		if (demoMode)
		{
			//Stopping the demo
			demoMode = false;
			OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
			buildGUI();
		}
		else
		{
			//Restarting the demo
			changeAppState(findByName("ElbeState"));
		}
	}
	else
	if (button->getName() == "helpButton")
		showHelp("Elbe");
	else
	if (button->getName() == "screenButton")
		showScreens();
	else
	if (button->getName() == "logButton"){
		m_pPipe->saveTrack("log.txt");
	}
	else if(button->getName() == "dracoButton"){
        changeAppState(findByName("DracoState"));
	}
	else if(button->getName() == "menuButton"){
        changeAppState(findByName("MenuState"));
	}
	else if(button->getName() == "radiButton"){
        changeAppState(findByName("Oger"));
	}
	else if(button->getName() == "collButton"){
        changeAppState(findByName("CollisionState"));
	}
	else if(button->getName() == "freeViewButton")
	{
		if (m_pCamera->getOgreCamera()->getName() != "ElbeCameraFree")
		{
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
				button->setCaption("Orbitsicht");
			else
				button->setCaption("Orbit View");
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
				OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("fixedHeightButton")->show();
			// switch to FreeViewCamera
			m_pCamera->getOgreCamera()->detachFromParent();

			FreeViewCamera* cam = new FreeViewCamera(m_pSceneMgr->createCamera("ElbeCameraFree"));
			cam->setFixedHeight(ElbeState::FIXED_CAMERA_HEIGHT);

			if (fixedHeight)
				cam->enableFixedHeight();
			else
				cam->disableFixedHeight();

			setCamera(cam);


			m_pCamera->getOgreCamera()->setPosition(Vector3(177, 21, -6.3f));
			m_pCamera->getOgreCamera()->lookAt(Vector3(-0.13f, -0.6f, -50.1f));
			m_pCamera->getOgreCamera()->setNearClipDistance(1);

		}
		else
		{
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
				button->setCaption("freie Sicht");
			else
				button->setCaption("Free View");
			if (OgreFramework::getSingletonPtr()->explanationMode == expert)
				OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("fixedHeightButton")->hide();
			// switch to OrbitCamera
			m_pCamera->getOgreCamera()->detachFromParent();

			OrbitCamera* cam = new OrbitCamera(m_pSceneMgr->createCamera("ElbeCameraOrbit"), m_pPipe->getCameraBunch()->getOgreNode());
			cam->setMinDistance(ElbeState::MIN_CAMERA_DISTANCE);
			cam->setMaxDistance(ElbeState::MAX_CAMERA_DISTANCE);

			setCamera(cam);
		}
	}
	else if(button->getName() == "pauseButton")
	{
		if(m_Pause)
		{
			m_Pause = false;
			button->setCaption("Pause");
		}
		else
		{
			m_Pause = true;
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
				button->setCaption("Start");
			else
				button->setCaption("Play");
		}
	}
	else if(button->getName() == "stopButton")
	{
		m_Pause = true;
		OgreBites::Button* play = (OgreBites::Button*) OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("pauseButton");
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
			play->setCaption("Start");
		else
			play->setCaption("Play");

		m_pPipe->reset();
		if (m_pCamera->getOgreCamera()->getName() == "ElbeCameraOrbit") {
			// Recreate to OrbitCamera
			Ogre::Quaternion orientation = ((OrbitCamera*)(m_pCamera))->getCameraTargetNode()->getOrientation();
			m_pCamera->getOgreCamera()->detachFromParent();
			m_pSceneMgr->destroyCamera(m_pCamera->getOgreCamera());
			OrbitCamera* cam = new OrbitCamera(m_pSceneMgr->createCamera("ElbeCameraOrbit"), m_pPipe->getCameraBunch()->getOgreNode());
			cam->setMinDistance(ElbeState::MIN_CAMERA_DISTANCE);
			cam->setMaxDistance(ElbeState::MAX_CAMERA_DISTANCE);
			setCameraWithOutDestroy(cam);
			((OrbitCamera*)(m_pCamera))->getCameraTargetNode()->setOrientation(orientation);
		}

	}
	else if(button->getName() == "helpOkay")
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(HELP_POSITION);
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Hilfe (F1)");
		else
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Help (F1)");
	}
	else if(button->getName() == "descrOkay")
	{
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Hilfe (F1)");
		else
			OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_TOPLEFT, "helpButton","Help (F1)");
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
		if(trackState==1)
			m_pPipe->refreshTrack(trackState!=0);

	}
	else if(button->getName() == "descrDefault")
	{
		if (savedName == "Gun_BB")
		{	
			m_pPipe->setDefaults(GUN_1);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldUGun1"))->setValue(ElbePipe::U_G1);
		}
		if (savedName == "Solenoid_01_BB")
		{	
			m_pPipe->setDefaults(SOLENOID_1);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol1"))->setValue(ElbePipe::I_S1);
		}
		if (savedName == "Solenoid_02_BB")
		{	
			m_pPipe->setDefaults(SOLENOID_2);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol2"))->setValue(ElbePipe::I_S2);
		}
		if (savedName == "Solenoid_03_BB")
		{	
			m_pPipe->setDefaults(SOLENOID_3);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol3"))->setValue(ElbePipe::I_S3);
		}
		if (savedName == "Solenoid_04_BB")
		{	
			m_pPipe->setDefaults(SOLENOID_4);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol4"))->setValue(ElbePipe::I_S4);
		}
		if (savedName == "Solenoid_05_BB")
		{	
			m_pPipe->setDefaults(SOLENOID_5);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol5"))->setValue(ElbePipe::I_S5);
		}

		if (savedName == "Buncher_B_BB") {
			justGUICreation = true;
			m_pPipe->setDefaults(BUNCHER_1);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldUBun1"))->setValue(ElbePipe::U_B1);
			justGUICreation = false;
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldPBun1"))->setValue(ElbePipe::P_B1);
		}

		if (savedName == "Buncher_A_BB") {
			justGUICreation = true;
			m_pPipe->setDefaults(BUNCHER_2);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldUBun2"))->setValue(ElbePipe::U_B2);
			justGUICreation = false;
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldPBun2"))->setValue(ElbePipe::P_B2);
		}
		if (savedName == "Cryos_01_BB")
		{
			justGUICreation = true;
			m_pPipe->setDefaults(CRYOSTAT_1);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1PhaseSlider1"))->setValue(ElbePipe::P1_CRYOS1);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1EpeakSlider1"))->setValue(ElbePipe::E1_CRYOS1);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1PhaseSlider2"))->setValue(ElbePipe::P2_CRYOS1);
			justGUICreation = false;
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1EpeakSlider2"))->setValue(ElbePipe::E2_CRYOS1);
		}
		if (savedName == "Cryos_02_BB")
		{
			justGUICreation = true;
			m_pPipe->setDefaults(CRYOSTAT_2);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2PhaseSlider1"))->setValue(ElbePipe::P1_CRYOS2);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2EpeakSlider1"))->setValue(ElbePipe::E1_CRYOS2);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2PhaseSlider2"))->setValue(ElbePipe::P2_CRYOS2);
			justGUICreation = false;
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2EpeakSlider2"))->setValue(ElbePipe::E2_CRYOS2);
		}
		if (savedName == "QuadroPol_01_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_1);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole1Slider"))->setValue(ElbePipe::I_Q1);
		}
		if (savedName == "QuadroPol_02_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_2);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole2Slider"))->setValue(ElbePipe::I_Q2);
		}
		if (savedName == "QuadroPol_03_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_3);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole3Slider"))->setValue(ElbePipe::I_Q3);
		}
		if (savedName == "QuadroPol_04_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_4);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole4Slider"))->setValue(ElbePipe::I_Q4);
		}
		if (savedName == "QuadroPol_05_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_5);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole5Slider"))->setValue(ElbePipe::I_Q5);
		}
		if (savedName == "QuadroPol_06_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_6);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole6Slider"))->setValue(ElbePipe::I_Q6);
		}
		if (savedName == "QuadroPol_07_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_7);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole7Slider"))->setValue(ElbePipe::I_Q7);
		}
		if (savedName == "QuadroPol_08_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_8);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole8Slider"))->setValue(ElbePipe::I_Q8);
		}
		if (savedName == "QuadroPol_09_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_9);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole9Slider"))->setValue(ElbePipe::I_Q9);
		}
		if (savedName == "QuadroPol_10_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_10);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole10Slider"))->setValue(ElbePipe::I_Q10);
		}
		if (savedName == "QuadroPol_11_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_11);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole11Slider"))->setValue(ElbePipe::I_Q11);
		}
		if (savedName == "QuadroPol_12_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_12);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole12Slider"))->setValue(ElbePipe::I_Q12);
		}
		if (savedName == "QuadroPol_13_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_13);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole13Slider"))->setValue(ElbePipe::I_Q13);
		}
		if (savedName == "QuadroPol_14_BB")
		{
			m_pPipe->setDefaults(QUADRUPOLE_14);
			((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole14Slider"))->setValue(ElbePipe::I_Q14);
		}
	}
	else if(button->getName() == "simpleOptionsButton")
	{
		this->showSimpleOptions();
	}
	else if(button->getName() == "optionsButton")
	{
		this->showOptions();
	}
	else if(button->getName() == "optionsButton_Q")
	{
		this->showOptions_Q();
	}
	else if(button->getName() == "savestatesButton")
	{
		this->showSaveStates();
	}
	else if(button->getName() == "trackButton")
	{
		trackState = (trackState+1)%3;
		switch (trackState)
		{
			case 0:
				if (OgreFramework::getSingletonPtr()->m_iLanguage)
					button->setCaption("Envelope Aus");
				else
					button->setCaption("Envelope Off");
				break;
			case 1:
				if (OgreFramework::getSingletonPtr()->m_iLanguage)
					button->setCaption("Envelope An");
				else
					button->setCaption("Envelope On");
				break;
			case 2:
				if (OgreFramework::getSingletonPtr()->m_iLanguage)
					button->setCaption("Envelope Dyn.");
				else
					button->setCaption("Envelope Dynamic");
				break;
		}
		m_pPipe->refreshTrack(trackState!=0);
	}
	else if(button->getName() == "cancelOptionsButton")
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
	}
	else if(button->getName() == "cancelOptionsButton_Q")
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
	}
	else if(button->getName() == "defaultOptionsButton")
	{
		justGUICreation = true;
		m_pPipe->setDefaults(ALL_CONFIGABLE);

		// apply default values

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldUGun1"))->setValue(ElbePipe::U_G1);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldUBun1"))->setValue(ElbePipe::U_B1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldUBun2"))->setValue(ElbePipe::U_B2);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldPBun1"))->setValue(ElbePipe::P_B1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldPBun2"))->setValue(ElbePipe::P_B2);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1PhaseSlider1"))->setValue(ElbePipe::P1_CRYOS1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1EpeakSlider1"))->setValue(ElbePipe::E1_CRYOS1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1PhaseSlider2"))->setValue(ElbePipe::P2_CRYOS1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1EpeakSlider2"))->setValue(ElbePipe::E2_CRYOS1);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2PhaseSlider1"))->setValue(ElbePipe::P1_CRYOS2);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2EpeakSlider1"))->setValue(ElbePipe::E1_CRYOS2);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2PhaseSlider2"))->setValue(ElbePipe::P2_CRYOS2);
		justGUICreation = false;
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2EpeakSlider2"))->setValue(ElbePipe::E2_CRYOS2);

	}
	else if(button->getName() == "defaultSimpleOptionsButton")
	{
		justGUICreation = true;
		m_pPipe->setDefaults(ALL_CONFIGABLE | ALL_CONFIGABLE_Q);

		// apply default values

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldUGun1"))->setValue(ElbePipe::U_G1);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1PhaseSlider1"))->setValue(ElbePipe::P1_CRYOS1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1EpeakSlider1"))->setValue(ElbePipe::E1_CRYOS1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1PhaseSlider2"))->setValue(ElbePipe::P2_CRYOS1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos1EpeakSlider2"))->setValue(ElbePipe::E2_CRYOS1);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol1"))->setValue(ElbePipe::I_S1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol2"))->setValue(ElbePipe::I_S2);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol3"))->setValue(ElbePipe::I_S3);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol4"))->setValue(ElbePipe::I_S4);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol5"))->setValue(ElbePipe::I_S5);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2PhaseSlider1"))->setValue(ElbePipe::P1_CRYOS2);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2EpeakSlider1"))->setValue(ElbePipe::E1_CRYOS2);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2PhaseSlider2"))->setValue(ElbePipe::P2_CRYOS2);
		justGUICreation = false;
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("cryos2EpeakSlider2"))->setValue(ElbePipe::E2_CRYOS2);

	}
	else if(button->getName() == "defaultOptionsButton_Q")
	{
		justGUICreation = true;
		m_pPipe->setDefaults(ALL_CONFIGABLE_Q);

		// apply default values
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol1"))->setValue(ElbePipe::I_S1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol2"))->setValue(ElbePipe::I_S2);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol3"))->setValue(ElbePipe::I_S3);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol4"))->setValue(ElbePipe::I_S4);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("sldISol5"))->setValue(ElbePipe::I_S5);

		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole1Slider"))->setValue(ElbePipe::I_Q1);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole2Slider"))->setValue(ElbePipe::I_Q2);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole3Slider"))->setValue(ElbePipe::I_Q3);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole4Slider"))->setValue(ElbePipe::I_Q4);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole5Slider"))->setValue(ElbePipe::I_Q5);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole6Slider"))->setValue(ElbePipe::I_Q6);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole7Slider"))->setValue(ElbePipe::I_Q7);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole8Slider"))->setValue(ElbePipe::I_Q8);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole9Slider"))->setValue(ElbePipe::I_Q9);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole10Slider"))->setValue(ElbePipe::I_Q10);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole11Slider"))->setValue(ElbePipe::I_Q11);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole12Slider"))->setValue(ElbePipe::I_Q12);
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole13Slider"))->setValue(ElbePipe::I_Q13);
		justGUICreation = false;
		((OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("quadrupole14Slider"))->setValue(ElbePipe::I_Q14);
	}
	else if(button->getName() == "cancelStateButton")
	{
		// close save state dialog
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
	}
	else if(button->getName() == "restartStateButton")
	{
		m_pPipe->reset();
		// close options dialog
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgetsInTray(OgreBites::TL_LEFT);
	}
	else if(button->getName() == "loadStateButton")
	{	
		OgreBites::SelectMenu* sm; 
		sm = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("savestateswitch"); 
		m_pPipe->loadState(sm->getSelectedItem());
	}
	else if(button->getName() == "saveStateButton")
	{	
		OgreBites::SelectMenu* sm; 
		sm = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("savestateswitch"); 
		m_pPipe->saveState(sm->getSelectedItem());
	}


}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElbeState::sliderMoved(OgreBites::Slider *slider)
{
	if (slider == NULL)
		return;
	if(slider->getName() == "animationSlider")
	{
		float value = slider->getValue()*slider->getValue()/1000.0;
		animationSpeed = value;
		if (OgreFramework::getSingletonPtr()->explanationMode == expert)
			slider->setValueCaption(Ogre::StringConverter::toString(value) + " to 10e9");
		else
		if (OgreFramework::getSingletonPtr()->m_iLanguage)
		{
			if (slider->getValue() < 20)
				slider->setValueCaption("sehr langsam");
			else
			if (slider->getValue() < 40)
				slider->setValueCaption("langsam");
			else
			if (slider->getValue() < 60)
				slider->setValueCaption("mittel");
			else
			if (slider->getValue() < 80)
				slider->setValueCaption("schnell");
			else
				slider->setValueCaption("sehr schnell");
		}
		else
		{
			if (slider->getValue() < 20)
				slider->setValueCaption("very slow");
			else
			if (slider->getValue() < 40)
				slider->setValueCaption("slow");
			else
			if (slider->getValue() < 60)
				slider->setValueCaption("middle");
			else
			if (slider->getValue() < 80)
				slider->setValueCaption("fast");
			else
				slider->setValueCaption("very fast");
		}
		return;
	}
	if(slider->getName() == "screenSlider")
	{
		float detail = ElbePipe::screenPosition - float(int(ElbePipe::screenPosition/1000.0f)*1000);
		ElbePipe::screenPosition = slider->getValue()*1000.0f + detail;
		m_pPipe->updateScreenPosition();
		slider->setValueCaption(Ogre::StringConverter::toString(ElbePipe::screenPosition));
		return;
	}
	if(slider->getName() == "screenDetailSlider")
	{
		float vague = int(ElbePipe::screenPosition/1000.0f)*1000;
		ElbePipe::screenPosition = slider->getValue() + vague;
		m_pPipe->updateScreenPosition();
		slider->setValueCaption("");
		OgreBites::Slider* vagueSlider = (OgreBites::Slider*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("screenSlider");
		vagueSlider->setValueCaption(Ogre::StringConverter::toString(ElbePipe::screenPosition));
		return;
	}

	if (slider->getName() == "sldUGun1")
	{
		ElbePipe::U_G1 = slider->getValue();
		if (OgreFramework::getSingletonPtr()->explanationMode == standard)
		{
			float ekin = ElbePipe::U_G1 * ElbeConstants::QS;
			float gamma = ekin/ElbeConstants::EE+1.0f;
			float beta = sqrt(1.0f-1.0f/(gamma*gamma));
			if (OgreFramework::getSingletonPtr()->m_iLanguage)
				slider->setValueCaption(Ogre::StringConverter::toString(beta*100.0f,2)+ "% der Lichtgeschw.");
			else
				slider->setValueCaption(Ogre::StringConverter::toString(beta*100.0f,2)+ "% of the lightspeed");
		}
	}
	if (slider->getName() == "sldISol1")
		ElbePipe::I_S1 = slider->getValue();
	if (slider->getName() == "sldISol2")
		ElbePipe::I_S2 = slider->getValue();
	if (slider->getName() == "sldISol3")
		ElbePipe::I_S3 = slider->getValue();
	if (slider->getName() == "sldISol4")
		ElbePipe::I_S4 = slider->getValue();
	if (slider->getName() == "sldISol5")
		ElbePipe::I_S5 = slider->getValue();

	if (slider->getName() == "sldUBun1")
		ElbePipe::U_B1 = slider->getValue();
	if (slider->getName() == "sldPBun1")
		ElbePipe::P_B1 = slider->getValue();
	if (slider->getName() == "sldUBun2")
		ElbePipe::U_B2 = slider->getValue();
	if (slider->getName() == "sldPBun2")
		ElbePipe::P_B2 = slider->getValue();

	if (slider->getName() == "cryos1PhaseSlider1")
		ElbePipe::P1_CRYOS1 = slider->getValue();
	if (slider->getName() == "cryos1EpeakSlider1")
		ElbePipe::E1_CRYOS1 = slider->getValue();
	if (slider->getName() == "cryos1PhaseSlider2")
		ElbePipe::P2_CRYOS1 = slider->getValue();
	if (slider->getName() == "cryos1EpeakSlider2")
		ElbePipe::E2_CRYOS1 = slider->getValue();
	
	if (slider->getName() == "cryos2PhaseSlider1")
		ElbePipe::P1_CRYOS2 = slider->getValue();
	if (slider->getName() == "cryos2EpeakSlider1")
		ElbePipe::E1_CRYOS2 = slider->getValue();
	if (slider->getName() == "cryos2PhaseSlider2")
		ElbePipe::P2_CRYOS2 = slider->getValue();
	if (slider->getName() == "cryos2EpeakSlider2")
		ElbePipe::E2_CRYOS2 = slider->getValue();

	if (slider->getName() == "quadrupole1Slider")
	{
		ElbePipe::I_Q1 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole2Slider")
	{
		ElbePipe::I_Q2 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole3Slider")
	{
		ElbePipe::I_Q3 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole4Slider")
	{
		ElbePipe::I_Q4 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole5Slider")
	{
		ElbePipe::I_Q5 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole6Slider")
	{
		ElbePipe::I_Q6 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole7Slider")
	{
		ElbePipe::I_Q7 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole8Slider")
	{
		ElbePipe::I_Q8 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole9Slider")
	{
		ElbePipe::I_Q9 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole10Slider")
	{
		ElbePipe::I_Q10 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole11Slider")
	{
		ElbePipe::I_Q11 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole12Slider")
	{
		ElbePipe::I_Q12 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole13Slider")
	{
		ElbePipe::I_Q13 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "quadrupole14Slider")
	{
		ElbePipe::I_Q14 = slider->getValue();
		if (fabs(slider->getValue())<0.00001)
			slider->setValueCaption("0.0");
		else
			slider->setValueCaption(Ogre::StringConverter::toString(Ogre::Real(slider->getValue())));
	}
	if (slider->getName() == "tracksSlider")
	{
		track_count = slider->getValue();
		ElbeBunch* bunch = m_pPipe->getCameraBunch();
		if (bunch)
			bunch->updateParticle(track_count);
	}

	if(trackState==2 && !justGUICreation)
	{
		m_pPipe->refreshTrack(trackState!=0);
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||
