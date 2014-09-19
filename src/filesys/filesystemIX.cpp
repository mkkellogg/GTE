#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "filesystem.h"
#include "filesystemIX.h"
#include "util/util.h"

FileSystemIX::FileSystemIX()
{

}

FileSystemIX::~FileSystemIX()
{

}

std::string FileSystemIX::ConcatenatePaths(const std::string& pathA, const std::string& pathB) const
{
	std::string pathATrimmed = Util::Trim(pathA);
	std::string pathBTrimmed = Util::Trim(pathB);

	if(pathBTrimmed.substr(0,1).compare("/") == 0)
	{
		return std::string("");
	}

	if(pathATrimmed.substr(pathATrimmed.size()-1,1).compare("/") == 0)
	{
		pathATrimmed += "/";
	}

	return pathATrimmed + pathBTrimmed;
}

std::string FileSystemIX::GetBasePath(const std::string& path) const
{
	size_t pos = path.find_last_of("/");
	return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
}

std::string FileSystemIX::GetPathFromIXPath(const std::string& path) const
{
	return path;
}

