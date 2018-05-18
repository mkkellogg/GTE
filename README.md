

### 1. Overview

GTE is a basic graphics engine developed in C++ that currently supports the following features:
	
* Basic scene-graph API
* Forward rendering
* Dynamic per-pixel lighting with directional lights and point lights
* Skeletal animation with animation blending and vertex skinning
* Dynamic shadows with shadow volumes
* Particle systems
* Model loading capabilities through the inclusion of the Assimp library
* Screen-Space Ambient Occlusion (alpha stage)
* Multisample Anti-aliasing (MSAA)
* Render-to-texture
	
The engine is designed so that it can be extended to work with multiple graphics APIs (OpenGL, DirectX, etc...). Currently only OpenGL is implemented.

Minimum OpenGL version: 3.2
	
Supported operating systems: Microsoft Windows, Apple OS X, Linux, Unix

	

### 2. Building GTE

#### 2.1 Build overview

A number of pre-requisite libraries must be installed prior to building GTE. These libraries are:
	
* Assimp (Asset import library) **Version 3.1.1 required, new versions are broken**
* DevIL image loading library
* GLFW 3
* GLEW (OpenGL Extension Wrangler library)
	
Your Assimp installation needs to include the ability to load FBX files in order to run the demo included in the project.

#### 2.2 Windows notes
	
The binaries for all required libraries are available for Windows and can be downloaded from the following links:
	
* Assimp: http://sourceforge.net/projects/assimp/files/assimp-3.1/
* DevIL:  http://openil.sourceforge.net/download.php (choose SDK for 64-bit Windows)
* GLFW 3: http://www.glfw.org/download.html (choose 64-bit Windows binaries)
* GLEW:   http://glew.sourceforge.net/index.html
	
This repository includes a Visual Studio 2015 solution for GTE. You will need to update it to point to the correct include and library folders for the above libraries on your system. The solution is configured for 64-bit development so make sure to link to the 64-bit versions of those libraries.
	
The include paths can be set by right clicking on the "GTE" project, selecting "Properties", and then expanding "Configuration Properties" to find the "C/C++" page, which contains the field "Additional Include Directories". Edit that field as appropriate.
	
The library paths can be set by right clicking on the "GTE" project, selecting "Properties", and then expanding "Configuration Properties" to find the "Linker" page, which contains the field "Additional Library Directories". Edit that field as appropriate.
	
Lastly before running the included demo you will need to add the appropriate executable directories for each library's DLL to your PATH environment variable.

#### 2.3 Linux notes
	
On multiple flavors of Linux the Assimp package that is available through standard package management channels seems to not have the ability to load FBX files. Therefore if you are trying to build GTE on Linux you will probably have to build Assimp from source.
	
Building Assimp 3.1.1 on Linux can be accomplished through the following steps:
	
* Make sure CMake is installed.
* Make sure you have the cmake-gui package installed
* Use cmake-gui to configure the CMake build for Assimp
* To clone the Git repo, run:
		
		git clone --branch v3.1.1 https://github.com/assimp/assimp.git
	
* In a terminal window, navigate to the root of your Assimp repo and run:
	
		cmake -G "Unix Makefiles"
	
* Now run make, followed by make install. Once the build is complete, update your LD_LIBRARY_PATH:

		export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<path to assimp/lib>
	
GLFW 3 is not yet (at the time of this writing) available via standard package management channels, so you will have to build it. 

First install GLFW's dependencies, including OpenGL/Mesa:

		sudo apt-get install cmake xorg-dev libglu1-mesa-dev
	
You must have sudo privileges. (On Red Hat, you'll need to install cmake28 instead.)

To acquire the source visit:
	
		http://www.glfw.org/download.html
	
Unzip the GLFW source archive and change your working directory to the glfw-x.x.x directory. Install GLFW:
	
		cd glfw-x.x.x
		cmake -G "Unix Makefiles"
		make
		sudo make install
	
You should now have:
	
		/usr/local/include/GLFW/usr/local/lib/libglfw3.a

DeVIL and GLEW are available through standard package management channels, although the package names differ from the official names:

* DevIL: libdevil-dev
* GLEW: libglew-dev

On certain Linux distros you might encounter linkage errors if you have installed proprietary graphics drivers that come with their own OpenGL shared library. In this case you may need to modify the location in which the linker searches for the OpenGL shared library during the build process. In the included Unix (Linux) makefile, a variable called OPENGL_LIB is defined to point to such a location. Additionally you will need to update the makefile to point to the correct locations of the Assimp include and lib directories.
	

#### 2.4 OSX notes
	
On OSX it is fairly straight forward to install the pre-requisite libraries mentioned above. They are all available through the Homebrew package management utility, and the version of Assimp available through Homebrew seems to support FBX models. The actual names of the Homebrew packages you need to install are as follows:
	
* Assimp: assimp
* DevIL: devil 
* GLFW: run 'brew install glfw3' to get the correct full package name
* GLEW: glew

**Update**: Homebrew now installs Assimp 3.2 by default, which causes bugs with the included "Warrior" animation. At the time of this writing it is best to download and build Assimp 3.1.1 from source.
	
You will also need to make sure to have XCode command line tools installed. Even after installing XCode command line tools you may need to install gcc with brew:

		brew install gcc
	
**Note**: Newer versions of `gcc` cause DevIL installation to fail with:
	
		Error: devil cannot be built with any available compilers.
	
In this case try installing version 4.9 of `gcc`:

		brew install homebrew/versions/gcc49
	
	
#### 2.5 Compiling & building GTE on Windows

Using the standard build commands from within Visual Studio will produce an executable named "GTE.exe" in x64/release.
	
#### 2.6 Compiling & building GTE on Linux, Unix, or OS X
	
On Linux, Unix, and OS X the build process produces a single executable: bin/gtedemo. It has not yet been modified to produce a library (shared or static); that is still on the to-do list. The 'gtedemo' executable is a sample of what the engine can do, and the source for the demo is in the engine's source tree at src/gtedemo.
	
To invoke the build process, navigate to the root of the GTE installation and run the following command:
	
		make -f <makefile>
		
Where `makefile` is the makefile that is appropriate for your platform. Currently there are two:
		
		Makefile.osx - Apple OSX
		Makefile.unix - Unix & Linux systems
		
Assuming the build succeeded, the 'gtedemo' executable will be produced in the 'bin' subdirectory of the GTE installation. 

### 3. Running the demo

Before running the demo, make sure you have downloaded the models and textures that it needs. They can be found at: http://projects.markkellogg.org/downloads/models-textures.zip. Uncompress the archive in the 'resources' sub-folder, the process should create two new folders: 'resources/models' and 'resources/textures'.

To run the demo, do not run the executable directly. Instead, there are scripts in the root of the GTE installation that will ensure the demo's working directory is correctly set up. 
	
* Linux, Unix, or OS X: Execute the 'rungtedemo.sh' script.
* Windows: Execute the 'rungtedemo.bat' script.
	
On all platforms, ensure that the shared libraries for the external libraries Assimp, DevIL, GLFW, and GLEW are all added to your PATH environment variable.
	
Note: Screen-Space Ambient Occlusion is still in the alpha stage, so it won't look right in some places (such as terrain meshes).
	
#### Demo Controls:
	
There are three scenes in the demo and they can be accessed via the numeric keys:
	
* Lava scene - '1' Key
* Castle Scene - '2' Key
* Reflecting pool scene - '3' Key
	
Arrow keys: Move the knight around the environment. Physics are not incorporated into the engine so you will be able to walk through objects and off ledges.
	
	'X' Key: Play stab animation.
	
	'C' Key: Play chop animation.
	
	'V' Key: Play spin attack animation.
	
	'B' Key: Play defend animation.
	
	'S' Key: Toggle skybox rendering.
	
	'O' Key: Toggle Screen-Space Ambient occlusion (SSAO).
	
	'I' Key: Toggle between standard SSAO, where SSAO is applied right after ambient lighting, to 'Outline SSAO', where SSAO is applied after all lighting, and produces an interesting outlining effect (and some undesirable side-effects).

	'M' Key: Toggle the smoke effect in the castle scene (it has a significant effect on performance).
	
	Spacebar: Add a ripple/drop in the reflecting pool scene.
	
	Lighting functionality:
	
	'A' Key: Select ambient lighting.
	
	'D' Key: Select directional lighting.
	
	'P' Key: Select point lights in the scene.
	
	'L' Key: Select lava lights.
	
	'Q' Key: Toggle currently selected light.
	
	'W' Key: Increase the intensity of currently selected light.
	
	'E' Key: Decrease intensity of currently selected light.
	
	'R' Key: Toggle shadow casting for light (will only work for meshes that cast shadows)
	

	

