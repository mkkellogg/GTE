#ifndef _GTE_ENGINE_TYPES_H_
#define _GTE_ENGINE_TYPES_H_

#include <memory>
#include <functional>

namespace GTE
{

//#define _GTE_Real_DoublePrecision

#ifdef _WIN32

	typedef bool Bool;

	typedef unsigned char Byte;
	typedef unsigned char UChar;
	typedef char Char;

	typedef double RealDouble;

#ifdef _GTE_Real_DoublePrecision
	typedef double Real;
#else
	typedef float Real;
#endif

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

#ifdef _GTE_Real_DoublePrecision
	typedef double Real;
#else
	typedef float Real;
#endif

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
	class Atlas;
	class Transform;
	class Skeleton;
	class Animation;
	class AnimationInstance;
	class AnimationPlayer;
	class RenderTarget;

	template <typename T> class EngineObjectSharedPtr : public std::shared_ptr<T>
	{
		friend class EngineObjectManager;

	public:
		static EngineObjectSharedPtr<T> NullRef;
		
		EngineObjectSharedPtr() : std::shared_ptr<T>(nullptr) {}
		EngineObjectSharedPtr(const EngineObjectSharedPtr<T>& ref) : std::shared_ptr<T>(ref){}
		EngineObjectSharedPtr(const std::shared_ptr<T>& ref) : std::shared_ptr<T>(ref){}
		EngineObjectSharedPtr(T * ptr, std::function<void(T*)> deleter) : std::shared_ptr<T>(ptr, deleter){}

		operator EngineObjectSharedPtr<const T>() const
		{ 
			return EngineObjectSharedPtr<const T>(std::shared_ptr<T>(*this));
		}

		T* GetPtr()
		{
			return std::shared_ptr<T>::get();
		}

		T& GetRef()
		{
			return std::shared_ptr<T>::operator*();
		}

		const T* GetConstPtr() const
		{
			return std::shared_ptr<T>::get();
		}

		const T& GetConstRef() const
		{
			return std::shared_ptr<T>::operator*();
		}

		Bool operator==(const EngineObjectSharedPtr<T>& other) const
		{
			return GetConstPtr() == other.GetConstPtr();
		}

		Bool IsValid() const
		{
			return std::shared_ptr<T>::operator bool();
		}

		void ForceDelete()
		{
			return std::shared_ptr<T>::reset();
		}

		static EngineObjectSharedPtr<T> Null()
		{
			return EngineObjectSharedPtr<T>();
		}
	};

	template <typename T> EngineObjectSharedPtr<T> EngineObjectSharedPtr<T>::NullRef;

	typedef EngineObjectSharedPtr<Shader> ShaderSharedPtr;
	typedef EngineObjectSharedPtr<SubMesh3DRenderer> SubMesh3DRendererSharedPtr;
	typedef EngineObjectSharedPtr<SubMesh3D> SubMesh3DSharedPtr;
	typedef EngineObjectSharedPtr<Mesh3DRenderer> Mesh3DRendererSharedPtr;
	typedef EngineObjectSharedPtr<SkinnedMesh3DRenderer> SkinnedMesh3DRendererSharedPtr;
	typedef EngineObjectSharedPtr<Mesh3D> Mesh3DSharedPtr;
	typedef EngineObjectSharedPtr<Mesh3DFilter> Mesh3DFilterSharedPtr;
	typedef EngineObjectSharedPtr<Camera> CameraSharedPtr;
	typedef EngineObjectSharedPtr<Light> LightSharedPtr;
	typedef EngineObjectSharedPtr<SceneObject> SceneObjectSharedPtr;
	typedef EngineObjectSharedPtr<Material> MaterialSharedPtr;
	typedef EngineObjectSharedPtr<Texture> TextureSharedPtr;
	typedef EngineObjectSharedPtr<Atlas> AtlasSharedPtr;
	typedef EngineObjectSharedPtr<Skeleton> SkeletonSharedPtr;
	typedef EngineObjectSharedPtr<Animation> AnimationSharedPtr;
	typedef EngineObjectSharedPtr<AnimationInstance> AnimationInstanceSharedPtr;
	typedef EngineObjectSharedPtr<AnimationPlayer> AnimationPlayerSharedPtr;
	typedef EngineObjectSharedPtr<RenderTarget> RenderTargetSharedPtr;

	typedef EngineObjectSharedPtr<const Shader> ShaderSharedConstPtr;
	typedef EngineObjectSharedPtr<const SubMesh3DRenderer> SubMesh3DRendererSharedConstPtr;
	typedef EngineObjectSharedPtr<const SubMesh3D> SubMesh3DSharedConstPtr;
	typedef EngineObjectSharedPtr<const Mesh3DRenderer> Mesh3DRendererSharedConstPtr;
	typedef EngineObjectSharedPtr<const SkinnedMesh3DRenderer> SkinnedMesh3DRendererSharedConstPtr;
	typedef EngineObjectSharedPtr<const Mesh3D> Mesh3DSharedConstPtr;
	typedef EngineObjectSharedPtr<const Mesh3DFilter> Mesh3DFilterSharedConstPtr;
	typedef EngineObjectSharedPtr<const Camera> CameraSharedConstPtr;
	typedef EngineObjectSharedPtr<const Light> LightSharedConstPtr;
	typedef EngineObjectSharedPtr<const SceneObject> SceneObjectSharedConstPtr;
	typedef EngineObjectSharedPtr<const Material> MaterialSharedConstPtr;
	typedef EngineObjectSharedPtr<const Texture> TextureSharedConstPtr;
	typedef EngineObjectSharedPtr<const Atlas> AtlasSharedConstPtr;
	typedef EngineObjectSharedPtr<const Skeleton> SkeletonSharedConstPtr;
	typedef EngineObjectSharedPtr<const Animation> AnimationSharedConstPtr;
	typedef EngineObjectSharedPtr<const AnimationInstance> AnimationInstanceSharedConstPtr;
	typedef EngineObjectSharedPtr<const AnimationPlayer> AnimationPlayerSharedConstPtr;
	typedef EngineObjectSharedPtr<const RenderTarget> RenderTargetSharedConstPtr;
}


#endif
