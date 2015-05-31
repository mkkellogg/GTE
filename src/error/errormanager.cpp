#include "errormanager.h"
#include "debug/gtedebug.h"

namespace GTE
{
	ErrorManager::ErrorManager()
	{
		Reset();
	}

	ErrorManager::~ErrorManager()
	{

	}

	void ErrorManager::SetError(Int32 code, const std::string& message)
	{
		errorCode = code;
		errorMessage = message;
	}

	void ErrorManager::SetError(Int32 code, const char * message)
	{
		SetError(code, std::string(message));
	}

	void ErrorManager::AddError(Int32 code, const std::string& message)
	{
		errorCode = code;
		errorMessage = message;
	}

	void ErrorManager::AddError(Int32 code, const char * message)
	{
		AddError(code, std::string(message));
	}

	void ErrorManager::SetAndReportError(Int32 code, const std::string& message)
	{
		SetError(code, message);
		Debug::PrintAtLevel(message, DebugLevel::Error);
	}

	void ErrorManager::SetAndReportError(Int32 code, const char * message)
	{
		SetAndReportError(code, std::string(message));
	}

	void ErrorManager::AddAndReportError(Int32 code, const std::string& message)
	{
		AddError(code, message);
		Debug::PrintAtLevel(message, DebugLevel::Error);
	}

	void ErrorManager::AddAndReportError(Int32 code, const char * message)
	{
		AddAndReportError(code, std::string(message));
	}

	void ErrorManager::Reset()
	{
		errorCode = -1;
		errorMessage = std::string("No error");
	}

	Int32 ErrorManager::GetErrorCode() const
	{
		return errorCode;
	}

	const std::string& ErrorManager::GetErrorMessage() const
	{
		return errorMessage;
	}
}
