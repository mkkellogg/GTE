#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

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

void InputManagerGL::Update()
{

}

void InputManagerGL::KeyboardFunction(unsigned char key, int x, int y)
{
	SetKeyState(GetKeyIndexFromCharacter(key), KeyState::Down);
	SetOnKeyDown(GetKeyIndexFromCharacter(key), true);
	//printf("key down: %d, x:%d, y:%d\n", key,x,y);
}

void InputManagerGL::KeyboardFunctionUp(unsigned char key, int x, int y)
{
	SetKeyState(GetKeyIndexFromCharacter(key), KeyState::Up);
	//printf("key up: %d, x:%d, y:%d\n", key,x,y);
}

void InputManagerGL::SpecialInput(int key, int x, int y)
{
	//printf("key: %d, x:%d, y:%d\n", key,x,y);

	if( key >= 0)SetKeyState(MapSpecialInputToKeyIndex(key), KeyState::Down);

	DigitalInput input = MapSpecialInputToDigitalInput(key);

	if(input != DigitalInput::None)
	{
		this->SetDigitalInputState(input, true);
	}
}

void InputManagerGL::SpecialInputUp(int key, int x, int y)
{
	//printf("key: %d, x:%d, y:%d\n", key,x,y);

	if( key >= 0)SetKeyState(MapSpecialInputToKeyIndex(key), KeyState::Up);

	DigitalInput input = MapSpecialInputToDigitalInput(key);

	if(input != DigitalInput::None)
	{
		this->SetDigitalInputState(input, false);
	}
}

DigitalInput InputManagerGL::MapSpecialInputToDigitalInput(int key)
{
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

int InputManagerGL::MapSpecialInputToKeyIndex(int key)
{
	switch(key)
	{
		case  GLUT_KEY_F1:
			return 256;
			break;
		case  GLUT_KEY_F2:
			return 257;
			break;
		case  GLUT_KEY_F3:
			return 258;
			break;
		case  GLUT_KEY_F4:
			return 259;
			break;
		case  GLUT_KEY_F5:
			return 260;
			break;
		case  GLUT_KEY_F6:
			return 261;
			break;
		case  GLUT_KEY_F7:
			return 262;
			break;
		case  GLUT_KEY_F8:
			return 263;
			break;
		case  GLUT_KEY_F9:
			return 264;
			break;
		case  GLUT_KEY_F10:
			return 265;
			break;
		case  GLUT_KEY_F11:
			return 266;
			break;
		case  GLUT_KEY_F12:
			return 267;
			break;
		case  GLUT_KEY_LEFT:
			return 268;
			break;
		case  GLUT_KEY_UP:
			return 269;
			break;
		case  GLUT_KEY_RIGHT:
			return 270;
			break;
		case  GLUT_KEY_DOWN:
			return 271;
			break;
		case  GLUT_KEY_PAGE_UP:
			return 272;
			break;
		case  GLUT_KEY_PAGE_DOWN:
			return 273;
			break;
		case  GLUT_KEY_HOME:
			return 274;
			break;
		case  GLUT_KEY_END:
			return 275;
			break;
		case  GLUT_KEY_INSERT:
			return 276;
			break;
	}
	return 0;
}

int InputManagerGL::MapModifierToKeyIndex(int modifier)
{
	switch(modifier)
	{
		case GLUT_ACTIVE_SHIFT:
			return 320;
			break;
		case GLUT_ACTIVE_CTRL:
			return 321;
			break;
		case GLUT_ACTIVE_ALT:
			return 322;
			break;
	}
	return 0;
}

void InputManagerGL::DetectModifiers()
{
	int modifiers = glutGetModifiers();

	int compare[] = {GLUT_ACTIVE_SHIFT, GLUT_ACTIVE_CTRL, GLUT_ACTIVE_ALT};

	for(int i =0; i < 3; i++)
	{
		if(modifiers & compare[i])
			SetKeyState(MapModifierToKeyIndex(compare[i]), KeyState::Down);
		else
			SetKeyState(MapModifierToKeyIndex(compare[i]), KeyState::Up);
	}
}

unsigned int InputManagerGL::GetKeyIndexForNonCharacterKey(NonCharacterKey key)
{
	switch(key)
	{
	case NonCharacterKey::SpaceBar:
		return KEY_SPACE_ASCII;
		break;
	case NonCharacterKey::Tab:
		return KEY_TAB_ASCII;
		break;
	case NonCharacterKey::LeftShift:
		return MapModifierToKeyIndex(GLUT_ACTIVE_SHIFT);
		break;
	case NonCharacterKey::RightShift:
		return MapModifierToKeyIndex(GLUT_ACTIVE_SHIFT);
		break;
	case NonCharacterKey::LeftCtrl:
		return MapModifierToKeyIndex(GLUT_ACTIVE_CTRL);
		break;
	case NonCharacterKey::RightCtrl:
		return MapModifierToKeyIndex(GLUT_ACTIVE_CTRL);
		break;
	case NonCharacterKey::LeftAlt:
		return MapModifierToKeyIndex(GLUT_ACTIVE_ALT);
		break;
	case NonCharacterKey::RightAlt:
		return MapModifierToKeyIndex(GLUT_ACTIVE_ALT);
		break;
	case NonCharacterKey::F1:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F1);
		break;
	case NonCharacterKey::F2:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F2);
		break;
	case NonCharacterKey::F3:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F3);
		break;
	case NonCharacterKey::F4:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F4);
		break;
	case NonCharacterKey::F5:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F5);
		break;
	case NonCharacterKey::F6:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F6);
		break;
	case NonCharacterKey::F7:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F7);
		break;
	case NonCharacterKey::F8:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F8);
		break;
	case NonCharacterKey::F9:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F9);
		break;
	case NonCharacterKey::F10:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F10);
		break;
	case NonCharacterKey::F11:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F11);
		break;
	case NonCharacterKey::F12:
		return MapSpecialInputToKeyIndex(GLUT_KEY_F12);
		break;
	case NonCharacterKey::PageUp:
		return MapSpecialInputToKeyIndex(GLUT_KEY_PAGE_UP);
		break;
	case NonCharacterKey::PageDown:
		return MapSpecialInputToKeyIndex(GLUT_KEY_PAGE_DOWN);
		break;
	case NonCharacterKey::Hone:
		return MapSpecialInputToKeyIndex(GLUT_KEY_HOME);
		break;
	case NonCharacterKey::End:
		return MapSpecialInputToKeyIndex(GLUT_KEY_END);
		break;
	case NonCharacterKey::Insert:
		return MapSpecialInputToKeyIndex(GLUT_KEY_INSERT);
		break;
	}

	return 0;
}

unsigned int InputManagerGL::GetKeyIndexFromCharacter(unsigned char key)
{
	return (unsigned int)key;
}
