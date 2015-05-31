#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "inputmanager.h"
#include "global/global.h"
#include "util/time.h"
#include "debug/gtedebug.h"

namespace GTE
{
	InputManager::InputManager()
	{
		ClearStates();
		ClearDigitalInput();
	}

	InputManager::~InputManager()
	{

	}

	Bool InputManager::Init()
	{
		return true;
	}

	void InputManager::ClearStates()
	{
		for (UInt32 i = 0; i < MAX_KEY_INDICES; i++)
		{
			onKeyDown[i] = false;
			keyState[i] = KeyState::Up;
		}
	}

	void InputManager::ClearDigitalInput()
	{
		for (UInt32 i = 0; i < (UInt32)DigitalInput::_Last; i++)
		{
			digitalInputState[i] = false;
		}
	}

	void InputManager::GetKeyIndex(Key key, UInt32 * indices, UInt32& indexCount)
	{
		switch (key)
		{
		case Key::A:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'A');
			indices[1] = GetKeyIndexFromCharacter((UChar)'a');
			break;
		case Key::B:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'B');
			indices[1] = GetKeyIndexFromCharacter((UChar)'b');
			break;
		case Key::C:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'C');
			indices[1] = GetKeyIndexFromCharacter((UChar)'c');
			break;
		case Key::D:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'D');
			indices[1] = GetKeyIndexFromCharacter((UChar)'d');
			break;
		case Key::E:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'E');
			indices[1] = GetKeyIndexFromCharacter((UChar)'e');
			break;
		case Key::F:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'F');
			indices[1] = GetKeyIndexFromCharacter((UChar)'f');
			break;
		case Key::G:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'G');
			indices[1] = GetKeyIndexFromCharacter((UChar)'g');
			break;
		case Key::H:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'H');
			indices[1] = GetKeyIndexFromCharacter((UChar)'h');
			break;
		case Key::I:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'I');
			indices[1] = GetKeyIndexFromCharacter((UChar)'i');
			break;
		case Key::J:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'J');
			indices[1] = GetKeyIndexFromCharacter((UChar)'j');
			break;
		case Key::K:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'K');
			indices[1] = GetKeyIndexFromCharacter((UChar)'k');
			break;
		case Key::L:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'L');
			indices[1] = GetKeyIndexFromCharacter((UChar)'l');
			break;
		case Key::M:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'M');
			indices[1] = GetKeyIndexFromCharacter((UChar)'m');
			break;
		case Key::N:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'N');
			indices[1] = GetKeyIndexFromCharacter((UChar)'n');
			break;
		case Key::O:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'O');
			indices[1] = GetKeyIndexFromCharacter((UChar)'o');
			break;
		case Key::P:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'P');
			indices[1] = GetKeyIndexFromCharacter((UChar)'p');
			break;
		case Key::Q:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'Q');
			indices[1] = GetKeyIndexFromCharacter((UChar)'q');
			break;
		case Key::R:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'R');
			indices[1] = GetKeyIndexFromCharacter((UChar)'r');
			break;
		case Key::S:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'S');
			indices[1] = GetKeyIndexFromCharacter((UChar)'s');
			break;
		case Key::T:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'T');
			indices[1] = GetKeyIndexFromCharacter((UChar)'t');
			break;
		case Key::U:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'U');
			indices[1] = GetKeyIndexFromCharacter((UChar)'u');
			break;
		case Key::V:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'V');
			indices[1] = GetKeyIndexFromCharacter((UChar)'v');
			break;
		case Key::W:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'W');
			indices[1] = GetKeyIndexFromCharacter((UChar)'w');
			break;
		case Key::X:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'X');
			indices[1] = GetKeyIndexFromCharacter((UChar)'x');
			break;
		case Key::Y:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'Y');
			indices[1] = GetKeyIndexFromCharacter((UChar)'y');
			break;
		case Key::Z:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'Z');
			indices[1] = GetKeyIndexFromCharacter((UChar)'z');
			break;
		case Key::One:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'1');
			indices[1] = GetKeyIndexFromCharacter((UChar)'!');
			break;
		case Key::Two:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'2');
			indices[1] = GetKeyIndexFromCharacter((UChar)'@');
			break;
		case Key::Three:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'3');
			indices[1] = GetKeyIndexFromCharacter((UChar)'#');
			break;
		case Key::Four:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'4');
			indices[1] = GetKeyIndexFromCharacter((UChar)'$');
			break;
		case Key::Five:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'5');
			indices[1] = GetKeyIndexFromCharacter((UChar)'%');
			break;
		case Key::Six:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'6');
			indices[1] = GetKeyIndexFromCharacter((UChar)'^');
			break;
		case Key::Seven:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'7');
			indices[1] = GetKeyIndexFromCharacter((UChar)'&');
			break;
		case Key::Eight:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'8');
			indices[1] = GetKeyIndexFromCharacter((UChar)'*');
			break;
		case Key::Nine:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'9');
			indices[1] = GetKeyIndexFromCharacter((UChar)'(');
			break;
		case Key::Zero:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'0');
			indices[1] = GetKeyIndexFromCharacter((UChar)')');
			break;
		case Key::Tilda:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'~');
			indices[1] = GetKeyIndexFromCharacter((UChar)'`');
			break;
		case Key::Dash:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'-');
			indices[1] = GetKeyIndexFromCharacter((UChar)'_');
			break;
		case Key::Plus:
			indexCount = 2;
			indices[0] = GetKeyIndexFromCharacter((UChar)'+');
			indices[1] = GetKeyIndexFromCharacter((UChar)'=');
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

	void InputManager::SetOnKeyDown(UInt32 index, Bool value)
	{
		if (index >= MAX_KEY_INDICES)return;
		onKeyDown[index] = value;
	}

	Bool InputManager::GetOnKeyDown(UInt32 index)
	{
		if (index >= MAX_KEY_INDICES)return false;
		return onKeyDown[index];
	}

	void InputManager::SetKeyState(UInt32 index, KeyState state)
	{
		if (index >= MAX_KEY_INDICES)return;
		keyState[index] = state;
	}

	KeyState InputManager::GetKeyState(UInt32 index)
	{
		if (index >= MAX_KEY_INDICES)return KeyState::_Invalid;
		return keyState[index];
	}

	KeyState InputManager::GetKeyState(UChar key)
	{
		return GetKeyState(GetKeyIndexFromCharacter(key));
	}

	Bool InputManager::ShouldHandleOnKeyDown(UChar key)
	{
		UInt32 index = GetKeyIndexFromCharacter(key);
		if (GetOnKeyDown(index))
		{
			SetOnKeyDown(index, false);
			return true;
		}
		return false;
	}

	Bool InputManager::ShouldHandleOnKeyDown(Key key)
	{
		UInt32 indices[12];
		UInt32 indexCount;
		GetKeyIndex(key, indices, indexCount);
		if (indexCount > 0)
		{
			for (UInt32 i = 0; i < indexCount; i++)
			{
				UInt32 index = indices[i];
				if (GetOnKeyDown(index))
				{
					SetOnKeyDown(index, false);
					return true;
				}
			}
		}
		return false;
	}

	Bool InputManager::IsKeyDown(UChar key)
	{
		return GetKeyState(key) == KeyState::Down;
	}

	Bool InputManager::IsKeyDown(Key key)
	{
		UInt32 indices[12];
		UInt32 indexCount;
		GetKeyIndex(key, indices, indexCount);
		if (indexCount > 0)
		{
			for (UInt32 i = 0; i < indexCount; i++)
			{
				UInt32 index = indices[i];
				if (GetKeyState(index) == KeyState::Down)return true;
			}
		}

		return false;
	}


	void InputManager::SetDigitalInputState(DigitalInput input, Bool state)
	{
		digitalInputState[(Int32)input] = state;
	}

	Bool InputManager::GetDigitalInputState(DigitalInput input)
	{
		return digitalInputState[(Int32)input];
	}
}



