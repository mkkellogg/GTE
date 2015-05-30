
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "shadersource.h"
#include "global/global.h"

namespace GTE
{
	ShaderSource::ShaderSource()
	{
		initialized = false;
	}

	ShaderSource::ShaderSource(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& name)
	{
		Init(vertexSource, fragmentSource, sourceType, name);
	}

	ShaderSource::ShaderSource(const ShaderSource& source)
	{
		if (this == &source)return;
		CopyToThis(source);
	}

	ShaderSource& ShaderSource::operator= (const ShaderSource& source)
	{
		if (this == &source)return *this;
		CopyToThis(source);
		return *this;
	}

	ShaderSource::~ShaderSource()
	{

	}

	void ShaderSource::CopyToThis(const ShaderSource& source)
	{
		if (this == &source)return;
		this->loaded = source.loaded;
		this->sourceType = source.sourceType;
		this->vertexSourcePath = source.vertexSourcePath;
		this->vertexSourceString = source.vertexSourceString;
		this->fragmentSourcePath = source.fragmentSourcePath;
		this->fragmentSourceString = source.fragmentSourceString;
	}

	void ShaderSource::Init(const std::string& vertexSource, const std::string& fragmentSource, ShaderSourceType sourceType, const std::string& name)
	{
		this->name = name;
		this->sourceType = sourceType;
		if (sourceType == ShaderSourceType::File)
		{
			loaded = false;
			vertexSourcePath = vertexSource;
			fragmentSourcePath = fragmentSource;
			vertexSourceString = std::string("");
			fragmentSourceString = std::string("");
		}
		else if (sourceType == ShaderSourceType::String)
		{
			loaded = true;
			vertexSourcePath = std::string("");
			fragmentSourcePath = std::string("");
			vertexSourceString = vertexSource;
			fragmentSourceString = fragmentSource;
		}

		initialized = true;
	}

	bool ShaderSource::Load()
	{
		if (sourceType != ShaderSourceType::File)return true;
		char * vTemp = ReadShaderSource(vertexSourcePath.c_str());
		char * fTemp = ReadShaderSource(fragmentSourcePath.c_str());
		loaded = vTemp != NULL && fTemp != NULL ? true : false;
		if (loaded)
		{
			vertexSourceString = std::string(vTemp);
			fragmentSourceString = std::string(fTemp);
		}
		else
		{
			if (vTemp == NULL)
			{
				std::string errMsg = std::string("Could not load vertex shader: ") + vertexSourcePath;
				Engine::Instance()->GetErrorManager()->SetAndReportError(ShaderSourceError::LoadError, errMsg);
				return false;
			}

			if (fTemp)
			{
				std::string errMsg = std::string("Could not load fragment shader: ") + fragmentSourcePath;
				Engine::Instance()->GetErrorManager()->SetAndReportError(ShaderSourceError::LoadError, errMsg);
				return false;
			}
		}

		return loaded;
	}

	const std::string& ShaderSource::GetVertexSourceString() const
	{
		return vertexSourceString;
	}

	const std::string& ShaderSource::GetFragmentSourceString() const
	{
		return fragmentSourceString;
	}

	const std::string& ShaderSource::GetName() const
	{
		return name;
	}

	char * ShaderSource::ReadShaderSource(const char *fn)
	{
		FILE *fp;
		char *content = NULL;

		int count = 0;

		if (fn != NULL)
		{
			fp = fopen(fn, "rt");

			if (fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				count = ftell(fp);
				rewind(fp);

				if (count > 0)
				{
					content = new char[count + 1];
					count = fread(content, sizeof(char), count, fp);
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
}
