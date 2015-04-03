#ifndef _GTE_DEBUG_H_
#define _GTE_DEBUG_H_

#include <string>

enum class DebugLevel
{
	Normal = 0,
	Warning = 1,
	Error = 2
};

class Debug
{
    public :

    static void PrintMessage(const char * message); 
    static void PrintWarning(const char * message);
    static void PrintError(const char * message); 

    static void PrintMessage(std::string& message);
    static void PrintWarning(std::string& message);
    static void PrintError(std::string& message);

    static void PrintAtLevel(const std::string& message, DebugLevel level);
    static void PrintAtLevel(const char* message, DebugLevel level);
};

#endif
