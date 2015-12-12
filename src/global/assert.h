#ifndef _GTE_ASSERT_H_
#define _GTE_ASSERT_H_

#include "error/errormanager.h"
#include "debug/gtedebug.h"

#include <stdlib.h>

#define ASSERT(exp, msg)    				        \
{										   	        \
	if(!(exp))					     		        \
	{										        \
		::GTE::Debug::PrintError((msg));			\
		exit(-1);							        \
	}									   	        \
    else                                            \
    {                                               \
    }                                               \
}

#define EXPAND(X) X    // for MSVC10 compatibility

// compute number of (variadic) macro arguments
// from http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?pli=1
#define PP_NARG(...) EXPAND( PP_NARG_(__VA_ARGS__, PP_RSEQ_N()) )
#define PP_NARG_(...) EXPAND( PP_ARG_N(__VA_ARGS__) )
#define PP_ARG_N(_1, _2, _3, _4, _5, N, ...) N
#define PP_RSEQ_N() 5, 4, 3, 2, 1, 0




// macro selection by number of arguments
#define NONFATAL_ASSERT_RTRN_(N) NONFATAL_ASSERT_RTRN_##N
#define NONFATAL_ASSERT_RTRN_EVAL(N) NONFATAL_ASSERT_RTRN_(N)
#define NONFATAL_ASSERT_RTRN(...) EXPAND( NONFATAL_ASSERT_RTRN_EVAL(EXPAND( PP_NARG(__VA_ARGS__) ))(__VA_ARGS__) )

#define NONFATAL_ASSERT_RTRN_5(exp, msg, errCode, returnExp, reset)    										\
{										   																	\
	if(!(exp))					     			 															\
	{																										\
		::GTE::Engine::Instance()->GetErrorManager()->SetError((errCode), (msg));    						\
		::GTE::Debug::PrintAtLevel((msg), ::GTE::DebugLevel::Error);										\
		return (returnExp);			       		 															\
	}									   		 															\
}

#define NONFATAL_ASSERT_RTRN_4(exp, msg, returnExp, reset)             												\
{										   		 																	\
	if(!(exp))					     			 																	\
	{																												\
		::GTE::Engine::Instance()->GetErrorManager()->SetError((Int32)::GTE::ErrorCode::GENERAL_NONFATAL, (msg));     \
		::GTE::Debug::PrintAtLevel((msg), DebugLevel::Error);														\
		return (returnExp);			       																			\
	}									   																			\
	else																	                                		\
	{																								        		\
	}																												\
}




// macro selection by number of arguments
#define NONFATAL_ASSERT_(N)  NONFATAL_ASSERT_##N
#define NONFATAL_ASSERT_EVAL(N) NONFATAL_ASSERT_(N)
#define NONFATAL_ASSERT(...) EXPAND( NONFATAL_ASSERT_EVAL(EXPAND( PP_NARG(__VA_ARGS__) ))(__VA_ARGS__) )

#define NONFATAL_ASSERT_4(exp, msg, errCode, reset)        	 							\
{										   		 										\
	if(!(exp))					     			 										\
	{																					\
		::GTE::Engine::Instance()->GetErrorManager()->SetError((errCode), (msg));    	\
		::GTE::Debug::PrintAtLevel((msg), ::GTE::DebugLevel::Error);					\
		return;			       			 												\
	}									   		 				    					\
	else																	            \
	{																					\
	}																					\
}

#define NONFATAL_ASSERT_3(exp, msg, reset)        	         														\
{										   																			\
	if(!(exp))					     			 																	\
	{									   		 																	\
		::GTE::Engine::Instance()->GetErrorManager()->SetError((Int32)::GTE::ErrorCode::GENERAL_NONFATAL, (msg)); 	\
		::GTE::Debug::PrintAtLevel((msg), ::GTE::DebugLevel::Error);												\
		return;			       		 			 																	\
	}									   		 																	\
	else																											\
	{																												\
	}																												\
}

//#define _GTE_Real_DoublePrecision

#endif
