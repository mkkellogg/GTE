/*
 * class: BaseVector4
 *
 * author: Mark Kellogg
 *
 * Forms the basis of several vertex attribute classes (Point3, Vector3, Color4, ...) and contains
 * functionality that is shared by all of them.
 */

#ifndef _GTE_BASEVECTOR4_H_
#define _GTE_BASEVECTOR4_H_

#define BaseVector4_QuickCopy(source, dest)        \
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=3;					       		   	   \
  (dest)-=3;							   		   \
}

#define BaseVector4_QuickCopy_IncDest(source, dest)\
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=3;					       		   	   \
  (dest)++;							   		   	   \
}

#define BaseVector4_QuickCopy_ZeroW(source, dest)  \
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=2;					       		   	   \
  (dest)++;					   		   		       \
  *(dest) = 0;                      	   		   \
  (dest)-=3;						   		   	   \
}

#define BaseVector4_QuickCopy_ZeroW_IncDest(source, dest)\
{                                          		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;						   		   		   \
  *(dest) = *(source);                      	   \
  (source)++;					       		   	   \
  (dest)++;							   		   	   \
  *(dest) = *(source);                      	   \
  (source)-=2;					       		   	   \
  (dest)++;					   		   		       \
  *(dest) = 0;                      	   		   \
  (dest)++;					   		   		       \
}

namespace GTE
{
	class BaseVector4
	{
		friend class BaseVector4Array;

	protected:

		void Init(float *target, bool permAttach);

		float * data;
		float baseData[4];
		bool attached;
		bool canDetach;

	public:

		BaseVector4();
		BaseVector4(bool permAttached, float * target);
		BaseVector4(float x, float y, float z, float w);
		BaseVector4(const BaseVector4& baseVector);
		BaseVector4(const float * copyData);
		virtual ~BaseVector4();

		virtual BaseVector4& operator=(const BaseVector4& source);

		float * GetDataPtr();
		void Set(float x, float y, float z, float w);
		void SetTo(const BaseVector4& baseVector);
		void Get(BaseVector4& baseVector) const;

		virtual void AttachTo(float * data);
		virtual void Detach();
	};
}

#endif
