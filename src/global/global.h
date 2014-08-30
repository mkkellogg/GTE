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

#endif
