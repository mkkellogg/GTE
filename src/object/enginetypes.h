#ifndef _GTE_ENGINE_TYPES_H_
#define _GTE_ENGINE_TYPES_H_

#include <memory>
#include <functional>

namespace GTE
{
#ifdef _WIN32

	typedef bool Bool;

	typedef unsigned char Byte;
	typedef unsigned char UChar;
	typedef char Char;

	typedef double RealDouble;
	typedef float Real;

	typedef __int8 Int8;
	typedef unsigned __int8 UInt8;

	typedef __int16 Int16;
	typedef unsigned __int16 UInt16;

	typedef __int32 Int32;
	typedef unsigned __int32 UInt32;

	typedef __int64 Int64;
	typedef unsigned __int64 UInt64;

#else

	typedef bool Bool;

	typedef unsigned char Byte;
	typedef unsigned char UChar;
	typedef char Char;

	typedef double RealDouble;
	typedef float Real;

	typedef char Int8;
	typedef unsigned char UInt8;

	typedef short Int16;
	typedef unsigned short UInt16;

	typedef int Int32;
	typedef unsigned int UInt32;

	typedef long long Int64;
	typedef unsigned long long UInt64;

#endif

}

namespace GTE
{
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

	template <typename T> class EngineObjectRef : public std::shared_ptr<T>
	{
		friend class EngineObjectManager;

	public:

		EngineObjectRef() : shared_ptr(NULL) {}
		EngineObjectRef(const EngineObjectRef<T>& ref) : shared_ptr(ref){}
		EngineObjectRef(const std::shared_ptr<T>& ref) : shared_ptr(ref){}
		EngineObjectRef(T * ptr, std::function<void(T*)> deleter) : shared_ptr(ptr, deleter){}

		operator EngineObjectRef<const T>() const 
		{ 
			return EngineObjectRef<const T>(std::shared_ptr<T>(*this));
		}

		T* GetPtr()
		{
			return get();
		}

		T& GetRef()
		{
			return std::shared_ptr<T>::operator*();
		}

		const T* GetConstPtr() const
		{
			return get();
		}

		const T& GetConstRef() const
		{
			return std::shared_ptr<T>::operator*();
		}

		Bool operator==(const EngineObjectRef<T>& other) const
		{
			return GetConstPtr() == other.GetConstPtr();
		}

		Bool IsValid() const
		{
			return std::shared_ptr<T>::operator bool();
		}

		void ForceDelete()
		{
			reset();
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

	typedef EngineObjectRef<const Shader> ShaderRefConst;
	typedef EngineObjectRef<const SubMesh3DRenderer> SubMesh3DRendererRefConst;
	typedef EngineObjectRef<const SubMesh3D> SubMesh3DRefConst;
	typedef EngineObjectRef<const Mesh3DRenderer> Mesh3DRendererRefConst;
	typedef EngineObjectRef<const SkinnedMesh3DRenderer> SkinnedMesh3DRendererRefConst;
	typedef EngineObjectRef<const Mesh3D> Mesh3DRefConst;
	typedef EngineObjectRef<const Mesh3DFilter> Mesh3DFilterRefConst;
	typedef EngineObjectRef<const Camera> CameraRefConst;
	typedef EngineObjectRef<const Light> LightRefConst;
	typedef EngineObjectRef<const SceneObject> SceneObjectRefConst;
	typedef EngineObjectRef<const Material> MaterialRefConst;
	typedef EngineObjectRef<const Texture> TextureRefConst;
	typedef EngineObjectRef<const Shader> ShaderRefConst;
	typedef EngineObjectRef<const Skeleton> SkeletonRefConst;
	typedef EngineObjectRef<const Animation> AnimationRefConst;
	typedef EngineObjectRef<const AnimationInstance> AnimationInstanceRefConst;
	typedef EngineObjectRef<const AnimationPlayer> AnimationPlayerRefConst;
	typedef EngineObjectRef<const RenderTarget> RenderTargetRefConst;

	typedef std::shared_ptr<Transform> TransformRef;
}


#endif
