#include <memory.h>
#include <iostream>
#include <fstream>

#include "filesystem.h"
#include "filesystemIX.h"
#include "util/engineutility.h"
#include "global/global.h"
#include "global/assert.h"
#include "engine.h"

namespace GTE {
    FileSystemIX::FileSystemIX() {

    }

    FileSystemIX::~FileSystemIX() {

    }

    std::string FileSystemIX::ConcatenatePaths(const std::string& pathA, const std::string& pathB) const {
        std::string pathATrimmed = EngineUtility::Trim(pathA);
        std::string pathBTrimmed = EngineUtility::Trim(pathB);

        Char separator = '/';

        UInt32 iB = 0;
        while (iB < pathBTrimmed.size() && pathBTrimmed.c_str()[iB] != separator) iB++;
        Int32 iA = 0;
        while (iA >= 0 && pathATrimmed.c_str()[iA] != separator) iA--;

        if (iA >= 0) pathATrimmed = pathATrimmed.substr(0, iA + 1);
        if (iB < pathBTrimmed.size()) pathBTrimmed = pathBTrimmed.substr(iB, pathBTrimmed.size() - 1 - iB);

        pathATrimmed.append(1, separator);

        return pathATrimmed + pathBTrimmed;
    }

    std::string FileSystemIX::GetBasePath(const std::string& path) const {
        size_t pos = path.find_last_of("/");
        return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
    }

    std::string FileSystemIX::FixupPathForLocalFilesystem(const std::string& path) const {
        const UInt32 size = (UInt32)path.size() + 1;
        Char *chars = new(std::nothrow) Char[size];
        ASSERT(chars != nullptr, " FileSystemIX::FixupPathForLocalFilesystem -> Could not allocate path array.");

        strcpy(chars, path.c_str());

        for (UInt32 i = 0; i < path.size(); i++) {
            if (chars[i] == '\\')chars[i] = '/';
        }

        std::string newPath = std::string(chars);

        delete[] chars;

        return newPath;
    }

    std::string FileSystemIX::GetFileName(const std::string& fullPath) const {
        size_t pos = fullPath.find_last_of("/");
        return (std::string::npos == pos) ? std::string() : fullPath.substr(pos + 1);
    }

    Bool FileSystemIX::FileExists(const std::string& fullPath) const {
        std::ifstream f(fullPath.c_str());
        Bool isGood = f.good();
        f.close();
        return isGood;
    }
}
