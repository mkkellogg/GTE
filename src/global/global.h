#ifndef _GTE_GLOBAL_H_
#define _GTE_GLOBAL_H_

#include "error/errormanager.h"
#include "engine.h"
#include "debug/debug.h"

#define GL_GLEXT_PROTOTYPES

#define SAFE_DELETE(ptr)                   \
{                                          \
  if((ptr) != NULL){                       \
  	delete (ptr);					       \
    (ptr) = NULL;						   \
  }										   \
}

#define SAFE_DELETE_DEBUG(ptr, msg)        \
{                                          \
  if((ptr) != NULL){                       \
  	delete (ptr);					       \
    (ptr) = NULL;						   \
  }										   \
  else									   \
  {										   \
	  Debug::PrintError(msg);              \
  }										   \
}

#define GET_ASSERT_MACRO(_1,_2,_3,_4, NAME,...) NAME
#define ASSERT(...) GET_ASSERT_MACRO(__VA_ARGS__, ASSERT4, ASSERT3, ASSERT2)(__VA_ARGS__)

#define ASSERT4(exp, msg, returnExp, errCode)    															\
{										   																	\
	if(!(exp))					     			 															\
	{																										\
		Engine::Instance()->GetErrorManager()->SetAndReportError((errCode), (msg));    						\
		return (returnExp);			       		 															\
	}									   		 															\
}

#define ASSERT3(exp, msg, returnExp)             															\
{										   		 															\
	if(!(exp))					     			 															\
	{																										\
		Engine::Instance()->GetErrorManager()->SetAndReportError((int)ErrorCode::GENERAL_FATAL, (msg));  	\
		return (returnExp);			       																	\
	}									   																	\
}

#define GET_ASSERT_RTRN_MACRO(_1,_2,_3, NAME,...) NAME
#define ASSERT_RTRN(...) GET_ASSERT_RTRN_MACRO(__VA_ARGS__, ASSERT_RTRN3, ASSERT_RTRN2)(__VA_ARGS__)

#define ASSERT_RTRN3(exp, msg, errCode)        	 										\
{										   		 										\
	if(!(exp))					     			 										\
	{																					\
		Engine::Instance()->GetErrorManager()->SetAndReportError((errCode), (msg));    	\
		Debug::PrintError((msg));  		   		 										\
		return;			       			 												\
	}									   		 				    					\
}

#define ASSERT_RTRN2(exp, msg)        	         														\
{										   																\
	if(!(exp))					     			 														\
	{									   		 														\
		Engine::Instance()->GetErrorManager()->SetAndReportError((int)ErrorCode::GENERAL_FATAL, (msg)); \
		Debug::PrintError((msg));  		   		 														\
		return;			       		 			 														\
	}									   		 														\
}

typedef unsigned char BYTE;

#endif
