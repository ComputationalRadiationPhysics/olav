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
#include <list>

using namespace std;

/// This class encapsulates the data from the nodedescriptions.xml file for better handling and search at runtime
/// Its values are parsed and stored in the EventManager class
class NodeDescription
{
public:
	NodeDescription(string *scnName, string* token, string *realName, string *parentScnNode, string *parentRealName, string *desc_content, list<string> pics, int pathNode, int tourId, int boxId,int width,int height);
	virtual ~NodeDescription(void);
	
	/// Attributes of each node
	string *scnName;
	string *token;
	string *realName;
	string *parentScnNode;
	string *parentRealName;
	string *desc_tour;
	string *desc_content;
	list<string> pics;
	int pathNode;
	int boxId;
	int tourId;
	int width;
	int height;
};
