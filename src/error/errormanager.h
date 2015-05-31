#ifndef _GTE_ERRORMANAGER_H_
#define _GTE_ERRORMANAGER_H_

#include "object/enginetypes.h"
#include <vector>
#include <string>

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
		friend class Engine;

	protected:

		std::string errorMessage;
		Int32 errorCode;

		ErrorManager();
		~ErrorManager();

	public:

		void SetError(Int32 code, const std::string& message);
		void SetError(Int32 code, const char * message);
		void AddError(Int32 code, const std::string& message);
		void AddError(Int32 code, const char * message);
		void SetAndReportError(Int32 code, const std::string& message);
		void SetAndReportError(Int32 code, const char * message);
		void AddAndReportError(Int32 code, const std::string& message);
		void AddAndReportError(Int32 code, const char * message);
		void Reset();
		Int32 GetErrorCode() const;
		const std::string& GetErrorMessage() const;
	};
}

#endif
