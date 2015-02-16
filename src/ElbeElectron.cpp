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

#include "ElbeElectron.hpp"
#include "ElbePipe.hpp"
#include <Ogre.h>

#define calc_gamma(alpha,beta) ((1+alpha*alpha) / beta)

ElbeElectron::ElbeElectron(double time,double max_time,int electronCount){
	shown_particles = electronCount;
	ekin = ElbePipe::U_G1 * ElbeConstants::QS;
	gamma = ekin/ElbeConstants::EE+1.0f;
	beta = sqrt(1.0f-1.0f/(gamma*gamma));
	v = beta * ElbeConstants::CPSMM;
	energy = ElbeConstants::EE * gamma - ElbeConstants::EE;
	matrix = mat<double>(6,6);
	std::fstream f;
	f.open(std::string("Default.matrix").c_str(), std::ios_base::in);
	if (f)
	{
		char buffer[256];
		f.get(buffer,256,' ');
		matrix(0,0) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(0,1) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(0,2) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(0,3) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(0,4) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,'\n');
		matrix(0,5) = atof(buffer);
		f.get(); //the '\n'

		f.get(buffer,256,' ');
		matrix(1,0) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(1,1) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(1,2) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(1,3) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(1,4) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,'\n');
		matrix(1,5) = atof(buffer);
		f.get(); //the '\n'

		f.get(buffer,256,' ');
		matrix(2,0) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(2,1) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(2,2) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(2,3) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(2,4) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,'\n');
		matrix(2,5) = atof(buffer);
		f.get(); //the '\n'

		f.get(buffer,256,' ');
		matrix(3,0) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(3,1) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(3,2) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(3,3) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(3,4) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,'\n');
		matrix(3,5) = atof(buffer);
		f.get(); //the '\n'

		f.get(buffer,256,' ');
		matrix(4,0) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(4,1) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(4,2) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(4,3) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(4,4) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,'\n');
		matrix(4,5) = atof(buffer);
		f.get(); //the '\n'

		f.get(buffer,256,' ');
		matrix(5,0) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(5,1) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(5,2) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(5,3) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,' ');
		matrix(5,4) = atof(buffer);
		f.get(); //the ' '
		f.get(buffer,256,'\n');
		matrix(5,5) = atof(buffer);
		f.get(); //the '\n'

		f.close();
	}
	else
	{
		matrix.identity();
		matrix(0,0) = 4;
		matrix(0,1) = -0.0175;
		matrix(1,0) = -0.0175;
		matrix(1,1) = 0.0001;
		matrix(2,2) = 4;
		matrix(2,3) = -0.0175;
		matrix(3,2) = -0.0175;
		matrix(3,3) = 0.0001;
		matrix(4,4) = 2500;
		matrix(5,5) = 1.6 * pow(10.0, -7.0);
	}
	stable = mat<double>(6,6);
	matrix.copy(0, 0, 6, 6, stable);
	drift = mat<double>(6,6);

	L = 0.0;
	pos = 0;
	driftnumber = 1;
	previousDriftnumber = 1;

	//init alpha, beta, gamma and epsilon of the Phasenrooms
	float A = stable(0,1);
	float B = stable(0,0);
	float C = stable(1,1);
	float epsilon = sqrt(C*B-A*A);
	Xraum.start_alpha = Xraum.alpha = A/(-epsilon);
	Xraum.start_beta = Xraum.beta = B/epsilon;
	Xraum.epsilon = epsilon;
	Xraum.start_gamma = Xraum.gamma = calc_gamma(Xraum.start_alpha,Xraum.start_beta);
	A = stable(2,3);
	B = stable(2,2);
	C = stable(3,3);
	epsilon = sqrt(C*B-A*A);
	Yraum.start_alpha = Yraum.alpha = A/(-epsilon);
	Yraum.start_beta = Yraum.beta = B/epsilon;
	Yraum.epsilon = epsilon;
	Yraum.start_gamma = Yraum.gamma = calc_gamma(Yraum.start_alpha,Yraum.start_beta);
	A = stable(4,5);
	B = stable(4,4);
	C = stable(5,5);
	epsilon = sqrt(C*B-A*A);
	Zraum.start_alpha = Zraum.alpha = A/(-epsilon);
	Zraum.start_beta = Zraum.beta = B/epsilon;
	Zraum.epsilon = epsilon;
	Zraum.start_gamma = Zraum.gamma = calc_gamma(Zraum.start_alpha,Zraum.start_beta);
	diceRaum(&Xraum);
	diceRaum(&Yraum);
	diceRaum(&Zraum);

	if (time>0)
		calc(time,max_time);
}

ElbeElectron::~ElbeElectron(){
}

float ElbeElectron::calc_phi(float alpha,float beta,float gamma)
{
	float ATAN = 2.0f*alpha/(gamma-beta);
	float phi = atan(ATAN)/2.0f;
	if (gamma > beta)
	{
		if (alpha < 0.0f)
			phi-=M_PI/2.0f;
		else
			phi+=M_PI/2.0f;
	}
	return phi;
}

float ElbeElectron::GAUSS(tPhasenraum* raum,float x)
{
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(phi);
	float COS = cos(phi);
	float A=sqrt(raum->epsilon/(raum->gamma*COS*COS-2.0f*raum->alpha*COS*SIN+raum->beta*SIN*SIN));
	float B=sqrt(raum->epsilon/(raum->gamma*SIN*SIN+2.0f*raum->alpha*COS*SIN+raum->beta*COS*COS));
	float R11 = +COS/A;
	float R12 = +SIN/B;
	float R21 = -SIN/A;
	float R22 = +COS/B;
	float a = (R11*R11+R12*R12)/2.0f;
	float b =  R12*R22+R11*R21;
	float c = (R21*R21+R22*R22)/2.0f;
	float k = sqrt(1.0f/(16.0f*M_PI*c));
	float l = b*b/(4.0f*c)-a;
	float m = (b*raum->beta-2.0f*c*raum->alpha)/(2.0f*sqrt(c)*raum->beta);
	float n = c;
	float part = sqrt(n*(raum->beta*raum->epsilon-x*x))/raum->beta;
	float result = fabs(k*exp(l*4.0f*x*x)*(erf(m*2.0f*x+part)-erf(m*2.0f*x-part)));
	return result;
}


void ElbeElectron::diceRaum(tPhasenraum* raum)
{
	//dicing the particles positions:
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Dicing Phasenraum with the following values:");
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("alpha = "+Ogre::StringConverter::toString(raum->alpha));
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("beta = "+Ogre::StringConverter::toString(raum->beta));
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("gamma = "+Ogre::StringConverter::toString(raum->gamma));
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("epsilon = "+Ogre::StringConverter::toString(raum->epsilon));
	float phi = calc_phi(raum->alpha,raum->beta,raum->gamma);
	float SIN = -sin(phi);
	float COS = cos(phi);
	float a=sqrt(raum->epsilon/(raum->gamma*COS*COS-2.0f*raum->alpha*COS*SIN+raum->beta*SIN*SIN));
	float b=sqrt(raum->epsilon/(raum->gamma*SIN*SIN+2.0f*raum->alpha*COS*SIN+raum->beta*COS*COS));
	int i;
	for (i = 0; i < shown_particles; i++)
	{
		float u1,u2,q;
		do
		{
			u1 = ((float)(rand())/(float)RAND_MAX)*2.0f-1.0f;
			u2 = ((float)(rand())/(float)RAND_MAX)*2.0f-1.0f;
			q = u1*u1+u2*u2;
		}
		while (q == 0.0f || q > 1.0f);
		float p = sqrt(-2.0f*log(q)/q);
		float x = u1*p/3.0f*a;
		float y = u2*p/3.0f*b;
		raum->particle[0][i] = raum->start_particle[0][i] = x*COS+y*SIN;
		raum->particle[1][i] = raum->start_particle[1][i] = x*-SIN+y*COS;
	}	
}

double ElbeElectron::getDriftLength(int numDriftTrack) {
	switch(numDriftTrack)
	{
		case 0: return ElbePipe::L_DT0;
		case 1: return ElbePipe::L_DT1;
		case 2: return ElbePipe::L_DT2;
		case 3: return ElbePipe::L_DT3;
		case 4: return ElbePipe::L_DT4;
		case 5: return ElbePipe::L_DT5;
		case 6: return ElbePipe::L_DT6;
		case 7: return ElbePipe::L_DT7;
		case 8: return ElbePipe::L_DT8;
		case 9: return ElbePipe::L_DT9;
		case 10: return ElbePipe::L_DT10;
		case 11: return ElbePipe::L_DT11;
		case 12: return ElbePipe::L_DT12;
		case 13: return ElbePipe::L_DT13;
		case 14: return ElbePipe::L_DT14;
		case 15: return ElbePipe::L_DT15;
		case 16: return ElbePipe::L_DT16;
		case 17: return ElbePipe::L_DT17;
		case 18: return ElbePipe::L_DT18;
		case 19: return ElbePipe::L_DT19;
		case 20: return ElbePipe::L_DT20;
		case 21: return ElbePipe::L_DT21;
		case 22: return ElbePipe::L_DT22;
		case 23: return ElbePipe::L_DT23;
		case 24: return ElbePipe::L_DT24;
		case 25: return ElbePipe::L_DT25;
		case 26: return ElbePipe::L_DT26;
		case 27: return ElbePipe::L_DT27;
		case 28: return ElbePipe::L_DT28;
		case 29: return ElbePipe::L_DT29;
		case 30: return ElbePipe::L_DT30;
		default: return 10000;
	}
}

double ElbeElectron::getKumDriftLength(int numDriftTrack)
{
	if (numDriftTrack<0)
		return 0;
	return getKumDriftLength(numDriftTrack-1)+getDriftLength(numDriftTrack);
}

int ElbeElectron::getNumFromPosition(float position) {
	if (position < 0.0f)
		return 0;
	int i;
	for (i=0;i<=30;i++)
	{
		position-=getDriftLength(i);
		if (position<0.0f)
			break;
	}
	return i;
}

double ElbeElectron::getRealDriftLength(int numDriftTrack) {
	switch(numDriftTrack)
	{
		case 0: return ElbePipe::PATHSTART_POS   - ElbePipe::EMITTER_POS;
		case 1: return ElbePipe::EMITTER_POS     - ElbePipe::SOLENOID_1_POS;
		case 2: return ElbePipe::SOLENOID_1_POS  - ElbePipe::BUNCHER_1_POS;
		case 3: return ElbePipe::BUNCHER_1_POS   - ElbePipe::SOLENOID_2_POS;
		case 4: return ElbePipe::SOLENOID_2_POS  - ElbePipe::SOLENOID_3_POS;
		case 5: return ElbePipe::SOLENOID_3_POS  - ElbePipe::SOLENOID_4_POS;
		case 6: return ElbePipe::SOLENOID_4_POS  - ElbePipe::BUNCHER_2_POS;
		case 7: return ElbePipe::BUNCHER_2_POS   - ElbePipe::SOLENOID_5_POS;
		case 8: return ElbePipe::SOLENOID_5_POS  - ElbePipe::CAVITY_START_POS;
		case 9: return ElbePipe::CAVITY_START_POS  - ElbePipe::CAVITY_END_POS;
		case 10: return ElbePipe::CAVITY_END_POS  - ElbePipe::CAVITY_START_POS_2;
		case 11: return ElbePipe::CAVITY_START_POS_2  - ElbePipe::CAVITY_END_POS_2;
		case 12: return ElbePipe::CAVITY_END_POS_2  - ElbePipe::QUADRUPOL_1_POS;
		case 13: return ElbePipe::QUADRUPOL_1_POS  - ElbePipe::QUADRUPOL_2_POS;
		case 14: return ElbePipe::QUADRUPOL_2_POS  - ElbePipe::QUADRUPOL_3_POS;
		case 15: return ElbePipe::QUADRUPOL_3_POS  - ElbePipe::QUADRUPOL_4_POS;
		case 16: return ElbePipe::QUADRUPOL_4_POS  - ElbePipe::QUADRUPOL_5_POS;
		case 17: return ElbePipe::QUADRUPOL_5_POS  - ElbePipe::QUADRUPOL_6_POS;
		case 18: return ElbePipe::QUADRUPOL_6_POS  - ElbePipe::QUADRUPOL_7_POS;
		case 19: return ElbePipe::QUADRUPOL_7_POS  - ElbePipe::CAVITY_START_POS_3;
		case 20: return ElbePipe::CAVITY_START_POS_3  - ElbePipe::CAVITY_END_POS_3;
		case 21: return ElbePipe::CAVITY_END_POS_3  - ElbePipe::CAVITY_START_POS_4;
		case 22: return ElbePipe::CAVITY_START_POS_4  - ElbePipe::CAVITY_END_POS_4;
		case 23: return ElbePipe::CAVITY_END_POS_4  - ElbePipe::QUADRUPOL_8_POS;
		case 24: return ElbePipe::QUADRUPOL_8_POS  - ElbePipe::QUADRUPOL_9_POS;
		case 25: return ElbePipe::QUADRUPOL_9_POS  - ElbePipe::QUADRUPOL_10_POS;
		case 26: return ElbePipe::QUADRUPOL_10_POS  - ElbePipe::QUADRUPOL_11_POS;
		case 27: return ElbePipe::QUADRUPOL_11_POS  - ElbePipe::QUADRUPOL_12_POS;
		case 28: return ElbePipe::QUADRUPOL_12_POS  - ElbePipe::QUADRUPOL_13_POS;
		case 29: return ElbePipe::QUADRUPOL_13_POS  - ElbePipe::QUADRUPOL_14_POS;
		case 30: return ElbePipe::QUADRUPOL_14_POS  - ElbePipe::END_POS;
		default: return 1000;
	}
}

double ElbeElectron::getRealDriftPosition(int numDriftTrack) {
	switch(numDriftTrack)
	{
		case 0: return ElbePipe::PATHSTART_POS - ElbePipe::PATHSTART_POS;
		case 1: return ElbePipe::PATHSTART_POS - ElbePipe::EMITTER_POS;
		case 2: return ElbePipe::PATHSTART_POS - ElbePipe::SOLENOID_1_POS;
		case 3: return ElbePipe::PATHSTART_POS - ElbePipe::BUNCHER_1_POS;
		case 4: return ElbePipe::PATHSTART_POS - ElbePipe::SOLENOID_2_POS;
		case 5: return ElbePipe::PATHSTART_POS - ElbePipe::SOLENOID_3_POS;
		case 6: return ElbePipe::PATHSTART_POS - ElbePipe::SOLENOID_4_POS;
		case 7: return ElbePipe::PATHSTART_POS - ElbePipe::BUNCHER_2_POS;
		case 8: return ElbePipe::PATHSTART_POS - ElbePipe::SOLENOID_5_POS;
		case 9: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_START_POS;
		case 10: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_END_POS;
		case 11: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_START_POS_2;
		case 12: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_END_POS_2;
		case 13: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_1_POS;
		case 14: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_2_POS;
		case 15: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_3_POS;
		case 16: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_4_POS;
		case 17: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_5_POS;
		case 18: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_6_POS;
		case 19: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_7_POS;
		case 20: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_START_POS_3;
		case 21: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_END_POS_3;
		case 22: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_START_POS_4;
		case 23: return ElbePipe::PATHSTART_POS - ElbePipe::CAVITY_END_POS_4;
		case 24: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_8_POS;
		case 25: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_9_POS;
		case 26: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_10_POS;
		case 27: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_11_POS;
		case 28: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_12_POS;
		case 29: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_13_POS;
		case 30: return ElbePipe::PATHSTART_POS - ElbePipe::QUADRUPOL_14_POS;
		default: return 1000;
	}
}

mat<double> ElbeElectron::getDriftMatrix(int driftNumber,double position,double gamma, double optionalLength) { //0..1
	mat<double> result(6, 6, 0.0);
	double length = position * getTrackLength(driftNumber);
	if (driftnumber == 9 || driftnumber == 11 || driftnumber == 20 || driftnumber == 22)
		length = optionalLength;
	result.identity();
	if (driftNumber!=0)
	{
		result(0,1) = result(2,3) = length;
		result(4,5) = length / (gamma * gamma);
	}
	return result;
}

mat<double> ElbeElectron::getCavityDriftMatrix(int driftNumber,double position,double gamma, double optionalLength) { //0..1
	mat<double> result(6, 6, 0.0);
	double length = position * getTrackLength(driftNumber);
	if (driftnumber == 9 || driftnumber == 11 || driftnumber == 20 || driftnumber == 22)
		length = optionalLength;
	result.identity();
	double p1 = oldGamma * ElbeConstants::ME_E31 * oldV; //v is in mm/ps!
	double p2 =    gamma * ElbeConstants::ME_E31 *    v; //v is in mm/ps!
	double E1 = sqrt(ElbeConstants::EE*ElbeConstants::EE + (p1*ElbeConstants::CPSMM)*(p1*ElbeConstants::CPSMM));
	double E2 = sqrt(ElbeConstants::EE*ElbeConstants::EE + (p2*ElbeConstants::CPSMM)*(p2*ElbeConstants::CPSMM));
	if (driftNumber!=0 && E1!=E2)
	{
		energy = E2 - ElbeConstants::EE;
		double L_cav = optionalLength;//1038.0f; //mm
		double L_eff = L_cav * p1*ElbeConstants::CPSMM/(E2-E1)*log((p2*ElbeConstants::CPSMM+E2)/(p1*ElbeConstants::CPSMM+E1)); //length/pow(gamma,1.0/3.0);
		result(0,1) = result(2,3) = L_eff;
		result(1,1) = result(3,3) = p1 / p2;
		result(4,4) = beta/oldBeta;
		result(4,5) = L_cav*p1*(beta-oldBeta)*ElbeConstants::CPSMM/(E2-E1);
		result(5,5) = p1 / p2 * oldBeta/beta;
	}
	return result;
}

double ElbeElectron::getTrackLength(int driftNumber) {
	double maxlength = 0.0;
	switch (driftNumber) {
		case 0: maxlength = ElbePipe::L_DT0; break;
		case 1: maxlength = ElbePipe::L_DT1; break;
		case 2: maxlength = ElbePipe::L_DT2; break;
		case 3: maxlength = ElbePipe::L_DT3; break;
		case 4: maxlength = ElbePipe::L_DT4; break;
		case 5: maxlength = ElbePipe::L_DT5; break;
		case 6: maxlength = ElbePipe::L_DT6; break;
		case 7: maxlength = ElbePipe::L_DT7; break;
		case 8: maxlength = ElbePipe::L_DT8; break;
		case 9: maxlength = ElbePipe::L_DT9; break;
		case 10: maxlength = ElbePipe::L_DT10; break;
		case 11: maxlength = ElbePipe::L_DT11; break;
		case 12: maxlength = ElbePipe::L_DT12; break;
		case 13: maxlength = ElbePipe::L_DT13; break;
		case 14: maxlength = ElbePipe::L_DT14; break;
		case 15: maxlength = ElbePipe::L_DT15; break;
		case 16: maxlength = ElbePipe::L_DT16; break;
		case 17: maxlength = ElbePipe::L_DT17; break;
		case 18: maxlength = ElbePipe::L_DT18; break;
		case 19: maxlength = ElbePipe::L_DT19; break;
		case 20: maxlength = ElbePipe::L_DT20; break;
		case 21: maxlength = ElbePipe::L_DT21; break;
		case 22: maxlength = ElbePipe::L_DT22; break;
		case 23: maxlength = ElbePipe::L_DT23; break;
		case 24: maxlength = ElbePipe::L_DT24; break;
		case 25: maxlength = ElbePipe::L_DT25; break;
		case 26: maxlength = ElbePipe::L_DT26; break;
		case 27: maxlength = ElbePipe::L_DT27; break;
		case 28: maxlength = ElbePipe::L_DT28; break;
		case 29: maxlength = ElbePipe::L_DT29; break;
		case 30: maxlength = ElbePipe::L_DT30; break;
		default: maxlength = 10000; break;
	}
	return maxlength;
}

bool ElbeElectron::calc_drift(double oldPos,int drift_matrix)
{
	switch (drift_matrix)
	{
		case 0:
			drift = getDriftMatrix(driftnumber,L/getTrackLength(driftnumber),gamma);
			break;
		case 1:
			drift = getCavityDriftMatrix(driftnumber,1.0,gamma,pos-oldPos);
			/*for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 6; j++)
					printf("%.7f ",drift(i,j));
				printf("\n");
			}
			printf("%f\n",pos-oldPos);*/
			break;
		case 2:
			drift = getDriftMatrix(driftnumber,1.0f,gamma);
			break;
	}	
	mat<double> driftT(6, 6);
	drift.copy(0,0,6,6,driftT);
	driftT.transpose();
	if (drift_matrix == 2)
		stable = drift * stable *driftT;
	else
		matrix = drift * stable *driftT;
	int i;
	for (i = 0; i < shown_particles; i++)
	{
		Xraum.particle[0][i] = drift(0,0)*Xraum.start_particle[0][i]
							 + drift(0,1)*Xraum.start_particle[1][i];
		Xraum.particle[1][i] = drift(1,0)*Xraum.start_particle[0][i]
							 + drift(1,1)*Xraum.start_particle[1][i];
		Yraum.particle[0][i] = drift(2,2)*Yraum.start_particle[0][i]
							 + drift(2,3)*Yraum.start_particle[1][i];
		Yraum.particle[1][i] = drift(3,2)*Yraum.start_particle[0][i]
							 + drift(3,3)*Yraum.start_particle[1][i];
		Zraum.particle[0][i] = drift(4,4)*Zraum.start_particle[0][i]
							 + drift(4,5)*Zraum.start_particle[1][i];
		Zraum.particle[1][i] = drift(5,4)*Zraum.start_particle[0][i]
							 + drift(5,5)*Zraum.start_particle[1][i];
	}
	Xraum.beta = drift(0,0)*drift(0,0)*Xraum.start_beta
		       - 2.0f*drift(0,0)*drift(0,1)*Xraum.start_alpha
		       + drift(0,1)*drift(0,1)*Xraum.start_gamma;
	Xraum.alpha = -drift(0,0)*drift(1,0)*Xraum.start_beta
		        + (drift(0,0)*drift(1,1)+drift(0,1)*drift(1,0))*Xraum.start_alpha
		        - drift(0,1)*drift(1,1)*Xraum.start_gamma;
	Xraum.gamma = drift(1,0)*drift(1,0)*Xraum.start_beta
		        - 2.0f*drift(1,0)*drift(1,1)*Xraum.start_alpha
		        + drift(1,1)*drift(1,1)*Xraum.start_gamma;
	Yraum.beta = drift(2,2)*drift(2,2)*Yraum.start_beta
		       - 2.0f*drift(2,2)*drift(2,3)*Yraum.start_alpha
		       + drift(2,3)*drift(2,3)*Yraum.start_gamma;
	Yraum.alpha = -drift(2,2)*drift(3,2)*Yraum.start_beta
		        + (drift(2,2)*drift(3,3)+drift(2,3)*drift(3,2))*Yraum.start_alpha
		        - drift(2,3)*drift(3,3)*Yraum.start_gamma;
	Yraum.gamma = drift(3,2)*drift(3,2)*Yraum.start_beta
		        - 2.0f*drift(3,2)*drift(3,3)*Yraum.start_alpha
		        + drift(3,3)*drift(3,3)*Yraum.start_gamma;
	Zraum.beta = drift(4,4)*drift(4,4)*Zraum.start_beta
		       - 2.0f*drift(4,4)*drift(4,5)*Zraum.start_alpha
		       + drift(4,5)*drift(4,5)*Zraum.start_gamma;
	Zraum.alpha = -drift(4,4)*drift(5,4)*Zraum.start_beta
		        + (drift(4,4)*drift(5,5)+drift(4,5)*drift(5,4))*Zraum.start_alpha
		        - drift(4,5)*drift(5,5)*Zraum.start_gamma;
	Zraum.gamma = drift(5,4)*drift(5,4)*Zraum.start_beta
		        - 2.0f*drift(5,4)*drift(5,5)*Zraum.start_alpha
		        + drift(5,5)*drift(5,5)*Zraum.start_gamma;
	//Screen:
	if (oldPos < ElbePipe::screenPosition && pos >= ElbePipe::screenPosition)
	{
		ElbePipe::screenW = sqrt(matrix(0,0));
		ElbePipe::screenH = sqrt(matrix(2,2));
	}
	return true;
}

bool ElbeElectron::calc_discrete(double time,double time_in_period)
{
	double oldPos = pos;
	oldGamma = gamma;
	oldV = v;
	oldBeta = beta;
	//Everyday I'm drifting!
	L   += time*v;
	pos += time*v;
	if (driftnumber == 9) //1. Cavity
	{
		v   += pow(1.0-pow(v/ElbeConstants::CPSMM,2.0),1.5) * ElbePipe::E1_CRYOS1 * 0.00017585 * ElbePipe::cavityGetE11ZTmm(L,time_in_period)*time;
		if (L < 0.0)
			return false;
	}
	else
	if (driftnumber == 10 || driftnumber == 21)
	{
		if (L < 0.0)
		{
			driftnumber--;
			L = getDriftLength(driftnumber)+L;
		}
	}
	else
	if (driftnumber == 11) //2. Cavity
	{
		v   += pow(1.0-pow(v/ElbeConstants::CPSMM,2.0),1.5) * ElbePipe::E2_CRYOS1 * 0.00017585 * ElbePipe::cavityGetE12ZTmm(L,time_in_period)*time;
		if (L < 0.0)
		{
			driftnumber--;
			L = getDriftLength(driftnumber)+L;
		}
	}
	else
	if (driftnumber == 20) //1. Cavity
	{
		v   += pow(1.0-pow(v/ElbeConstants::CPSMM,2.0),1.5) * ElbePipe::E1_CRYOS2 * 0.00017585 * ElbePipe::cavityGetE21ZTmm(L,time_in_period)*time;
		if (L < 0.0)
			return false;
	}
	else
	if (driftnumber == 22) //2. Cavity
	{
		v   += pow(1.0-pow(v/ElbeConstants::CPSMM,2.0),1.5) * ElbePipe::E2_CRYOS2 * 0.00017585 * ElbePipe::cavityGetE22ZTmm(L,time_in_period)*time;
		if (L < 0.0)
		{
			driftnumber--;
			L = getDriftLength(driftnumber)+L;
		}
	}
	while (L >= getDriftLength(driftnumber))
	{
		L -= getDriftLength(driftnumber);
		if (driftnumber!=9 && driftnumber!=11 && driftnumber!=20 && driftnumber!=22)
		{
			calc_drift(oldPos,2);
			mat<double> change(6,6);
			change.identity();
			switch (driftnumber)
			{
				case 1:
					change = createSolenoidMatrix(ElbePipe::I_S1);
					break;
				case 2: 
					change = createBuncherMatrix(ElbePipe::A_B, ElbePipe::U_B1, ElbePipe::P_B1, ElbePipe::F_B1);
					break;
				case 3:
					change = createSolenoidMatrix(ElbePipe::I_S2);
					break;
				case 4:
					change = createSolenoidMatrix(ElbePipe::I_S3);
					break;
				case 5:
					change = createSolenoidMatrix(ElbePipe::I_S4);
					break;
				case 6:
					change = createBuncherMatrix(ElbePipe::A_B, ElbePipe::U_B2, ElbePipe::P_B2, ElbePipe::F_B2);
					break;
				case 7:
					change = createSolenoidMatrix(ElbePipe::I_S5);
					break;
				case 12:
					change = createQuadrupoleMatrix(ElbePipe::I_Q1);
					break;
				case 13:
					change = createQuadrupoleMatrix(ElbePipe::I_Q2);
					break;
				case 14:
					change = createQuadrupoleMatrix(ElbePipe::I_Q3);
					break;
				case 15:
					change = createQuadrupoleMatrix(ElbePipe::I_Q4);
					break;
				case 16:
					change = createQuadrupoleMatrix(ElbePipe::I_Q5);
					break;
				case 17:
					change = createQuadrupoleMatrix(ElbePipe::I_Q6);
					break;
				case 18:
					change = createQuadrupoleMatrix(ElbePipe::I_Q7);
					break;
				case 23:
					change = createQuadrupoleMatrix(ElbePipe::I_Q8);
					break;
				case 24:
					change = createQuadrupoleMatrix(ElbePipe::I_Q9);
					break;
				case 25:
					change = createQuadrupoleMatrix(ElbePipe::I_Q10);
					break;
				case 26:
					change = createQuadrupoleMatrix(ElbePipe::I_Q11);
					break;
				case 27:
					change = createQuadrupoleMatrix(ElbePipe::I_Q12);
					break;
				case 28:
					change = createQuadrupoleMatrix(ElbePipe::I_Q13);
					break;
				case 29:
					change = createQuadrupoleMatrix(ElbePipe::I_Q14);
					break;
			}
			int i;
			for (i = 0; i < shown_particles; i++)
			{
				Xraum.start_particle[0][i] = change(0,0)*Xraum.particle[0][i]
										   + change(0,1)*Xraum.particle[1][i];
				Xraum.start_particle[1][i] = change(1,0)*Xraum.particle[0][i]
										   + change(1,1)*Xraum.particle[1][i];
				Yraum.start_particle[0][i] = change(2,2)*Yraum.particle[0][i]
										   + change(2,3)*Yraum.particle[1][i];
				Yraum.start_particle[1][i] = change(3,2)*Yraum.particle[0][i]
										   + change(3,3)*Yraum.particle[1][i];
				Zraum.start_particle[0][i] = change(4,4)*Zraum.particle[0][i]
										   + change(4,5)*Zraum.particle[1][i];
				Zraum.start_particle[1][i] = change(5,4)*Zraum.particle[0][i]
										   + change(5,5)*Zraum.particle[1][i];
			}
			memcpy(Xraum.particle[0],Xraum.start_particle[0],sizeof(float)*shown_particles);
			memcpy(Xraum.particle[1],Xraum.start_particle[1],sizeof(float)*shown_particles);
			memcpy(Yraum.particle[0],Yraum.start_particle[0],sizeof(float)*shown_particles);
			memcpy(Yraum.particle[1],Yraum.start_particle[1],sizeof(float)*shown_particles);
			memcpy(Zraum.particle[0],Zraum.start_particle[0],sizeof(float)*shown_particles);
			memcpy(Zraum.particle[1],Zraum.start_particle[1],sizeof(float)*shown_particles);
			Xraum.start_beta = change(0,0)*change(0,0)*Xraum.beta
					   - 2.0f*change(0,0)*change(0,1)*Xraum.alpha
					   + change(0,1)*change(0,1)*Xraum.gamma;
			Xraum.start_alpha = -change(0,0)*change(1,0)*Xraum.beta
						+ (change(0,0)*change(1,1)+change(0,1)*change(1,0))*Xraum.alpha
						- change(0,1)*change(1,1)*Xraum.gamma;
			Xraum.start_gamma = change(1,0)*change(1,0)*Xraum.beta
						- 2.0f*change(1,0)*change(1,1)*Xraum.alpha
						+ change(1,1)*change(1,1)*Xraum.gamma;
			Yraum.start_beta = change(2,2)*change(2,2)*Yraum.beta
					   - 2.0f*change(2,2)*change(2,3)*Yraum.alpha
					   + change(2,3)*change(2,3)*Yraum.gamma;
			Yraum.start_alpha = -change(2,2)*change(3,2)*Yraum.beta
						+ (change(2,2)*change(3,3)+change(2,3)*change(3,2))*Yraum.alpha
						- change(2,3)*change(3,3)*Yraum.gamma;
			Yraum.start_gamma = change(3,2)*change(3,2)*Yraum.beta
						- 2.0f*change(3,2)*change(3,3)*Yraum.alpha
						+ change(3,3)*change(3,3)*Yraum.gamma;
			Zraum.start_beta = change(4,4)*change(4,4)*Zraum.beta
					   - 2.0f*change(4,4)*change(4,5)*Zraum.alpha
					   + change(4,5)*change(4,5)*Zraum.gamma;
			Zraum.start_alpha = -change(4,4)*change(5,4)*Zraum.beta
						+ (change(4,4)*change(5,5)+change(4,5)*change(5,4))*Zraum.alpha
						- change(4,5)*change(5,5)*Zraum.gamma;
			Zraum.start_gamma = change(5,4)*change(5,4)*Zraum.beta
						- 2.0f*change(5,4)*change(5,5)*Zraum.alpha
						+ change(5,5)*change(5,5)*Zraum.gamma;
			Xraum.beta = Xraum.start_beta;
			Xraum.alpha = Xraum.start_alpha;
			Xraum.gamma = Xraum.start_gamma;
			Yraum.beta = Yraum.start_beta;
			Yraum.alpha = Yraum.start_alpha;
			Yraum.gamma = Yraum.start_gamma;
			Zraum.beta = Zraum.start_beta;
			Zraum.alpha = Zraum.start_alpha;
			Zraum.gamma = Zraum.start_gamma;
		    mat<double> changeT(6,6);
			change.copy(0,0,6,6,changeT);
			changeT.transpose();
			stable = change * stable * changeT;
		}
		driftnumber++;
	}
	beta = v/ElbeConstants::CPSMM;
	gamma = sqrt(1.0/(1.0-beta*beta));

	mat<double> drift(6, 6);
	/*Xraum.epsilon = matrix(0,0)/Xraum.beta;
	Yraum.epsilon = matrix(2,2)/Yraum.beta;
	Zraum.epsilon = matrix(4,4)/Zraum.beta;
	printf("X α: %.3f β: %.3f γ: %.3f ε: %.3f\n",Xraum.alpha,Xraum.beta,Xraum.gamma,Xraum.epsilon);
	printf("Y α: %.3f β: %.3f γ: %.3f ε: %.3f\n",Yraum.alpha,Yraum.beta,Yraum.gamma,Yraum.epsilon);
	printf("Z α: %.3f β: %.3f γ: %.3f ε: %.3f\n",Zraum.alpha,Zraum.beta,Zraum.gamma,Zraum.epsilon);*/
	if (driftnumber==9 || driftnumber==11 || driftnumber==20 || driftnumber==22)
	{
		stable = matrix;
		memcpy(Xraum.start_particle[0],Xraum.particle[0],sizeof(float)*shown_particles);
		memcpy(Xraum.start_particle[1],Xraum.particle[1],sizeof(float)*shown_particles);
		memcpy(Yraum.start_particle[0],Yraum.particle[0],sizeof(float)*shown_particles);
		memcpy(Yraum.start_particle[1],Yraum.particle[1],sizeof(float)*shown_particles);
		memcpy(Zraum.start_particle[0],Zraum.particle[0],sizeof(float)*shown_particles);
		memcpy(Zraum.start_particle[1],Zraum.particle[1],sizeof(float)*shown_particles);
		Xraum.start_beta = Xraum.beta;
		Xraum.start_alpha = Xraum.alpha;
		Xraum.start_gamma = Xraum.gamma;
		Yraum.start_beta = Yraum.beta;
		Yraum.start_alpha = Yraum.alpha;
		Yraum.start_gamma = Yraum.gamma;
		Zraum.start_beta = Zraum.beta;
		Zraum.start_alpha = Zraum.alpha;
		Zraum.start_gamma = Zraum.gamma;
		calc_drift(oldPos,1);
	}
	else
		calc_drift(oldPos,0);

	if (driftnumber >= 31) //end of pipe
		return false;
	return true;
}

bool ElbeElectron::calc(double time,double max_time)
{
	double time_in_period = ElbePipe::oldGlobalTime;
	previousDriftnumber = driftnumber;
	while (time > 0.0f)
	{
		double oldTime = time;
		if (max_time > time)
			max_time = time;
		//Special drift case...
		if (driftnumber == 9 || driftnumber == 11 || driftnumber == 20 || driftnumber == 22) //no cavity drift!
		{
			if (!calc_discrete(max_time,time_in_period))
				return false;
			time -= max_time;
		}
		else
		{
			double s = time*v;
			if (L+s > getDriftLength(driftnumber))
			{
				s = getDriftLength(driftnumber)-L;
				if (!calc_discrete(s/v,time_in_period))
					return false;
				time -= s/v;
			}
			else
			{
				if (!calc_discrete(time,time_in_period))
					return false;
				time = 0.0f;
			}
		}
		time_in_period += oldTime-time;
		while (time_in_period > ElbeConstants::P_026PS) //260 Mhz
			time_in_period -= ElbeConstants::P_026PS;
	}
	return true;
}

Ogre::Vector3 ElbeElectron::getBunchScale(){
	double sx = sqrt(matrix(0,0));
	double sy = sqrt(matrix(2,2));
	double sz = sqrt(matrix(4,4));
	return Ogre::Vector3(sx, sy, sz);
}

mat<double> ElbeElectron::createSolenoidMatrix(double I)
{
	mat<double> matrix(6, 6, 0.0);
	double f = 344.6 / (I * I);

	matrix.identity();
	matrix(1,0) = matrix(3,2) = -1 / f;

	return matrix;
}

mat<double> ElbeElectron::createBuncherMatrix(double a, double U, double phi, double f)
{
	mat<double> matrix(6, 6, 0.0);

	phi = ElbeConstants::PI * phi / 180;

	double w1 = ekin;
	double w0 = ElbeConstants::QS * U * cos(phi);
	double lambda = ElbeConstants::CMM / f;
	double r65 = sin(phi) * (2 * ElbeConstants::PI * ElbeConstants::QS * U)/((w1 + w0) * beta * lambda);

	matrix.identity();
	matrix(0,1) = a;
	matrix(2,3) = a;
	matrix(4,5) = a / (gamma * gamma);
	matrix(5,4) = r65;
	matrix(5,5) = w1 / (w1 + w0);

	return matrix;
}

mat<double> ElbeElectron::createQuadrupoleMatrix(double I)
{
	mat<double> matrix(6, 6, 0.0);

	matrix.identity();
	double k = -153.8*I/(energy*0.6242-0.511)/1000000.0; //1/mm²
	double L = 100.0; //mm
	matrix(1,0) = -k*L;
	matrix(3,2) = k*L;
	matrix(4,5) = 1.0;

	return matrix;
}

#ifdef WIN32
float erf_positive(float x)
{
	float devisor = 1.0+0.278393*x+0.230389*x*x+0.000972*x*x*x+0.078108*x*x*x*x;
	devisor *= devisor; //²
	devisor *= devisor; //²
	return 1.0f-1.0f/devisor;
}

float erf(float x)
{
	if (x < 0.0f) 
		return -erf_positive(-x);
	return erf_positive(x);
}
#endif
