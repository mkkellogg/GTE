#include "shadersourceloaderGL.h"
#include "graphics/shader/shadersource.h"
#include "filesys/filesystem.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"

namespace GTE {
    ShaderSourceLoaderGL::ShaderSourceLoaderGL() {

    }

    ShaderSourceLoaderGL::~ShaderSourceLoaderGL() {

    }

    void ShaderSourceLoaderGL::LoadShaderSource(const std::string name, ShaderSource& shaderSource) const {
        std::string vertexSource;
        std::string fragmentSource;

        FileSystem * fileSystem = FileSystem::Instance();
        std::string builtinPath = fileSystem->FixupPathForLocalFilesystem(Constants::BuiltinShaderPathOpenGL);
        vertexSource = std::string(name + std::string(".vertex.shader"));
        fragmentSource = std::string(name + std::string(".fragment.shader"));
        shaderSource.Init(vertexSource, fragmentSource, ShaderSourceType::File, builtinPath, name);
    }
}

