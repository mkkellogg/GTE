
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "debug.h"

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

