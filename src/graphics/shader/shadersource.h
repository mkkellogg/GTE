#ifndef _GTE_SHADER_SOURCE_H_
#define _GTE_SHADER_SOURCE_H_

#include <string>
#include <vector>

#include "engine.h"
#include "shadersourcelines.h"

namespace GTE {
    enum class ShaderSourceType {
        File = 0,
        String = 1
    };

    enum ShaderSourceError {
        LoadError = 0
    };

    class ShaderSource {
        class ProcessingContext {
        public:
            std::vector<std::string> LoadedIncludes;
        };

        std::string name;
        std::string baseDir;
        ShaderSourceType sourceType;
        Bool loaded;
        Bool initialized;

        ShaderSourceLines vertexSourceLines;
        std::string vertexSourceString;
        std::string vertexSourceFile;
        ShaderSourceLines fragmentSourceLines;
        std::string fragmentSourceFile;
        std::string fragmentSourceString;

        Bool ReadShaderSourceLines(const Char *fn, ShaderSourceLines& dest);
        void CopyToThis(const ShaderSource& source);

        void ProcessShaderLinesToString(const ShaderSourceLines& lineSource, std::string& output, ProcessingContext& context);
        Bool ProcessShaderLine(const std::string& line, std::string& strProcesingResult, ProcessingContext& context);
        Bool TokenizeShaderLine(const std::string& line, std::string& command, std::vector<std::string>& args);
        Bool ProcessShaderCommand(const std::string& command, const std::vector <std::string>& args, std::string& strProcesingResult, ProcessingContext& context);

    public:

        ShaderSource();
        ShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& baseDir, const std::string& name);
        ShaderSource(const ShaderSource& source);
        ShaderSource& operator= (const ShaderSource& source);
        ~ShaderSource();
        void Init(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& baseDir, const std::string& name);
        const std::string& GetVertexSourceString() const;
        const std::string& GetFragmentSourceString() const;
        const std::string& GetName() const;
        Bool Load();
        Bool IsLoaded();
    };
}

#endif
