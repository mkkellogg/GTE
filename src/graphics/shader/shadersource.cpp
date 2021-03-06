#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <memory.h>

#include "shadersource.h"
#include "filesys/filesystem.h"
#include "global/global.h"
#include "global/assert.h"
#include "engine.h"
#include "util/engineutility.h"

namespace GTE {
    ShaderSource::ShaderSource() {
        initialized = false;
        loaded = false;
        sourceType = ShaderSourceType::String;
    }

    ShaderSource::ShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& baseDir, const std::string& name) {
        Init(vertexSource, fragmentSource, sourceType, baseDir, name);
    }

    ShaderSource::ShaderSource(const ShaderSource& source) {
        if (this == &source)return;
        CopyToThis(source);
    }

    ShaderSource& ShaderSource::operator= (const ShaderSource& source) {
        if (this == &source)return *this;
        CopyToThis(source);
        return *this;
    }

    ShaderSource::~ShaderSource() {

    }

    void ShaderSource::CopyToThis(const ShaderSource& source) {
        if (this == &source)return;
        this->loaded = source.loaded;
        this->baseDir = source.baseDir;
        this->sourceType = source.sourceType;
        this->vertexSourceFile = source.vertexSourceFile;
        this->vertexSourceLines = source.vertexSourceLines;
        this->vertexSourceString = source.vertexSourceString;
        this->fragmentSourceFile = source.fragmentSourceFile;
        this->fragmentSourceString = source.fragmentSourceString;
        this->fragmentSourceLines = source.fragmentSourceLines;
    }

    void ShaderSource::ProcessShaderLinesToString(const ShaderSourceLines& lineSource, std::string& output, ProcessingContext& context) {
        if (sourceType != ShaderSourceType::File)return;

        output.clear();

        for (ShaderSourceLines::Iterator iter = lineSource.Begin(); iter != lineSource.End(); ++iter) {
            const std::string& str = *iter;
            std::string strProcesingResult;
            Bool processed = ProcessShaderLine(str, strProcesingResult, context);

            if (!processed) {
                output.append(str);
                output.append("\n");
            }
            else {
                output.append(strProcesingResult);
                output.append("\n");
            }
        }
    }

    Bool ShaderSource::ProcessShaderLine(const std::string& line, std::string& strProcesingResult, ProcessingContext& context) {
        if (line.size() > 0) {
            std::string command;
            std::vector<std::string> args;
            Bool success = TokenizeShaderLine(line, command, args);

            if (success == true) {
                return ProcessShaderCommand(command, args, strProcesingResult, context);
            }
        }

        return false;
    }

    Bool ShaderSource::TokenizeShaderLine(const std::string& line, std::string& command, std::vector<std::string>& args) {
        std::stringstream ss(line);
        std::istream_iterator<std::string> iter(ss);
        std::istream_iterator<std::string> end;

        unsigned int count = 0;
        while (iter != end) {
            if (count == 0) {
                command = *iter;
            }
            else {
                args.push_back(*iter);
            }

            count++;
            ++iter;
        }

        if (count > 0)return true;
        else return false;
    }

    Bool ShaderSource::ProcessShaderCommand(const std::string& command, const std::vector <std::string>& args, std::string& strProcesingResult, ProcessingContext& context) {
        if (command == "#include") {
            if (args.size() > 0) {
                std::string includeFile = "";
                for (UInt32 i = 0; i < args.size(); i++) {
                    includeFile.append(args[i]);
                }

                ShaderSourceLines temp;
                includeFile = EngineUtility::Trim(includeFile);
                includeFile = includeFile.substr(1, includeFile.size() - 2);
                std::string fullIncludePath = FileSystem::Instance()->ConcatenatePaths(baseDir, includeFile);
                for (unsigned int i = 0; i < context.LoadedIncludes.size(); i++) {
                    std::string& include = context.LoadedIncludes[i];
                    if (include == fullIncludePath) {
                        return true;
                    }
                }
                context.LoadedIncludes.push_back(fullIncludePath);
                Bool includeSuccess = ReadShaderSourceLines(fullIncludePath.c_str(), temp);
                if (includeSuccess) {
                    ProcessShaderLinesToString(temp, strProcesingResult, context);
                }
            }
            else {
                strProcesingResult = "";
            }
            return true;
        }
        return false;
    }

    void ShaderSource::Init(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& baseDir, const std::string& name) {
        this->baseDir = baseDir;
        this->name = name;
        this->sourceType = sourceType;
        if (sourceType == ShaderSourceType::File) {
            loaded = false;
            vertexSourceFile = vertexSource;
            fragmentSourceFile = fragmentSource;
            vertexSourceString = std::string("");
            fragmentSourceString = std::string("");
        }
        else if (sourceType == ShaderSourceType::String) {
            loaded = true;
            vertexSourceFile = std::string("");
            fragmentSourceFile = std::string("");
            vertexSourceString = vertexSource;
            fragmentSourceString = fragmentSource;
        }

        initialized = true;
    }

    Bool ShaderSource::Load() {
        if (sourceType != ShaderSourceType::File)return true;

        FileSystem * fileSystem = FileSystem::Instance();
        std::string realVertexSource = fileSystem->ConcatenatePaths(baseDir, vertexSourceFile);
        std::string realfragmentSource = fileSystem->ConcatenatePaths(baseDir, fragmentSourceFile);

        Bool vertexSuccess = ReadShaderSourceLines(realVertexSource.c_str(), vertexSourceLines);
        Bool fragmentSuccess = ReadShaderSourceLines(realfragmentSource.c_str(), fragmentSourceLines);

        loaded = vertexSuccess && fragmentSuccess ? true : false;

        if (!loaded) {
            if (!vertexSuccess) {
                std::string errMsg = std::string("Could not load vertex shader: ") + realVertexSource;
                Engine::Instance()->GetErrorManager()->SetAndReportError(ShaderSourceError::LoadError, errMsg);
                return false;
            }

            if (!fragmentSuccess) {
                std::string errMsg = std::string("Could not load fragment shader: ") + realfragmentSource;
                Engine::Instance()->GetErrorManager()->SetAndReportError(ShaderSourceError::LoadError, errMsg);
                return false;
            }
        }

        ProcessingContext vertexContext;
        ProcessShaderLinesToString(vertexSourceLines, vertexSourceString, vertexContext);
        ProcessingContext fragmentContext;
        ProcessShaderLinesToString(fragmentSourceLines, fragmentSourceString, fragmentContext);

        return loaded;
    }

    const std::string& ShaderSource::GetVertexSourceString() const {
        return vertexSourceString;
    }

    const std::string& ShaderSource::GetFragmentSourceString() const {
        return fragmentSourceString;
    }

    const std::string& ShaderSource::GetName() const {
        return name;
    }

    Bool ShaderSource::ReadShaderSourceLines(const Char *fn, ShaderSourceLines& dest) {
        NONFATAL_ASSERT_RTRN(fn != nullptr, " ShaderSource::ReadShaderSource -> Invalid file name.", false, true);

        int linecount = 0;
        std::string line;

        std::ifstream infile;
        infile.exceptions(std::ifstream::failbit);

        try {
            infile.open(fn, std::ifstream::in);
        }
        catch (std::ifstream::failure e) {
            std::string msg = " ShaderSource::ReadShaderSource -> Unable to open shader file: ";
            msg.append(fn);
            NONFATAL_ASSERT_RTRN(false, msg, false, true);
            return false;
        }

        dest.Clear();
        try {
            while (std::getline(infile, line)) {
                dest.AddLine(line);
                linecount++;

            }
        }
        catch (std::ifstream::failure e) {

        }

        infile.close();

        return true;
    }

    Bool ShaderSource::IsLoaded() {
        return loaded;
    }
}
