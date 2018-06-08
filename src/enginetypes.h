#ifndef _GTE_ENGINE_TYPES_H_
#define _GTE_ENGINE_TYPES_H_

#include <memory>
#include <functional>

namespace GTE {

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

namespace GTE {
    //forward declarations
    class Shader;
    class Renderer;
    class Mesh3DRenderer;
    class SubMesh3DRenderer;
    class SkinnedMesh3DRenderer;
    class Mesh3D;
    class SubMesh3D;
    class Mesh3DFilter;
    class Camera;
    class SceneObject;
    class Light;
    class EngineObjectManager;
    class Material;
    class MultiMaterial;
    class Texture;
    class Atlas;
    class Transform;
    class Skeleton;
    class Animation;
    class AnimationInstance;
    class AnimationPlayer;
    class RenderTarget;
    class ParticleSystem;
    class ParticleMeshRenderer;

    template <typename T> class EngineObjectSharedPtr : public std::shared_ptr<T> {
        friend class EngineObjectManager;

    public:
        static const EngineObjectSharedPtr<T> NullRef;

        EngineObjectSharedPtr() : std::shared_ptr<T>(nullptr) {}
        EngineObjectSharedPtr(const EngineObjectSharedPtr<T>& ref) : std::shared_ptr<T>(ref) {}
        EngineObjectSharedPtr(const std::shared_ptr<T>& ref) : std::shared_ptr<T>(ref) {}
        EngineObjectSharedPtr(T * ptr, std::function<void(T*)> deleter) : std::shared_ptr<T>(ptr, deleter) {}

        operator std::shared_ptr<T>() const {
            return std::shared_ptr<T>(std::shared_ptr<T>(*this));
        }

        operator EngineObjectSharedPtr<const T>() const {
            return EngineObjectSharedPtr<const T>(std::shared_ptr<T>(*this));
        }

        T* GetPtr() const {
            return std::shared_ptr<T>::get();
        }

        T& GetRef() const {
            return std::shared_ptr<T>::operator*();
        }

        const T* GetConstPtr() const {
            return std::shared_ptr<T>::get();
        }

        const T& GetConstRef() const {
            return std::shared_ptr<T>::operator*();
        }

        Bool operator==(const EngineObjectSharedPtr<T>& other) const {
            return GetConstPtr() == other.GetConstPtr();
        }

        Bool IsValid() const {
            return std::shared_ptr<T>::operator bool();
        }

        void ForceDelete() {
            return std::shared_ptr<T>::reset();
        }

        static EngineObjectSharedPtr<T> Null() {
            return EngineObjectSharedPtr<T>();
        }
    };

    template <typename T> const EngineObjectSharedPtr<T> EngineObjectSharedPtr<T>::NullRef;

    typedef EngineObjectSharedPtr<Shader> ShaderSharedPtr;
    typedef EngineObjectSharedPtr<Renderer> RendererSharedPtr;
    typedef EngineObjectSharedPtr<Mesh3DRenderer> Mesh3DRendererSharedPtr;
    typedef EngineObjectSharedPtr<SubMesh3DRenderer> SubMesh3DRendererSharedPtr;
    typedef EngineObjectSharedPtr<SkinnedMesh3DRenderer> SkinnedMesh3DRendererSharedPtr;
    typedef EngineObjectSharedPtr<Mesh3D> Mesh3DSharedPtr;
    typedef EngineObjectSharedPtr<SubMesh3D> SubMesh3DSharedPtr;
    typedef EngineObjectSharedPtr<Mesh3DFilter> Mesh3DFilterSharedPtr;
    typedef EngineObjectSharedPtr<Camera> CameraSharedPtr;
    typedef EngineObjectSharedPtr<Light> LightSharedPtr;
    typedef EngineObjectSharedPtr<SceneObject> SceneObjectSharedPtr;
    typedef EngineObjectSharedPtr<Material> MaterialSharedPtr;
    typedef EngineObjectSharedPtr<MultiMaterial> MultiMaterialSharedPtr;
    typedef EngineObjectSharedPtr<Texture> TextureSharedPtr;
    typedef EngineObjectSharedPtr<Atlas> AtlasSharedPtr;
    typedef EngineObjectSharedPtr<Skeleton> SkeletonSharedPtr;
    typedef EngineObjectSharedPtr<Animation> AnimationSharedPtr;
    typedef EngineObjectSharedPtr<AnimationInstance> AnimationInstanceSharedPtr;
    typedef EngineObjectSharedPtr<AnimationPlayer> AnimationPlayerSharedPtr;
    typedef EngineObjectSharedPtr<RenderTarget> RenderTargetSharedPtr;
    typedef EngineObjectSharedPtr<ParticleSystem> ParticleSystemSharedPtr;
    typedef EngineObjectSharedPtr<ParticleMeshRenderer> ParticleMeshRendererSharedPtr;

    typedef EngineObjectSharedPtr<const Shader> ShaderSharedConstPtr;
    typedef EngineObjectSharedPtr<const Renderer> RendererSharedConstPtr;
    typedef EngineObjectSharedPtr<const Mesh3DRenderer> Mesh3DRendererSharedConstPtr;
    typedef EngineObjectSharedPtr<const SubMesh3DRenderer> SubMesh3DRendererSharedConstPtr;
    typedef EngineObjectSharedPtr<const SkinnedMesh3DRenderer> SkinnedMesh3DRendererSharedConstPtr;
    typedef EngineObjectSharedPtr<const Mesh3D> Mesh3DSharedConstPtr;
    typedef EngineObjectSharedPtr<const SubMesh3D> SubMesh3DSharedConstPtr;
    typedef EngineObjectSharedPtr<const Mesh3DFilter> Mesh3DFilterSharedConstPtr;
    typedef EngineObjectSharedPtr<const Camera> CameraSharedConstPtr;
    typedef EngineObjectSharedPtr<const Light> LightSharedConstPtr;
    typedef EngineObjectSharedPtr<const SceneObject> SceneObjectSharedConstPtr;
    typedef EngineObjectSharedPtr<const Material> MaterialSharedConstPtr;
    typedef EngineObjectSharedPtr<const MultiMaterial> MultiMaterialSharedConstPtr;
    typedef EngineObjectSharedPtr<const Texture> TextureSharedConstPtr;
    typedef EngineObjectSharedPtr<const Atlas> AtlasSharedConstPtr;
    typedef EngineObjectSharedPtr<const Skeleton> SkeletonSharedConstPtr;
    typedef EngineObjectSharedPtr<const Animation> AnimationSharedConstPtr;
    typedef EngineObjectSharedPtr<const AnimationInstance> AnimationInstanceSharedConstPtr;
    typedef EngineObjectSharedPtr<const AnimationPlayer> AnimationPlayerSharedConstPtr;
    typedef EngineObjectSharedPtr<const RenderTarget> RenderTargetSharedConstPtr;
    typedef EngineObjectSharedPtr<const ParticleSystem> ParticleSystemSharedConstPtr;
    typedef EngineObjectSharedPtr<const ParticleMeshRenderer> ParticleMeshRendererSharedConstPtr;

    typedef const ShaderSharedPtr& ShaderRef;
    typedef const RendererSharedPtr& RendererRef;
    typedef const Mesh3DRendererSharedPtr& Mesh3DRendererRef;
    typedef const SubMesh3DSharedPtr& SubMesh3DRef;
    typedef const SkinnedMesh3DRendererSharedPtr& SkinnedMesh3DRendererRef;
    typedef const Mesh3DSharedPtr& Mesh3DRef;
    typedef const SubMesh3DRendererSharedPtr& SubMesh3DRendererRef;
    typedef const Mesh3DFilterSharedPtr& Mesh3DFilterRef;
    typedef const CameraSharedPtr& CameraRef;
    typedef const LightSharedPtr& LightRef;
    typedef const SceneObjectSharedPtr& SceneObjectRef;
    typedef const MaterialSharedPtr& MaterialRef;
    typedef const MultiMaterialSharedPtr& MultiMaterialRef;
    typedef const TextureSharedPtr& TextureRef;
    typedef const AtlasSharedPtr& AtlasRef;
    typedef const SkeletonSharedPtr& SkeletonRef;
    typedef const AnimationSharedPtr& AnimationRef;
    typedef const AnimationInstanceSharedPtr& AnimationInstanceRef;
    typedef const AnimationPlayerSharedPtr& AnimationPlayerRef;
    typedef const RenderTargetSharedPtr& RenderTargetRef;
    typedef const ParticleSystemSharedPtr& ParticleSystemRef;
    typedef const ParticleMeshRendererSharedPtr& ParticleMeshRendererRef;

    extern const ShaderSharedPtr& NullShaderRef;
    extern const RendererSharedPtr& NullRendererRef;
    extern const Mesh3DRendererSharedPtr& NullMesh3DRendererRef;
    extern const SubMesh3DRendererSharedPtr& NullSubMesh3DRendererRef;
    extern const SkinnedMesh3DRendererSharedPtr& NullSkinnedMesh3DRendererRef;
    extern const Mesh3DSharedPtr& NullMesh3DRef;
    extern const SubMesh3DSharedPtr& NullSubMesh3DRef;
    extern const Mesh3DFilterSharedPtr& NullMesh3DFilterRef;
    extern const CameraSharedPtr& NullCameraRef;
    extern const LightSharedPtr& NullLightRef;
    extern const SceneObjectSharedPtr& NullSceneObjectRef;
    extern const MaterialSharedPtr& NullMaterialRef;
    extern const MultiMaterialSharedPtr& NullMultiMaterialRef;
    extern const TextureSharedPtr& NullTextureRef;
    extern const AtlasSharedPtr& NullAtlasRef;
    extern const SkeletonSharedPtr& NullSkeletonRef;
    extern const AnimationSharedPtr& NullAnimationRef;
    extern const AnimationInstanceSharedPtr& NullAnimationInstanceRef;
    extern const AnimationPlayerSharedPtr& NullAnimationPlayerRef;
    extern const RenderTargetSharedPtr& NullRenderTargetRef;
    extern const ParticleSystemSharedPtr& NullParticleSystemRef;
    extern const ParticleMeshRendererSharedPtr& NullParticleMeshRendererRef;

    typedef const ShaderSharedConstPtr& ShaderConstRef;
    typedef const RendererSharedConstPtr& RendererConstRef;
    typedef const Mesh3DRendererSharedConstPtr& Mesh3DRendererConstRef;
    typedef const SubMesh3DRendererSharedConstPtr& SubMesh3DRendererConstRef;
    typedef const SkinnedMesh3DRendererSharedConstPtr& SkinnedMesh3DRendererConstRef;
    typedef const Mesh3DSharedConstPtr& Mesh3DConstRef;
    typedef const SubMesh3DSharedConstPtr& SubMesh3DConstRef;
    typedef const Mesh3DFilterSharedConstPtr& Mesh3DFilterConstRef;
    typedef const CameraSharedConstPtr& CameraConstRef;
    typedef const LightSharedConstPtr& LightConstRef;
    typedef const SceneObjectSharedConstPtr& SceneObjectConstRef;
    typedef const MaterialSharedConstPtr& MaterialConstRef;
    typedef const MultiMaterialSharedConstPtr& MultiMaterialConstRef;
    typedef const TextureSharedConstPtr& TextureConstRef;
    typedef const AtlasSharedConstPtr& AtlasConstRef;
    typedef const SkeletonSharedConstPtr& SkeletonConstRef;
    typedef const AnimationSharedConstPtr& AnimationConstRef;
    typedef const AnimationInstanceSharedConstPtr& AnimationInstanceConstRef;
    typedef const AnimationPlayerSharedConstPtr& AnimationPlayerConstRef;
    typedef const RenderTargetSharedConstPtr& RenderTargetConstRef;
    typedef const ParticleSystemSharedConstPtr& ParticleSystemConstRef;
    typedef const ParticleMeshRendererSharedConstPtr& ParticleMeshRendererConstRef;

    template<typename T, typename U> EngineObjectSharedPtr<U> DynamicCastEngineObject(const EngineObjectSharedPtr<T>& src) {
        const std::shared_ptr<T>& s1 = src;
        const std::shared_ptr<U>& s2 = std::dynamic_pointer_cast<U>(s1);

        return EngineObjectSharedPtr<U>(s2);
    }
}

#endif
