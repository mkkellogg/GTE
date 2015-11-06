
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "shadersourcelines.h"
#include "global/global.h"
#include "global/assert.h"
#include "engine.h"

namespace GTE
{
	ShaderSourceLines::ShaderSourceLines()
	{

	}

	void ShaderSourceLines::Clear()
	{
		lines.clear();
	}

	void ShaderSourceLines::AddLine(std::string& line)
	{
		lines.push_back(line);
	}

	UInt32 ShaderSourceLines::GetLineCount()
	{
		return (UInt32)lines.size();
	}

	ShaderSourceLines::Iterator ShaderSourceLines::Begin() const
	{
		ShaderSourceLines::Iterator begin(0, this);
		return begin;
	}

	ShaderSourceLines::Iterator ShaderSourceLines::End() const
	{
		ShaderSourceLines::Iterator end((UInt32)lines.size(), this);
		return end;
	}
}