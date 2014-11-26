
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>
 
#include "shadersource.h"
#include "gte.h"
#include "global/global.h"

ShaderSource::ShaderSource(const std::string& filePath)
{
    loaded = false;
    sourcePath = filePath;
    sourceString = NULL;
}

ShaderSource::~ShaderSource()
{
	SAFE_DELETE(sourceString);
}

bool ShaderSource::Load()
{
    sourceString = ReadShaderSource(sourcePath.c_str());
    loaded = sourceString != NULL ? true : false;
    return loaded;
}

char * ShaderSource::GetSourceString()
{
    return sourceString;
}

char * ShaderSource::ReadShaderSource(const char *fn)
{
    FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) 
    {
		fp = fopen(fn,"rt");

		if (fp != NULL) 
        {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

			if (count > 0) 
            {
				content = new char[count+1];
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

bool ShaderSource::IsLoaded()
{
    return loaded;
}
