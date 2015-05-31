
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "gtedebug.h"

namespace GTE
{
	void Debug::PrintMessage(const Char * message)
	{
		printf("%s\n", message);
	}

	void Debug::PrintWarning(const Char * message)
	{
		printf("%s\n", message);
	}

	void Debug::PrintError(const Char * message)
	{
		printf("%s\n", message);
	}

	void Debug::PrintMessage(std::string& message)
	{
		PrintMessage((const Char *)message.c_str());
	}

	void Debug::PrintWarning(std::string& message)
	{
		PrintWarning((const Char *)message.c_str());
	}

	void Debug::PrintError(std::string& message)
	{
		PrintError((const Char *)message.c_str());
	}

	void Debug::PrintAtLevel(const std::string& message, DebugLevel level)
	{
		PrintAtLevel(message.c_str(), level);
	}

	void Debug::PrintAtLevel(const Char* message, DebugLevel level)
	{
		switch (level)
		{
		case DebugLevel::Normal:
			PrintMessage(message);
			break;
		case DebugLevel::Warning:
			PrintWarning(message);
			break;
		case DebugLevel::Error:
			PrintError(message);
			break;
		}
	}
}

