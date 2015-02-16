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

#include "DocumentationManager.hpp"
#include "tinyxml.hpp"
#include "globale_defines.h"

using namespace Ogre;
bool DocumentationManager::firstTime = true;

std::string Documentation::CleanUpFileName(const std::string & fileN)
{
	std::string result(fileN);

	std::string forbiddenChars = " /\\{([])}'\"éèùàçäëïöüâêîôû@+=*$£%!?,;.:";
	std::string::size_type found = result.find_first_of(forbiddenChars);
	
	while(found != std::string::npos)
	{
		result[found] = '_';
		found = result.find_first_of(forbiddenChars, found+1);
	}

	return result;
}

DocumentationManager::DocumentationManager(const std::string & pBaseDir, const std::string & fileName) : baseDir(pBaseDir)
{
	OverlayManager& overlayManager = OverlayManager::getSingleton();
	unsigned int width = OgreFramework::getSingletonPtr()->m_pRenderWnd->getWidth();
	unsigned int height = OgreFramework::getSingletonPtr()->m_pRenderWnd->getHeight();

	#ifdef WIN33
	TiXmlDocument file(std::string(baseDir + "\\" + fileName).c_str());
	#else
	TiXmlDocument file(std::string(baseDir + "/" + fileName).c_str());
	#endif
	if(!file.LoadFile())
		throw 1;
	TiXmlElement * description = file.FirstChildElement()->FirstChildElement();
	while(description)
	{
		Documentation doc;
		doc.name = description->FirstChildElement("name")->GetText();
		doc.title = description->FirstChildElement("title")->GetText();
		TiXmlElement * pictures = description->FirstChildElement("pictures");
		TiXmlElement * picture = 0;
		if(pictures)
			picture = pictures->FirstChildElement("picture");
		while(picture)
		{
			Documentation::Picture pic;
			TiXmlElement * size = picture->FirstChildElement("size");
			pic.Width = atoi(size->Attribute("width"));
			pic.Height = atoi(size->Attribute("height"));
			pic.FileName = picture->FirstChildElement("file_name")->GetText();
			pic.Desc = picture->FirstChildElement("desc")->GetText();
			//if (!overlayManager.getByName(pic.FileName))
			{
				try
				{
					OverlayManager::getSingleton().destroyOverlayElement(pic.FileName, true);
				}
				catch (...)
				{ }
				printf("Loading %s\n",pic.FileName.c_str());
				OgreFramework::getSingletonPtr()->m_pLog->logMessage("Loading: "+pic.FileName);
				OverlayContainer* panel = static_cast<OverlayContainer*>(overlayManager.createOverlayElement("Panel", pic.FileName, true));
				MaterialPtr material = MaterialManager::getSingleton().create("descr" + pic.FileName, "General");
				material->getTechnique(0)->getPass(0)->createTextureUnitState(pic.FileName);
				material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
				material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
				material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
				panel->setMetricsMode(Ogre::GMM_PIXELS);
				if (OgreFramework::getSingletonPtr()->explanationMode == expert)
					panel->setDimensions(INFOBOX_WIDTH,PICTURE_HEIGHT);
				else
					panel->setDimensions(INFOBOX_WIDTH+60,PICTURE_HEIGHT);
				panel->setMaterialName("descr" + pic.FileName);
			}
			doc.pictures.push_back(pic);
			picture = picture->NextSiblingElement("picture");
		}
		doc.text = description->FirstChildElement("text")->GetText();
		this->docs.push_back(doc);
		description = description->NextSiblingElement();
	}
	firstTime = false;
	errorDoc.name = "Error while loading";
	errorDoc.title = "Error while loading";
	errorDoc.text = "Error while loading";
}

DocumentationManager::~DocumentationManager(void)
{
	for(unsigned int i=0; i<docs.size();i++)
	{
		for(unsigned int j=0; j<docs[i].GetNumPictures();j++)
		{
			OverlayManager::getSingleton().destroyOverlayElement(docs[i].GetIthPictureFileName(j), true);
		}
	}
}

const Documentation * DocumentationManager::GetDocumentation(unsigned int i)
{
	if(i < docs.size())
		return &docs[i];
	return &errorDoc;
}

const Documentation * DocumentationManager::GetDocumentation(const std::string & name)
{
	for(std::vector<Documentation>::iterator it = this->docs.begin() ; it != this->docs.end() ; ++it)
		if(it->GetName() == name)
			return &(*it);
	return &errorDoc;
}
