#ifndef _ENGINE_TYPES_H_
#define _ENGINE_TYPES_H_

//forward declarations
class SubMesh3DRenderer;
class SubMesh3D;
class Mesh3DRenderer;
class Mesh3D;
class Camera;
class SceneObject;
class Light;

#include <memory>

template <typename T> class EngineObjectRef
{
	std::shared_ptr<T> sharedPtr;
	std::function<void(T*)> deleter;

	public:

	EngineObjectRef() : sharedPtr(NULL)
	{

	}

	EngineObjectRef(T * ptr) : sharedPtr(ptr, [=](T*p){DoDelete(p);})
	{

	}

	EngineObjectRef(const EngineObjectRef<T>& ref) : sharedPtr(ref.sharedPtr)
	{

	}

	EngineObjectRef(const std::shared_ptr<T>& ref) : sharedPtr(ref)
	{

	}

	EngineObjectRef( std::shared_ptr<T>& ref) : sharedPtr(ref)
	{

	}

	EngineObjectRef(T * ptr, std::function<void(T*)> deleter) : sharedPtr(ptr, [=](T*p){DoDelete(p);})
	{
		this->deleter = deleter;
	}

	void DoDelete(T* p)
	{
		if(deleter)
		{
			deleter(p);
		}
		else
		{
			EngineObjectManager * objectManager = EngineObjectManager::Instance();

		}
	}

	EngineObjectRef<T>& operator=(const EngineObjectRef<T>& other)
	{
		if(&other == this)return *this;
		sharedPtr = other.sharedPtr;
		return *this;
	}

	T& operator*()
	{
	    return sharedPtr.operator*();
	}

	const T& operator*() const
	{
	    return sharedPtr.operator*();
	}

	T* operator->()
	{
	    return sharedPtr.operator->();
	}

	const T* operator->() const
	{
	    return sharedPtr.operator->();
	}

	T* GetPtr()
	{
		return sharedPtr.get();
	}

	T& GetRef()
	{
		return *sharedPtr;
	}

	bool IsValid()
	{
		if(sharedPtr)return true;
		return false;
	}
};

//typedef std::shared_ptr<Light> LightRef;

typedef EngineObjectRef<Light> LightRef;
typedef EngineObjectRef<SceneObject> SceneObjectRef;

#endif
