#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>

#include "engineutility.h"
#include "geometry/matrix4x4.h"

EngineUtility::EngineUtility()
{

}

EngineUtility::~EngineUtility()
{

}

std::string EngineUtility::TrimLeft(const std::string& str)
{
	std::size_t first = str.find_first_not_of(' ');
	return str.substr(first, str.size());
}

std::string EngineUtility::TrimRight(const std::string& str)
{
	std::size_t last  = str.find_last_not_of(' ');
	return str.substr(0, last+1);
}

std::string EngineUtility::Trim(const std::string& str)
{
	std::size_t first = str.find_first_not_of(' ');
	std::size_t last  = str.find_last_not_of(' ');
	return str.substr(first, last-first+1);
}

void EngineUtility::PrintMatrix(const Matrix4x4& matrix)
{
	for(unsigned i = 0; i < 4; i++)
	{
		for(unsigned int j = 0; j < 4; j++)
		{
			if(j>0)printf(",");
			printf("%f", matrix.GetDataPtr()[i * 4 + j]);
		}
		printf("\n");
	}
}
