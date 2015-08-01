#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "filesystem.h"
#include "filesystemIX.h"
#include "filesystemWin.h"

namespace GTE
{
	FileSystem * FileSystem::theInstance = nullptr;

	FileSystem::FileSystem()
	{

	}

	FileSystem::~FileSystem()
	{

	}

	FileSystem * FileSystem::Instance()
	{
		//TODO: make thread safe and add double checked locking
		if (theInstance == nullptr)
		{

#ifdef _WIN32
			theInstance = new FileSystemWin();
#else		
			theInstance = new FileSystemIX();
#endif
		}

		return theInstance;
	}

}