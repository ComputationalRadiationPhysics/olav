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

#include "ElbeConstants.hpp"

const double ElbeConstants::CMM     = 299792458000.0; //	c in mm/sec (Lichtgeschwindigkeit)
const double ElbeConstants::C       = 299792458.0; //	c in m/sec
const double ElbeConstants::CPSMM   = 0.2997924580; //	c in mm/ps
const double ElbeConstants::ME      = 0.0000000000000000000000000000009109; // m in kg (Elektronenmasse)
const double ElbeConstants::ME_E31  = 9.10938291; // m in 10^-28 g = 10^-4 yg (Elektronenmasse)
const double ElbeConstants::EE		= 0.818710438; //Electron energy in 10*-4 yg * mm²/ps² (Ruheenergie)
const double ElbeConstants::Q       = 0.0000000000000000001602; // e in As (Elementarladung)
const double ElbeConstants::QS      = 0.000001602176565; // e in * 10^-14 As (Elementarladung)
const double ElbeConstants::PI      = 3.14159265358979323846;
const double ElbeConstants::F_13    = 1300000000.0;
const double ElbeConstants::F_13PS  = 0.0013;
const double ElbeConstants::P_13PS  = 769.23076923076923076923076923077;
const double ElbeConstants::F_026   = 260000000.0;
const double ElbeConstants::F_026PS = 0.00026;
const double ElbeConstants::P_026PS = 3846.1538461538461538461538461538;

const double ElbeConstants::MODEL_SCALE   = 0.0025;
const double ElbeConstants::MODEL_SCALE_2 = 0.0125;
double ElbeConstants::MIN_MAX_CALC_TIME = 4.0;
const double ElbeConstants::MAX_MAX_CALC_TIME = 25.0;
double ElbeConstants::MAX_CALC_TIME = 4.0;
