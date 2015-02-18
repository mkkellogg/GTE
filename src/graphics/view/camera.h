#ifndef _GTE_CAMERA_H_
#define _GTE_CAMERA_H_

//forward declarations
class Camera;
class Graphics;

#include "geometry/transform.h"
#include "object/sceneobjectcomponent.h"
#include "graphics/render/rendertarget.h"
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
    MaterialRef skyboxMaterial;
    TextureRef skyboxTexture;
    SceneObjectRef skyboxSceneObject;
    Mesh3DRef skyboxMesh;
    Mesh3DFilterRef skyboxMeshFilter;
    Mesh3DRendererRef skyboxMeshRenderer;

    protected:

    Camera();
    ~Camera();

    public:

    void SetSkybox(TextureRef cubeTexture);
    bool HasActiveSkybox() const;
    SceneObjectRef GetSkyboxSceneObject();
    const Transform& GetProjectionTransform() const ;
    void AddClearBuffer(RenderBufferType buffer);
    void RemoveClearBuffer(RenderBufferType buffer);
    unsigned int GetClearBufferMask() const;
    void UpdateDisplay();
};

#endif
