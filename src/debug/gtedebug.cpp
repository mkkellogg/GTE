
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "gtedebug.h"

void Debug::PrintMessage(const char * message)
{
    printf("%s\n", message);
}

void Debug::PrintWarning(const char * message)
{
    printf("%s\n", message);
}

void Debug::PrintError(const char * message)
{
    printf("%s\n", message);
}

void Debug::PrintMessage(std::string& message)
{
	PrintMessage((const char *)message.c_str());
}

void Debug::PrintWarning(std::string& message)
{
	PrintWarning((const char *)message.c_str());
}

void Debug::PrintError(std::string& message)
{
	PrintError((const char *)message.c_str());
}

void Debug::PrintAtLevel(const std::string& message, DebugLevel level)
{
	PrintAtLevel(message.c_str(), level);
}

void Debug::PrintAtLevel(const char* message, DebugLevel level)
{
	switch(level)
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

