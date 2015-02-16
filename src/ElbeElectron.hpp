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

#ifndef _ELBE_ELECTRON_H
#define _ELBE_ELECTRON_H

#include "mat.h"
#include "OgreFramework.hpp"

//! An electron of the ELBE particle accelerator. Just calculations, no visualization
/*!
  This class calculates the electron flying through the beam line.
*/

#define MAX_PARTICLE_COUNT 21000

typedef struct {
	float start_particle[2][MAX_PARTICLE_COUNT];
	float particle[2][MAX_PARTICLE_COUNT];
	float start_alpha;
	float start_beta;
	float start_gamma;
	float epsilon;
	float alpha;
	float beta;
	float gamma;
} tPhasenraum;

class ElbeElectron {
	public:
		ElbeElectron(double time,double max_time,int electronCount = MAX_PARTICLE_COUNT);
		~ElbeElectron();
		/// calculates the next "time" milliseconds, but with maximal "max_way" mm way.
		/** That means, if you e.g. have time=4ms and max_way=0.9mm and v is 1 m/s, this
		function would calculate for the way time*v=4mm, but in four 0.9mm steps and 
		one 0.4mm step. Furthermore, if v is very small or even 0, min_time will be 
		calculated */
		bool calc(double time,double max_time);
		///Returns the scale of this electron bunch
		Ogre::Vector3 getBunchScale();
		///the matrix of the electron
		mat<double> matrix;
		///the last stable matrix
		mat<double> stable;
		///the current drift matrix
		mat<double> drift;
		///gamma
		double ekin;
		double oldGamma;
		double gamma;
		///beta
		double oldBeta;
		double beta;
		///the velocity of the electron in mm/ps
		double oldV;
		double v;
		///the relative position of the electron on the drift line in mm
		double L;
		///the position of the electron;
		double pos;
		///number of the driftline
		int driftnumber;

		tPhasenraum Xraum,Yraum,Zraum;

		float calc_phi(float alpha,float beta,float gamma);
		void diceRaum(tPhasenraum* raum);
		float GAUSS(tPhasenraum* raum,float x);

		int previousDriftnumber;
		static double getDriftLength(int numDriftTrack);
		static double getKumDriftLength(int numDriftTrack);
		static int getNumFromPosition(float position);
		static double getRealDriftLength(int numDriftTrack);
		static double getRealDriftPosition(int numDriftTrack);

		///Calculates the Drift Matrix
		/** <b>L</b> is the length of the drift line<br/>
		    The matrix looks like this:
			<!-- matrix expression begin -->
			<table style="color:#000" border="0"><tr style ="height:80px;">
			<td><table border=0 cellpadding=0 cellspacing=0px style="border-left:1px solid #000; border-right:1px solid #000; color:#000"><tr style ="height:80px;">
			<td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td><td><table border=0 cellpadding=0 cellspacing=0 style="color:#000;">
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>L</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>L</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>L <hr/> γ·γ</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			</tr>
			</table></td><td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td></tr></table></td>
			</tr></table>
			<!-- matrix expression end -->
		*/
		mat<double> getDriftMatrix(int driftNumber,double position,double gamma,double optionalLength = 0.0);

		double energy;
		///Calculates the Drift Matrix for the cavity
		/** <b>L</b> is the length of the drift line<br/>
		    The matrix looks like this:
			<!-- matrix expression begin -->
			<table style="color:#000" border="0"><tr style ="height:80px;">
			<td><table border=0 cellpadding=0 cellspacing=0px style="border-left:1px solid #000; border-right:1px solid #000; color:#000"><tr style ="height:80px;">
			<td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td><td><table border=0 cellpadding=0 cellspacing=0 style="color:#000;">
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>L <hr/> ∛γ</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>γ<sub>0</sub>·β<sub>0</sub>·c <hr/> γ<sub>1</sub>·β<sub>1</sub>·c</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>L <hr/> ∛γ</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>γ<sub>0</sub>·β<sub>0</sub>·c <hr/> γ<sub>1</sub>·β<sub>1</sub>·c</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>β<sub>0</sub> <hr/> β<sub>1</sub></td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>γ<sub>0</sub>·β<sub>0</sub>·c <hr/> γ<sub>1</sub>·β<sub>1</sub>·c</td>
			</tr>
			</table></td><td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td></tr></table></td>
			</tr></table>
			<!-- matrix expression end -->
		*/
		mat<double> getCavityDriftMatrix(int driftNumber,double position,double gamma,double optionalLength = 0.0);
		/// Calculates a solenoid matrix
		/** <b>I</b> is the current in [A].<br/>
		    The matrix looks like this:
			<!-- matrix expression begin -->
			<table style="color:#000" border="0"><tr style ="height:80px;">
			<td><table border=0 cellpadding=0 cellspacing=0px style="border-left:1px solid #000; border-right:1px solid #000; color:#000"><tr style ="height:80px;">
			<td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td><td><table border=0 cellpadding=0 cellspacing=0 style="color:#000;">
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>I·I <hr/> -344.6</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>I·I <hr/> -344.6</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			</tr>
			</table></td><td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td></tr></table></td>
			</tr></table>
			<!-- matrix expression end -->
		*/
		mat<double> createSolenoidMatrix(double I);

		/// Calculates a transform matrix of an buncher
		/**	<b>a</b> is the gap width in [mm] <br/>
			<b>U</b> is the gap voltage in [kV] <br/>
			<b>ɸ</b> is the phase angle in [degree] <br/>
			<b>f</b> is the buncher frequency in [MHz] <br/>
			<b>e</b> is the elementary charge<br/>
			<b>w<sub>1</sub></b> is the kinetic energy of the electron (at 74.1% of c)<br/>
			<b>w<sub>0</sub></b> = e·U·cos(ɸ)<br/>
			The matrix looks like this:
			<!-- matrix expression begin -->
			<table style="color:#000" border="0"><tr style ="height:80px;">
			<td><table border=0 cellpadding=0 cellspacing=0px style="border-left:1px solid #000; border-right:1px solid #000; color:#000"><tr style ="height:80px;">
			<td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td><td><table border=0 cellpadding=0 cellspacing=0 style="color:#000;">
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>a</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>a</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>a <hr/> γ·γ </td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>2πsin(ɸ)·e·U <hr/> (w<sub>1</sub>+w<sub>0</sub>)·β·λ</td>
			<td align="center" valign="middle" width=80>w<sub>1</sub> <hr/> w<sub>1</sub>+w<sub>0</sub></td>
			</tr>
			</table></td><td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td></tr></table></td>
			</tr></table>
			<!-- matrix expression end -->
		*/
		mat<double> createBuncherMatrix(double a, double U, double phi, double f);
		/// Calculates a quadrupole matrix
		/** <b>I</b> is the current in [A].<br/>
			<b>E</b> is the energy of the electron.<br/>
		    The matrix looks like this:
			<!-- matrix expression begin -->
			<table style="color:#000" border="0"><tr style ="height:80px;">
			<td><table border=0 cellpadding=0 cellspacing=0px style="border-left:1px solid #000; border-right:1px solid #000; color:#000"><tr style ="height:80px;">
			<td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td><td><table border=0 cellpadding=0 cellspacing=0 style="color:#000;">
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0.01538·I <hr/> E·0.6242-0.511</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>-0.01538·I <hr/> E·0.6242-0.511</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			<td align="center" valign="middle" width=80>1</td>
			</tr>
			<tr style ="height:80px;">
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>0</td>
			<td align="center" valign="middle" width=80>1</td>
			</tr>
			</table></td><td style ="border-top:1px solid #000; border-bottom:1px solid #000;">&nbsp;</td></tr></table></td>
			</tr></table>
			<!-- matrix expression end -->
		*/
		mat<double> createQuadrupoleMatrix(double I);

		double getTrackLength(int driftNumber);

		int shown_particles;

	private:
		///calculates the electron for time ms
		bool calc_discrete(double time, double time_in_period);
		///calculates the Drift matrizes
		bool calc_drift(double oldPos,int drift_matrix);

};

#ifdef WIN32
float erf(float x);
#endif

#endif
