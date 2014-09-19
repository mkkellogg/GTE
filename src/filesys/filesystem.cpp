#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "filesystem.h"
#include "filesystemIX.h"

FileSystem * FileSystem::theInstance = NULL;

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

FileSystem * FileSystem::Instance()
{
	//TODO: make thread safe and add double checked locking
	if(theInstance == NULL)
	{
		// for now default to Linux/UNIX/OSX
		theInstance = new FileSystemIX();
	}

	return theInstance;
}
