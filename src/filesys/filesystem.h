/*
 * class: FileSystem
 *
 * author: Mark Kellogg
 *
 * Purpose of this class is to hide platform-specific
 * file-system characteristics.
 *
 */

#ifndef _GTE_FILE_SYSTEM_H_
#define _GTE_FILE_SYSTEM_H_

#include <string>

namespace GTE
{
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
		virtual std::string FixupPathForLocalFilesystem(const std::string& path) const = 0;
		virtual std::string GetFileName(const std::string& fullPath) const = 0;
		virtual bool FileExists(const std::string& fullPath) const = 0;
	};
}

#endif
