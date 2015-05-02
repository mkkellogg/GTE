#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "graphics/gl_include.h"
#include "inputmanagerGL.h"
#include "graphics/graphics.h"
#include "graphics/graphicsGL.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/gtedebug.h"

InputManagerGL * _thisPtr = NULL;

void _keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(_thisPtr != NULL)
    {
        if(action == GLFW_PRESS || action == GLFW_REPEAT)_thisPtr->KeyboardFunction(key, scancode, mods);
	else _thisPtr->KeyboardFunctionUp(key, scancode, mods);
    }
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
    Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
    GraphicsGL * graphicsGL = dynamic_cast<GraphicsGL*>(graphics);

    ASSERT(graphicsGL != NULL, "InputManagerGL::Init -> Unable to cast to GraphicsGL.");
	
    GLFWwindow* window = graphicsGL->GetGLFWWindow();
    glfwSetKeyCallback(window, _keyboardFunc);
    return true;
}

void InputManagerGL::Update()
{

}

void InputManagerGL::KeyboardFunction(int key, int scanCode, int mods)
{
	SetKeyState(GetKeyIndexFromCharacter(key), KeyState::Down);
	SetOnKeyDown(GetKeyIndexFromCharacter(key), true);

	DigitalInput input = MapSpecialInputToDigitalInput(key);
	if(input != DigitalInput::None)
	{
		this->SetDigitalInputState(input, true);
	}

	//printf("key down: %d, scancode:%d\n", key, scanCode);
}

void InputManagerGL::KeyboardFunctionUp(int key, int scanCode, int mods)
{
	SetKeyState(GetKeyIndexFromCharacter(key), KeyState::Up);
	//printf("key up: %d, x:%d, y:%d\n", key,x,y);

	DigitalInput input = MapSpecialInputToDigitalInput(key);
	if(input != DigitalInput::None)
	{
		this->SetDigitalInputState(input, false);
	}
}

/*
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
}*/

DigitalInput InputManagerGL::MapSpecialInputToDigitalInput(int key)
{
	switch(key)
	{
		case GLFW_KEY_LEFT:
			return DigitalInput::Left;
			break;
		case GLFW_KEY_RIGHT:
			return DigitalInput::Right;
			break;
		case GLFW_KEY_UP:
			return DigitalInput::Up;
			break;
		case GLFW_KEY_DOWN:
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
		case  GLFW_KEY_F1:
			return 256;
			break;
		case  GLFW_KEY_F2:
			return 257;
			break;
		case  GLFW_KEY_F3:
			return 258;
			break;
		case  GLFW_KEY_F4:
			return 259;
			break;
		case  GLFW_KEY_F5:
			return 260;
			break;
		case  GLFW_KEY_F6:
			return 261;
			break;
		case  GLFW_KEY_F7:
			return 262;
			break;
		case  GLFW_KEY_F8:
			return 263;
			break;
		case  GLFW_KEY_F9:
			return 264;
			break;
		case  GLFW_KEY_F10:
			return 265;
			break;
		case  GLFW_KEY_F11:
			return 266;
			break;
		case  GLFW_KEY_F12:
			return 267;
			break;
		case  GLFW_KEY_LEFT:
			return 268;
			break;
		case  GLFW_KEY_UP:
			return 269;
			break;
		case  GLFW_KEY_RIGHT:
			return 270;
			break;
		case  GLFW_KEY_DOWN:
			return 271;
			break;
		case  GLFW_KEY_PAGE_UP:
			return 272;
			break;
		case  GLFW_KEY_PAGE_DOWN:
			return 273;
			break;
		case  GLFW_KEY_HOME:
			return 274;
			break;
		case  GLFW_KEY_END:
			return 275;
			break;
		case  GLFW_KEY_INSERT:
			return 276;
			break;
	}
	return 0;
}

int InputManagerGL::MapModifierToKeyIndex(int modifier)
{
	switch(modifier)
	{
		case GLFW_KEY_RIGHT_SHIFT:
			return 320;
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			return 321;
			break;
		case GLFW_KEY_RIGHT_ALT:
			return 322;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			return 323;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			return 324;
			break;
		case GLFW_KEY_LEFT_ALT:
			return 325;
			break;

	}
	return 0;
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
		return MapModifierToKeyIndex(GLFW_KEY_LEFT_SHIFT);
		break;
	case NonCharacterKey::RightShift:
		return MapModifierToKeyIndex(GLFW_KEY_RIGHT_SHIFT);
		break;
	case NonCharacterKey::LeftCtrl:
		return MapModifierToKeyIndex(GLFW_KEY_LEFT_CONTROL);
		break;
	case NonCharacterKey::RightCtrl:
		return MapModifierToKeyIndex(GLFW_KEY_RIGHT_CONTROL);
		break;
	case NonCharacterKey::LeftAlt:
		return MapModifierToKeyIndex(GLFW_KEY_LEFT_ALT);
		break;
	case NonCharacterKey::RightAlt:
		return MapModifierToKeyIndex(GLFW_KEY_RIGHT_ALT);
		break;
	case NonCharacterKey::F1:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F1);
		break;
	case NonCharacterKey::F2:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F2);
		break;
	case NonCharacterKey::F3:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F3);
		break;
	case NonCharacterKey::F4:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F4);
		break;
	case NonCharacterKey::F5:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F5);
		break;
	case NonCharacterKey::F6:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F6);
		break;
	case NonCharacterKey::F7:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F7);
		break;
	case NonCharacterKey::F8:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F8);
		break;
	case NonCharacterKey::F9:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F9);
		break;
	case NonCharacterKey::F10:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F10);
		break;
	case NonCharacterKey::F11:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F11);
		break;
	case NonCharacterKey::F12:
		return MapSpecialInputToKeyIndex(GLFW_KEY_F12);
		break;
	case NonCharacterKey::PageUp:
		return MapSpecialInputToKeyIndex(GLFW_KEY_PAGE_UP);
		break;
	case NonCharacterKey::PageDown:
		return MapSpecialInputToKeyIndex(GLFW_KEY_PAGE_DOWN);
		break;
	case NonCharacterKey::Hone:
		return MapSpecialInputToKeyIndex(GLFW_KEY_HOME);
		break;
	case NonCharacterKey::End:
		return MapSpecialInputToKeyIndex(GLFW_KEY_END);
		break;
	case NonCharacterKey::Insert:
		return MapSpecialInputToKeyIndex(GLFW_KEY_INSERT);
		break;
	}

	return 0;
}

unsigned int InputManagerGL::GetKeyIndexFromCharacter(unsigned char key)
{
	return (unsigned int)key;
}
