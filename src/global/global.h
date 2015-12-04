#ifndef _GTE_GLOBAL_H_
#define _GTE_GLOBAL_H_

#define SAFE_DELETE(ptr)                   \
{                                          \
	if((ptr) != nullptr){                  \
	delete (ptr);					       \
	(ptr) = nullptr;					   \
	}									   \
	else                                   \
	{                                      \
	}                                      \
}

#define SAFE_DELETE_ARRAY(ptr)             \
{                                          \
	if((ptr) != nullptr){                  \
	delete[] (ptr);					       \
	(ptr) = nullptr;					   \
	}									   \
	else                                   \
	{                                      \
	}                                      \
}


#endif
