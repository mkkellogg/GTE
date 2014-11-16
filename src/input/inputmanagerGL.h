#ifndef _INPUT_MANAGER_GL_H_
#define _INPUT_MANAGER_GL_H_

//forward declarations

#include <string>
#include "inputmanager.h"
#include "object/enginetypes.h"

class InputManagerGL : public InputManager
{
	friend class Engine;

	protected:

	InputManagerGL();
    ~InputManagerGL();
    DigitalInput MapSpecialInputToDigitalInput(int key);

    public :

    bool Init();
    void KeyboardFunction(unsigned char key, int x, int y);
    void SpecialInputUp(int key, int x, int y);
    void SpecialInput(int key, int x, int y);
};

#endif
