#ifndef _GTE_ERRORMANAGER_H_
#define _GTE_ERRORMANAGER_H_

//forward declarations

#include <vector>
#include <string>

enum class ErrorCode
{
	GENERAL_FATAL = 5,
	GENERAL_SEVERE = 4,
	GENERAL_WARNING = 3
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
    void SetAndReportError(int code, const std::string& message);
    void SetAndReportError(int code, const char * message);
    void Reset();
    int GetErrorCode() const;
    const std::string& GetErrorMessage() const;
};

#endif
