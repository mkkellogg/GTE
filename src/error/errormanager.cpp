
#include "errormanager.h"
#include "global/global.h"
#include "debug/debug.h"

ErrorManager::ErrorManager()
{
	Reset();
}

ErrorManager::~ErrorManager()
{

}

void ErrorManager::SetError(int code, const std::string& message)
{
	errorCode = code;
	errorMessage = message;
}

void ErrorManager::SetError(int code, const char * message)
{
	SetError(code, std::string(message));
}

void ErrorManager::SetAndReportError(int code, const std::string& message)
{
	SetError(code, message);
	Debug::PrintAtLevel(message, DebugLevel::Error);
}

void ErrorManager::SetAndReportError(int code, const char * message)
{
	SetAndReportError(code, std::string(message));
}

void ErrorManager::Reset()
{
	errorCode = -1;
	errorMessage = std::string("No error");
}

int ErrorManager::GetErrorCode() const
{
	return errorCode;
}

const std::string& ErrorManager::GetErrorMessage() const
{
	return errorMessage;
}
