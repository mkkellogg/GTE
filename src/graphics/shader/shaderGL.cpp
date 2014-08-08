
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "shader.h"
#include "shaderGL.h"
#include "ui/debug.h"
#include "gte.h"


ShaderGL::ShaderGL(const char * vertexShaderPath, const char * fragmentShaderPath) : Shader(vertexShaderPath, fragmentShaderPath)
{
    ready = false;

    programID = 0;
    vertexShaderID = 0;
    fragmentShaderID = 0;

    vertexShaderSource = new ShaderSource(vertexShaderPath);
    fragmentShaderSource = new ShaderSource(fragmentShaderPath);
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

    return true;
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
 
