#ifndef _GTE_ERRORMANAGER_H_
#define _GTE_ERRORMANAGER_H_

#include <vector>
#include <string>

#include "engine.h"

namespace GTE
{
	enum ErrorCode
	{
		GENERAL_FATAL = 5,
		GENERAL_NONFATAL = 4,
		GENERAL_SEVERE = 3,
		GENERAL_WARNING = 2
	};

	class ErrorManager
	{
		// necessary to trigger lifecycle events and manage allocation
		friend class Engine;

		protected:

		std::string errorMessage;
		Int32 errorCode;

		ErrorManager();
		~ErrorManager();

		public:

		void SetError(Int32 code, const std::string& message);
		void SetError(Int32 code, const Char * message);
		void AddError(Int32 code, const std::string& message);
		void AddError(Int32 code, const Char * message);
		void SetAndReportError(Int32 code, const std::string& message);
		void SetAndReportError(Int32 code, const Char * message);
		void AddAndReportError(Int32 code, const std::string& message);
		void AddAndReportError(Int32 code, const Char * message);
		void Reset();
		Int32 GetErrorCode() const;
		const std::string& GetErrorMessage() const;
	};
}

#endif
