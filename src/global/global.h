#ifndef _GTE_GLOBAL_H_
#define _GTE_GLOBAL_H_

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
