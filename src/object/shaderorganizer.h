/*
 * class: ShaderManager
 *
 * author: Mark Kellogg
 *
 * Class for managing built-in shaders
 *
 */

#ifndef _GTE_SHADER_MANAGER_H_
#define _GTE_SHADER_MANAGER_H_

#include <map>

#include "engine.h"
#include "base/bitmask.h"

namespace GTE {
    //forward declarations
    class Shader;

    enum class ShaderMaterialCharacteristic {
        DiffuseColored = 0,
        SpecularColored = 1,
        DiffuseTextured = 2,
        SpecularTextured = 3,
        Bumped = 4,
        EmissiveColored = 5,
        EmissiveTextured = 6,
        VertexColors = 7,
        VertexNormals = 8
    };

    class ShaderOrganizer {
        std::map<LongMask, ShaderSharedPtr> loadedShaders;

    protected:

    public:

        ShaderOrganizer();
        ~ShaderOrganizer();

        void AddShader(LongMask properties, ShaderSharedPtr shader);
        ShaderSharedPtr GetShader(LongMask flags);
    };
}

#endif


