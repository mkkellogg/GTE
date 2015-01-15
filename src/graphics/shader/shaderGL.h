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

#include <GL/glew.h>
#include <GL/glut.h>

//forward declarations
class ShaderSource;
class GraphicsGL;
class Texture;
class AttributeDescriptor;
class UniformDescriptor;

#include "shader.h"
#include <string>

class ShaderGL : public Shader
{
    friend class GraphicsGL;

    // is this shader loaded, compiled and linked?
    bool ready;

    std::string name;

    // OpenGL identifier for the linked shader program
    GLuint programID;

    // OpenGL identifier for the loaded and compiled vertex shader
    GLuint vertexShaderID;

    // OpenGL identifier for the loaded and compiled fragment shader
    GLuint fragmentShaderID;

    // number of attributes exposed by this shader
    unsigned int attributeCount;

    // number of uniforms exposed by this shader
    unsigned int uniformCount;

    // descriptors for this shader's attributes
    AttributeDescriptor ** attributes;

    // descriptors for this shader's uniforms
    UniformDescriptor ** uniforms;

    void DestroyShaders();
    void DestroyProgram();
    void DestroyComponents();
    void DestroyUniformAndAttributeInfo();

    char * GetShaderLog(GLuint obj);
    char * GetProgramLog(GLuint obj);
    bool CheckCompilation(int shaderID, ShaderType shaderType);

    bool StoreUniformAndAttributeInfo();

    protected:

    ShaderGL(const ShaderSource& shaderSource);
    virtual ~ShaderGL();

    public :

    bool Load();
    bool IsLoaded();
    int GetAttributeVarID(const std::string& varName) const;
    int GetUniformVarID(const std::string& varName) const;
    GLuint GetProgramID();

    void SendBufferToShader(int varID, VertexAttrBuffer * buffer);

    void SendUniformToShader(unsigned int samplerUnitIndex, const TextureRef texture);
    void SendUniformToShader(int varID, const Matrix4x4 * mat);
    void SendUniformToShader(int varID, const Point3 * point);
    void SendUniformToShader(int varID, const Vector3 * vector);
    void SendUniformToShader(int varID, const Color4 * color);

    void SendUniformToShader4v(int varID, const float * data);
    void SendUniformToShader3v(int varID, const float * data);
    void SendUniformToShader2v(int varID, const float * data);
    void SendUniformToShader4(int varID, float x, float y, float z, float w);
    void SendUniformToShader3(int varID, float x, float y, float z);
    void SendUniformToShader2(int varID, float x, float y);
    void SendUniformToShader(int varID, float  data);

    void SendUniformToShader(int varID, int  data);

    unsigned int GetUniformCount() const;
    const UniformDescriptor * GetUniformDescriptor(unsigned int index) const;

    unsigned int GetAttributeCount() const;
    const AttributeDescriptor * GetAttributeDescriptor(unsigned int index) const;
};

#endif
