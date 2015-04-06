

1. Overview

	Basic graphics engine developed in C++ that currently supports the following features:
	
		- Scene-graph API
		- Forward rendering
		- Dynamic per-pixel lighting with directional lights and point lights
		- Skeletal animation with animation blending and vertex skinning
		- Dynamic shadows with shadow volumes
		- Model loading capabilities through the inclusion of the Assimp library
		- Screen-Space Ambient Occlusion (alpha stage)
		- Multisample Anti-aliasing (MSAA)
		- Render-to-texture
	
	The engine is designed so that it can be extended to work with multiple graphics APIs (OpenGL, DirectX, etc...). Currently only OpenGL is implemented.

2. Building GTE

	A number of pre-requisite libraries must be installed prior to building GTE. These libraries are:
	
		- Assimp (Asset import library)
		- DevIL image loading library
		- FreeGLUT (OpenGL Utility Toolkit)
		- GLEW (OpenGL Extension Wrangler library)
	
	Your Assimp installation needs to include the ability to load FBX files in order to run the demo included in the project.

	2.2 Linux notes
	
	On multiple flavors of Linux the Assimp package that is available through standard package management channels seems to not have the ability to load FBX files. Therefore if you are trying to build GTE on Linux you will probably have to build Assimp from source.
	
	Building Assimp on Linux can be accomplished through the following steps:
	
	- Make sure CMake is installed.
	- Make sure you have the cmake-gui package installed
	- Use cmake-gui to configure the CMake build for Assimp
	- In a terminal window, navigate to the root of your Assimp repo and run:
	
		cmake -G "Unix Makefiles"
	
	- Now run make, followed by make install
	
	DeVIL, GLUT, and GLEW are available through standard package management channels, although the package names differ from the official names:

	- DevIL: libdevil-dev
	- FreeGLUT: freeglut3-dev
	- GLEW: libglew-dev

	On certain Linux distros you might encounter linkage errors if you have installed proprietary graphics drivers that come with their own OpenGL shared library. In this case you may need to modify the location in which the linker searches for the OpenGL shared library during the build process. In the included Unix (Linux) makefile, a variable called OPENGL_LIB is defined to point to such a location.

	2.3 OSX Notes
	
	On OSX it is fairly straight forward to install the pre-requisite libraries mentioned above. They are all available through the Brew package management utility, and the version of Assimp available through Brew seems to support FBX models. The one caveat is that Brew package names vary slightly from the official names mentioned above. The actual names of the packages you need to install are as follows:
	
	- Assimp: assimp
	- DevIL: devil
	- FreeGLUT: freeglut
	- GLEW: glew
	
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
	
	Note: Screen-Space Ambient Occlusion is still in the alpha stage, so it won't look right in some places (such as terrain meshes).
	
	Demo Controls:
	
	There are three scenes in the demo and they can be accessed via the numeric keys:
	
	- Lava scene - '1' Key
	- Castle Scene - '2' Key
	- Reflecting pool scene - '3' Key
	
	Arrow keys: Move the knight around the environment. Physics are not incorporated into the engine so you will be able to walk through objects and off ledges.
	
	'X' Key: Play stab animation.
	
	'C' Key: Play chop animation.
	
	'V' Key: Play spin attack animation.
	
	'B' Key: Play defend animation.
	
	'S' Key: Toggle skybox rendering.
	
	'O' Key: Toggle Screen-Space Ambient occlusion (SSAO).
	
	'I' Key: Toggle between standard SSAO, where SSAO is applied right after ambient lighting, to 'Outline SSAO', where SSAO is applied after all lighting, and produces an interesting outlining effect (and some undesirable side-effects).

	Lighting functionality:
	
	'A' Key: Select ambient lighting.
	
	'D' Key: Select directional lighting.
	
	'P' Key: Select point lights in the scene.
	
	'L' Key: Select lava lights.
	
	'Q' Key: Toggle currently selected light.
	
	'W' Key: Increase the intensity of currently selected light.
	
	'E' Key: Decrease intensity of currently selected light.
	
	'R' Key: Toggle shadow casting for light (will only work for meshes that cast shadows)
	

	

