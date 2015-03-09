#ifndef _GTE_CAMERA_H_
#define _GTE_CAMERA_H_

//forward declarations
class Camera;
class Graphics;

#include "geometry/transform.h"
#include "object/sceneobjectcomponent.h"
#include "graphics/render/rendertarget.h"
#include "graphics/graphics.h"
#include "object/enginetypes.h"


class Camera : public SceneObjectComponent
{
	// Since this ultimately derives from EngineObject, we make this class
	// a friend of EngineObjectManager, and the constructor & destructor
	// protected so its life-cycle can be handled completely by EngineObjectManager.
	friend class EngineObjectManager;

    unsigned int clearBufferMask;

    Transform projection;

    bool skyboxSetup;
    bool skyboxEnabled;
    MaterialRef skyboxMaterial;
    TextureRef skyboxTexture;
    SceneObjectRef skyboxSceneObject;
    Mesh3DRef skyboxMesh;
    Mesh3DFilterRef skyboxMeshFilter;
    Mesh3DRendererRef skyboxMeshRenderer;

    SSAORenderMode ssaoMode;
    bool ssaoEnabled;

    unsigned int renderOrderIndex;

    protected:

    Camera();
    ~Camera();

    public:

    void SetSkybox(TextureRef cubeTexture);
    bool IsSkyboxSetup() const;
    void SetSkyboxEnabled(bool enabled);
    bool IsSkyboxEnabled() const;
    SceneObjectRef GetSkyboxSceneObject();

    void SetSSAOEnabled(bool enabled);
    bool IsSSAOEnabled();
    void SetSSAORenderMode(SSAORenderMode mode);
    SSAORenderMode GetSSAORenderMode();

    void SetRendeOrderIndex(unsigned int index);
    unsigned int GetRendeOrderIndex();

    const Transform& GetProjectionTransform() const ;
    void AddClearBuffer(RenderBufferType buffer);
    void RemoveClearBuffer(RenderBufferType buffer);
    IntMask GetClearBufferMask() const;
    void UpdateDisplay();
};

#endif
