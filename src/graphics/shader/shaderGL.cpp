
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "shader.h"
#include "shaderGL.h"
#include "shadersource.h"
#include "graphics/render/vertexattrbuffer.h"
#include "graphics/render/vertexattrbufferGL.h"
#include "geometry/matrix4x4.h"
#include "graphics/texture/texture.h"
#include "graphics/texture/textureGL.h"
#include "ui/debug.h"
#include "global/global.h"
#include "gte.h"
#include "uniformdesc.h"
#include "attributedesc.h"
#include <string>


ShaderGL::ShaderGL(const char * vertexShaderPath, const char * fragmentShaderPath) : Shader(vertexShaderPath, fragmentShaderPath)
{
    ready = false;
    name = NULL;

    programID = 0;
    vertexShaderID = 0;
    fragmentShaderID = 0;

    vertexShaderSource = new ShaderSource(vertexShaderPath);
    fragmentShaderSource = new ShaderSource(fragmentShaderPath);

    attributeCount = 0;
    uniformCount = 0;

    attributes = NULL;
    uniforms = NULL;
}

ShaderGL::~ShaderGL() 
{
    if(vertexShaderSource != NULL)
    {
        delete vertexShaderSource;
        vertexShaderSource = NULL;
    }

    if(fragmentShaderSource != NULL)
    {
        delete fragmentShaderSource;
        fragmentShaderSource = NULL;
    }

    DestroyComponents();
}

void ShaderGL::DestroyShaders()
{
    if(vertexShaderID > 0)
    {
        glDeleteShader(vertexShaderID);
        vertexShaderID = 0;
    }

    if(fragmentShaderID > 0)
    {
        glDeleteShader(fragmentShaderID);
        fragmentShaderID = 0;
    }
}

void ShaderGL::DestroyProgram()
{
    if(programID > 0)
    {
        glDeleteProgram(programID);
        programID = 0;
    }
}

void ShaderGL::DestroyComponents()
{
    DestroyShaders();
    DestroyProgram();
    DestroyUniformAndAttributeInfo();
}

void ShaderGL::DestroyUniformAndAttributeInfo()
{
	if(attributes != NULL)
	{
		for(unsigned int i=0; i < attributeCount; i ++)
		{
			if(attributes[i] != NULL)
			{
				delete attributes[i];
			}
		}
		delete attributes;
	}

	if(uniforms != NULL)
	{
		for(unsigned  int i=0; i < uniformCount; i ++)
		{
			if(uniforms[i] != NULL)
			{
				delete uniforms[i];
			}
		}
		delete uniforms;
	}
}

bool ShaderGL::Load()
{
    bool vertexSrcLoaded = vertexShaderSource->Load();
    bool fragmentSrcLoaded = fragmentShaderSource->Load();

    if(!vertexSrcLoaded)
    {
        Debug::PrintError("Unable to load vertex shader source file.");
        return false;
    }

    if(!fragmentSrcLoaded)
    {
        Debug::PrintError("Unable to load fragment shader source file.");
        return false;
    }

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    if(vertexShaderID == 0)
    {
        Debug::PrintError("Unable to create GL vertex shader.");
        return false;
    }

    if(fragmentShaderID == 0)
    {
        DestroyShaders();
        Debug::PrintError("Unable to create GL fragment shader.");
        return false;
    }

    const char * vertexSourceString = (const char *)vertexShaderSource->GetSourceString();
    const char * fragmentSourceString = (const char *)fragmentShaderSource->GetSourceString();
 
    glShaderSource(vertexShaderID, 1, &vertexSourceString,NULL);
    glShaderSource(fragmentShaderID, 1, &fragmentSourceString,NULL);

    glCompileShader(vertexShaderID);
    if(!CheckCompilation(vertexShaderID, ShaderType::Vertex))
    {
        DestroyShaders();
        return false;
    }

    glCompileShader(fragmentShaderID);
    if(!CheckCompilation(fragmentShaderID, ShaderType::Fragment))
    {
        DestroyShaders();
        return false;
    }

    programID = glCreateProgram();
    if(programID == 0)
    {
        DestroyShaders();
        Debug::PrintError("Unable to create GL program.");
        return false;
    }

    glAttachShader(programID,vertexShaderID);
    glAttachShader(programID,fragmentShaderID);
 
    glLinkProgram(programID);

    GLint programLinked;
    glGetProgramiv(programID, GL_LINK_STATUS,  &programLinked);

    if(programLinked == GL_FALSE)
    {
        Debug::PrintError("Error linking program: ");
    	char * info = GetProgramLog(programID);
        Debug::PrintError(info != NULL ? info : "(no error info provided).");
        return false;
    }
    else
    {
        Debug::PrintMessage("Program linked successfully!");
    }

    if(!StoreUniformAndAttributeInfo())
    {
    	return false;
    }

    ready = true;
    return true;
}

bool ShaderGL::IsLoaded()
{
	return ready;
}

bool ShaderGL::CheckCompilation(int shaderID, ShaderType shaderType)
{
    GLint shaderCompiled;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);

    if(shaderCompiled == GL_FALSE)
    {
        if(shaderType == ShaderType::Vertex)
            Debug::PrintError("Error compiling vertex shader: ");
        else if(shaderType == ShaderType::Fragment)
            Debug::PrintError("Error compiling fragment shader: ");
    	char * info = GetShaderLog(shaderID);
        Debug::PrintError(info != NULL ? info : "(no error info provided).");
        return false;
    }
    else
    { 
        if(shaderType == ShaderType::Vertex)
	        Debug::PrintMessage("Vertex shader compiled successfully!");
        else if(shaderType == ShaderType::Fragment)
	        Debug::PrintMessage("Fragment shader compiled successfully!");
    }

    return true;
} 

char * ShaderGL::GetProgramLog(GLuint obj)
{
    int infologLength = 0;

    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);

        return infoLog;
    }

    return NULL;
}
 

char * ShaderGL::GetShaderLog(GLuint obj)
{
    int infologLength = 0;

    int charsWritten  = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);

        return infoLog;
    }

    return NULL;
}

bool ShaderGL::StoreUniformAndAttributeInfo()
{
	int totalUniforms = -1;
	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &totalUniforms);
	if(totalUniforms > 0)
	{
		uniforms = new UniformDescriptor*[totalUniforms];
		if(uniforms == NULL)
		{
			Debug::PrintError("Unable to allocate shader uniform descriptors.");
			return false;
		}
		memset(uniforms, 0, sizeof(UniformDescriptor*)*totalUniforms);
		for(int i=0; i < totalUniforms; i++)
		{
			int nameLen = -1;
			int size = -1;
			GLenum type = GL_ZERO;
			char name[126];

			glGetActiveUniform(programID, GLuint(i), 125, &nameLen, &size, &type, name);
			GLuint loc = glGetUniformLocation(programID, name);

			UniformDescriptor * desc = new UniformDescriptor();
			uniforms[i] = desc;

			desc->ShaderVarID = loc;
			desc->Size = size;
			desc->Type = UniformType::Unknown;

			switch(type)
			{
				case GL_SAMPLER_2D:
					desc->Type = UniformType::Sampler2D;
				break;
				case GL_FLOAT_MAT4:
					desc->Type = UniformType::Matrix4x4;
				break;
				case GL_FLOAT_VEC4:
					desc->Type = UniformType::Float4;
				break;
				case GL_FLOAT_VEC3:
					desc->Type = UniformType::Float3;
				break;
				case GL_FLOAT_VEC2:
					desc->Type = UniformType::Float2;
				break;
				case GL_FLOAT:
					desc->Type = UniformType::Float;
				break;
			}

			strcpy(desc->Name, name);
		}

		uniformCount = totalUniforms;
	}

	int totalAttributes = -1;
	glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &totalAttributes);
	if(totalAttributes > 0)
	{
		attributes = new AttributeDescriptor*[totalAttributes];
		if(attributes == NULL)
		{
			Debug::PrintError("Unable to allocate shader attribute descriptors.");
			return false;
		}
		memset(attributes, 0, sizeof(AttributeDescriptor*)*totalAttributes);
		for(int i=0; i < totalUniforms; i++)
		{
			int nameLen = -1;
			int size = -1;
			GLenum type = GL_ZERO;
			char name[126];

			glGetActiveAttrib(programID, GLuint(i), 125, &nameLen, &size, &type, name);
			GLuint loc = glGetAttribLocation(programID, name);

			AttributeDescriptor * desc = new AttributeDescriptor();
			attributes[i] = desc;

			desc->ShaderVarID = loc;
			desc->Size = size;
			desc->Type = AttributeType::Unknown;

			switch(type)
			{
				case GL_FLOAT_MAT4:
					desc->Type = AttributeType::Matrix4x4;
				break;
				case GL_FLOAT_VEC4:
					desc->Type = AttributeType::Float4;
				break;
				case GL_FLOAT_VEC3:
					desc->Type = AttributeType::Float3;
				break;
				case GL_FLOAT_VEC2:
					desc->Type = AttributeType::Float2;
				break;
				case GL_FLOAT:
					desc->Type = AttributeType::Float;
				break;
			}

			strcpy(desc->Name, name);
		}

		attributeCount = totalAttributes;
	}

	return true;
}

int ShaderGL::GetAttributeVarID(const std::string& varName) const
{
	GLint varID = glGetAttribLocation(programID, varName.c_str());
	return (int)varID;
}

int ShaderGL::GetUniformVarID(const std::string& varName) const
{
	GLint varID = glGetUniformLocation(programID, varName.c_str());
	return (int)varID;
}

GLuint ShaderGL::GetProgramID()
{
	return programID;
}

void ShaderGL::SendBufferToShader(int varID, VertexAttrBuffer * buffer)
{
	if(varID < 0)return;

	VertexAttrBufferGL * bufferGL =  dynamic_cast<VertexAttrBufferGL *>(buffer);

	NULL_CHECK_RTRN(bufferGL, "ShaderGL::SendBufferToShader -> buffer is not VertexAttrBufferGL !!");

	const float * data = bufferGL->GetDataPtr();
	int componentCount = bufferGL->GetComponentCount();
	int stride = bufferGL->GetStride();

	//TODO: move this so it is not called every single draw call
	glEnableVertexAttribArray((GLuint)varID);

	if(bufferGL->IsGPUBuffer())
	{
		glBindBuffer(GL_ARRAY_BUFFER, bufferGL->GetGPUBufferID());
		glVertexAttribPointer(varID, componentCount, GL_FLOAT, 0, stride, 0);
	}
	else
	{
		glVertexAttribPointer(varID, componentCount, GL_FLOAT, GL_FALSE, stride, data);
	}
}

void ShaderGL::SendUniformToShader(int varID, const Texture * texture)
{
	NULL_CHECK_RTRN(texture, "ShaderGL::SendUniformToShader(int, Texture *) -> NULL texture passed");

	const TextureGL * texGL = dynamic_cast<const TextureGL *>(texture);

	NULL_CHECK_RTRN(texGL, "ShaderGL::SendUniformToShader(int, Texture *) -> texture is not TextureGL !!");

	if(varID >= 0)
	{
		glActiveTexture(GL_TEXTURE0 + varID);
		glBindTexture(GL_TEXTURE_2D, texGL->GetTextureID());
	}
	else
	{
		Debug::PrintError("ShaderGL::SendUniformToShader(int, Texture *) -> could not find sampler location for texture");
		return;
	}

}

void ShaderGL::SendUniformToShader(int varID, const Matrix4x4 * mat)
{
	glUniformMatrix4fv(varID,1, GL_FALSE, mat->GetDataPtr());
}

void ShaderGL::SendUniformToShader(int varID, const Point3 * point)
{

}

void ShaderGL::SendUniformToShader(int varID, const Vector3 * vector)
{

}

void ShaderGL::SendUniformToShader(int varID, const Color4 * color)
{

}


void ShaderGL::SendUniformToShader4v(int varID, const float * data)
{

}

void ShaderGL::SendUniformToShader3v(int varID, const float * data)
{

}

void ShaderGL::SendUniformToShader2v(int varID, const float * data)
{

}

void ShaderGL::SendUniformToShader4(int varID, float x, float y, float z, float w)
{

}

void ShaderGL::SendUniformToShader3(int varID, float x, float y, float z)
{

}

void ShaderGL::SendUniformToShader2(int varID, float x, float y)
{

}

void ShaderGL::SendUniformToShader(int varID, float  data)
{

}

unsigned int ShaderGL::GetUniformCount() const
{
	return uniformCount;
}

const UniformDescriptor * ShaderGL::GetUniformDescriptor(unsigned int index) const
{
	if(index >=0 && index < uniformCount)
	{
		return (const UniformDescriptor *)uniforms[index];
	}

	return NULL;
}

unsigned int ShaderGL::GetAttributeCount() const
{
	return attributeCount;
}

const AttributeDescriptor * ShaderGL::GetAttributeDescriptor(unsigned int index) const
{
	if(index >=0 && index < attributeCount)
	{
		return (const AttributeDescriptor *)attributes[index];
	}

	return NULL;
}
