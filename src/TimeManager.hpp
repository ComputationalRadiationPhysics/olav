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

//!self-explanatory ^_^
class TimeManager
{
public:
	static TimeManager* getTimeManager();

	// call this on frameStarted event of ONE frameListener to update the global time
	// when manipulating the global time directly (e.g. by using a UI-Control) set frameUpdated 
	// false, to avoid problem with the CameraControl, which uses timeSinceLastFrame for 
	// smooothing the camera-adjustment
	static void addTime(Ogre::Real timeSpan, bool frameUpdated = true);

	/// resets time to zero; is called when time > than getTotalAniamtionLenght()
	static void resetTime();

	/// returns the current time
	static Ogre::Real getTime();

	/// setter for the time : useful to jump between the animation (e.g. tour)
	static void setTime(Ogre::Real time);

	// returns the time since the last frame update. used for mouse smoothing and animation
	static Ogre::Real getTimeSinceLastFrame();

	/// the length of the animation excluding the waiting time at the stretcher and th explosion at the end
	static const double TOTAL_ANIMATION_LENGTH;

	/// the amount of time in seconds which the strecther animation lasts
	static double TOTAL_STRETCHER_LENGTH;

	/// the length of the explosion at the end
	static double EXPLOSION_TIME;

	/// the number of the PathNode when the stretcher animation starts
	static const int STRETCHER_NODE = 60;

	/// modificator which is used to accellerate or decellerate the animation
	static Ogre::Real timeModificator;

	/// gets the complet length of the animation (TOTAL + STRETCHER + EXPLOSION)
	static double getTotalAnimationLength();

private:
	TimeManager(); 

	static TimeManager *mgr;
	static Ogre::Real time;
	static Ogre::Real timeSinceLastFrame;

	TimeManager( const TimeManager& );
};
