#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <GL/glew.h>
#include <GL/glut.h>

class Debug
{
    public :

    static void PrintMessage(const char * message); 
    static void PrintWarning(const char * message);
    static void PrintError(const char * message); 
};

#endif
