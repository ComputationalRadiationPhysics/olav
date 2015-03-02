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

#ifndef CONSTANTS_H
#define CONSTANTS_H

class ElbeConstants
{
	public:
	static const double CMM; //	c in mm/sec (Lichtgeschwindigkeit)
	static const double C; //	c in m/sec
	static const double CPSMM; //	c in mm/ps
	static const double ME; // m in kg (Elektronenmasse)
	static const double ME_E31; // m in 10^-28 g = 10^-4 yg (Elektronenmasse)
	static const double EE; //Electron energy in 10*-4 yg * mm²/ps²
	static const double Q; // e in As (Elementarladung)
	static const double QS; // e in As (Elementarladung)
	static const double PI;
	static const double F_13;
	static const double F_13PS;
	static const double P_13PS;
	static const double F_026;
	static const double F_026PS;
	static const double P_026PS;

	static const double MODEL_SCALE;
	static const double MODEL_SCALE_2;
	static double MIN_MAX_CALC_TIME;
	static const double MAX_MAX_CALC_TIME;
	static double MAX_CALC_TIME;
};

#endif
