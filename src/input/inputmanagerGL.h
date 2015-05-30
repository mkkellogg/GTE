#ifndef _GTE_INPUT_MANAGER_GL_H_
#define _GTE_INPUT_MANAGER_GL_H_

#include <string>
#include "inputmanager.h"
#include "object/enginetypes.h"

namespace GTE
{

	class InputManagerGL : public InputManager
	{
		friend class Engine;

	protected:

		InputManagerGL();
		~InputManagerGL();
		DigitalInput MapSpecialInputToDigitalInput(int key);
		int MapSpecialInputToKeyIndex(int key);
		int MapModifierToKeyIndex(int modifier);
		unsigned int GetKeyIndexForNonCharacterKey(NonCharacterKey key);
		unsigned int GetKeyIndexFromCharacter(unsigned char key);

	public:

		bool Init();
		void Update();
		void KeyboardFunction(int key, int scanCode, int mods);
		void KeyboardFunctionUp(int key, int scanCode, int mods);
	};
}

#endif
