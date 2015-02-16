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

#include "TimeManager.hpp"

TimeManager* TimeManager::mgr = 0;
Ogre::Real TimeManager::time = 0;
Ogre::Real TimeManager::timeSinceLastFrame = 0;
const double TimeManager::TOTAL_ANIMATION_LENGTH = 180.0;
double TimeManager::TOTAL_STRETCHER_LENGTH = 20.0;
double TimeManager::EXPLOSION_TIME = 10.0;
Ogre::Real TimeManager::timeModificator = 1;

TimeManager::TimeManager()
{
}

TimeManager* TimeManager::getTimeManager()
{
	if (mgr == 0) {
		mgr = new TimeManager();
	}
	return mgr;
}

/// adds the given timeSpan to the current time and sets timeSincelastFrame if frameUpdated is true 
void TimeManager::addTime(Ogre::Real timeSpan, bool frameUpdated)
{
	TimeManager::time += timeSpan;
	
	if (frameUpdated)
	{
		TimeManager::timeSinceLastFrame = timeSpan;
	}	
}

void TimeManager::resetTime()
{
	TimeManager::time = 0;
}

Ogre::Real TimeManager::getTimeSinceLastFrame()
{
	return TimeManager::timeSinceLastFrame;
}

Ogre::Real TimeManager::getTime()
{
	return TimeManager::time;
}

void TimeManager::setTime(Ogre::Real time)
{
	TimeManager::time = time;
}

double TimeManager::getTotalAnimationLength()
{
	return TOTAL_ANIMATION_LENGTH + TOTAL_STRETCHER_LENGTH + EXPLOSION_TIME;
}

