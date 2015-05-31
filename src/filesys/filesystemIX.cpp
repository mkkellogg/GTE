#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>

#include "filesystem.h"
#include "filesystemIX.h"
#include "util/engineutility.h"
#include "global/global.h"

namespace GTE
{
	FileSystemIX::FileSystemIX()
	{

	}

	FileSystemIX::~FileSystemIX()
	{

	}

	std::string FileSystemIX::ConcatenatePaths(const std::string& pathA, const std::string& pathB) const
	{
		std::string pathATrimmed = EngineUtility::Trim(pathA);
		std::string pathBTrimmed = EngineUtility::Trim(pathB);

		while (pathBTrimmed.size() > 0 && pathBTrimmed.substr(0, 1).compare("/") == 0)
		{
			pathBTrimmed = pathBTrimmed.substr(1, pathBTrimmed.size() - 1);
		}

		while (pathATrimmed.size() > 0 && pathATrimmed.substr(pathATrimmed.size() - 1, 1).compare("/") == 0)
		{
			pathATrimmed = pathATrimmed.substr(0, pathATrimmed.size() - 1);
		}

		return pathATrimmed + std::string("/") + pathBTrimmed;
	}

	std::string FileSystemIX::GetBasePath(const std::string& path) const
	{
		size_t pos = path.find_last_of("/");
		return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
	}

	std::string FileSystemIX::FixupPathForLocalFilesystem(const std::string& path) const
	{
		const UInt32 size = (UInt32)path.size() + 1;
		Char *chars = new Char[size];
		ASSERT(chars != NULL, " FileSystemIX::FixupPath -> Could not allocate path array.");

		strcpy(chars, path.c_str());

		for (UInt32 i = 0; i < path.size(); i++)
		{
			if (chars[i] == '\\')chars[i] = '/';
		}

		std::string newPath = std::string(chars);
		//newPath.replace(newPath.begin(),newPath.end(), "\\", "/");

		delete chars;

		return newPath;
	}

	std::string FileSystemIX::GetFileName(const std::string& fullPath) const
	{
		size_t pos = fullPath.find_last_of("/");
		return (std::string::npos == pos) ? std::string() : fullPath.substr(pos + 1);
	}

	Bool FileSystemIX::FileExists(const std::string& fullPath) const
	{
		std::ifstream f(fullPath.c_str());
		if (f.good())
		{
			f.close();
			return true;
		}
		else
		{
			f.close();
			return false;
		}
	}
}


