/*
 * class: FileSystem
 *
 * author: Mark Kellogg
 *
 * Purpose of this class is to hide platform-specific
 * file-system characteristics.
 *
 */

#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

//forward declarations

#include <string>

class FileSystem
{
	static FileSystem * theInstance;

	protected:

	FileSystem();
	virtual ~FileSystem();

	public:

	static FileSystem * Instance();

	virtual std::string ConcatenatePaths(const std::string& pathA, const std::string& pathB) const = 0;
	virtual std::string GetBasePath(const std::string& path) const = 0;
	virtual std::string GetPathFromIXPath(const std::string& path) const = 0;
	virtual std::string FixupPath(const std::string& path) const = 0;
	virtual std::string GetFileName(const std::string& fullPath) const = 0;
	virtual bool FileExists(const std::string& fullPath) const = 0;
};

#endif
