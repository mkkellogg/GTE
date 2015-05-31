#ifndef _GTE_ERRORMANAGER_H_
#define _GTE_ERRORMANAGER_H_

#include "global/global.h"
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
		int errorCode;

		ErrorManager();
		~ErrorManager();

	public:

		void SetError(int code, const std::string& message);
		void SetError(int code, const char * message);
		void AddError(int code, const std::string& message);
		void AddError(int code, const char * message);
		void SetAndReportError(int code, const std::string& message);
		void SetAndReportError(int code, const char * message);
		void AddAndReportError(int code, const std::string& message);
		void AddAndReportError(int code, const char * message);
		void Reset();
		int GetErrorCode() const;
		const std::string& GetErrorMessage() const;
	};
}

#endif
