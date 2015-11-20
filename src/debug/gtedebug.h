#ifndef _GTE_DEBUG_H_
#define _GTE_DEBUG_H_

#include "engine.h"

#include <string>

namespace GTE
{
	enum class DebugLevel
	{
		Normal = 0,
		Warning = 1,
		Error = 2
	};

	class Debug
	{
	public:

		static void PrintMessage(const Char * message);
		static void PrintWarning(const Char * message);
		static void PrintError(const Char * message);

		static void PrintMessage(std::string& message);
		static void PrintWarning(std::string& message);
		static void PrintError(std::string& message);

		static void PrintAtLevel(const std::string& message, DebugLevel level);
		static void PrintAtLevel(const Char* message, DebugLevel level);
	};
}

#endif
