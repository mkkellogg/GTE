

1. Overview

	Basic graphics engine developed in C++ that currently supports the following features:
	
		- Scene-graph API
		- Forward rendering
		- Dynamic per-pixel lighting with directional lights and point lights
		- Skeletal animation with animation blending and vertex skinning
		- Dynamic shadows with shadow volumes
		- Model loading capabilities through the inclusion of the Assimp library
	
	The engine is designed so that it can be extended to work with multiple graphics APIs (OpenGL, DirectX, etc...). Currently only OpenGL is implemented.

2. Building GTE

	A number of pre-requisite libraries must be installed prior to building GTE. These libraries are:
	
		- Assimp (Asset import library)
		- DevIL image loading library
		- GLUT (OpenGL Utility Toolkit)
		- GLEW (OpenGL Extension Wrangler library)
	
	Your Assimp installation needs to include the ability to load FBX files in order to run the demo included in the project. The variables ASSIMP_LIB and ASSIMP_INC in the make files were defined to point to a custom version of the library that has this ability.

	2.2 Linux notes
	
	On multiple flavors of Linux the Assimp package that is available through standard package management channels seems to not have the ability to load FBX files. Therefore if you are trying to build GTE on Linux you will probably have to build Assimp from source.
	
	2.3 OSX Notes
	
	On OSX it is fairly straight forward to install the pre-requisite libraries mentioned above. They are all available through the Brew package management utility, although the package names vary slightly from the official names mentioned above. The actual names of the packages you need to install are as follows:
	
	Assimp -> assimp
	DevIL -> devil
	Glut -> freeglut
	Glew -> glew
	
	You will also need to make sure to have 'gcc' and 'g++' installed on your machine.
	
	2.4 Compiling GTE
	
	Currently, the build process produces a single executable: bin/gtedemo. It has not yet been modified to produce a library (shared or static); that is still on the to-do list. The 'gtedemo' executable is a sample of what the engine can do, and the source for the demo is in the engine's source tree at src/gtedemo.
	
	To invoke the build process, navigate to the root of the GTE installation and run the following command:
	
		make -f <makefile>
		
	Where `makefile` is the makefile that is appropriate for your platform. Currently there are two:
		
		Makefile.osx - Apple OSX
		Makefile.unix - Unix & Linux systems
		
	Assuming the build succeeded, the 'gtedemo' executable will be produced in the 'bin' subdirectory of the GTE installation. 

3. Running the demo

	Before running the demo, make sure you have downloaded the models and textures that it needs. They can be found at: http://projects.markkellogg.org/downloads/models-textures.zip. Uncompress the archive in the 'resources' sub-folder, the process should create two new folders: 'resources/models' and 'resources/textures'.

	To run the demo, do not run the 'gtedemo' executable directly. Instead, execute the 'rungtedemo.sh' script that is in the root of the GTE installation, which will ensure the demo's working directory is correctly set up.


