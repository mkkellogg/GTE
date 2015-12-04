#ifndef _GTE_INPUT_MANAGER_H_
#define _GTE_INPUT_MANAGER_H_

#include <string>

#include "engine.h"
#include "global/global.h"

namespace GTE
{
	enum class DigitalInput
	{
		Left = 0,
		Right = 1,
		Up = 2,
		Down = 3,
		_Last = 4, // Keep last
		None
	};

	enum class KeyState
	{
		_Invalid = -1,
		Up = 0,
		Down = 1
	};

	enum class Key
	{
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Zero,
		Tilda, Dash, Plus,
		SpaceBar,
		Tab,
		LeftShift,
		RightShift,
		LeftCtrl,
		RightCtrl,
		LeftAlt,
		RightAlt,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		PageUp, PageDown, Hone, End, Insert
	};

	class InputManager
	{
		// necessary to trigger lifecycle events and manage allocation
		friend class Engine;

	protected:

		static const Int32 KEY_SPACE_ASCII = 32;
		static const Int32 KEY_TAB_ASCII = 9;

		enum class NonCharacterKey
		{
			SpaceBar,
			Tab,
			LeftShift,
			RightShift,
			LeftCtrl,
			RightCtrl,
			LeftAlt,
			RightAlt,
			F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			PageUp, PageDown, Hone, End, Insert
		};

		static const UInt32 MAX_KEY_INDICES = 512;
		KeyState keyState[MAX_KEY_INDICES];
		Bool onKeyDown[MAX_KEY_INDICES];
		Bool digitalInputState[(Int32)DigitalInput::_Last];

		InputManager();
		virtual ~InputManager();
		void ClearStates();
		void ClearDigitalInput();
		void SetOnKeyDown(UInt32 index, Bool value);
		Bool GetOnKeyDown(UInt32 index);
		void SetKeyState(UInt32 index, KeyState state);
		void SetDigitalInputState(DigitalInput input, Bool state);
		void GetKeyIndex(Key key, UInt32 * indices, UInt32& indexCount);
		virtual UInt32 GetKeyIndexForNonCharacterKey(NonCharacterKey key) = 0;
		virtual UInt32 GetKeyIndexFromCharacter(UChar key) = 0;

	public:

		virtual Bool Init();
		virtual void Update() = 0;

		Bool ShouldHandleOnKeyDown(UChar key);
		Bool ShouldHandleOnKeyDown(Key key);
		Bool IsKeyDown(UChar key);
		Bool IsKeyDown(Key key);
		KeyState GetKeyState(UInt32 index);
		KeyState GetKeyState(UChar key);
		Bool GetDigitalInputState(DigitalInput input);
	};
}

#endif
