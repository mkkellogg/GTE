#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "inputmanager.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/debug.h"

InputManager::InputManager()
{
	ClearStates();
	ClearDigitalInput();
}

InputManager::~InputManager()
{

}

bool InputManager::Init()
{
	return true;
}

void InputManager::ClearStates()
{
	memset(keyState, 0, sizeof(BYTE) * MAX_KEYS);
}

void InputManager::ClearDigitalInput()
{
	for(unsigned int i = 0; i < (unsigned int)DigitalInput::_Last; i++)
	{
		digitalInputState[i] = false;
	}
}

void InputManager::SetKeyState(unsigned int index, BYTE state)
{
	if(index >= MAX_KEYS)return;
	keyState[index] = state;
}

BYTE InputManager::GetKeyState(unsigned int index)
{
	if(index >= MAX_KEYS)return (BYTE)-1;
	return keyState[index];
}

void InputManager::SetDigitalInputState(DigitalInput input, bool state)
{
	digitalInputState[(int)input] = state;
}

bool InputManager::GetDigitalInputState(DigitalInput input)
{
	return digitalInputState[(int)input];
}
