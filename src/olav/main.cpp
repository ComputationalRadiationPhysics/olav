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

//|||||||||||||||||||||||||||||||||||||||||||||||

#include <stdio.h>
#include <fcntl.h>
#ifdef WIN32
	#include <io.h>
#endif
#include <iostream>
#include <string>
#include "LMCApp.hpp"
#include <Ogre.h>
#include "ElbePipe.hpp"
#include "CmakeDefines.h"

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
	#ifndef ELBE_ONLY
		int enter_state = 0; //0 menu, 1 collision, 2 draco, 3 elbe, 4 radiation
	#else
		int enter_state = 3; //3 elbe
	#endif
	bool show_settings = true;
	bool stereo = false;
	bool mouse_emulation = false;
	float spacing = 0.06;
	int language = 0; //English
	int demo = 0;
	int change = 0;
	bool simple = false;
	bool show_debug = false;
	Ogre::StereoManager::StereoMode mode = Ogre::StereoManager::SM_DUALOUTPUT;
	//command line parameters
#ifdef WIN32
	//creating default unix stuff -_-
	int argc = 1;
	char* cmdLine = (char*)malloc(strlen(strCmdLine)+1);
	sprintf(cmdLine,"%s",strCmdLine);
	char* pch = strtok(cmdLine," ");
	while (pch != NULL)
	{
		argc++;
		pch = strtok(NULL," ");
	}
	char** argv = (char**)malloc(sizeof(char*)*argc);
	argv[0] = (char*)malloc(strlen("./LMC")+1);
	sprintf(argv[0],"./LMC");
	argc = 1;
	sprintf(cmdLine,"%s",strCmdLine);
	pch = strtok(cmdLine," ");
	while (pch != NULL)
	{
		argv[argc] = (char*)malloc(strlen(pch)+1);
		sprintf(argv[argc],"%s",pch);
		argc++;
		pch = strtok(NULL," ");
	}

#endif
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i],"--resolutions") == 0)
			{
				new OgreFramework();
				FILE * pFile;
				pFile = fopen ((OgreFramework::getSingletonPtr()->appdata+"resolutions.txt").c_str() , "w");
				Ogre::ConfigOptionMap& configOptions = Ogre::Root::getSingleton().getRenderSystem()->getConfigOptions();
				Ogre::ConfigOptionMap::iterator result = configOptions.find( "Video Mode" );
				if ( result != configOptions.end() )
				{
					Ogre::ConfigOption& option = result->second;
					for( Ogre::StringVector::iterator i( option.possibleValues.begin() ), iEnd( option.possibleValues.end() ); i != iEnd; ++i )
						fprintf(pFile,"%s\n", (*i).c_str());
				}
				fclose (pFile);
				return 0;
			}
			if (strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0)
			{
				#ifdef WIN32
					MessageBox(0,"LMC command line options\n" \
						         "  --help        or -h      : shows this help\n" \
						         "  --version     or -v      : shows this version\n" \
					             "  --enter=STATE or -e STATE: Enters the state STATE at beginning. Values for\n"\
					             "                             STATE are elbe, draco, radiation or collision.\n"\
					             "  --no-settings or -n      : Deactivates showing the settings dialog. Saved\n"\
					             "                             settings will be used.\n"\
					             "  --stereo      or -s      : Activates stereostuff\n"\
					             "  --mode MODE   or -m MODE : Selects the stereo mode. Possible values are:\n"\
								 "                             - dualoutput for side by side;\n"\
					             "                             - vertical, horizental or checkboard for\n"\
					             "                               interlaced output;\n"\
								 "                             - redcyan, yellowblue or redgreen for\n"\
								 "                               color-glas output;\n"\
								 "                             Default is dualoutput.\n"\
								 "  --emulate     or -u        Emulates the mouse with the keyboard. Use the\n"\
								 "                             numpad keys 2,4,6 and 8 for moving, the key\n"\
								 "                             5 for left click, 3 or 9 for right click and\n"\
								 "                             7 and 1 for the mouse wheel.\n"\
								 "  --eye-spacing or -y VALUE: Sets the eye spacing in stereo graphic mode.\n"\
								 "                             VALUE have to be a floating point number.\n"\
								 "                             Default is 0.06.\n"\
								 "  --german, --english      : Sets the language to German or English (default).\n"\
								 "  --demo                   : Activates the demo mode\n"\
								 "  --change                 : Activates the state change in demo mode\n"\
								 "  --particles=N or -p N    : Elbe Particles to show. Default: 6000\n"\
								 "  --debug       or -d      : Show debug informations (especially from OGRE)\n"\
								 ,"Help",MB_OK | MB_ICONINFORMATION);
				#endif
				printf("LMC command line options\n");
				printf("  --help        or -h      : shows this help\n");
				printf("  --version     or -v      : shows this version\n");
				#ifndef ELBE_ONLY
					printf("  --enter=STATE or -e STATE: Enters the state STATE at beginning. Values for\n");
					printf("                             STATE are elbe, draco, radiation or collision.\n");
				#endif
				printf("  --no-settings or -n      : Deactivates showing the settings dialog. Saved\n");
				printf("                             settings will be used.\n");
				printf("  --stereo      or -s      : Activates stereostuff\n");
				printf("  --mode=MODE   or -m MODE : Selects the stereo mode. Possible values are:\n");
				printf("                             - dualoutput for side by side;\n");
				printf("                             - vertical, horizental or checkboard for\n");
				printf("                               interlaced output;\n");
				printf("                             - redcyan, yellowblue or redgreen for\n");
				printf("                               color-glas output;\n");
				printf("                             Default is dualoutput.\n");
				printf("  --emulate     or -u      : Emulates the mouse with the keyboard. Use the\n");
				printf("                             numpad keys 2,4,6 and 8 for moving, the key\n");
				printf("                             5 for left click, 3 or 9 for right click and\n");
				printf("                             7 and 1 for the mouse wheel.\n");
				printf("  --space=VALUE or -a VALUE: Sets the eye spacing in stereo graphic mode.\n");
				printf("                             VALUE have to be a floating point number.\n");
				printf("                             Default is 0.06.\n");
				printf("  --german, --english      : Sets the language to German or English (default).\n");
				printf("  --demo                   : Activates the demo mode\n");
				printf("  --change                 : Activates the state change in demo mode\n");
				printf("  --particles=N or -p N    : Elbe Particles to show. Default: 6000\n");
				printf("  --debug       or -d      : Show debug informations (especially from OGRE)\n");
				return 0;
			}
			else
			if (strcmp(argv[i],"--debug") == 0 || strcmp(argv[i],"-d") == 0)
			{
				show_debug = true;
			}
			else
			if (strcmp(argv[i],"--version") == 0 || strcmp(argv[i],"-v") == 0)
			{
				#ifdef WIN32
					MessageBox(0,OLAV_VERSION,"Version",MB_OK | MB_ICONINFORMATION);
				#endif
				printf("OLAV Version: "OLAV_VERSION"\n");
				return 0;
			}
			else
			if (strcmp(argv[i],"--demo") == 0)
			{
				demo = 1;
			}
			else
			if (strcmp(argv[i],"--change") == 0)
			{
				change = 1;
			}
			else
			if (strcmp(argv[i],"--german") == 0)
			{
				language = 1;
			}
			else
			if (strcmp(argv[i],"--english") == 0)
			{
				language = 0;
			}
			else
			if (strcmp(argv[i],"--no-settings") == 0 || strcmp(argv[i],"-n") == 0)
			{
				show_settings = false;
			}
			else
			if (strcmp(argv[i],"--stereo") == 0 || strcmp(argv[i],"-s") == 0)
			{
				stereo = true;
			}
			else
			if (strcmp(argv[i],"--simple") == 0 || strcmp(argv[i],"-s") == 0)
			{
				simple = true;
			}
			else
			if (strcmp(argv[i],"--emulate") == 0 || strcmp(argv[i],"-u") == 0)
			{
				mouse_emulation = true;
			}
			else
			if (strstr(argv[i],"--space=")!=NULL || strcmp(argv[i],"-a") == 0)
			{
				char* space = NULL;
				if (strcmp(argv[i],"-a") == 0)
				{
					i++;
					if (i >= argc)
					{
						printf("ERROR: Expected floating point number after \"-a\"\n");
						#ifdef WIN32
							MessageBox(0,"Expected floating point number after \"-a\"","ERROR",MB_OK | MB_ICONERROR);
						#endif
						return 1;
					}
					space = argv[i];
				}
				else
					space = &((argv[i])[8]);
				
				spacing = atof(space);

				if (spacing == 0.0f)
				{
					printf("ERROR: not possible eye spacing \"%s\"\n",space);
					#ifdef WIN32
						char buffer[256];
						sprintf(buffer,"not possible eye spacing \"%s\"\n",space);
						MessageBox(0,buffer,"ERROR",MB_OK | MB_ICONERROR);
					#endif
					return 1;
				}
			}
			else				
			if (strstr(argv[i],"--mode=")!=NULL || strcmp(argv[i],"-m") == 0)
			{
				char* state = NULL;
				if (strcmp(argv[i],"-m") == 0)
				{
					i++;
					if (i >= argc)
					{
						printf("ERROR: Expected mode after \"-m\"\n");
						#ifdef WIN32
							MessageBox(0,"Expected mode after \"-m\"","ERROR",MB_OK | MB_ICONERROR);
						#endif
						return 1;
					}
					state = argv[i];
				}
				else
					state = &((argv[i])[7]);
				if (strcmp(state,"dualoutput") == 0)
					mode = Ogre::StereoManager::SM_DUALOUTPUT;
				else
				if (strcmp(state,"vertical") == 0)
					mode = Ogre::StereoManager::SM_INTERLACED_V;
				else
				if (strcmp(state,"horizental") == 0)
					mode = Ogre::StereoManager::SM_INTERLACED_H;
				else
				if (strcmp(state,"checkboard") == 0)
					mode = Ogre::StereoManager::SM_INTERLACED_CB;
				else
				if (strcmp(state,"redcyan") == 0)
					mode = Ogre::StereoManager::SM_ANAGLYPH_RC;
				else
				if (strcmp(state,"yellowblue") == 0)
					mode = Ogre::StereoManager::SM_ANAGLYPH_YB;
				else
				if (strcmp(state,"redgreen") == 0)
					mode = Ogre::StereoManager::SM_ANAGLYPH_RG;
				else
				{
					printf("ERROR: unknown mode \"%s\"\n",state);
					#ifdef WIN32
						char buffer[256];
						sprintf(buffer,"unknown mode \"%s\"\n",state);
						MessageBox(0,buffer,"ERROR",MB_OK | MB_ICONERROR);
					#endif
					return 1;
				}
			}
			else			
			#ifndef ELBE_ONLY
				if (strstr(argv[i],"--enter=")!=NULL || strcmp(argv[i],"-e") == 0)
				{
					char* state = NULL;
					if (strcmp(argv[i],"-e") == 0)
					{
						i++;
						if (i >= argc)
						{
							printf("ERROR: Expected state after \"-e\"\n");
							#ifdef WIN32
								MessageBox(0,"Expected state after \"-e\"","ERROR",MB_OK | MB_ICONERROR);
							#endif
							return 1;
						}
						state = argv[i];
					}
					else
						state = &((argv[i])[8]);
					if (strcmp(state,"collision") == 0)
						enter_state = 1;
					else
					if (strcmp(state,"draco") == 0)
						enter_state = 2;
					else
					if (strcmp(state,"elbe") == 0)
						enter_state = 3;
					else
					if (strcmp(state,"radiation") == 0)
						enter_state = 4;
					else
					{
						printf("ERROR: unknown state \"%s\"\n",state);
						#ifdef WIN32
							char buffer[256];
							sprintf(buffer,"unknown state \"%s\"\n",state);
							MessageBox(0,buffer,"ERROR",MB_OK | MB_ICONERROR);
						#endif
						return 1;
					}
				}
				else
			#endif
			if (strstr(argv[i],"--particles=")!=NULL || strcmp(argv[i],"-p") == 0)
			{
				int particle_count;
				if (strcmp(argv[i],"-p") == 0)
				{
					i++;
					if (i >= argc)
					{
						printf("ERROR: Expected number of particles after \"-p\"\n");
						#ifdef WIN32
							MessageBox(0,"Expected number of particles after \"-p\"","ERROR",MB_OK | MB_ICONERROR);
						#endif
						return 1;
					}
					particle_count = atoi(argv[i]);
				}
				else
					particle_count = atoi(&((argv[i])[12]));
				if (particle_count < 1000)
					particle_count = 1000;
				if (particle_count > 21000)
					particle_count = 21000;
				ElbePipe::particle_count = particle_count;
			}
			else
			{
				printf("Unknown parameter \"%s\"\n",argv[i]);
				#ifdef WIN32
					char buffer[256];
					sprintf(buffer,"Unknown parameter \"%s\"\n",argv[i]);
					MessageBox(0,buffer,"ERROR",MB_OK | MB_ICONERROR);
				#endif
				return 1;
			}
		}
	}

	srand(time(NULL));
	LMCApp lmc;
	try	
	{
		lmc.startDemo(enter_state,show_settings,stereo,mode,mouse_emulation,spacing,language,simple,demo,change,show_debug);
	}
	catch(std::exception& e)
    {
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        fprintf(stderr, "An exception has occurred: %s\n", e.what());
#endif
    }
    return 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||
