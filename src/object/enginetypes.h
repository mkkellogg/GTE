#ifndef _GTE_ENGINE_TYPES_H_
#define _GTE_ENGINE_TYPES_H_

//forward declarations
class Shader;
class SubMesh3DRenderer;
class SubMesh3D;
class Mesh3DRenderer;
class SkinnedMesh3DRenderer;
class Mesh3D;
class Mesh3DFilter;
class Camera;
class SceneObject;
class Light;
class EngineObjectManager;
class Material;
class Texture;
class Transform;
class Skeleton;
class Animation;
class AnimationInstance;
class AnimationPlayer;
class RenderTarget;

#include <memory>
#include <functional>

template <typename T> class EngineObjectRef
{
	friend class EngineObjectManager;

	std::shared_ptr<T> sharedPtr;

	/*void InvokeDeleter(T* p)
	{
		printf("deleting...\n");
		if(deleter)
		{
			printf("Invoking deleter...\n");
			deleter(p);
		}
		printf("delete complete!\n");
	}*/

	public:

	EngineObjectRef() : sharedPtr(NULL) {}
	EngineObjectRef(const EngineObjectRef<T>& ref) : sharedPtr(ref.sharedPtr){}
	EngineObjectRef(EngineObjectRef<T>& ref) : sharedPtr(ref.sharedPtr){}
	//EngineObjectRef(const std::shared_ptr<T>& ref) : sharedPtr(ref) {}
	//EngineObjectRef( std::shared_ptr<T>& ref) : sharedPtr(ref) {}
	EngineObjectRef(T * ptr, std::function<void(T*)> deleter) : sharedPtr(ptr,/*[this](T*p){InvokeDeleter(p);}*/ deleter)
	{
		//this->deleter = deleter;
	}

	EngineObjectRef<T>& operator=(const EngineObjectRef<T>& other)
	{
		if(&other == this)return *this;
		sharedPtr = other.sharedPtr;
		return *this;
	}

	EngineObjectRef<T>& operator=(EngineObjectRef<T>& other)
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

	bool operator==(const EngineObjectRef<T> & other) const
	{
		return sharedPtr == other.sharedPtr;
	}

	bool IsValid() const
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

typedef EngineObjectRef<Shader> ShaderRef;
typedef EngineObjectRef<SubMesh3DRenderer> SubMesh3DRendererRef;
typedef EngineObjectRef<SubMesh3D> SubMesh3DRef;
typedef EngineObjectRef<Mesh3DRenderer> Mesh3DRendererRef;
typedef EngineObjectRef<SkinnedMesh3DRenderer> SkinnedMesh3DRendererRef;
typedef EngineObjectRef<Mesh3D> Mesh3DRef;
typedef EngineObjectRef<Mesh3DFilter> Mesh3DFilterRef;
typedef EngineObjectRef<Camera> CameraRef;
typedef EngineObjectRef<Light> LightRef;
typedef EngineObjectRef<SceneObject> SceneObjectRef;
typedef EngineObjectRef<Material> MaterialRef;
typedef EngineObjectRef<Texture> TextureRef;
typedef EngineObjectRef<Shader> ShaderRef;
typedef EngineObjectRef<Skeleton> SkeletonRef;
typedef EngineObjectRef<Animation> AnimationRef;
typedef EngineObjectRef<AnimationInstance> AnimationInstanceRef;
typedef EngineObjectRef<AnimationPlayer> AnimationPlayerRef;
typedef EngineObjectRef<RenderTarget> RenderTargetRef;

typedef std::shared_ptr<Transform> TransformRef;

#endif
