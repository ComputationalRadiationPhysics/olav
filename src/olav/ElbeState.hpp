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

#ifndef ELBE_STATE_HPP
#define ELBE_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"
#include "ElbeBunch.hpp"
#include "ElbePipe.hpp"
#include "DotSceneLoader.hpp"
#include "DocumentationManager.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>


//|||||||||||||||||||||||||||||||||||||||||||||||

class tElbeDemoType {
	public:
	tElbeDemoType ()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->time = 0;
		this->caption = "";
		this->text = "";
		this->speed = 0.0f;
		this->envelope = false;
	}
	tElbeDemoType (float x, float y, float z, int time,std::string caption,std::string text,float speed, bool envelope)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->time = time;
		this->caption = caption;
		this->text = text;
		this->speed = speed;
		this->envelope = envelope;
	}

	float x,y,z;
	int time;
	std::string caption,text;
	float speed;
	bool envelope;
};

class ElbeState : public AppState
{
public:
	ElbeState();

	DECLARE_APPSTATE_CLASS(ElbeState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void moveCamera();
	void getInput();
    void buildGUI();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	void itemSelected(OgreBites::SelectMenu *selectMenu);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	void checkBoxToggled(OgreBites::CheckBox *checkbox);
	void buttonHit(OgreBites::Button *button);
	void onLeftPressed(const OIS::MouseEvent &evt);
	void sliderMoved(OgreBites::Slider *slider);

	void update(double timeSinceLastFrame);

	void showDescription(Ogre::Entity* current);
	void showScreens();
	void showOptions();
	void showOptions_Q();
	void showSimpleOptions();
	void showSaveStates();
	bool updateDemo();
	void refillDemoList();

	// Camera control
	static float			MIN_CAMERA_DISTANCE;		// minimum distance between bunch and orbit/chase camera
	static float			MAX_CAMERA_DISTANCE;		// maximum distance between bunch and orbit/chase camera
	static float			FIXED_CAMERA_HEIGHT;		// default fixed heigth over ground for freeview camera
	static int				followed_random_seed;

	// Bunch speed
	static const float		BUNCH_SPEED;				// the initial walkspeed of the bunch

private:
	bool show_billboard;
	int volume_state;
	void triggerElbeMap(bool mousepressed);
    OgreBites::ParamsPanel*		m_pDetailsPanel;
	bool						m_bQuit;

	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed;
	Ogre::Degree				m_RotateSpeed;
	float						m_MoveScale;
	Ogre::Degree				m_RotScale;

	Ogre::RaySceneQuery*		m_pRSQ;
	Ogre::SceneNode*			m_pCurrentObject;
	Ogre::Entity*				m_pCurrentEntity;
	bool						m_bLMouseDown, m_bRMouseDown;
	bool						m_bSettingsMode;
	bool						m_bHidden;
	bool						m_Pause;				// Animation paused?

	OgreBites::TextBox*			description;
	DocumentationManager*		mDocMgr;
	DocumentationManager*		mDemoDoc;

	ElbePipe*					m_pPipe;

	double						animationSpeed;

	std::string					savedName;
	int							trackState;
	bool						justGUICreation;
	bool						details;
	bool						fixedHeight;
	bool						coverVisible;
	bool						explosion;
	std::list<Ogre::AxisAlignedBox> collisionBox;
	int mouseX,mouseY;
	bool mousepressed;
	bool showArrows;
	Ogre::String				defaultString;
	Ogre::String				defaultsString;
	Ogre::String				closeString;
	int demoTime;
	int demoPoint;
	int once;
	std::list<tElbeDemoType> demoList;
	tElbeDemoType demoLast;
	bool demoMode;
	int track_count;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
