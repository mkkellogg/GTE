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
	memset(keyState, 0, sizeof(BYTE) * MAX_KEYS);
}

void InputManager::ClearDigitalInput()
{
	for(unsigned int i = 0; i < (unsigned int)DigitalInput::_Last; i++)
	{
		digitalInputState[i] = false;
	}
}

void InputManager::SetKeyState(unsigned int index, KeyState state)
{
	if(index >= MAX_KEYS)return;
	keyState[index] = state;
}

KeyState InputManager::GetKeyState(unsigned int index)
{
	if(index >= MAX_KEYS)return KeyState::_Invalid;
	return keyState[index];
}

KeyState InputManager::GetKeyState(unsigned char key)
{
	return GetKeyState(GetKeyIndexFromCharacter(key));
}

KeyState InputManager::GetKeyState(Key key)
{
	switch(key)
	{
		case Key::A:
			if(GetKeyState((unsigned char)'A') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'a') == KeyState::Down)return KeyState::Down;
		break;
		case Key::B:
			if(GetKeyState((unsigned char)'B') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'b') == KeyState::Down)return KeyState::Down;
		break;
		case Key::C:
			if(GetKeyState((unsigned char)'C') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'C') == KeyState::Down)return KeyState::Down;
		break;
		case Key::D:
			if(GetKeyState((unsigned char)'D') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'d') == KeyState::Down)return KeyState::Down;
		break;
		case Key::E:
			if(GetKeyState((unsigned char)'E') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'e') == KeyState::Down)return KeyState::Down;
		break;
		case Key::F:
			if(GetKeyState((unsigned char)'F') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'f') == KeyState::Down)return KeyState::Down;
		break;
		case Key::G:
			if(GetKeyState((unsigned char)'G') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'g') == KeyState::Down)return KeyState::Down;
		break;
		case Key::H:
			if(GetKeyState((unsigned char)'H') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'h') == KeyState::Down)return KeyState::Down;
		break;
		case Key::I:
			if(GetKeyState((unsigned char)'I') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'i') == KeyState::Down)return KeyState::Down;
		break;
		case Key::J:
			if(GetKeyState((unsigned char)'J') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'j') == KeyState::Down)return KeyState::Down;
		break;
		case Key::K:
			if(GetKeyState((unsigned char)'K') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'k') == KeyState::Down)return KeyState::Down;
		break;
		case Key::L:
			if(GetKeyState((unsigned char)'L') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'l') == KeyState::Down)return KeyState::Down;
		break;
		case Key::M:
			if(GetKeyState((unsigned char)'M') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'m') == KeyState::Down)return KeyState::Down;
		break;
		case Key::N:
			if(GetKeyState((unsigned char)'N') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'n') == KeyState::Down)return KeyState::Down;
		break;
		case Key::O:
			if(GetKeyState((unsigned char)'O') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'o') == KeyState::Down)return KeyState::Down;
		break;
		case Key::P:
			if(GetKeyState((unsigned char)'P') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'p') == KeyState::Down)return KeyState::Down;
		break;
		case Key::Q:
			if(GetKeyState((unsigned char)'Q') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'q') == KeyState::Down)return KeyState::Down;
		break;
		case Key::R:
			if(GetKeyState((unsigned char)'R') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'r') == KeyState::Down)return KeyState::Down;
		break;
		case Key::S:
			if(GetKeyState((unsigned char)'S') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'s') == KeyState::Down)return KeyState::Down;
		break;
		case Key::T:
			if(GetKeyState((unsigned char)'T') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'t') == KeyState::Down)return KeyState::Down;
		break;
		case Key::U:
			if(GetKeyState((unsigned char)'U') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'u') == KeyState::Down)return KeyState::Down;
		break;
		case Key::V:
			if(GetKeyState((unsigned char)'V') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'v') == KeyState::Down)return KeyState::Down;
		break;
		case Key::W:
			if(GetKeyState((unsigned char)'W') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'w') == KeyState::Down)return KeyState::Down;
		break;
		case Key::X:
			if(GetKeyState((unsigned char)'X') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'x') == KeyState::Down)return KeyState::Down;
		break;
		case Key::Y:
			if(GetKeyState((unsigned char)'Y') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'y') == KeyState::Down)return KeyState::Down;
		break;
		case Key::Z:
			if(GetKeyState((unsigned char)'Z') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'z') == KeyState::Down)return KeyState::Down;
		break;
		case Key::One:
			if(GetKeyState((unsigned char)'!') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'1') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Two:
			if(GetKeyState((unsigned char)'@') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'2') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Three:
			if(GetKeyState((unsigned char)'#') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'3') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Four:
			if(GetKeyState((unsigned char)'$') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'4') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Five:
			if(GetKeyState((unsigned char)'%') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'5') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Six:
			if(GetKeyState((unsigned char)'^') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'6') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Seven:
			if(GetKeyState((unsigned char)'&') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'7') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Eight:
			if(GetKeyState((unsigned char)'*') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'8') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Nine:
			if(GetKeyState((unsigned char)'(') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'9') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Zero:
			if(GetKeyState((unsigned char)')') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'0') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Tilda:
			if(GetKeyState((unsigned char)'~') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'`') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Dash:
			if(GetKeyState((unsigned char)'_') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'-') == KeyState::Down)return KeyState::Down;
			break;
		case Key::Plus:
			if(GetKeyState((unsigned char)'+') == KeyState::Down)return KeyState::Down;
			if(GetKeyState((unsigned char)'=') == KeyState::Down)return KeyState::Down;
			break;
		case Key::SpaceBar:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::SpaceBar)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::Tab:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::Tab)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::LeftShift:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::LeftShift)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::RightShift:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::RightShift)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::LeftCtrl:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::LeftCtrl)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::RightCtrl:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::RightCtrl)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::LeftAlt:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::LeftAlt)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::RightAlt:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::RightAlt)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F1:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F1)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F2:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F2)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F3:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F3)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F4:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F4)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F5:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F5)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F6:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F6)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F7:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F7)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F8:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F8)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F9:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F9)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F10:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F10)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F11:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F11)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::F12:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::F12)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::PageUp:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::PageUp)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::PageDown:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::PageDown)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::Hone:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::Hone)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::End:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::End)) == KeyState::Down)return KeyState::Down;
			break;
		case Key::Insert:
			if(GetKeyState(GetKeyIndexForNonCharacterKey(NonCharacterKey::Insert)) == KeyState::Down)return KeyState::Down;
			break;
		default:
			return KeyState::_Invalid;
		break;
	}

	return KeyState::Up;
}

bool InputManager::IsKeyDown(unsigned char key)
{
	return GetKeyState(key) == KeyState::Down;
}

bool InputManager::IsKeyDown(Key key)
{
	return GetKeyState(key) == KeyState::Down;
}


void InputManager::SetDigitalInputState(DigitalInput input, bool state)
{
	digitalInputState[(int)input] = state;
}

bool InputManager::GetDigitalInputState(DigitalInput input)
{
	return digitalInputState[(int)input];
}

