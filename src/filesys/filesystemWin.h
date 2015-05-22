/*
 * class: FileSystemWin
 *
 * author: Mark Kellogg
 *
 * Implementation of Windows specific file system mechanics
 *
 */

#ifndef _GTE_FILE_SYSTEM_WIN_H_
#define _GTE_FILE_SYSTEM_WIN_H_

//forward declarations

#include "filesystem.h"
#include <string>

class FileSystemWin : public FileSystem
{
	friend class FileSystem;

	protected:

	FileSystemWin();
	~FileSystemWin();

	public:

	std::string ConcatenatePaths(const std::string& pathA, const std::string& pathB) const;
    std::string GetBasePath(const std::string& path) const;
	std::string GetPathFromIXPath(const std::string& path) const;
	std::string FixupPath(const std::string& path) const;
	std::string GetFileName(const std::string& fullPath) const;
	bool FileExists(const std::string& fullPath) const;
};

#endif
