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

#pragma once

#include <Ogre.h>
#include "mat.h"
#include "ElbeConstants.hpp"
#include "ElbeBunch.hpp"
#include "AbstractCamera.hpp"
#include <list>

#define ELBE_DRAW_ARROWS
#define ELBE_UPDATE_ARROWS

#define GUN_1 8388608
#define SOLENOID_1 1
#define SOLENOID_2 2
#define SOLENOID_3 4
#define SOLENOID_4 8
#define SOLENOID_5 16
#define BUNCHER_1 32
#define BUNCHER_2 64
#define CRYOSTAT_1 128
#define CRYOSTAT_2 256
#define ALL_CONFIGABLE BUNCHER_1 | BUNCHER_2 | CRYOSTAT_1 | CRYOSTAT_2 | GUN_1
#define QUADRUPOLE_1 512
#define QUADRUPOLE_2 1024
#define QUADRUPOLE_3 2048
#define QUADRUPOLE_4 4096
#define QUADRUPOLE_5 8192
#define QUADRUPOLE_6 16384
#define QUADRUPOLE_7 32768
#define QUADRUPOLE_8 65536
#define QUADRUPOLE_9 131072
#define QUADRUPOLE_10 262144
#define QUADRUPOLE_11 524288
#define QUADRUPOLE_12 1048576
#define QUADRUPOLE_13 2097152
#define QUADRUPOLE_14 4194304
#define ALL_CONFIGABLE_Q SOLENOID_1 | SOLENOID_2 | SOLENOID_3 | SOLENOID_4 | SOLENOID_5 | QUADRUPOLE_1 | QUADRUPOLE_2 | QUADRUPOLE_3 | QUADRUPOLE_4 | QUADRUPOLE_5 | QUADRUPOLE_6 | QUADRUPOLE_7 | QUADRUPOLE_8 | QUADRUPOLE_9 | QUADRUPOLE_10 | QUADRUPOLE_11 | QUADRUPOLE_12 | QUADRUPOLE_13 | QUADRUPOLE_14
#define SCREEN_WIDTH 400

//VOLUME_SHADER
// -1 : No Volume shader
//  0 : particle shader
//  1 : blob shader
//  2 : analytic shader
//  3 : test shader for analytic simplification
#define VOLUME_SHADER 1

class ElbePipe
{
public:
	static int particle_count;
	// track sections - exploiting the fact, that the bunch only moves along the (negative) x-axis
	static const double PATHSTART_POS;
	static const double EMITTER_POS;
	static const double SOLENOID_1_POS;
	static const double BUNCHER_1_POS;
	static const double SOLENOID_2_POS;
	static const double MAKROPULSER_POS;
	static const double SOLENOID_3_POS;
	static const double SOLENOID_4_POS;
	static const double BUNCHER_2_POS;
	static const double SOLENOID_5_POS;
	static const double CAVITY_START_POS;
	static const double CAVITY_END_POS;
	static const double CAVITY_START_POS_2;
	static const double CAVITY_END_POS_2;
	static const double QUADRUPOL_1_POS;
	static const double QUADRUPOL_2_POS;
	static const double QUADRUPOL_3_POS;
	static const double QUADRUPOL_4_POS;
	static const double QUADRUPOL_5_POS;
	static const double CHICANE_1_POS;
	static const double CHICANE_2_POS;
	static const double CHICANE_3_POS;
	static const double CHICANE_4_POS;
	static const double QUADRUPOL_6_POS;
	static const double QUADRUPOL_7_POS;
	static const double CAVITY_START_POS_3;
	static const double CAVITY_END_POS_3;
	static const double CAVITY_START_POS_4;
	static const double CAVITY_END_POS_4;
	static const double QUADRUPOL_8_POS;
	static const double QUADRUPOL_9_POS;
	static const double QUADRUPOL_10_POS;
	static const double QUADRUPOL_11_POS;
	static const double QUADRUPOL_12_POS;
	static const double QUADRUPOL_13_POS;
	static const double QUADRUPOL_14_POS;
	static const double END_POS;


	// lengths of the drift tracks
	static const double L_DT0; // The way from the beginning to the emitter
	static const double L_DT1; // Emitter -> Solenoid 1
	static const double L_DT2; // Solenoid 1 -> Buncher 1
	static const double L_DT3; // Buncher 1 -> Solenoid 2
	static const double L_DT4; // Solenoid 2 -> Solenoid 3
	static const double L_DT5; // Solenoid 3 -> Solenoid 4
	static const double L_DT6; // Solenoid 4 -> Buncher 2
	static const double L_DT7; // Buncher 2 -> Solenoid 5
	static const double L_DT8; // Solenoid 5 -> Cryostat 1
	static const double L_DT9; // Cryostat 1.1
	static const double L_DT10; // Cryostat 1 break
	static const double L_DT11; // Cryostat 1.2  
	static const double L_DT12; // Cryostat 1 -> Quadropol 1
	static const double L_DT13; // Quadropol 1 -> Quadropol 2
	static const double L_DT14; // Quadropol 2 -> Quadropol 3
	static const double L_DT15; // Quadropol 3 -> Quadropol 4
	static const double L_DT16; // Quadropol 4 -> Quadropol 5
	static const double L_DT17; // Quadropol 5 -> Quadropol 6
	static const double L_DT18; // Quadropol 6 -> Quadropol 7
	static const double L_DT19; // Quadropol 7 -> Cryosat 2
	static const double L_DT20; // Cryostat 2.1
	static const double L_DT21; // Cryostat 2 break
	static const double L_DT22; // Cryostat 2.2  
	static const double L_DT23; // Cryostat 2 -> Quadropol 8
	static const double L_DT24; // Quadropol 8 -> Quadropol 9
	static const double L_DT25; // Quadropol 9 -> Quadropol 10
	static const double L_DT26; // Quadropol 10 -> Quadropol 11
	static const double L_DT27; // Quadropol 11 -> Quadropol 12
	static const double L_DT28; // Quadropol 12 -> Quadropol 13
	static const double L_DT29; // Quadropol 13 -> Quadropol 14
	static const double L_DT30; // Quadropol 14 -> END

	// default values for devices
	static double U_G1_DEFAULT;
	static double I_S1_DEFAULT;
	static double I_S2_DEFAULT;
	static double I_S3_DEFAULT;
	static double I_S4_DEFAULT;
	static double I_S5_DEFAULT;
	static double U_B1_DEFAULT;
	static double U_B2_DEFAULT;
	static double P_B1_DEFAULT;
	static double P_B2_DEFAULT;
	static double A_B_DEFAULT;
	static double F_B1_DEFAULT;
	static double F_B2_DEFAULT;
	static double P1_CRYOS1_DEFAULT;
	static double E1_CRYOS1_DEFAULT;
	static double P2_CRYOS1_DEFAULT;
	static double E2_CRYOS1_DEFAULT;
	static double P1_CRYOS2_DEFAULT;
	static double E1_CRYOS2_DEFAULT;
	static double P2_CRYOS2_DEFAULT;
	static double E2_CRYOS2_DEFAULT;
	static double I_Q1_DEFAULT;
	static double I_Q2_DEFAULT;
	static double I_Q3_DEFAULT;
	static double I_Q4_DEFAULT;
	static double I_Q5_DEFAULT;
	static double I_Q6_DEFAULT;
	static double I_Q7_DEFAULT;
	static double I_Q8_DEFAULT;
	static double I_Q9_DEFAULT;
	static double I_Q10_DEFAULT;
	static double I_Q11_DEFAULT;
	static double I_Q12_DEFAULT;
	static double I_Q13_DEFAULT;
	static double I_Q14_DEFAULT;


	// parameters of the transforming devices 
	static double U_G1;
	static double I_S1;		// current of solenoid 1 in [A]
	static double I_S2;
	static double I_S3;
	static double I_S4;
	static double I_S5;
	static double U_B1;		// voltage of buncher 1 in [kV]
	static double U_B2;
	static double P_B1;		// phase angle (phi) of buncher 1 in [Degree]
	static double P_B2;
	static double A_B;		// Gap width of both bunchers in [mm]
	static double F_B1;		// frequency of buncher 1 in [MHz]
	static double F_B2;
	static double P1_CRYOS1;	// Phase Shift of the Cryostat
	static double E1_CRYOS1;	// E_peak of the Cryostat in [MeV]
	static double P2_CRYOS1;	// Phase Shift of the Cryostat
	static double E2_CRYOS1;	// E_peak of the Cryostat in [MeV]
	static double P1_CRYOS2;	// Phase Shift of the Cryostat
	static double E1_CRYOS2;	// E_peak of the Cryostat in [MeV]
	static double P2_CRYOS2;	// Phase Shift of the Cryostat
	static double E2_CRYOS2;	// E_peak of the Cryostat in [MeV]
	static double I_Q1;
	static double I_Q2;
	static double I_Q3;
	static double I_Q4;
	static double I_Q5;
	static double I_Q6;
	static double I_Q7;
	static double I_Q8;
	static double I_Q9;
	static double I_Q10;
	static double I_Q11;
	static double I_Q12;
	static double I_Q13;
	static double I_Q14;

	// scaling factor for the arrow mesh
	static const double ARROW_SCALE;

	static int counter;

	static float screenPosition;
	static float screenW;
	static float screenH;


	// recalculates the device matrices
	void reset();

	// sets the devices parameters to its initial values
	void setDefaults(int which);

	void loadState(std::string name);
	void saveState(std::string name);

	//returns false, if the node has been changed
	bool update(double timeSinceLastFrame,double factor,bool explosion,bool show_billboard,int volume_state,int update_tracks = TRACK_COUNT);

	void updateFields();

	//The Bunch to which the camera is connected
	ElbeBunch* getCameraBunch();

	ElbeBunch* getFakeBunch();
	bool newCameraBunch();
	void visualize(AbstractCamera* camera,float speed);

	int getBunchCount();
	 
	static double getGlobalTime();

	void resetCameraBunch();

	ElbePipe(Ogre::SceneManager* m_pSceneMgr);
	static double cavityGetEZmm(double z);
	static double cavityGetE11ZTmm(double z,double time_in_period);
	static double cavityGetE12ZTmm(double z,double time_in_period);
	static double cavityGetE21ZTmm(double z,double time_in_period);
	static double cavityGetE22ZTmm(double z,double time_in_period);
	static double globalTime;
	static double oldGlobalTime;
	void refreshTrack(bool show);
	void saveTrack(Ogre::String filename);
	void refreshScreenTexture();
	float updateScreenPosition();
	~ElbePipe();
	Ogre::ManualObject*			track;
	Ogre::ManualObject*			track2;
	
private:
#ifdef ELBE_DRAW_ARROWS
	std::vector<Ogre::Billboard*> billboardAList;
	std::vector<float> billboardAListMultiplicator;
	std::vector<Ogre::Billboard*> billboardBList;
	std::vector<Ogre::Billboard*> billboardC1_1List;
	std::vector<Ogre::Billboard*> billboardC1_2List;
	std::vector<Ogre::Billboard*> billboardC2_1List;
	std::vector<Ogre::Billboard*> billboardC2_2List;
#endif
	std::vector<std::vector<Ogre::MaterialPtr> > quadropol;
	std::vector<Ogre::Vector3> quadropolPos;

	// synchronization value for cryostat arrows/field
	static const double SYNC_FACTOR;

	Ogre::SceneNode* holdNode;
	Ogre::SceneManager* m_pSceneMgr;

	std::list<ElbeBunch*>		bunch;
	ElbeBunch*					lastBunch;
	ElbeBunch*					cameraBunch;
	Ogre::SceneNode*			fakeBunchNode;	//Never moving Bunch, just for the camera ;-)
	ElbeBunch*					fakeBunch;
	bool						newCamera;
	Ogre::ManualObject*			emitterField;
	double						emitterFieldPosition;
	Ogre::Entity* screenEntity;
	Ogre::SceneNode* screenNode;
	Ogre::TexturePtr screenTexture;
	Ogre::Material* screenMaterial;
	float screenGauss[SCREEN_WIDTH][SCREEN_WIDTH];
	float oldScreenW,oldScreenH;
	Ogre::TexturePtr screenMap;
	Ogre::MaterialPtr arrowFieldMaterial[20];
	int wrongLine[2];
	static bool firstTime;
};
