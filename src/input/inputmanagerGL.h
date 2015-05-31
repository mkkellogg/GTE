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
		DigitalInput MapSpecialInputToDigitalInput(Int32 key);
		Int32 MapSpecialInputToKeyIndex(Int32 key);
		Int32 MapModifierToKeyIndex(Int32 modifier);
		UInt32 GetKeyIndexForNonCharacterKey(NonCharacterKey key);
		UInt32 GetKeyIndexFromCharacter(UChar key);

	public:

		Bool Init();
		void Update();
		void KeyboardFunction(Int32 key, Int32 scanCode, Int32 mods);
		void KeyboardFunctionUp(Int32 key, Int32 scanCode, Int32 mods);
	};
}

#endif
