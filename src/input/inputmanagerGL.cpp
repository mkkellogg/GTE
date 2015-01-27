#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "inputmanagerGL.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/gtedebug.h"

InputManagerGL * _thisPtr = NULL;
void _keyboardFunc(unsigned char key, int x, int y)
{
	if(_thisPtr != NULL)_thisPtr->KeyboardFunction(key, x, y);
}

void _keyboardFuncUp(unsigned char key, int x, int y)
{
	if(_thisPtr != NULL)_thisPtr->KeyboardFunctionUp(key, x, y);
}

void _specialInputUp(int key, int x, int y)
{
	if(_thisPtr != NULL)_thisPtr->SpecialInputUp(key, x, y);
}

void _specialInput(int key, int x, int y)
{
	if(_thisPtr != NULL)_thisPtr->SpecialInput(key, x, y);
}

InputManagerGL::InputManagerGL()
{
	_thisPtr = this;
}

InputManagerGL::~InputManagerGL()
{

}

bool InputManagerGL::Init()
{
	glutKeyboardFunc(_keyboardFunc);
	glutKeyboardUpFunc(_keyboardFuncUp);
	glutSpecialUpFunc(_specialInputUp);
	glutSpecialFunc(_specialInput);
	return true;
}

void InputManagerGL::KeyboardFunction(unsigned char key, int x, int y)
{
	SetKeyState((int)key, 0x1);
	//printf("key down: %d, x:%d, y:%d\n", key,x,y);
}

void InputManagerGL::KeyboardFunctionUp(unsigned char key, int x, int y)
{
	SetKeyState((int)key, 0x0);
	//printf("key up: %d, x:%d, y:%d\n", key,x,y);
}

void InputManagerGL::SpecialInput(int key, int x, int y)
{
	//printf("key: %d, x:%d, y:%d\n", key,x,y);

	DigitalInput input = MapSpecialInputToDigitalInput(key);

	if(input != DigitalInput::None)
	{
		this->SetDigitalInputState(input, true);
	}
}

void InputManagerGL::SpecialInputUp(int key, int x, int y)
{
	//printf("key: %d, x:%d, y:%d\n", key,x,y);

	DigitalInput input = MapSpecialInputToDigitalInput(key);

	if(input != DigitalInput::None)
	{
		this->SetDigitalInputState(input, false);
	}
}

DigitalInput InputManagerGL::MapSpecialInputToDigitalInput(int key)
{
	// key == 100 -> Left
	// key == 102 -> Right
	// key == 101 -> Up
	// key == 103 -> Down

	switch(key)
	{
		case GLUT_KEY_LEFT:
			return DigitalInput::Left;
			break;
		case GLUT_KEY_RIGHT:
			return DigitalInput::Right;
			break;
		case GLUT_KEY_UP:
			return DigitalInput::Up;
			break;
		case GLUT_KEY_DOWN:
			return DigitalInput::Down;
			break;
		default:
			return DigitalInput::None;
	}
}
