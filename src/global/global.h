#ifndef _GTE_GLOBAL_H_
#define _GTE_GLOBAL_H_

#include "error/errormanager.h"
#include "engine.h"
#include "debug/gtedebug.h"

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

#define ASSERT(exp, msg)    				\
{										   	\
	if(!(exp))					     		\
	{										\
		Debug::PrintError((msg));			\
		exit(-1);							\
	}									   	\
}

#define GET_NONFATAL_ASSERT_RTRN_MACRO(_1,_2,_3,_4,_5, NAME,...) NAME
#define NONFATAL_ASSERT_RTRN(...) GET_NONFATAL_ASSERT_RTRN_MACRO(__VA_ARGS__, NONFATAL_ASSERT_RTRN5, NONFATAL_ASSERT_RTRN4, NONFATAL_ASSERT_RTRN3, NONFATAL_ASSERT_RTRN2)(__VA_ARGS__)

#define NONFATAL_ASSERT_RTRN5(exp, msg, errCode, returnExp, reset)    										\
{										   																	\
	if(!(exp))					     			 															\
	{																										\
		Engine::Instance()->GetErrorManager()->SetError((errCode), (msg));    								\
		Debug::PrintAtLevel((msg), DebugLevel::Error);														\
		return (returnExp);			       		 															\
	}									   		 															\
}

#define NONFATAL_ASSERT_RTRN4(exp, msg, returnExp, reset)             										\
{										   		 															\
	if(!(exp))					     			 															\
	{																										\
		Engine::Instance()->GetErrorManager()->SetError((int)ErrorCode::GENERAL_NONFATAL, (msg));  			\
		Debug::PrintAtLevel((msg), DebugLevel::Error);														\
		return (returnExp);			       																	\
	}									   																	\
}

#define GET_NONFATAL_ASSERT_MACRO(_1,_2,_3,_4, NAME,...) NAME
#define NONFATAL_ASSERT(...) GET_NONFATAL_ASSERT_MACRO(__VA_ARGS__, NONFATAL_ASSERT4, NONFATAL_ASSERT3, NONFATAL_ASSERT2)(__VA_ARGS__)

#define NONFATAL_ASSERT4(exp, msg, errCode, reset)        	 							\
{										   		 										\
	if(!(exp))					     			 										\
	{																					\
		Engine::Instance()->GetErrorManager()->SetError((errCode), (msg));    			\
		Debug::PrintAtLevel((msg), DebugLevel::Error);									\
		return;			       			 												\
	}									   		 				    					\
}

#define NONFATAL_ASSERT3(exp, msg, reset)        	         											\
{										   																\
	if(!(exp))					     			 														\
	{									   		 														\
		Engine::Instance()->GetErrorManager()->SetError((int)ErrorCode::GENERAL_NONFATAL, (msg)); 		\
		Debug::PrintAtLevel((msg), DebugLevel::Error);													\
		return;			       		 			 														\
	}									   		 														\
}

typedef unsigned char BYTE;

#endif
