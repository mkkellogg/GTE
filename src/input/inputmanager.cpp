#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "inputmanager.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/gtedebug.h"

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
	for(unsigned int i=0; i < MAX_KEY_INDICES; i++)
	{
		onKeyDown[i] = false;
		keyState[i] = KeyState::Up;
	}
}

void InputManager::ClearDigitalInput()
{
	for(unsigned int i = 0; i < (unsigned int)DigitalInput::_Last; i++)
	{
		digitalInputState[i] = false;
	}
}

void InputManager::GetKeyIndex(Key key, unsigned int * indices, unsigned int& indexCount)
{
	switch(key)
	{
		case Key::A:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'A');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'a');
		break;
		case Key::B:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'B');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'b');
		break;
		case Key::C:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'C');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'c');
		break;
		case Key::D:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'D');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'d');
		break;
		case Key::E:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'E');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'e');
		break;
		case Key::F:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'F');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'f');
		break;
		case Key::G:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'G');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'g');
		break;
		case Key::H:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'H');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'h');
		break;
		case Key::I:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'I');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'i');
		break;
		case Key::J:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'J');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'j');
		break;
		case Key::K:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'K');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'k');
		break;
		case Key::L:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'L');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'l');
		break;
		case Key::M:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'M');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'m');
		break;
		case Key::N:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'N');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'n');
		break;
		case Key::O:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'O');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'o');
		break;
		case Key::P:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'P');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'p');
		break;
		case Key::Q:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'Q');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'q');
		break;
		case Key::R:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'R');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'r');
		break;
		case Key::S:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'S');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'s');
		break;
		case Key::T:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'T');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'t');
		break;
		case Key::U:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'U');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'u');
		break;
		case Key::V:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'V');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'v');
		break;
		case Key::W:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'W');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'w');
		break;
		case Key::X:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'X');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'x');
		break;
		case Key::Y:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'Y');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'y');
		break;
		case Key::Z:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'Z');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'z');
		break;
		case Key::One:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'1');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'!');
			break;
		case Key::Two:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'2');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'@');
			break;
		case Key::Three:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'3');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'#');
			break;
		case Key::Four:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'4');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'$');
			break;
		case Key::Five:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'5');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'%');
			break;
		case Key::Six:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'6');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'^');
			break;
		case Key::Seven:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'7');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'&');
			break;
		case Key::Eight:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'8');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'*');
			break;
		case Key::Nine:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'9');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'(');
			break;
		case Key::Zero:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'0');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)')');
			break;
		case Key::Tilda:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'~');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'`');
			break;
		case Key::Dash:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'-');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'_');
			break;
		case Key::Plus:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((unsigned char)'+');
			indices[1] = GetKeyIndexFromCharacter((unsigned char)'=');
			break;
		case Key::SpaceBar:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::SpaceBar);
			break;
		case Key::Tab:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::Tab);
			break;
		case Key::LeftShift:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::LeftShift);
			break;
		case Key::RightShift:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::RightShift);
			break;
		case Key::LeftCtrl:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::LeftCtrl);
			break;
		case Key::RightCtrl:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::RightCtrl);
			break;
		case Key::LeftAlt:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::LeftAlt);
			break;
		case Key::RightAlt:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::RightAlt);
			break;
		case Key::F1:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F1);
			break;
		case Key::F2:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F2);
			break;
		case Key::F3:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F3);
			break;
		case Key::F4:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F4);
			break;
		case Key::F5:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F5);
			break;
		case Key::F6:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F6);
			break;
		case Key::F7:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F7);
			break;
		case Key::F8:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F8);
			break;
		case Key::F9:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F9);
			break;
		case Key::F10:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F10);
			break;
		case Key::F11:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F11);
			break;
		case Key::F12:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::F12);
			break;
		case Key::PageUp:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::PageUp);
			break;
		case Key::PageDown:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::PageDown);
			break;
		case Key::Hone:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::Hone);
			break;
		case Key::End:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::End);
			break;
		case Key::Insert:
			indexCount = 1;
			indices[0] = GetKeyIndexForNonCharacterKey(NonCharacterKey::Insert);
			break;
		default:
			indexCount = 0;
		break;
	}
}

void InputManager::SetOnKeyDown(unsigned int index, bool value)
{
	if(index >= MAX_KEY_INDICES)return;
	onKeyDown[index] = value;
}

bool InputManager::GetOnKeyDown(unsigned int index)
{
	if(index >= MAX_KEY_INDICES)return false;
	return onKeyDown[index];
}

void InputManager::SetKeyState(unsigned int index, KeyState state)
{
	if(index >= MAX_KEY_INDICES)return;
	keyState[index] = state;
}

KeyState InputManager::GetKeyState(unsigned int index)
{
	if(index >= MAX_KEY_INDICES)return KeyState::_Invalid;
	return keyState[index];
}

KeyState InputManager::GetKeyState(unsigned char key)
{
	return GetKeyState(GetKeyIndexFromCharacter(key));
}

bool InputManager::ShouldHandleOnKeyDown(unsigned char key)
{
	unsigned int index = GetKeyIndexFromCharacter(key);
	if(GetOnKeyDown(index))
	{
		SetOnKeyDown(index,false);
		return true;
	}
	return false;
}

bool InputManager::ShouldHandleOnKeyDown(Key key)
{
	unsigned int indices[12];
	unsigned int indexCount;
	GetKeyIndex(key, indices, indexCount);
	if(indexCount > 0)
	{
		for(unsigned int i=0; i < indexCount; i ++)
		{
			unsigned int index = indices[i];
			if(GetOnKeyDown(index))
			{
				SetOnKeyDown(index,false);
				return true;
			}
		}
	}
	return false;
}

bool InputManager::IsKeyDown(unsigned char key)
{
	return GetKeyState(key) == KeyState::Down;
}

bool InputManager::IsKeyDown(Key key)
{
	unsigned int indices[12];
	unsigned int indexCount;
	GetKeyIndex(key, indices, indexCount);
	if(indexCount > 0)
	{
		for(unsigned int i=0; i < indexCount; i ++)
		{
			unsigned int index = indices[i];
			if(GetKeyState(index) == KeyState::Down)return true;
		}
	}

	return false;
}


void InputManager::SetDigitalInputState(DigitalInput input, bool state)
{
	digitalInputState[(int)input] = state;
}

bool InputManager::GetDigitalInputState(DigitalInput input)
{
	return digitalInputState[(int)input];
}



