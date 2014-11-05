#ifndef _GLOBAL_H_
#define _GLOBAL_H_

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

#define NULL_CHECK_RTRN(ptr, msg)    			\
{										   		\
	if((ptr) == NULL)					   		\
	{									   		\
		Debug::PrintError((msg));  		   		\
		return;			       					\
	}									   		\
}

#define NULL_CHECK(ptr, msg, returnExp)    \
{										   \
	if((ptr) == NULL)					   \
	{									   \
		Debug::PrintError((msg));  		   \
		return (returnExp);			       \
	}									   \
}

#define SHARED_REF_CHECK_RTRN(ref, msg)    		\
{										   		\
	if(!(ref).IsValid())					    \
	{									   		\
		Debug::PrintError((msg));  		   		\
		return;			       					\
	}									   		\
}

#define SHARED_REF_CHECK(ref, msg, returnExp)    \
{										   		 \
	if(!(ref).IsValid())					     \
	{									   		 \
		Debug::PrintError((msg));  		   		 \
		return (returnExp);			       		 \
	}									   		 \
}

#define ASSERT(exp, msg, returnExp)          \
{										   		 \
	if(!(exp))					     			 \
	{									   		 \
		Debug::PrintError((msg));  		   		 \
		return (returnExp);			       		 \
	}									   		 \
}

#define ASSERT_RTRN(exp, msg)        	     \
{										   		 \
	if(!(exp))					     			 \
	{									   		 \
		Debug::PrintError((msg));  		   		 \
		return;			       		 			 \
	}									   		 \
}

typedef unsigned char BYTE;

#endif
