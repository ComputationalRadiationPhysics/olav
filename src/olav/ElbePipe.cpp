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

#include "ElbePipe.hpp"
#include "OgreFramework.hpp"
#include "OrbitCamera.hpp"
#include "globale_defines.h"
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

int ElbePipe::particle_count = 6000;

const double ElbePipe::PATHSTART_POS = 82.97;
const double ElbePipe::EMITTER_POS = 79.9;
const double ElbePipe::SOLENOID_1_POS = 72.6;
const double ElbePipe::BUNCHER_1_POS = 67.73;
const double ElbePipe::SOLENOID_2_POS = 59.357;
const double ElbePipe::MAKROPULSER_POS = 50.0;
const double ElbePipe::SOLENOID_3_POS = 45.39;
const double ElbePipe::SOLENOID_4_POS = 36.7;
const double ElbePipe::BUNCHER_2_POS = 34.23;
const double ElbePipe::SOLENOID_5_POS = 28.33;
const double ElbePipe::CAVITY_START_POS = 21.97; //21.9755;
const double ElbePipe::CAVITY_END_POS = 11.7; //11.8;
const double ElbePipe::CAVITY_START_POS_2 = 8.63; //57 //8.689;
const double ElbePipe::CAVITY_END_POS_2 = -1.53; //-1.45332;
const double ElbePipe::QUADRUPOL_1_POS = -11.89;
const double ElbePipe::QUADRUPOL_2_POS = -17.29;
const double ElbePipe::QUADRUPOL_3_POS = -19.59;
const double ElbePipe::QUADRUPOL_4_POS = -44.31;
const double ElbePipe::QUADRUPOL_5_POS = -46.57;
const double ElbePipe::CHICANE_1_POS = -54.89; //not correct
const double ElbePipe::CHICANE_2_POS = -63.56; //not correct
const double ElbePipe::CHICANE_3_POS = -72.22; //not correct
const double ElbePipe::CHICANE_4_POS = -80.89; //not correct
const double ElbePipe::QUADRUPOL_6_POS = -89.31;
const double ElbePipe::QUADRUPOL_7_POS = -92.07;
const double ElbePipe::CAVITY_START_POS_3 = -102.55;
const double ElbePipe::CAVITY_END_POS_3 = -112.74;
const double ElbePipe::CAVITY_START_POS_4 = -115.84;
const double ElbePipe::CAVITY_END_POS_4 = -126.02;
const double ElbePipe::QUADRUPOL_8_POS = -136.13;
const double ElbePipe::QUADRUPOL_9_POS = -138.88;
const double ElbePipe::QUADRUPOL_10_POS = -150.81;
const double ElbePipe::QUADRUPOL_11_POS = -165.95;
const double ElbePipe::QUADRUPOL_12_POS = -181.95;
const double ElbePipe::QUADRUPOL_13_POS = -195.33;
const double ElbePipe::QUADRUPOL_14_POS = -198.85;
const double ElbePipe::END_POS = -200.60;


const double ElbePipe::L_DT0 = 307.0; //3.07
const double ElbePipe::L_DT1 = 750.0; //7.3
const double ElbePipe::L_DT2 = 470.0; //4.87
const double ElbePipe::L_DT3 = 850.0; //8.373
const double ElbePipe::L_DT4 = 1400.0; //9.357 +4.61
const double ElbePipe::L_DT5 = 860.0; //8.69
const double ElbePipe::L_DT6 = 250.0; //2.47
const double ElbePipe::L_DT7 = 590.0; //5.9
const double ElbePipe::L_DT8 = 650.0; //6.36
const double ElbePipe::L_DT9 = 1038.0; //10.2 Cyrostat 1
const double ElbePipe::L_DT10 = 310.0; //3.2 between the Cyrostats
const double ElbePipe::L_DT11 = 1038.0; //10.1 Cyrostat 2
const double ElbePipe::L_DT12 = 950.0; // Cryostat 1 -> Quadropol 1
const double ElbePipe::L_DT13 = 550.0; // Quadropol 1 -> Quadropol 2
const double ElbePipe::L_DT14 = 230.0; // Quadropol 2 -> Quadropol 3
const double ElbePipe::L_DT15 = 2480.0; // Quadropol 3 -> Quadropol 4
const double ElbePipe::L_DT16 = 230.0; // Quadropol 4 -> Quadropol 5
const double ElbePipe::L_DT17 = 4310.0; // Quadropol 5 -> Quadropol 6
const double ElbePipe::L_DT18 = 270.0; // Quadropol 6 -> Quadropol 7
const double ElbePipe::L_DT19 = 930.0; // Quadropol 7 -> Cryosat 2
const double ElbePipe::L_DT20 = 1038.0; // Cryostat 2.1
const double ElbePipe::L_DT21 = 310.0; // Cryostat 2 break
const double ElbePipe::L_DT22 = 1038.0; // Cryostat 2.2  
const double ElbePipe::L_DT23 = 920.0; // Cryostat 2 -> Quadropol 8
const double ElbePipe::L_DT24 = 270.0; // Quadropol 8 -> Quadropol 9
const double ElbePipe::L_DT25 = 1250.0; // Quadropol 9 -> Quadropol 10
const double ElbePipe::L_DT26 = 1580.0; // Quadropol 10 -> Quadropol 11
const double ElbePipe::L_DT27 = 1540.0; // Quadropol 11 -> Quadropol 12
const double ElbePipe::L_DT28 = 1290.0; // Quadropol 12 -> Quadropol 13
const double ElbePipe::L_DT29 = 300.0; // Quadropol 13 -> Quadropol 14
const double ElbePipe::L_DT30 = 100.0; // Quadropol 14 -> End


double ElbePipe::U_G1 = 250000;
double ElbePipe::I_S1 = 1.05;
double ElbePipe::I_S2 = 0.97;
double ElbePipe::I_S3 = 1.12;
double ElbePipe::I_S4 = 1.25;
double ElbePipe::I_S5 = 1.15;
double ElbePipe::U_B1 = 19000.0;
double ElbePipe::U_B2 = 12000.0;
double ElbePipe::P_B1 = -90.0;
double ElbePipe::P_B2 = 90.0;
double ElbePipe::A_B = 40.0;
double ElbePipe::F_B1 = 260000000.0;
double ElbePipe::F_B2 = 1300000000.0;
double ElbePipe::P1_CRYOS1 = -41.0;
double ElbePipe::E1_CRYOS1 = 20.0;
double ElbePipe::P2_CRYOS1 = 0.0;
double ElbePipe::E2_CRYOS1 = 20.0;
double ElbePipe::P1_CRYOS2 = 0.0;
double ElbePipe::E1_CRYOS2 = 20.0;
double ElbePipe::P2_CRYOS2 = 0.0;
double ElbePipe::E2_CRYOS2 = 20.0;

double ElbePipe::I_Q1 = 0.0;
double ElbePipe::I_Q2 = 0.0;
double ElbePipe::I_Q3 = 0.0;
double ElbePipe::I_Q4 = 0.0;
double ElbePipe::I_Q5 = 0.0;
double ElbePipe::I_Q6 = 0.0;
double ElbePipe::I_Q7 = 0.0;
double ElbePipe::I_Q8 = 0.0;
double ElbePipe::I_Q9 = 0.0;
double ElbePipe::I_Q10 = 0.0;
double ElbePipe::I_Q11 = 0.0;
double ElbePipe::I_Q12 = 0.0;
double ElbePipe::I_Q13 = 0.0;
double ElbePipe::I_Q14 = 0.0;

double ElbePipe::U_G1_DEFAULT = 250000;
double ElbePipe::I_S1_DEFAULT = 1.05;
double ElbePipe::I_S2_DEFAULT = 0.97;
double ElbePipe::I_S3_DEFAULT = 1.12;
double ElbePipe::I_S4_DEFAULT = 1.25;
double ElbePipe::I_S5_DEFAULT = 1.15;
double ElbePipe::U_B1_DEFAULT = 19000.0;
double ElbePipe::U_B2_DEFAULT = 12000.0;
double ElbePipe::P_B1_DEFAULT = -90.0;
double ElbePipe::P_B2_DEFAULT = 90.0;
double ElbePipe::A_B_DEFAULT = 40.0;
double ElbePipe::F_B1_DEFAULT = 260000000.0;
double ElbePipe::F_B2_DEFAULT = 1300000000.0;
double ElbePipe::P1_CRYOS1_DEFAULT = 0.0;
double ElbePipe::E1_CRYOS1_DEFAULT = 20.0;
double ElbePipe::P2_CRYOS1_DEFAULT = 0.0;
double ElbePipe::E2_CRYOS1_DEFAULT = 20.0;
double ElbePipe::P1_CRYOS2_DEFAULT = 0.0;
double ElbePipe::E1_CRYOS2_DEFAULT = 20.0;
double ElbePipe::P2_CRYOS2_DEFAULT = 0.0;
double ElbePipe::E2_CRYOS2_DEFAULT = 20.0;
double ElbePipe::I_Q1_DEFAULT = 0.0;
double ElbePipe::I_Q2_DEFAULT = 0.0;
double ElbePipe::I_Q3_DEFAULT = 0.0;
double ElbePipe::I_Q4_DEFAULT = 0.0;
double ElbePipe::I_Q5_DEFAULT = 0.0;
double ElbePipe::I_Q6_DEFAULT = 0.0;
double ElbePipe::I_Q7_DEFAULT = 0.0;
double ElbePipe::I_Q8_DEFAULT = 0.0;
double ElbePipe::I_Q9_DEFAULT = 0.0;
double ElbePipe::I_Q10_DEFAULT = 0.0;
double ElbePipe::I_Q11_DEFAULT = 0.0;
double ElbePipe::I_Q12_DEFAULT = 0.0;
double ElbePipe::I_Q13_DEFAULT = 0.0;
double ElbePipe::I_Q14_DEFAULT = 0.0;

const double ElbePipe::ARROW_SCALE = 0.0152703;

const double ElbePipe::SYNC_FACTOR = 0.8866995 * ElbeConstants::PI;

double ElbePipe::globalTime = 0;
double ElbePipe::oldGlobalTime = 0;

int ElbePipe::counter = 0;

float ElbePipe::screenPosition = 0.0f;
float ElbePipe::screenW = 0.0f;
float ElbePipe::screenH = 0.0f;

bool ElbePipe::firstTime = true;

#define setScreenTexturePixel(x,y,r,g,b,a) 			{pDest[(x+(y)*SCREEN_WIDTH)*4+0] = b; \
			pDest[(x+(y)*SCREEN_WIDTH)*4+1] =   g; \
			pDest[(x+(y)*SCREEN_WIDTH)*4+2] =   r; \
			pDest[(x+(y)*SCREEN_WIDTH)*4+3] =   a; }

#define drawArrow_B(position,angle) \
{ \
	Ogre::ColourValue color; \
	color.setHSB(node->getPosition().length()/8.0f,1.0,1.0f); \
	Ogre::Billboard* b = billboardSet->createBillboard(Ogre::Vector3(position),color); \
	b->setDimensions(40.0f,40.0f); \
	b->setRotation(Ogre::Radian(angle)); \
	billboardBList.push_back(b); \
}

#define drawArrow_A(position,angle) \
{ \
	Ogre::ColourValue color; \
	color.setHSB(node->getPosition().length()/3.0f,1.0,1.0f); \
	Ogre::Billboard* b = billboardSet->createBillboard(Ogre::Vector3(position),color); \
	float mult = 1.0f; \
	if (wrongLine[0] == number || wrongLine[1] == number) \
		mult = 0.00745455f; \
	b->setDimensions(40.0f*mult,40.0f*mult); \
	b->setRotation(Ogre::Radian(angle)); \
	billboardAList.push_back(b); \
	billboardAListMultiplicator.push_back(mult); \
}

#define drawArrow_C(position) \
{ \
	Ogre::SceneNode* subNode = (Ogre::SceneNode*)(node->createChild(Ogre::Vector3(0.0f,0.0f,0.0f))); \
	Ogre::Vector3 add_this = Ogre::Vector3(0.0f,0.0f,0.0f);\
	for (int c = 0; c < 36; c++) \
	{ \
		if (c == 9) \
			add_this = Ogre::Vector3(795.0f,0.0f,0.0f); \
		if (c == 18) \
			add_this = Ogre::Vector3(26840.0f+0.0f,0.0f,0.0f); \
		if (c == 27) \
			add_this = Ogre::Vector3(26840.0f+795.0f,0.0f,0.0f); \
		Ogre::SceneNode* arrowNode = (Ogre::SceneNode*)(subNode->createChild(Ogre::Vector3(position))); \
		arrowNode->setScale(1.0f,1.0f,2.0f); \
		Ogre::Billboard* b;\
		Ogre::ColourValue color; \
		color.setHSB(node->getPosition().length()*0.8f-0.35f,1.0,1.0f); \
		float angle; \
		if ((arrowNode->_getDerivedPosition()-buncher->_getDerivedPosition()).y<-0.1f) \
		{ \
			b = billboardSet->createBillboard(arrowNode->getPosition()+Ogre::Vector3((float)c*292.0f,0.0f,0.0f)+add_this,color);\
			angle = -(arrowNode->_getDerivedPosition()-buncher->_getDerivedPosition()).x*1.0f; \
		} \
		else \
		{ \
			b = billboardSet->createBillboard(arrowNode->getPosition()-Ogre::Vector3((float)c*292.0f,0.0f,0.0f)-add_this,color);\
			angle = (arrowNode->_getDerivedPosition()-buncher->_getDerivedPosition()).x*1.0f; \
		} \
		if (!((c % 9) & 1)) \
			angle += M_PI; \
		b->setDimensions(40.0f,40.0f); \
		b->setRotation(Ogre::Radian(angle)); \
		if (c < 9) \
			billboardC1_1List.push_back(b); \
		else \
		if (c < 18) \
			billboardC1_2List.push_back(b); \
		else \
		if (c < 27) \
			billboardC2_1List.push_back(b); \
		else \
			billboardC2_2List.push_back(b); \
		subNode->removeChild(arrowNode); \
		delete arrowNode; \
	} \
	node->removeChild(subNode); \
	delete subNode;	\
}

#define drawArrow_CM(position) drawArrow_C(position)
/*{ \
	Ogre::SceneNode* subNode = (Ogre::SceneNode*)(node->createChild(Ogre::Vector3(0.0f,0.0f,0.0f))); \
	Ogre::Vector3 add_this = Ogre::Vector3(0.0f,0.0f,0.0f);\
	for (int c = 0; c < 36; c++) \
	{ \
		if (c == 9) \
			add_this = Ogre::Vector3(795.0f,0.0f,0.0f); \
		if (c == 18) \
			add_this = Ogre::Vector3(26840.0f+0.0f,0.0f,0.0f); \
		if (c == 27) \
			add_this = Ogre::Vector3(26840.0f+795.0f,0.0f,0.0f); \
		Ogre::SceneNode* arrowNode = (Ogre::SceneNode*)(subNode->createChild(Ogre::Vector3(position))); \
		arrowNode->setScale(1.0f,1.0f,2.0f); \
		float angle = (arrowNode->_getDerivedPosition()-buncher->_getDerivedPosition()).x*1.0f; \
		Ogre::Billboard* b;\
		Ogre::ColourValue color = Ogre::ColourValue(1.0,1.0,1.0); \
		if ((arrowNode->_getDerivedPosition()-buncher->_getDerivedPosition()).y<-0.1f) \
			b = billboardSet->createBillboard(arrowNode->getPosition()+Ogre::Vector3((float)c*292.0f,0.0f,0.0f)+add_this,color);\
		else \
			b = billboardSet->createBillboard(arrowNode->getPosition()-Ogre::Vector3((float)c*292.0f,0.0f,0.0f)-add_this,color);\
		b->setDimensions(20.0f,20.0f); \
		b->setRotation(Ogre::Radian(angle)); \
		if (c < 18) \
			billboardC1List.push_back(b); \
		else \
			billboardC2List.push_back(b); \
		subNode->removeChild(arrowNode); \
		delete arrowNode; \
	} \
	node->removeChild(subNode); \
	delete subNode;	\
}*/

ElbePipe::ElbePipe(Ogre::SceneManager* m_pSceneMgr)
{
	this->m_pSceneMgr = m_pSceneMgr;
	counter = 0;
	globalTime = ElbeConstants::P_026PS - 100.0;
	oldGlobalTime = globalTime;
	this->m_pSceneMgr = m_pSceneMgr;

	for (int i = 1; i<15; i++)
	{
		Ogre::String name = "QuadroPol_";
		if (i<10)
			name += "0";
		name+=Ogre::StringConverter::toString(i);
		quadropolPos.push_back(m_pSceneMgr->getSceneNode(name)->_getDerivedPosition());
		Ogre::Node::ChildNodeIterator it = m_pSceneMgr->getSceneNode(name)->getChildIterator();
		std::vector<Ogre::MaterialPtr> newQuadropol;
		while (it.hasMoreElements())
		{
			Ogre::Entity* entity = ((Ogre::Entity*)(((Ogre::SceneNode*)it.getNext())->getAttachedObjectIterator().getNext()));
			//entity->setRenderQueueGroupAndPriority(entity->getRenderQueueGroup(),-32768);
			Ogre::MaterialPtr material = Ogre::MaterialManager::getSingletonPtr()->create(entity->getName()+"_Mat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			Ogre::MaterialPtr oldMaterial = entity->getSubEntity(0)->getMaterial();

			material->getTechnique(0)->createPass();

			material->setAmbient(oldMaterial->getTechnique(0)->getPass(0)->getAmbient());
			material->setDiffuse(oldMaterial->getTechnique(0)->getPass(0)->getDiffuse());
			material->setSpecular(oldMaterial->getTechnique(0)->getPass(0)->getSpecular());
			material->setShininess(oldMaterial->getTechnique(0)->getPass(0)->getShininess());

			material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(true);
			//material->getTechnique(0)->getPass(0)->setTransparentSortingEnabled(true);
			//material->getTechnique(0)->getPass(0)->setTransparentSortingForced(true);
			material->getTechnique(0)->getPass(0)->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL,3);
			material->getTechnique(0)->getPass(0)->setColourWriteEnabled(false);

			material->getTechnique(0)->getPass(1)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
			material->getTechnique(0)->getPass(1)->setDepthWriteEnabled(false);
			//material->getTechnique(0)->getPass(1)->setTransparentSortingEnabled(true);
			//material->getTechnique(0)->getPass(1)->setTransparentSortingForced(true);
			material->getTechnique(0)->getPass(1)->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL,3);
			entity->getSubEntity(0)->setMaterial(material);
			newQuadropol.push_back(material);
		}
		quadropol.push_back(newQuadropol);
	}
	Ogre::MaterialPtr mat_arrow = Ogre::MaterialManager::getSingletonPtr()->create("Elbe_Arrow_Mat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	mat_arrow->setDiffuse(1.0f, 0.55f, 0.0f, 1.0f);

	for (int i = 1;i <=18;i++)
	{
		char buffer[256];
		sprintf(buffer,"arrows_w%02i",i);
		m_pSceneMgr->getSceneNode(buffer)->setVisible(false);
		sprintf(buffer,"brrows_w%02i",i);
		m_pSceneMgr->getSceneNode(buffer)->setVisible(false);
	}

#ifdef ELBE_DRAW_ARROWS
	billboardAList.clear();
	billboardAListMultiplicator.clear();
	billboardBList.clear();
	billboardC1_1List.clear();
	billboardC2_2List.clear();
	billboardC1_1List.clear();
	billboardC2_2List.clear();
#endif

	Ogre::SceneNode* emitter = m_pSceneMgr->getSceneNode("Emitter");
	emitterField = m_pSceneMgr->createManualObject();
	emitterField->setDynamic(true);
	emitterField->begin("EmitterField", Ogre::RenderOperation::OT_TRIANGLE_FAN);
		emitterField->position( 0.0f, 0.0f,  0.28f);
		emitterField->colour(0,1,0);
		emitterField->textureCoord(10,0);
		emitterField->position(-3.0f, 0.0f,  0.28f);
		emitterField->colour(1,0,0);
		emitterField->textureCoord(0,0);
		emitterField->position(-3.0f, 0.0f, -0.28f);
		emitterField->colour(1,0,0);
		emitterField->textureCoord(0,1);
		emitterField->position( 0.0f, 0.0f, -0.28f);
		emitterField->colour(0,1,0);
		emitterField->textureCoord(10,1);
	emitterField->end();
	emitterFieldPosition = 0;

	emitter->attachObject(emitterField);

	holdNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	holdNode->setPosition(Ogre::Vector3(80.6301f,13.7751f,-17.3819f));

	fakeBunchNode = holdNode->createChildSceneNode();
	fakeBunch = new ElbeBunch(m_pSceneMgr, fakeBunchNode);
	fakeBunch->getOgreNode()->setVisible(false);
	cameraBunch = NULL;
	newCamera = false;
	setDefaults(ALL_CONFIGABLE);
	setDefaults(ALL_CONFIGABLE_Q);
	track = NULL;
	track2 = NULL;
	refreshTrack(false);

	screenEntity = m_pSceneMgr->createEntity("elbe_screen.mesh");
	screenEntity->setCastShadows(false);
	screenEntity->setVisible(true);
	screenNode = emitter->createChildSceneNode();
	screenNode->attachObject(screenEntity);
	//Creating ScreenTexture
	screenTexture = Ogre::TextureManager::getSingleton().createManual(
		"screenTexture", // name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,      // type
		SCREEN_WIDTH, SCREEN_WIDTH,         // width & height
		0,                // number of mipmaps
		Ogre::PF_BYTE_BGRA,     // pixel format
		Ogre::TU_DYNAMIC);      // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
						  // textures updated very often (e.g. each frame)
 
	screenMaterial = NULL;

	// Create a material using the texture
	if (screenMaterial)
	{
		Ogre::MaterialManager::getSingleton().remove("screenTextureMaterial");
		Ogre::OverlayManager::getSingleton().destroyOverlayElement("screenTexturePanel",true);
	}
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
		"screenTextureMaterial", // name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	screenMaterial = material.getPointer();
 
	screenMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("screenTexture");
	screenMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	screenMaterial->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
	screenMaterial->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	screenMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);

	if (firstTime)
	{
		Ogre::OverlayContainer* panel = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "screenTexturePanel", true));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setDimensions(SCREEN_WIDTH,SCREEN_WIDTH);
		panel->setMaterialName("screenTextureMaterial");

		panel = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "elbeMapPanel", true));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setDimensions(400,67);
		panel->setMaterialName("elbe_map");
	}
	firstTime = false;

	memset(screenGauss,0,SCREEN_WIDTH*SCREEN_WIDTH*sizeof(float));
	oldScreenH = 0.0f;
	oldScreenW = 0.0f;

	//Creating MapTexture
	screenMap = Ogre::TextureManager::getSingleton().createManual(
		"screenMap", // name
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D,      // type
		400, 67,         // width & height
		0,                // number of mipmaps
		Ogre::PF_BYTE_BGRA,     // pixel format
		Ogre::TU_DYNAMIC);      // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
						  // textures updated very often (e.g. each frame)
	Ogre::MaterialPtr mapMaterial = Ogre::MaterialManager::getSingleton().getByName("elbe_map");
	mapMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("screenMap");

	refreshScreenTexture();
	//Field Arrow Colors:
	for (int i = 0; i < 20; i++)
	{
		arrowFieldMaterial[i] = Ogre::MaterialManager::getSingletonPtr()->create("arrowFieldMaterial"+Ogre::StringConverter::toString(i), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::Pass* pass = arrowFieldMaterial[i]->getTechnique(0)->createPass();
		Ogre::TextureUnitState* tex = pass->createTextureUnitState();
		if (i<10)
			tex->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(1.0f,float(i)/9.0f,0.0f,1.0f)*0.8f);
		else
			tex->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(1.0f-float(i-10)/9.0f,1.0f,0.0f,1.0f)*0.8f);
		arrowFieldMaterial[i]->setCullingMode(Ogre::CULL_NONE);
	}

#ifdef ELBE_DRAW_ARROWS
	//+------------------------+
	//| Field Arrows Buncher B |
	//+------------------------+
	Ogre::SceneNode* buncher = m_pSceneMgr->getSceneNode("Buncher_B_Field");
	Ogre::Node::ChildNodeIterator it = buncher->getChildIterator();
	int number = 0;
	while (it.hasMoreElements())
	{
		Ogre::SceneNode* node = (Ogre::SceneNode*)(it.getNext());
		Ogre::BillboardSet* billboardSet = m_pSceneMgr->createBillboardSet();
		billboardSet->setBillboardType(Ogre::BBT_POINT);
		billboardSet->setMaterialName("Examples/ElectronArrow");
		Ogre::SceneNode::ObjectIterator ot = node->getAttachedObjectIterator();
		if (ot.hasMoreElements())
		{
			float extra_fix = 0.0f;
			float extra_sign = 1.0f;
			switch (number)
			{
				case 20:
				case 57:
				case 32:
				case 24:
				case 51:
				case 58:
				case 8:
				case 16:
				case 15:
				case 35:
				case 44:
				case 29:
				case 18:
				case 0:
				case 40:
				case 47:
				case 34:
				case 41:
				case 55:
				case 48:
				case 12:
				case 42:
				case 7:
				case 25:
				case 30:
					extra_sign = -1.0f;
			}
			Ogre::Entity* entity = static_cast<Ogre::Entity*>(ot.getNext());
			entity->getSubEntity(0)->getMaterial()->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);
			Ogre::MeshPtr line = entity->getMesh();
			PathParser* pp = new PathParser(line);
			/*if (pp->pointList->at(0)->position.x < 0.0f)
			{
				extra_sign = 0.0f;
				extra_fix = M_PI/2.0f;
			}*/
			
			//Calculating the length of the fieldline
			float length = 0.0f;
			for(unsigned int i = 1; i < pp->pointList->size(); i++)
				length+=(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
			//Finding the last point BEFORE the middle
			float position = 0;
			int i = 1;
			while (position < length/2.0f) //it stops at the point AFTER the middle
			{
				position+=(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				i++;
			}
			//Setting i and position to the point BEFORE the middle
			i--;
			position -= (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
			//Calculating the way from the point before the middle to the middle:
			float partPosition = length/2.0f - position;
			//Calculating the way from the point before the middle to the point after the middle:
			float distance = (pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length();
			int middle_i = i;
			Ogre::Vector3 middle = pp->pointList->at(middle_i)->position;// + partPosition/distance*(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position);

			float ac = (pp->pointList->at(middle_i+1)->position-pp->pointList->at(middle_i-1)->position).x /(pp->pointList->at(middle_i+1)->position-pp->pointList->at(middle_i-1)->position).length();
			if (ac <= -0.99f)
				ac = -0.99f;
			if (ac >= 0.99f)
				ac = 0.99f;
			float angle = extra_fix+extra_sign*(M_PI/2.0f-acos(ac));
			drawArrow_B(middle,angle)
			float way_to_go = ELBE_ARROW_DISTANCE_B;
			i = middle_i-1;
			float diff = (pp->pointList->at(i)->position-pp->pointList->at(i+1)->position).length();
			//left
			while (i>=0)
			{
				while (way_to_go > diff)
				{
					i--;
					if (i<0)
						break;
					way_to_go -= diff;
					diff = (pp->pointList->at(i)->position-pp->pointList->at(i+1)->position).length();
				}
				if (i<0)
					break;
				//now way_to_go is smaller(equal) as diff.
				float ac = (pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).x /(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length();
				if (ac <= -0.99f)
					ac = -0.99f;
				if (ac >= 0.99f)
					ac = 0.99f;
				float angle = extra_fix+extra_sign*(M_PI/2.0f-acos(ac));
				drawArrow_B(pp->pointList->at(i+1)->position-way_to_go/(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length()*(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position),angle)
				diff-=way_to_go;
				way_to_go = ELBE_ARROW_DISTANCE_B;
			}
			//right
			way_to_go = ELBE_ARROW_DISTANCE_B;
			i = middle_i+1;
			diff = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
			while (i<pp->pointList->size())
			{
				while (way_to_go > diff)
				{
					i++;
					if (i>=pp->pointList->size())
						break;
					way_to_go -= diff;
					diff = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				}
				if (i>=pp->pointList->size())
					break;
				//now way_to_go is smaller(equal) as diff.
				float ac = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).x /(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				if (ac <= -0.99f)
					ac = -0.99f;
				if (ac >= 0.99f)
					ac = 0.99f;
				float angle = extra_fix+extra_sign*(M_PI/2.0f-acos(ac));
				drawArrow_B(pp->pointList->at(i-1)->position-way_to_go/(pp->pointList->at(i-1)->position-pp->pointList->at(i)->position).length()*(pp->pointList->at(i-1)->position-pp->pointList->at(i)->position),angle)
				diff-=way_to_go;
				way_to_go = ELBE_ARROW_DISTANCE_B;
			}
			delete pp;
		}
		node->attachObject(billboardSet);
		number++;
	}

	//+------------------------+
	//| Field Arrows Buncher A |
	//+------------------------+
	buncher = m_pSceneMgr->getSceneNode("Buncher_A_Field");	
	it = buncher->getChildIterator();
	number = 0;
	while (it.hasMoreElements())
	{
		Ogre::SceneNode* node = (Ogre::SceneNode*)(it.getNext());
		if (node->getName() == "Line_37_1")
			wrongLine[0] = number;
		if (node->getName() == "Line_12_1")
			wrongLine[1] = number;
		float extra_fix = 0.0f;
		float extra_sign = 1.0f;
		switch (number)
		{
			case 18:
			case 9:
			case 48:
			case 23:
			case 14:
			case 31:
			case 38:
			case 16:
			case 28:
			case 34:
			case 40:
			case 7:
			case 4:
			case 6:
			case 37:
			case 21:
			case 26:
			case 46:
			case 10:
			case 35:
				extra_sign = -1.0f;
				break;
			case 25: //ganz falsch!
			case 42: //ganz falsch!
				number++;
				continue;
		}
		Ogre::SceneNode::ObjectIterator ot = node->getAttachedObjectIterator();
		Ogre::BillboardSet* billboardSet = m_pSceneMgr->createBillboardSet();
		billboardSet->setBillboardType(Ogre::BBT_POINT);
		billboardSet->setMaterialName("Examples/ElectronArrow");
		if (ot.hasMoreElements())
		{
			Ogre::Entity* entity = static_cast<Ogre::Entity*>(ot.getNext());
			entity->getSubEntity(0)->getMaterial()->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);
			Ogre::MeshPtr line = entity->getMesh();
			PathParser* pp = new PathParser(line);
			
			//Calculating the length of the fieldline
			float length = 0.0f;
			for(unsigned int i = 1; i < pp->pointList->size(); i++)
				length+=(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
			//Finding the last point BEFORE the middle
			float position = 0;
			int i = 1;
			while (position < length/2.0f) //it stops at the point AFTER the middle
			{
				position+=(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				i++;
			}
			//Setting i and position to the point BEFORE the middle
			i--;
			position -= (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
			//Calculating the way from the point before the middle to the middle:
			float partPosition = length/2.0f - position;
			//Calculating the way from the point before the middle to the point after the middle:
			float distance = (pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length();
			int middle_i = i;
			Ogre::Vector3 middle = pp->pointList->at(middle_i)->position;// + partPosition/distance*(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position);

			float ac = (pp->pointList->at(middle_i+1)->position-pp->pointList->at(middle_i-1)->position).x /(pp->pointList->at(middle_i+1)->position-pp->pointList->at(middle_i-1)->position).length();
			if (ac <= -0.99f)
				ac = -0.99f;
			if (ac >= 0.99f)
				ac = 0.99f;
			float angle = extra_fix+extra_sign*(M_PI/2.0f-acos(ac));
			drawArrow_A(middle,angle)
			float way_to_go = ELBE_ARROW_DISTANCE_A;
			i = middle_i-1;
			float diff = (pp->pointList->at(i)->position-pp->pointList->at(i+1)->position).length();
			//left
			while (i>=0)
			{
				while (way_to_go > diff)
				{
					i--;
					if (i<0)
						break;
					way_to_go -= diff;
					diff = (pp->pointList->at(i)->position-pp->pointList->at(i+1)->position).length();
				}
				if (i<0)
					break;
				//now way_to_go is smaller(equal) as diff.
				float ac = (pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).x /(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length();
				if (ac <= -0.99f)
					ac = -0.99f;
				if (ac >= 0.99f)
					ac = 0.99f;
				float angle = extra_fix+extra_sign*(M_PI/2.0f-acos(ac));
				drawArrow_A(pp->pointList->at(i+1)->position-way_to_go/(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length()*(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position),angle)
				diff-=way_to_go;
				way_to_go = ELBE_ARROW_DISTANCE_A;
			}
			//right
			way_to_go = ELBE_ARROW_DISTANCE_A;
			i = middle_i+1;
			diff = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
			while (i<pp->pointList->size())
			{
				while (way_to_go > diff)
				{
					i++;
					if (i>=pp->pointList->size())
						break;
					way_to_go -= diff;
					diff = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				}
				if (i>=pp->pointList->size())
					break;
				//now way_to_go is smaller(equal) as diff.
				float ac = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).x /(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				if (ac <= -0.99f)
					ac = -0.99f;
				if (ac >= 0.99f)
					ac = 0.99f;
				float angle = extra_fix+extra_sign*(M_PI/2.0f-acos(ac));
				drawArrow_A(pp->pointList->at(i-1)->position-way_to_go/(pp->pointList->at(i-1)->position-pp->pointList->at(i)->position).length()*(pp->pointList->at(i-1)->position-pp->pointList->at(i)->position),angle)
				diff-=way_to_go;
				way_to_go = ELBE_ARROW_DISTANCE_A;
			}
			delete pp;
		}
		node->attachObject(billboardSet);
		number++;		
	}
	//+---------------------+
	//| Field Arrows Cavity |
	//+---------------------+
	buncher = m_pSceneMgr->getSceneNode("Cavity_Field");	
	it = buncher->getChildIterator();
	number = 0;
	while (it.hasMoreElements())
	{
		Ogre::BillboardSet* billboardSet = m_pSceneMgr->createBillboardSet();
		billboardSet->setBillboardType(Ogre::BBT_POINT);
		billboardSet->setMaterialName("Examples/ElectronArrow");
		Ogre::SceneNode* node = (Ogre::SceneNode*)(it.getNext());
		Ogre::SceneNode::ObjectIterator ot = node->getAttachedObjectIterator();
		if (ot.hasMoreElements())
		{
			Ogre::Entity* entity = static_cast<Ogre::Entity*>(ot.getNext());
			entity->getSubEntity(0)->getMaterial()->setDepthFunction(Ogre::CMPF_ALWAYS_PASS);
			Ogre::MeshPtr line = entity->getMesh();
			PathParser* pp = new PathParser(line);
			
			//Calculating the length of the fieldline
			float length = 0.0f;
			for(unsigned int i = 1; i < pp->pointList->size(); i++)
			{
				length+=(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				//OgreFramework::getSingletonPtr()->m_pLog->logMessage(buncher->getName()+": "+Ogre::StringConverter::toString(i));
			}
			//Finding the last point BEFORE the middle
			float position = 0;
			int i = 1;
			while (position < length/2.0f) //it stops at the point AFTER the middle
			{
				position+=(pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				i++;
			}
			//Setting i and position to the point BEFORE the middle
			i--;
			if (i+1>=pp->pointList->size()) //middle fix. dirty³...
			{
				Ogre::Vector3 middle = pp->pointList->at(i)->position;
				drawArrow_CM(middle)
				for (i = 1;i<4;i++)
				{
					drawArrow_CM(middle+Ogre::Vector3(float(i)*ELBE_ARROW_DISTANCE_B,0.0f,0.0f))
					drawArrow_CM(middle-Ogre::Vector3(float(i)*ELBE_ARROW_DISTANCE_B,0.0f,0.0f))
				}
				continue;
			}
			position -= (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
			//Calculating the way from the point before the middle to the middle:
			float partPosition = length/2.0f - position;
			//Calculating the way from the point before the middle to the point after the middle:
			int middle_i = i;
			Ogre::Vector3 middle = pp->pointList->at(middle_i)->position;// + partPosition/distance*(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position);

			drawArrow_C(middle)
			float way_to_go = ELBE_ARROW_DISTANCE_B;
			i = middle_i-1;
			float diff = (pp->pointList->at(i)->position-pp->pointList->at(i+1)->position).length();
			//left
			while (i>=0)
			{
				while (way_to_go > diff)
				{
					i--;
					if (i<0)
						break;
					way_to_go -= diff;
					diff = (pp->pointList->at(i)->position-pp->pointList->at(i+1)->position).length();
				}
				if (i<0)
					break;
				//now way_to_go is smaller(equal) than diff.
				drawArrow_C(pp->pointList->at(i+1)->position-way_to_go/(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position).length()*(pp->pointList->at(i+1)->position-pp->pointList->at(i)->position))
				diff-=way_to_go;
				way_to_go = ELBE_ARROW_DISTANCE_B;
			}
			//right
			way_to_go = ELBE_ARROW_DISTANCE_B;
			i = middle_i+1;
			if (i < pp->pointList->size())
			{
				diff = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
				while (i<pp->pointList->size())
				{
					while (way_to_go > diff)
					{
						i++;
						if (i>=pp->pointList->size())
							break;
						way_to_go -= diff;
						diff = (pp->pointList->at(i)->position-pp->pointList->at(i-1)->position).length();
					}
					if (i>=pp->pointList->size())
						break;
					//now way_to_go is smaller(equal) as diff.
					drawArrow_C(pp->pointList->at(i-1)->position-way_to_go/(pp->pointList->at(i-1)->position-pp->pointList->at(i)->position).length()*(pp->pointList->at(i-1)->position-pp->pointList->at(i)->position))
					diff-=way_to_go;
					way_to_go = ELBE_ARROW_DISTANCE_B;
				}
			}
			delete pp;
		}
		number++;
		node->attachObject(billboardSet);	
	}
#endif
}

ElbePipe::~ElbePipe()
{
	reset();
	//bunch.clear();
}

void ElbePipe::refreshScreenTexture()
{
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = screenTexture->getBuffer();
 	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD); // for best performance use HBL_DISCARD!
	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
 	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);
 
	if (oldScreenW != screenW || oldScreenH != screenH)
	{
		oldScreenW = screenW;
		oldScreenH = screenH;

		float rx = screenW/10.0f;
		float ry = screenH/10.0f;
		float one_over_rx_rx = 1.0f/(rx*rx);
		float one_over_ry_ry = 1.0f/(ry*ry);

		for (size_t j = 0; j < SCREEN_WIDTH; j++)
			for(size_t i = 0; i < SCREEN_WIDTH; i++)
			{
				float x = float(i)/float(SCREEN_WIDTH)*2.0f-1.0f;
				float y = float(j)/float(SCREEN_WIDTH)*2.0f-1.0f;
				screenGauss[i][j] = Ogre::Math::gaussianDistribution(sqrt(x*x*one_over_rx_rx+y*y*one_over_ry_ry),0.0f,0.4f);
			}
 	}
	for (size_t j = 0; j < SCREEN_WIDTH; j++)
		for(size_t i = 0; i < SCREEN_WIDTH; i++)
		{
			//if (x*x*ry*ry+y*y*rx*rx < rx*rx*ry*ry)
				setScreenTexturePixel(i,j,16,16+int(192.0f*screenGauss[i][j]),16,192+(rand()&63))
		}
	pixelBuffer->unlock();

	pixelBuffer = screenMap->getBuffer();
 	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD); // for best performance use HBL_DISCARD!
	const Ogre::PixelBox& pixelBox2 = pixelBuffer->getCurrentLock();
 	pDest = static_cast<Ogre::uint8*>(pixelBox2.data);
	int pos = 385-int((screenPosition/27000.0f)*400.0f);
	if (pos > 40)
	{
		for (size_t j = 0; j < 67; j++)
			for(size_t i = 0; i < 400; i++)
				if (i == pos && j>=2 && j<12)
					setScreenTexturePixel(i,j,0,0,0,255)
				else
					setScreenTexturePixel(i,j,255,255,255,255)
	}
	else
	if (pos > 8)
	{
		memset(pDest,255,67*400*4);
		for (int j = -3; j <= 3; j++)
			setScreenTexturePixel(j+11+pos*7/10,j+8+(40-pos)*7/10,0,0,0,255)
	}
	else
	{
		for (size_t j = 0; j < 67; j++)
			for(size_t i = 0; i < 400; i++)
				if (j == -1+40-pos && i>16-4 && i<16+7)
					setScreenTexturePixel(i,j,0,0,0,255)
				else
					setScreenTexturePixel(i,j,255,255,255,255)
	}

	pixelBuffer->unlock();

}

float ElbePipe::updateScreenPosition()
{
	printf("%f\n",screenPosition);
	int numPoint = ElbeElectron::getNumFromPosition(screenPosition);
	float position = (ElbeElectron::getRealDriftPosition(numPoint)-ElbeElectron::getRealDriftLength(0))/0.368667;
	position += (ElbeElectron::getRealDriftLength(numPoint)*(screenPosition+L_DT0-ElbeElectron::getKumDriftLength(numPoint-1))/ElbeElectron::getDriftLength(numPoint))/0.368667f;
	float realposition = position*0.368667f-ElbePipe::EMITTER_POS;
	if (screenPosition == 0.0f)
		screenNode->setVisible(false);
	else
		screenNode->setVisible(true);
	if (realposition > 176.9f)
	{
		float angel = M_PI/4.0f;
		float angel2 = M_PI/2.0f;
		float x = 157.5f + cos(angel) * (176.9f - 157.5f);
		float y =   0.0f + sin(angel) * (176.9f - 157.5f);
		x += cos(angel2) * (realposition - 176.9f);
		y += sin(angel2) * (realposition - 176.9f);
		screenNode->setPosition(-(x+ElbePipe::EMITTER_POS)/0.368667f,-y/0.368667f,0);
		screenNode->resetOrientation();
		screenNode->roll(Ogre::Radian(angel2));
	}
	else
	if (realposition > 157.5f)
	{
		float angel = M_PI/4.0f;
		float x = 157.5f + cos(angel) * (realposition - 157.5f);
		float y =   0.0f + sin(angel) * (realposition - 157.5f);
		screenNode->setPosition(-(x+ElbePipe::EMITTER_POS)/0.368667f,-y/0.368667f,0);
		screenNode->resetOrientation();
		screenNode->roll(Ogre::Radian(angel));
	}
	else
	{
		screenNode->setPosition(-position,0,0);
		screenNode->resetOrientation();
	}
	return position;
}

void ElbePipe::refreshTrack(bool show)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("Creating Sensor Track...");
	float old_max_time = ElbeConstants::MAX_CALC_TIME;
	float old_min_max_time = ElbeConstants::MIN_MAX_CALC_TIME;
	ElbeConstants::MAX_CALC_TIME = 25.0f;
	ElbeConstants::MIN_MAX_CALC_TIME = 25.0f;
	bool newTrack = false;
	if (track)
		track->beginUpdate(0);
	else
	{
		newTrack = true;
		track = m_pSceneMgr->createManualObject();
		track->setDynamic(true);
		track->begin("Track", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	}
	if (track2)
		track2->beginUpdate(0);
	else
	{
		newTrack = true;
		track2 = m_pSceneMgr->createManualObject();
		track2->setDynamic(true);
		track2->begin("Track2", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
	}

	//Creating "sensor bunch"
	Ogre::SceneNode *newNode = holdNode->createChildSceneNode();
	ElbeBunch* sensorElectron = new ElbeBunch(m_pSceneMgr,newNode,0,false,false,TRACK_COUNT);

	float alpha = 0.8f;
	Ogre::Vector3 scale = sensorElectron->getElectron()->getBunchScale() * ElbeConstants::MODEL_SCALE_2;
	track->position( sensorElectron->getOgreNode()->_getDerivedPosition().x,
			         sensorElectron->getOgreNode()->_getDerivedPosition().y-scale.y,
					 sensorElectron->getOgreNode()->_getDerivedPosition().z);
	float r = scale.z/0.5;
	if (r>1.0)
		r=1.0;
	track->colour(r,1.0-r,1.0-r,alpha);
	track->position( sensorElectron->getOgreNode()->_getDerivedPosition().x,
			         sensorElectron->getOgreNode()->_getDerivedPosition().y+scale.y,
					 sensorElectron->getOgreNode()->_getDerivedPosition().z);
	track->colour(r,1.0-r,1.0-r,alpha);

	bool result = true;
	double realGlobalTime = globalTime;
	double realOldGlobalTime = oldGlobalTime;
	oldGlobalTime = 0.0;
	globalTime = 50.0;
	Ogre::Vector2 direction = Ogre::Vector2(0.0,1.0);
	while (sensorElectron->getElectron()->driftnumber<31 && result)
	{
		Ogre::Vector3 oldposition = sensorElectron->getOgreNode()->_getDerivedPosition();
		result = sensorElectron->update(50.0,false,false);
		Ogre::Vector2 tempvector;
		tempvector.x = oldposition.x - sensorElectron->getOgreNode()->_getDerivedPosition().x;
		tempvector.y = oldposition.z - sensorElectron->getOgreNode()->_getDerivedPosition().z;
		tempvector.normalise();
		direction.x = -tempvector.y;
		direction.y =  tempvector.x;
		oldGlobalTime += 50.0;
		globalTime += 50.0;
		double relative_position = sensorElectron->getElectron()->L/sensorElectron->getElectron()->getDriftLength(sensorElectron->getElectron()->driftnumber);
		double now_position = sensorElectron->getElectron()->getRealDriftPosition(sensorElectron->getElectron()->driftnumber) + relative_position*sensorElectron->getElectron()->getRealDriftLength(sensorElectron->getElectron()->driftnumber);
		Ogre::Vector3 scale = sensorElectron->getElectron()->getBunchScale() * ElbeConstants::MODEL_SCALE_2;
		track->position( sensorElectron->getOgreNode()->_getDerivedPosition().x,
			             sensorElectron->getOgreNode()->_getDerivedPosition().y-scale.y,
						 sensorElectron->getOgreNode()->_getDerivedPosition().z);
		float r = scale.z/0.5;
		if (r>1.0)
			r=1.0;
		track->colour(r,1.0-r,1.0-r,alpha);
		track->position( sensorElectron->getOgreNode()->_getDerivedPosition().x,
			             sensorElectron->getOgreNode()->_getDerivedPosition().y+scale.y,
						 sensorElectron->getOgreNode()->_getDerivedPosition().z);
		track->colour(r,1.0-r,1.0-r,alpha);

		if (sensorElectron->getElectron()->driftnumber>11)
		{
			track2->position( sensorElectron->getOgreNode()->_getDerivedPosition().x+scale.x*direction.x,
							  sensorElectron->getOgreNode()->_getDerivedPosition().y,
							  sensorElectron->getOgreNode()->_getDerivedPosition().z+scale.x*direction.y);
			track2->colour(r,1.0-r,1.0-r,alpha);
			track2->position( sensorElectron->getOgreNode()->_getDerivedPosition().x-scale.x*direction.x,
							  sensorElectron->getOgreNode()->_getDerivedPosition().y,
							  sensorElectron->getOgreNode()->_getDerivedPosition().z-scale.x*direction.y);
			track2->colour(r,1.0-r,1.0-r,alpha);
		}

	}

	globalTime = realGlobalTime;
	oldGlobalTime = realOldGlobalTime;
	
	track->end();
	if (newTrack)
	{
		Ogre::SceneNode* meowNode = m_pSceneMgr->createSceneNode("track1node");
		m_pSceneMgr->getRootSceneNode()->addChild(meowNode);
		meowNode->attachObject(track);

	}
	track->setVisible(show);

	track2->end();
	if (newTrack)
	{
		Ogre::SceneNode* meowNode = m_pSceneMgr->createSceneNode("track2node");
		m_pSceneMgr->getRootSceneNode()->addChild(meowNode);
		meowNode->attachObject(track2);
	}
	track2->setVisible(show);
	delete sensorElectron;
	holdNode->removeChild(newNode);
	ElbeConstants::MAX_CALC_TIME = old_max_time;
	ElbeConstants::MIN_MAX_CALC_TIME = old_min_max_time;
	Ogre::LogManager::getSingletonPtr()->logMessage("Done.");
}

void ElbePipe::saveTrack(Ogre::String filename)
{
    std::fstream f;
    f.open(std::string(filename).c_str(), std::ios_base::out);
	f << "time\tposition\tlength\theight\tdepth" << std::endl;
	//Creating "sensor bunch"
	ElbeElectron* sensorElectron = new ElbeElectron(0,ElbeConstants::MAX_CALC_TIME);
	Ogre::Vector3 scale = sensorElectron->getBunchScale();
	float offset = -0.008f;
	double time = 0.0;
	double realGlobalTime = globalTime;
	double realOldGlobalTime = oldGlobalTime;
	oldGlobalTime = 0.0;
	globalTime = 25.0;
	while (sensorElectron->driftnumber<32)
	{
		f << time << "\t" << sensorElectron->pos << "\t" << scale.z << "\t" << scale.x << "\t" << scale.y << std::endl;
		if (!sensorElectron->calc(25.0,ElbeConstants::MAX_CALC_TIME))
			break;
		time += 25.0;
		oldGlobalTime += 25.0;
		globalTime += 25.0;
		scale = sensorElectron->getBunchScale();
	}
	delete sensorElectron;
	globalTime = realGlobalTime;
	oldGlobalTime = realOldGlobalTime;

    f.close();
}
void ElbePipe::reset()
{
	for (std::list<ElbeBunch*>::iterator bu = bunch.begin();bu!=bunch.end();)
	{
		delete (*bu);
		bu = bunch.erase(bu);
	}
	bunch.clear();
	cameraBunch = NULL;
}

void ElbePipe::setDefaults(int which)
{
	std::fstream f;
	f.open(std::string("Default.save").c_str(), std::ios_base::in);
	if (f)
	{
		char buffer[256];
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::U_G1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S2_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S3_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S4_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S5_DEFAULT = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::U_B1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::U_B2_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P_B1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P_B2_DEFAULT = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P1_CRYOS1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E1_CRYOS1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P2_CRYOS1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E2_CRYOS1_DEFAULT = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P1_CRYOS2_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E1_CRYOS2_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P2_CRYOS2_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E2_CRYOS2_DEFAULT = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q2_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q3_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q4_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q5_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q6_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q7_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q8_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q9_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q10_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q11_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q12_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q13_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q14_DEFAULT = atof (buffer);

		f.close();
	}
	if (which & GUN_1)
		ElbePipe::U_G1 = ElbePipe::U_G1_DEFAULT;
	if (which & SOLENOID_1)
		ElbePipe::I_S1 = ElbePipe::I_S1_DEFAULT;
	if (which & SOLENOID_2)
		ElbePipe::I_S2 = ElbePipe::I_S2_DEFAULT;
	if (which & SOLENOID_3)
		ElbePipe::I_S3 = ElbePipe::I_S3_DEFAULT;
	if (which & SOLENOID_4)
		ElbePipe::I_S4 = ElbePipe::I_S4_DEFAULT;
	if (which & SOLENOID_5)
		ElbePipe::I_S5 = ElbePipe::I_S5_DEFAULT;
	if (which & BUNCHER_1)
	{
		ElbePipe::U_B1 = ElbePipe::U_B1_DEFAULT;
		ElbePipe::P_B1 = ElbePipe::P_B1_DEFAULT;
	}
	if (which & BUNCHER_2)
	{
		ElbePipe::U_B2 = ElbePipe::U_B2_DEFAULT;	
		ElbePipe::P_B2 = ElbePipe::P_B2_DEFAULT;
	}
	if (which & CRYOSTAT_1)
	{
		ElbePipe::P1_CRYOS1 = ElbePipe::P1_CRYOS1_DEFAULT;
		ElbePipe::E1_CRYOS1 = ElbePipe::E1_CRYOS1_DEFAULT;
		ElbePipe::P2_CRYOS1 = ElbePipe::P2_CRYOS1_DEFAULT;
		ElbePipe::E2_CRYOS1 = ElbePipe::E2_CRYOS1_DEFAULT;
	}
	if (which & CRYOSTAT_2)
	{
		ElbePipe::P1_CRYOS2 = ElbePipe::P1_CRYOS2_DEFAULT;
		ElbePipe::E1_CRYOS2 = ElbePipe::E1_CRYOS2_DEFAULT;
		ElbePipe::P2_CRYOS2 = ElbePipe::P2_CRYOS2_DEFAULT;
		ElbePipe::E2_CRYOS2 = ElbePipe::E2_CRYOS2_DEFAULT;
	}
	if (which & QUADRUPOLE_1)
		ElbePipe::I_Q1 = ElbePipe::I_Q1_DEFAULT;
	if (which & QUADRUPOLE_2)
		ElbePipe::I_Q2 = ElbePipe::I_Q2_DEFAULT;
	if (which & QUADRUPOLE_3)
		ElbePipe::I_Q3 = ElbePipe::I_Q3_DEFAULT;
	if (which & QUADRUPOLE_4)
		ElbePipe::I_Q4 = ElbePipe::I_Q4_DEFAULT;
	if (which & QUADRUPOLE_5)
		ElbePipe::I_Q5 = ElbePipe::I_Q5_DEFAULT;
	if (which & QUADRUPOLE_6)
		ElbePipe::I_Q6 = ElbePipe::I_Q6_DEFAULT;
	if (which & QUADRUPOLE_7)
		ElbePipe::I_Q7 = ElbePipe::I_Q7_DEFAULT;
	if (which & QUADRUPOLE_8)
		ElbePipe::I_Q8 = ElbePipe::I_Q8_DEFAULT;
	if (which & QUADRUPOLE_9)
		ElbePipe::I_Q9 = ElbePipe::I_Q9_DEFAULT;
	if (which & QUADRUPOLE_10)
		ElbePipe::I_Q10 = ElbePipe::I_Q10_DEFAULT;
	if (which & QUADRUPOLE_11)
		ElbePipe::I_Q11 = ElbePipe::I_Q11_DEFAULT;
	if (which & QUADRUPOLE_12)
		ElbePipe::I_Q12 = ElbePipe::I_Q12_DEFAULT;
	if (which & QUADRUPOLE_13)
		ElbePipe::I_Q13 = ElbePipe::I_Q13_DEFAULT;
	if (which & QUADRUPOLE_14)
		ElbePipe::I_Q14 = ElbePipe::I_Q14_DEFAULT;
}

void ElbePipe::loadState(std::string name)
{
	if (name == "Default")
		setDefaults(ALL_CONFIGABLE);
	else
	if (name == "Given 1") {
		setDefaults(ALL_CONFIGABLE);
		ElbePipe::I_S1*=1.05;
		ElbePipe::I_S2*=1.05;
		ElbePipe::I_S3*=1.05;
		ElbePipe::I_S4*=1.05;
		ElbePipe::I_S5*=1.05;
	}
	else
	if (name == "Given 2") {
		setDefaults(ALL_CONFIGABLE);
		ElbePipe::I_S1*=0.95;
		ElbePipe::I_S2*=0.95;
		ElbePipe::I_S3*=0.95;
		ElbePipe::I_S4*=0.95;
		ElbePipe::I_S5*=0.95;
	}
	else {
		std::fstream f;
		f.open(std::string(name+".save").c_str(), std::ios_base::in);
		if (!f)
			return;
		char buffer[256];
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::U_G1_DEFAULT = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S1 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S2 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S3 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S4 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_S5 = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::U_B1 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::U_B2 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P_B1 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P_B2 = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P1_CRYOS1 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E1_CRYOS1 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P2_CRYOS1 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E2_CRYOS1 = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P1_CRYOS2 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E1_CRYOS2 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::P2_CRYOS2 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::E2_CRYOS2 = atof (buffer);

		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q1 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q2 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q3 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q4 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q5 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q6 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q7 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q8 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q9 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q10 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q11 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q12 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q13 = atof (buffer);
		f.get(buffer,256,':');
		f.get(); //the ':'
		f.get(buffer,256,'\n');
		ElbePipe::I_Q14 = atof (buffer);

		f.close();
	}
}

void ElbePipe::saveState(std::string name)
{
    std::fstream f;
    f.open(std::string(name+".save").c_str(), std::ios_base::out);
	f << "U(Gun): " << ElbePipe::U_G1  << std::endl;
	f << "I(Solenoid 1): " << ElbePipe::I_S1 << std::endl;
	f << "I(Solenoid 2): " << ElbePipe::I_S2 << std::endl;
	f << "I(Solenoid 3): " << ElbePipe::I_S3 << std::endl;
	f << "I(Solenoid 4): " << ElbePipe::I_S4 << std::endl;
	f << "I(Solenoid 5): " << ElbePipe::I_S5 << std::endl;
	f << "U(Buncher 1): " << ElbePipe::U_B1 << std::endl;
	f << "U(Buncher 2): " << ElbePipe::U_B2 << std::endl;
	f << "P(Buncher 1): " << ElbePipe::P_B1 << std::endl;
	f << "P(Buncher 2): " << ElbePipe::P_B2 << std::endl;
	f << "P1(Cryostat 1): " << ElbePipe::P1_CRYOS1 << std::endl;
	f << "E1(Cryostat 1): " << ElbePipe::E1_CRYOS1 << std::endl;
	f << "P2(Cryostat 1): " << ElbePipe::P2_CRYOS1 << std::endl;
	f << "E2(Cryostat 1): " << ElbePipe::E2_CRYOS1 << std::endl;
	f << "P1(Cryostat 2): " << ElbePipe::P1_CRYOS2 << std::endl;
	f << "E1(Cryostat 2): " << ElbePipe::E1_CRYOS2 << std::endl;
	f << "P2(Cryostat 2): " << ElbePipe::P2_CRYOS2 << std::endl;
	f << "E2(Cryostat 2): " << ElbePipe::E2_CRYOS2 << std::endl;
	f << "I(Quadrupole 1): " << ElbePipe::I_Q1 << std::endl;
	f << "I(Quadrupole 2): " << ElbePipe::I_Q2 << std::endl;
	f << "I(Quadrupole 3): " << ElbePipe::I_Q3 << std::endl;
	f << "I(Quadrupole 4): " << ElbePipe::I_Q4 << std::endl;
	f << "I(Quadrupole 5): " << ElbePipe::I_Q5 << std::endl;
	f << "I(Quadrupole 6): " << ElbePipe::I_Q6 << std::endl;
	f << "I(Quadrupole 7): " << ElbePipe::I_Q7 << std::endl;
	f << "I(Quadrupole 8): " << ElbePipe::I_Q8 << std::endl;
	f << "I(Quadrupole 9): " << ElbePipe::I_Q9 << std::endl;
	f << "I(Quadrupole 10): " << ElbePipe::I_Q10 << std::endl;
	f << "I(Quadrupole 11): " << ElbePipe::I_Q11 << std::endl;
	f << "I(Quadrupole 12): " << ElbePipe::I_Q12 << std::endl;
	f << "I(Quadrupole 13): " << ElbePipe::I_Q13 << std::endl;
	f << "I(Quadrupole 14): " << ElbePipe::I_Q14 << std::endl;
    f.close();
}

ElbeBunch* ElbePipe::getCameraBunch() {
	if (bunch.empty())
		return fakeBunch;
	if (cameraBunch == NULL)
		return fakeBunch;
	return cameraBunch;
}

ElbeBunch* ElbePipe::getFakeBunch() {
	return fakeBunch;
}

void ElbePipe::resetCameraBunch() {
	cameraBunch = NULL;
}

void ElbePipe::visualize(AbstractCamera* camera,float speed) {
	std::list<ElbeBunch*>::iterator bu;
	for (bu = bunch.begin(); bu != bunch.end(); ++bu)
		(*bu)->visualize(camera,speed);

	/*std::vector<Ogre::Vector3>::iterator pos = quadropolPos.begin();
	for (std::vector<std::vector<Ogre::MaterialPtr> >::iterator qu = quadropol.begin(); qu != quadropol.end(); ++qu)
	{
		for (std::vector<Ogre::MaterialPtr>::iterator it = (*qu).begin(); it != (*qu).end(); ++it)
		{
			float mindistance = getCameraBunch()->getOgreNode()->_getDerivedPosition().squaredDistance((*pos));
			float alpha = 1.0;
			if (mindistance<1.0)
				alpha=0.0;
			else
			if (mindistance<2.0)
				alpha=pow((mindistance-1.0)*2.0,2.0)/4.0;
			it->get()->setDiffuse(it->get()->getTechnique(0)->getPass(0)->getAmbient().r,
								  it->get()->getTechnique(0)->getPass(0)->getAmbient().g,
								  it->get()->getTechnique(0)->getPass(0)->getAmbient().b,alpha);
		}
		pos++;
	}*/
}

bool ElbePipe::newCameraBunch() {
	return newCamera;
}

bool ElbePipe::update(double timeSinceLastFrame,double factor, bool explosion,bool show_billboard,bool show_volume,int update_tracks)
{
	oldGlobalTime = globalTime;
	globalTime += timeSinceLastFrame*factor;
	ElbeBunch* newBunch = NULL;
	if (globalTime > ElbeConstants::P_026PS) {//260 Mhz
		globalTime -= ElbeConstants::P_026PS;
		//Adding a bunch at time position globaltime;
		Ogre::SceneNode *newNode = holdNode->createChildSceneNode();
		if (cameraBunch)
			newBunch = new ElbeBunch(m_pSceneMgr, newNode,globalTime);
		else
		{
			newBunch = new ElbeBunch(m_pSceneMgr, newNode,globalTime,true,false,particle_count,show_billboard,show_volume,VOLUME_SHADER);
			cameraBunch = newBunch;
			newBunch->updateParticle(update_tracks);
		}
	}
	bool result = true;
	newCamera = false;
	for (std::list<ElbeBunch*>::iterator bu = bunch.begin();bu!=bunch.end();)
	{
		if (!(*bu)->update(timeSinceLastFrame*factor,explosion,true,update_tracks))
		{
			if ((*bu) == cameraBunch)
			{
				cameraBunch = NULL;
				newCamera = true;
			}
			delete (*bu);
			bu = bunch.erase(bu);
			result = false;
		}
		else
			bu++;
	}
	if (newBunch) //Adding afterwards, because the bunch has his "update" already in his creation
		bunch.push_back(newBunch);

	//emitterField Update
	emitterFieldPosition += timeSinceLastFrame*factor/500.0;
	while (emitterFieldPosition > 1.0)
		emitterFieldPosition-=1.0;
	emitterField->beginUpdate(0);
		emitterField->position( 0.0f, 0.0f,  0.28f);
		emitterField->colour(0,1,0);
		emitterField->textureCoord(10+emitterFieldPosition,0);
		emitterField->position(-3.0f, 0.0f,  0.28f);
		emitterField->colour(1,0,0);
		emitterField->textureCoord(0+emitterFieldPosition,0);
		emitterField->position(-3.0f, 0.0f, -0.28f);
		emitterField->colour(1,0,0);
		emitterField->textureCoord(0+emitterFieldPosition,1);
		emitterField->position( 0.0f, 0.0f, -0.28f);
		emitterField->colour(0,1,0);
		emitterField->textureCoord(10+emitterFieldPosition,1);
	emitterField->end();


	return result;
}

void ElbePipe::updateFields()
{
#ifdef ELBE_DRAW_ARROWS
#ifdef ELBE_UPDATE_ARROWS
	double ekin = U_G1_DEFAULT * ElbeConstants::QS;
	double gamma = ekin/ElbeConstants::EE + 1.0f;
	double beta = sqrt(1.0f-(1.0f/gamma)*(1.0f/gamma));
	//Field Arrows
	int i;
	double realtime = globalTime - ((L_DT1 + L_DT2)/(beta*ElbeConstants::CPSMM));
	double scalingFactor = realtime*ElbeConstants::CPSMM;
	double size = -sin(2.0*ElbeConstants::PI*ElbeConstants::F_026*scalingFactor/ElbeConstants::CMM+P_B1*M_PI/180.0f-M_PI/2.0f);
	for (i = 0; i < billboardBList.size(); i++)
		billboardBList[i]->setDimensions(size*40.0f,size*40.0f);

	realtime = globalTime - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6)/(beta*ElbeConstants::CPSMM));
	scalingFactor = realtime*ElbeConstants::CPSMM;
	size = -sin(2.0*ElbeConstants::PI*ElbeConstants::F_13*scalingFactor/ElbeConstants::CMM+P_B2*M_PI/180.0f-M_PI/2.0f)*0.5f;
	for (i = 0; i < billboardAList.size(); i++)
		billboardAList[i]->setDimensions(size*40.0f*billboardAListMultiplicator[i],size*40.0f*billboardAListMultiplicator[i]);


	//Cryosupdate 1
	realtime = globalTime - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8)/(beta*ElbeConstants::CPSMM));
	scalingFactor = ElbePipe::ARROW_SCALE * cavityGetEZmm(realtime*ElbeConstants::CPSMM + P1_CRYOS1*L_DT9/9.0/180.0);
	for (i = 0; i < billboardC1_1List.size(); i++)
		billboardC1_1List[i]->setDimensions(scalingFactor*40.0f*70.0f,scalingFactor*40.0f*70.0f);
	
	realtime = globalTime - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8 + L_DT9 + L_DT10)/(beta*ElbeConstants::CPSMM));
	scalingFactor = ElbePipe::ARROW_SCALE * cavityGetEZmm(realtime*ElbeConstants::CPSMM + (P2_CRYOS1-29.0)*L_DT11/9.0/180.0);
	for (i = 0; i < billboardC1_2List.size(); i++)
		billboardC1_2List[i]->setDimensions(scalingFactor*40.0f*70.0f,scalingFactor*40.0f*70.0f);
	
	//Cryosupdate 2
	realtime = globalTime - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8)/(beta*ElbeConstants::CPSMM));
	scalingFactor = ElbePipe::ARROW_SCALE * cavityGetEZmm(realtime*ElbeConstants::CPSMM + (P1_CRYOS2-224.0)*L_DT9/9.0/180.0);
	for (i = 0; i < billboardC2_1List.size(); i++)
		billboardC2_1List[i]->setDimensions(scalingFactor*40.0f*70.0f,scalingFactor*40.0f*70.0f);
	
	realtime = globalTime - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8 + L_DT9 + L_DT10)/(beta*ElbeConstants::CPSMM));
	scalingFactor = ElbePipe::ARROW_SCALE * cavityGetEZmm(realtime*ElbeConstants::CPSMM + (P2_CRYOS2-209.0)*L_DT11/9.0/180.0);
	for (i = 0; i < billboardC2_2List.size(); i++)
		billboardC2_2List[i]->setDimensions(scalingFactor*40.0f*70.0f,scalingFactor*40.0f*70.0f);

	/*double realtime = globalTime - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8)/(beta*ElbeConstants::CPSMM));
	double scalingFactor = ElbePipe::ARROW_SCALE * cavityGetEZmm(realtime*ElbeConstants::CPSMM + P1_CRYOS1*L_DT9/9.0/180.0);
	for(unsigned int i = 0; i < m_Arrows.size()/2; i++)
		for(unsigned int j = 0; j < m_Arrows.at(i).size(); j++)
		{
			m_Arrows.at(i).at(j)->setScale(scalingFactor*70.0f,scalingFactor*70.0f,scalingFactor*140.0f);// scalingFactor, scalingFactor);
			if(i % 2 != 1)
				m_Arrows.at(i).at(j)->scale(-1.0f, -1.0f, -1.0f);
		}

	realtime = globalTime - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8 + L_DT9 + L_DT10)/(beta*ElbeConstants::CPSMM));
	scalingFactor = ElbePipe::ARROW_SCALE * cavityGetEZmm(realtime*ElbeConstants::CPSMM + (P2_CRYOS1-29.0)*L_DT11/9.0/180.0);
	for(unsigned int i = m_Arrows.size()/2; i < m_Arrows.size(); i++)
		for(unsigned int j = 0; j < m_Arrows.at(i).size(); j++)
		{
			m_Arrows.at(i).at(j)->setScale(scalingFactor*70.0f,scalingFactor*70.0f,scalingFactor*140.0f);// scalingFactor, scalingFactor);
			if(i % 2 == 1)
				m_Arrows.at(i).at(j)->scale(-1.0f, -1.0f, -1.0f);
		}

	for(unsigned int i = 0; i < m_Brrows.size()/2; i++)
		for(unsigned int j = 0; j < m_Brrows.at(i).size(); j++)
		{
			m_Brrows.at(i).at(j)->setScale(scalingFactor*70.0f,scalingFactor*70.0f,scalingFactor*140.0f);// scalingFactor, scalingFactor);
			if(i % 2 != 1)
				m_Brrows.at(i).at(j)->scale(-1, -1, -1);
		}

	for(unsigned int i = m_Brrows.size()/2; i < m_Brrows.size(); i++)
		for(unsigned int j = 0; j < m_Brrows.at(i).size(); j++)
		{
			m_Brrows.at(i).at(j)->setScale(scalingFactor*70.0f,scalingFactor*70.0f,scalingFactor*140.0f);// scalingFactor, scalingFactor);
			if(i % 2 == 1)
				m_Brrows.at(i).at(j)->scale(-1, -1, -1);
		}*/
#endif
#endif
}

int ElbePipe::getBunchCount()
{
	return bunch.size();
}

double ElbePipe::getGlobalTime()
{
	return globalTime;
}

double ElbePipe::cavityGetEZmm(double z)
{
	return sin(2.0*ElbeConstants::PI*ElbeConstants::F_13*z/ElbeConstants::CMM);
}

double ElbePipe::cavityGetE11ZTmm(double z,double time_in_period)
{
	double ekin = U_G1_DEFAULT * ElbeConstants::QS;
	double gamma = ekin/ElbeConstants::EE + 1.0f;
	double beta = sqrt(1.0f-(1.0f/gamma)*(1.0f/gamma));
	double realtime = time_in_period - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8)/(beta*ElbeConstants::CPSMM));
	return cavityGetEZmm(z)*sin(2.0*ElbeConstants::PI*0.0013*realtime+P1_CRYOS1*ElbeConstants::PI/180.0);
}

double ElbePipe::cavityGetE12ZTmm(double z,double time_in_period)
{
	double ekin = U_G1_DEFAULT * ElbeConstants::QS;
	double gamma = ekin/ElbeConstants::EE + 1.0f;
	double beta = sqrt(1.0f-(1.0f/gamma)*(1.0f/gamma));
	double realtime = time_in_period - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8 + L_DT9 + L_DT10)/(beta*ElbeConstants::CPSMM));
	return cavityGetEZmm(z)*sin(2.0*ElbeConstants::PI*0.0013*realtime+(P2_CRYOS1-29.0)*ElbeConstants::PI/180.0);
}

double ElbePipe::cavityGetE21ZTmm(double z,double time_in_period)
{
	double ekin = U_G1_DEFAULT * ElbeConstants::QS;
	double gamma = ekin/ElbeConstants::EE + 1.0f;
	double beta = sqrt(1.0f-(1.0f/gamma)*(1.0f/gamma));
	double realtime = time_in_period - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8)/(beta*ElbeConstants::CPSMM));
	return cavityGetEZmm(z)*sin(2.0*ElbeConstants::PI*0.0013*realtime+(P1_CRYOS2-224.0)*ElbeConstants::PI/180.0);
}

double ElbePipe::cavityGetE22ZTmm(double z,double time_in_period)
{
	double ekin = U_G1_DEFAULT * ElbeConstants::QS;
	double gamma = ekin/ElbeConstants::EE + 1.0f;
	double beta = sqrt(1.0f-(1.0f/gamma)*(1.0f/gamma));
	double realtime = time_in_period - ((L_DT1 + L_DT2 + L_DT3 + L_DT4 + L_DT5 + L_DT6 + L_DT7 + L_DT8 + L_DT9 + L_DT10)/(beta*ElbeConstants::CPSMM));
	return cavityGetEZmm(z)*sin(2.0*ElbeConstants::PI*0.0013*realtime+(P2_CRYOS2-209.0)*ElbeConstants::PI/180.0);
}
