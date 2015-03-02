cmake_minimum_required(VERSION 2.6)

project(olav)

set(OLAV_VERSION "1.0")

add_definitions( -DRELEASE -DELBE_ONLY)

if(WIN32)
	set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	set(OGRE_SAMPLES_INCLUDEPATH
		$ENV{OGRE_HOME}/Samples/include
	)
	include_directories(
		$ENV{OGRE_HOME}/include/OGRE/RenderSystems/GL
		)
endif(WIN32)
 
if(UNIX)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
endif(UNIX)
 
if (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()
 
set(CMAKE_DEBUG_POSTFIX "_d")
 
find_package(OGRE REQUIRED)

if (NOT OGRE_Plugin_ParticleFX_FOUND)
	message(SEND_ERROR "OGRE Plugin_ParticleFX not found, but needed!")
endif()
if (NOT OGRE_RenderSystem_GL_FOUND)
	message(SEND_ERROR "OGRE RenderSystem_GL not found, but needed!")
endif()

find_package(OpenGL REQUIRED)
 
#if(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
#	message(SEND_ERROR "You need Ogre 1.7 Cthugha to build this.")
#endif()
 
find_package(OIS REQUIRED)
 
if(NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif()

get_filename_component(Plugin_Search_Path ${OGRE_Plugin_ParticleFX_LIBRARY_REL} PATH)
get_filename_component(Plugin_Search_Path_Debug ${OGRE_Plugin_ParticleFX_LIBRARY_DBG} PATH)
set(Plugin_Config ${CMAKE_INSTALL_PREFIX}/share/olav/plugins.cfg)
set(Resources_Config ${CMAKE_INSTALL_PREFIX}/share/olav/resources.cfg)
set(Resources_Dir ${CMAKE_INSTALL_PREFIX}/share/olav)
set(Starter_Src_Dir ${PROJECT_BINARY_DIR}/starter_src)
set(Olav_Call ${CMAKE_INSTALL_PREFIX}/bin/olav)

configure_file (
	"${PROJECT_SOURCE_DIR}/conf/plugins.cfg.in"
	"${PROJECT_BINARY_DIR}/plugins.cfg"
	)
configure_file (
	"${PROJECT_SOURCE_DIR}/conf/plugins_d.cfg.in"
	"${PROJECT_BINARY_DIR}/plugins_d.cfg"
	)
configure_file (
	"${PROJECT_SOURCE_DIR}/conf/resources.cfg.in"
	"${PROJECT_BINARY_DIR}/resources.cfg"
	)
configure_file (
	"${PROJECT_SOURCE_DIR}/src/olav/CmakeDefines.h.in"
	"${PROJECT_BINARY_DIR}/CmakeDefines.h"
	)
configure_file (
	"${PROJECT_SOURCE_DIR}/src/starter/olav_starter.lpi.in"
	"${Starter_Src_Dir}/olav_starter.lpi"
	)
configure_file (
	"${PROJECT_SOURCE_DIR}/src/starter/mainform.pas.in"
	"${Starter_Src_Dir}/mainform.pas"
	)
file (COPY
	"${PROJECT_SOURCE_DIR}/src/starter/mainform.lfm"
	"${PROJECT_SOURCE_DIR}/src/starter/olav_starter.ico"
	"${PROJECT_SOURCE_DIR}/src/starter/olav_starter.lpr"
	"${PROJECT_SOURCE_DIR}/src/starter/olav_starter.res"
	DESTINATION
	"${Starter_Src_Dir}"
	)

find_program(LAZARUS_AVAILABLE "lazbuild")	
if (LAZARUS_AVAILABLE)
	option(BUILD_STARTER "Build the starter application. This needs lazarus installed!" ON)
else ()
	option(BUILD_STARTER "Build the starter application. This needs lazarus installed!" OFF)
endif ()

# Find Boost
if (NOT OGRE_BUILD_PLATFORM_IPHONE)
	if (WIN32 OR APPLE)
		set(Boost_USE_STATIC_LIBS TRUE)
	else ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	endif ()
	if (MINGW)
		# this is probably a bug in CMake: the boost find module tries to look for
		# boost libraries with name libboost_*, but CMake already prefixes library
		# search names with "lib". This is the workaround.
		set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	endif ()
	set(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
	# Components that need linking (NB does not include header-only components like bind)
	set(OGRE_BOOST_COMPONENTS thread date_time)
	find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	if (NOT Boost_FOUND)
		# Try again with the other type of libs
		set(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	endif()
	find_package(Boost QUIET)
 
	# Set up referencing of Boost
	include_directories(${Boost_INCLUDE_DIR})
	add_definitions(-DBOOST_ALL_NO_LIB)
	set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
endif()

set(HDRS
	${CMAKE_SOURCE_DIR}/src/olav/mat.h
	${CMAKE_SOURCE_DIR}/src/olav/resource.h
	${CMAKE_SOURCE_DIR}/src/olav/vec.h
	${CMAKE_SOURCE_DIR}/src/olav/AbstractCamera.hpp
	${CMAKE_SOURCE_DIR}/src/olav/AppState.hpp
	${CMAKE_SOURCE_DIR}/src/olav/AppStateManager.hpp
	${CMAKE_SOURCE_DIR}/src/olav/ChaseCamera.hpp
	${CMAKE_SOURCE_DIR}/src/olav/CollisionTools.hpp
	${CMAKE_SOURCE_DIR}/src/olav/DocumentationManager.hpp
	${CMAKE_SOURCE_DIR}/src/olav/DotSceneLoader.hpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeBunch.hpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeConstants.hpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeElectron.hpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbePipe.hpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeState.hpp
	${CMAKE_SOURCE_DIR}/src/olav/FreeViewCamera.hpp
	${CMAKE_SOURCE_DIR}/src/olav/LMCApp.hpp
	${CMAKE_SOURCE_DIR}/src/olav/MoveablePathElement.hpp
	${CMAKE_SOURCE_DIR}/src/olav/NodeDescription.hpp
	${CMAKE_SOURCE_DIR}/src/olav/OgreFramework.hpp
	${CMAKE_SOURCE_DIR}/src/olav/OrbitCamera.hpp
	${CMAKE_SOURCE_DIR}/src/olav/PathParser.hpp
	${CMAKE_SOURCE_DIR}/src/olav/PathPoint.hpp
	${CMAKE_SOURCE_DIR}/src/olav/PauseState.hpp
	${CMAKE_SOURCE_DIR}/src/olav/rapidxml.hpp
	${CMAKE_SOURCE_DIR}/src/olav/StereoManager.h
	${CMAKE_SOURCE_DIR}/src/olav/TimeManager.hpp
	${CMAKE_SOURCE_DIR}/src/olav/tinystr.hpp
	${CMAKE_SOURCE_DIR}/src/olav/tinyxml.hpp
)
 
set(SRCS
	${CMAKE_SOURCE_DIR}/src/olav/AppStateManager.cpp
	${CMAKE_SOURCE_DIR}/src/olav/ChaseCamera.cpp
	${CMAKE_SOURCE_DIR}/src/olav/CollisionTools.cpp
	${CMAKE_SOURCE_DIR}/src/olav/DocumentationManager.cpp
	${CMAKE_SOURCE_DIR}/src/olav/DotSceneLoader.cpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeBunch.cpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeConstants.cpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeElectron.cpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbePipe.cpp
	${CMAKE_SOURCE_DIR}/src/olav/ElbeState.cpp
	${CMAKE_SOURCE_DIR}/src/olav/FreeViewCamera.cpp
	${CMAKE_SOURCE_DIR}/src/olav/LMCApp.cpp
	${CMAKE_SOURCE_DIR}/src/olav/main.cpp
	${CMAKE_SOURCE_DIR}/src/olav/MoveablePathElement.cpp
	${CMAKE_SOURCE_DIR}/src/olav/NodeDescription.cpp
	${CMAKE_SOURCE_DIR}/src/olav/OgreFramework.cpp
	${CMAKE_SOURCE_DIR}/src/olav/OrbitCamera.cpp
	${CMAKE_SOURCE_DIR}/src/olav/PathParser.cpp
	${CMAKE_SOURCE_DIR}/src/olav/PauseState.cpp
	${CMAKE_SOURCE_DIR}/src/olav/StereoManager.cc
	${CMAKE_SOURCE_DIR}/src/olav/TimeManager.cpp
	${CMAKE_SOURCE_DIR}/src/olav/tinystr.cpp
	${CMAKE_SOURCE_DIR}/src/olav/tinyxml.cpp
	${CMAKE_SOURCE_DIR}/src/olav/tinyxmlerror.cpp
	${CMAKE_SOURCE_DIR}/src/olav/tinyxmlparser.cpp
)
 
include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR}
)

add_executable(olav WIN32 ${HDRS} ${SRCS})
 
set_target_properties(olav PROPERTIES DEBUG_POSTFIX _d)
 
target_link_libraries(olav ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OPENGL_LIBRARY} ${BOOST_LIBRARIES} boost_system)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
 
# post-build copy for win32
if(WIN32 AND NOT MINGW)
	add_custom_command( TARGET olav PRE_BUILD
		COMMAND if not exist .\\dist\\bin mkdir .\\dist\\bin )
	add_custom_command( TARGET olav POST_BUILD
		COMMAND copy \"$(TargetPath)\" .\\dist\\bin )
endif(WIN32 AND NOT MINGW)

if(MINGW OR UNIX)
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
endif(MINGW OR UNIX)

if (BUILD_STARTER)
	add_custom_command(
		TARGET olav POST_BUILD
		COMMAND lazbuild -B ${Starter_Src_Dir}/olav_starter.lpr > /dev/null 
		COMMENT "Building olav_starter")
endif ()

if(WIN32)
	install (TARGETS olav DESTINATION bin) 
	install (DIRECTORY ${CMAKE_SOURCE_DIR}/media DESTINATION share/olav)
	install (FILES ${CMAKE_SOURCE_DIR}/conf/plugins.win32 DESTINATION bin CONFIGURATIONS Release RelWithDebInfo)
	install (FILES ${CMAKE_SOURCE_DIR}/conf/plugins_d.win32 DESTINATION bin CONFIGURATIONS Debug)
	install (FILES ${CMAKE_SOURCE_DIR}/conf/resources.win32 DESTINATION bin CONFIGURATIONS Debug)
	install (FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll ${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll ${OGRE_PLUGIN_DIR_REL}/Plugin_ParticleFX.dll ${OGRE_PLUGIN_DIR_REL}/libOIS.dll DESTINATION bin CONFIGURATIONS Release RelWithDebInfo)
	install (FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll ${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll ${OGRE_PLUGIN_DIR_DBG}/Plugin_ParticleFX_d.dll ${OGRE_PLUGIN_DIR_DBG}/libOIS_d.dll DESTINATION bin CONFIGURATIONS Debug)
	if (BUILD_STARTER)
		install (FILES ${CMAKE_BINARY_DIR}/olav_starter DESTINATION bin) 
		install (DIRECTORY ${CMAKE_SOURCE_DIR}/descriptions DESTINATION share/olav)
	endif ()
endif(WIN32)

if(UNIX)
	install (TARGETS olav DESTINATION bin) 
	install (DIRECTORY ${CMAKE_SOURCE_DIR}/media DESTINATION share/olav)
	install (FILES ${CMAKE_BINARY_DIR}/plugins.cfg DESTINATION share/olav) 
	install (FILES ${CMAKE_BINARY_DIR}/resources.cfg DESTINATION share/olav)
	if (BUILD_STARTER)
		install (FILES ${CMAKE_BINARY_DIR}/olav_starter DESTINATION bin PERMISSIONS WORLD_EXECUTE) 
		install (DIRECTORY ${CMAKE_SOURCE_DIR}/descriptions DESTINATION share/olav)
	endif ()
endif(UNIX)
