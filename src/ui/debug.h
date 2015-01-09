#ifndef _GTE_DEBUG_H_
#define _GTE_DEBUG_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

class Debug
{
    public :

    static void PrintMessage(const char * message); 
    static void PrintWarning(const char * message);
    static void PrintError(const char * message); 

    static void PrintMessage(std::string& message);
    static void PrintWarning(std::string& message);
    static void PrintError(std::string& message);
};

#endif
