/*
 * class: FileSystemIX
 *
 * author: Mark Kellogg
 *
 * Implementation of UNIX/Linux/OSX specific file system mechanics
 *
 */

#ifndef _GTE_FILE_SYSTEM_IX_H_
#define _GTE_FILE_SYSTEM_IX_H_

#include "filesystem.h"
#include <string>

namespace GTE
{
	class FileSystemIX : public FileSystem
	{
		friend class FileSystem;

	protected:

		FileSystemIX();
		~FileSystemIX();

	public:

		std::string ConcatenatePaths(const std::string& pathA, const std::string& pathB) const;
		std::string GetBasePath(const std::string& path) const;
		std::string FixupPathForLocalFilesystem(const std::string& path) const;
		std::string GetFileName(const std::string& fullPath) const;
		bool FileExists(const std::string& fullPath) const;
	};
}

#endif
