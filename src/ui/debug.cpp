
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "debug.h"
#include "gte.h"

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

