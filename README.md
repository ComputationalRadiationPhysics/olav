# olav
OLAV: Optical Laser Acceleration Visualization

# Building in Linux

This will build olav:
> mkdir build
> cd build
> cmake ..
> make
> (as root) make install

If lazarus is installed the starter is compiled and installed, too.
Start Olav with the starter of the menu or in the terminal with
> olav_starter
or without starter
> olav
Use
> olav --help
to get the parameters

# Building in Windows

The cmake file should work with Visual Studio (Express), but I tested
only with mingw32. Ogre is needed. I built my own Ogre version without
boost support before. The environment variable OGRE_HOME has to be set.
BOOST_ROOT is only needed if OGRE is built with Boost and needs to be
linked aginst it.
Furthermore Cmake is needed and Lazarus is optional, but recommended for
the starter

This will build olav (in the mingw32 shell)
> mkdir build
> cd build
> cmake-gui ..
> Hitting Configure twice. If it asks for the compiler choose native. Furthermore you should have an eye on the install target
> Hitting Generate
> mingw32-make
> mingw32-make install

After that you can start the olav_starter.exe (or the olav.exe if
lazarus was not found) from the install target folder.
