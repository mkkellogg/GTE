/*
 * class: ShaderGL
 *
 * Author: Mark Kellogg
 *
 * This class is the OpenGL specific implementation of Shader. It does not contain any
 * shader code; it contains functions to load, compile, and link GLSL shaders from source code
 * as well as functions to examine compiled shaders to determine variable information,
 * and set the values for those variables.
 *
 * An instance of a ShaderGL object actually means the combination of a vertex and fragment
 * shader. A shader is not complete unless it has both of those components.
 *
 * UniformDescriptor and AttributeDescriptor objects are created to describe each of the
 * uniforms and attributes exposed by the shader. These can be referred to "by index" and
 * "by shader var ID/location" and the distinction can be confusing. The 'index' of an attribute
 * or uniform is its index in [attributes] or [uniforms] respectively. These are the arrays of
 * AttributeDescriptor and UniformDescriptor objects. The 'shader var ID/location' of an attribute
 * or uniform is the unique identifier assigned by OpenGL.
 */

#ifndef _GTE_SHADER_GL_H_
#define _GTE_SHADER_GL_H_

#include "engine.h"
#include "graphics/gl_include.h"
#include "shader.h"

#include <string>

namespace GTE {
    //forward declarations
    class ShaderSource;
    class GraphicsGL;
    class Texture;
    class AttributeDescriptor;
    class UniformDescriptor;

    class ShaderGL : public Shader {
        friend class GraphicsGL;

        // is this shader loaded, compiled and linked?
        Bool ready;

        std::string name;

        // OpenGL identifier for the linked shader program
        GLuint programID;

        // OpenGL identifier for the loaded and compiled vertex shader
        GLuint vertexShaderID;

        // OpenGL identifier for the loaded and compiled fragment shader
        GLuint fragmentShaderID;

        // number of attributes exposed by this shader
        UInt32 attributeCount;

        // number of uniforms exposed by this shader
        UInt32 uniformCount;

        // descriptors for this shader's attributes
        AttributeDescriptor ** attributes;

        // descriptors for this shader's uniforms
        UniformDescriptor ** uniforms;

        void DestroyShaders();
        void DestroyProgram();
        void DestroyComponents();
        void DestroyUniformAndAttributeInfo();

        Char * GetShaderLog(GLuint obj);
        Char * GetProgramLog(GLuint obj);
        Bool CheckCompilation(Int32 shaderID, ShaderType shaderType);

        Bool StoreUniformAndAttributeInfo();

    protected:

        ShaderGL(const ShaderSource& shaderSource);
        virtual ~ShaderGL();

    public:

        Bool Load();
        Bool IsLoaded() const;
        Int32 GetAttributeVarID(const std::string& varName) const;
        Int32 GetUniformVarID(const std::string& varName) const;
        GLuint GetProgramID() const;

        void SendBufferToShader(Int32 varID, const VertexAttrBuffer * buffer)  override;

        void SendUniformToShader(Int32 varID, UInt32 samplerUnitIndex, const TextureSharedPtr texture) override;
        void SendUniformToShader(Int32 varID, const Matrix4x4& mat) override;
        void SendUniformToShader(Int32 varID, Real x, Real y, Real z, Real w) override;
        void SendUniformToShader(Int32 varID, Real x, Real y, Real z) override;
        void SendUniformToShader(Int32 varID, Real x, Real y) override;
        void SendUniformToShader(Int32 varID, Real  data) override;
        void SendUniformToShader(Int32 varID, Int32  data) override;

        void SendUniformToShader4FV(Int32 varID, const Real * data, UInt32 count) override;
        void SendUniformToShader3FV(Int32 varID, const Real * data, UInt32 count) override;
        void SendUniformToShader2FV(Int32 varID, const Real * data, UInt32 count) override;
        void SendUniformToShader1FV(Int32 varID, const Real * data, UInt32 count) override;
        void SendUniformToShader4IV(Int32 varID, const Int32 * data, UInt32 count) override;
        void SendUniformToShader3IV(Int32 varID, const Int32 * data, UInt32 count) override;
        void SendUniformToShader2IV(Int32 varID, const Int32 * data, UInt32 count) override;
        void SendUniformToShader1IV(Int32 varID, const Int32 * data, UInt32 count) override;
        void SendUniformToShaderM4x4V(Int32 varID, const Matrix4x4 * mat, UInt32 count) override;

        UInt32 GetUniformCount() const;
        const UniformDescriptor * GetUniformDescriptor(UInt32 index) const;

        UInt32 GetAttributeCount() const;
        const AttributeDescriptor * GetAttributeDescriptor(UInt32 index) const;
    };
}

#endif
