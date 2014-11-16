#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

//forward declarations

#include <string>
#include "object/enginetypes.h"
#include "global/global.h"

enum class DigitalInput
{
	Left = 0,
	Right = 1,
	Up = 2,
	Down = 3,
	_Last = 4, // Keep last
	None
};

class InputManager
{
	friend class Engine;

	protected:

	static const unsigned int MAX_KEYS = 512;
	BYTE keyState[MAX_KEYS];
	bool digitalInputState[(int)DigitalInput::_Last];

	InputManager();
    virtual ~InputManager();
    void ClearStates();
    void ClearDigitalInput();
    void SetKeyState(unsigned int index, BYTE state);
    void SetDigitalInputState(DigitalInput input, bool state);

    public :

    virtual bool Init();
    BYTE GetKeyState(unsigned int index);
    bool GetDigitalInputState(DigitalInput input);
};

#endif
