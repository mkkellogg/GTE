#ifndef _GLOBAL_H_
#define _GLOBAL_H_

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

#endif
