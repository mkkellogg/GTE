#ifndef _ENGINE_TYPES_H_
#define _ENGINE_TYPES_H_

//forward declarations
class Shader;
class SubMesh3DRenderer;
class SubMesh3D;
class Mesh3DRenderer;
class Mesh3D;
class Camera;
class SceneObject;
class Light;
class EngineObjectManager;
class Material;
class Texture;

#include <memory>

template <typename T> class EngineObjectRef
{
	friend class EngineObjectManager;

	std::shared_ptr<T> sharedPtr;
	std::function<void(T*)> deleter;

	void InvokeDeleter(T* p)
	{
		if(deleter)deleter(p);
	}

	public:

	EngineObjectRef() : sharedPtr(NULL) {}
	EngineObjectRef(const EngineObjectRef<T>& ref) : sharedPtr(ref.sharedPtr) {}
	EngineObjectRef(const std::shared_ptr<T>& ref) : sharedPtr(ref) {}
	EngineObjectRef( std::shared_ptr<T>& ref) : sharedPtr(ref) {}
	EngineObjectRef(T * ptr, std::function<void(T*)> deleter) : sharedPtr(ptr, [=](T*p){InvokeDeleter(p);})
	{
		this->deleter = deleter;
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

	bool operator==(const EngineObjectRef<T> & other)
	{
		return sharedPtr == other.sharedPtr;
	}

	bool IsValid()
	{
		if(sharedPtr)return true;
		return false;
	}

	void ForceDelete()
	{
		sharedPtr.reset();
	}

	static EngineObjectRef<T> Null()
	{
		return EngineObjectRef<T>();
	}
};

//typedef std::shared_ptr<Light> LightRef;

typedef EngineObjectRef<Shader> ShaderRef;
typedef EngineObjectRef<SubMesh3DRenderer> SubMesh3DRendererRef;
typedef EngineObjectRef<SubMesh3D> SubMesh3DRef;
typedef EngineObjectRef<Mesh3DRenderer> Mesh3DRendererRef;
typedef EngineObjectRef<Mesh3D> Mesh3DRef;
typedef EngineObjectRef<Camera> CameraRef;
typedef EngineObjectRef<Light> LightRef;
typedef EngineObjectRef<SceneObject> SceneObjectRef;
typedef EngineObjectRef<Material> MaterialRef;
typedef EngineObjectRef<Texture> TextureRef;
typedef EngineObjectRef<Shader> ShaderRef;

#endif
