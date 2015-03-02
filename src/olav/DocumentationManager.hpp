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

#include <string>
#include <vector>

#include <Ogre.h>
#include "OgreFramework.hpp"
#include "AppState.hpp"

class Documentation
{
public:
	static std::string CleanUpFileName(const std::string & fileN);
	struct Picture { unsigned int Width; unsigned int Height; std::string FileName; std::string Desc; };
	Documentation(void) {}
	~Documentation(void) {}

	const std::string & GetName(void) const { return name; }
	const std::string & GetTitle(void) const { return title; }
	const std::string & GetText(void) const { return text; }
	unsigned int GetNumPictures(void) const { return pictures.size(); }
	unsigned int GetIthPictureWidth(unsigned int i) const { return pictures[i].Width; }
	unsigned int GetIthPictureHeight(unsigned int i) const { return pictures[i].Height; }
	const std::string & GetIthPictureFileName(unsigned int i) const { return pictures[i].FileName; }
	const std::string & GetIthPictureDesc(unsigned int i) const { return pictures[i].Desc; }

	std::string name;
	std::string title;
	std::vector<Picture> pictures;
	std::string text;
	friend class DocumentationManager;
};

class DocumentationManager
{
public :
	DocumentationManager(const std::string & baseDir, const std::string & fileName);
	~DocumentationManager(void);
	const Documentation * GetDocumentation(const std::string & name);
	const Documentation * GetDocumentation(unsigned int i);
protected :
	std::string baseDir;
	std::vector<Documentation> docs;
	Documentation errorDoc;
	static bool firstTime;
};
