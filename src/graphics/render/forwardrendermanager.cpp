#include "forwardrendermanager.h"
#include "material.h"
#include "multimaterial.h"
#include "geometry/transform.h"
#include "geometry/point/point3.h"
#include "geometry/quaternion.h"
#include "geometry/sceneobjecttransform.h"
#include "scene/sceneobject.h"
#include "scene/eventmanager.h"
#include "object/engineobjectmanager.h"
#include "base/bitmask.h"
#include "graphics/shader/shader.h"
#include "graphics/graphics.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/render/skinnedmesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "graphics/object/mesh3Dfilter.h"
#include "graphics/object/submesh3D.h"
#include "graphics/view/camera.h"
#include "graphics/light/light.h"
#include "graphics/texture/texture.h"
#include "graphics/texture/textureattr.h"
#include "filesys/filesystem.h"
#include "asset/assetimporter.h"
#include "util/datastack.h"
#include "util/time.h"
#include "util/engineutility.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "debug/gtedebug.h"

namespace GTE {
    /*
    * Single default constructor
    */
    ForwardRenderManager::ForwardRenderManager() {
        lightCount = 0;
        ambientLightCount = 0;
        cameraCount = 0;
        renderableSceneObjectCount = 0;
        forwardBlending = FowardBlendingMethod::Additive;
    }

    /*
     * Clean up
     */
    ForwardRenderManager::~ForwardRenderManager() {
        DestroyCachedShadowVolumes();
    }

    /*
     * Initialize. Return false if initialization false, true if it succeeds.
     */
    Bool ForwardRenderManager::Init() {
        EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

        AssetImporter assetImporter;
        ShaderSource shaderSource;

        // construct shadow volume material
        assetImporter.LoadBuiltInShaderSource("shadowvolume", shaderSource);
        shadowVolumeMaterial = objectManager->CreateMaterial("ShadowVolumeMaterial", shaderSource);
        ASSERT(shadowVolumeMaterial.IsValid(), "ForwardRenderManager::Init -> Unable to create shadow volume material.");
        shadowVolumeMaterial->SetFaceCulling(RenderState::FaceCulling::None);
        shadowVolumeMaterial->SetDepthBufferWriteEnabled(false);

        // construct SSAO outline material
        assetImporter.LoadBuiltInShaderSource("ssaooutline", shaderSource);
        ssaoOutlineMaterial = objectManager->CreateMaterial("SSAOOutline", shaderSource);
        ASSERT(ssaoOutlineMaterial.IsValid(), "ForwardRenderManager::Init -> Unable to create SSAO outline material.");
        ssaoOutlineMaterial->SetUseLighting(false);

        // construct depth-only material
        assetImporter.LoadBuiltInShaderSource("depthonly", shaderSource);
        depthOnlyMaterial = objectManager->CreateMaterial("DepthOnlyMaterial", shaderSource);
        ASSERT(depthOnlyMaterial.IsValid(), "ForwardRenderManager::Init -> Unable to create depth only material.");
        depthOnlyMaterial->SetUseLighting(false);

        // construct depth-value material
        assetImporter.LoadBuiltInShaderSource("depthvalue", shaderSource);
        depthValueMaterial = objectManager->CreateMaterial("DepthValueMaterial", shaderSource);
        ASSERT(depthValueMaterial.IsValid(), "ForwardRenderManager::Init -> Unable to create depth value material.");
        depthValueMaterial->SetUseLighting(false);

        // build depth texture off-screen render target
        Graphics * graphics = Engine::Instance()->GetGraphicsSystem();
        RenderTargetSharedPtr defaultRenderTarget = graphics->GetDefaultRenderTarget();
        TextureAttributes colorTextureAttributes;
        colorTextureAttributes.Format = TextureFormat::R32F;
        colorTextureAttributes.FilterMode = TextureFilter::Point;
        colorTextureAttributes.WrapMode = TextureWrap::Clamp;
        depthRenderTarget = objectManager->CreateRenderTarget(true, true, false, colorTextureAttributes, defaultRenderTarget->GetWidth(), defaultRenderTarget->GetHeight());
        ASSERT(depthRenderTarget.IsValid(), "ForwardRenderManager::Init -> Unable to create off-screen rendering surface.");

        TextureSharedPtr depthTexture = depthRenderTarget->GetDepthTexture();
        TextureSharedPtr colorTexture = depthRenderTarget->GetColorTexture();

        ASSERT(colorTexture.IsValid(), "ForwardRenderManager::Init -> Unable to create off-screen color buffer.");
        ASSERT(depthTexture.IsValid(), "ForwardRenderManager::Init -> Unable to create off-screen depth buffer.");

        if (!InitFullScreenQuad())return false;

        Bool singleLightInit = singleLightDescriptor.Init(1);
        ASSERT(singleLightInit, "ForwardRenderManager::Init -> Unable to initialize single light descriptor.");

        Bool multiLightInit = multiLightDescriptor.Init(Constants::MaxShaderLights);
        ASSERT(multiLightInit, "ForwardRenderManager::Init -> Unable to initialize multi-light descriptor.");

        return true;
    }

    /*
     * Initialize the components needed to render a full screen quad.
     */
    Bool ForwardRenderManager::InitFullScreenQuad() {
        EngineObjectManager * objectManager = Engine::Instance()->GetEngineObjectManager();

        // create full screen quad mesh
        StandardAttributeSet meshAttributes = StandardAttributes::CreateAttributeSet();
        StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Position);
        StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::Normal);
        StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::FaceNormal);
        StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture0);
        StandardAttributes::AddAttribute(&meshAttributes, StandardAttribute::UVTexture1);
        fullScreenQuad = EngineUtility::CreateRectangularMesh(meshAttributes, 1, 1, 1, 1, true, true, false);
        ASSERT(fullScreenQuad.IsValid(), "ForwardRenderManager::InitFullScreenQuad -> Unable to create full screen quad.");

        // access position data
        Point3Array& positions = fullScreenQuad->GetSubMesh(0)->GetPositions();

        // transform full-screen quad to: X: [0..1], Y: [0..1]
        for (UInt32 i = 0; i < positions.GetCount(); i++) {
            Point3 * p = positions.GetElement(i);
            p->x += 0.5;
            p->y += 0.5;
        }
        fullScreenQuad->UpdateAll();

        // create camera for rendering to [fullScreenQuad]
        fullScreenQuadCam = objectManager->CreateCamera();
        ASSERT(fullScreenQuadCam.IsValid(), "ForwardRenderManager::InitFullScreenQuad -> Unable to camera for full screen quad.");
        fullScreenQuadCam->SetProjectionMode(ProjectionMode::Orthographic);

        fullScreenQuadObject = objectManager->CreateSceneObject();
        ASSERT(fullScreenQuadObject.IsValid(), "ForwardRenderManager::InitFullScreenQuad -> Unable to scene object for full screen quad.");

        Mesh3DFilterSharedPtr filter = objectManager->CreateMesh3DFilter();
        ASSERT(filter.IsValid(), "ForwardRenderManager::InitFullScreenQuad -> Unable to mesh filter for full screen quad.");

        filter->SetCastShadows(false);
        filter->SetReceiveShadows(false);
        filter->SetMesh3D(fullScreenQuad);
        fullScreenQuadObject->SetMesh3DFilter(filter);

        Mesh3DRendererSharedPtr renderer = objectManager->CreateMesh3DRenderer();
        ASSERT(filter.IsValid(), "ForwardRenderManager::InitFullScreenQuad -> Unable to renderer for full screen quad.");
        fullScreenQuadObject->SetRenderer(DynamicCastEngineObject<Mesh3DRenderer, Renderer>(renderer));

        fullScreenQuadObject->SetActive(false);

        return true;
    }

    /*
     * Tell the graphics system to clear the buffers ccorresponding to [clearMask].
     */
    void ForwardRenderManager::ClearRenderBuffers(IntMask clearMask) const {
        Engine::Instance()->GetGraphicsSystem()->ClearRenderBuffers(clearMask);
    }

    /*
     * Push a new render target on to the render target stack and activate it.
     */
    void ForwardRenderManager::PushRenderTarget(RenderTargetRef renderTarget) {
        renderTargetStack.push(renderTarget);
        // activate the new render target
        Engine::Instance()->GetGraphicsSystem()->ActivateRenderTarget(renderTarget);
    }

    /*
     * Pop the current render target off the stack and activate the one below it.
     * If there is none below, activate the default render target.
     */
    RenderTargetSharedPtr ForwardRenderManager::PopRenderTarget() {
        if (renderTargetStack.size() <= 0) {
            return NullRenderTargetRef;
        }

        RenderTargetSharedPtr old = renderTargetStack.top();
        renderTargetStack.pop();

        if (renderTargetStack.size() > 0) {
            RenderTargetRef top = renderTargetStack.top();
            // activate the new render target
            Engine::Instance()->GetGraphicsSystem()->ActivateRenderTarget(top);
        }
        else {
            // activate the default render target
            Engine::Instance()->GetGraphicsSystem()->RestoreDefaultRenderTarget();
        }

        return old;
    }

    /*
     * Clear [renderTargetStack].
     */
    void ForwardRenderManager::ClearRenderTargetStack() {
        renderTargetStack.empty();
    }

    /*
     * Kick off rendering of the entire scene. This method first processes the scene hierarchy and
     * stores a list of all cameras, lights, and meshes in the scene. After that it renders the scene
     * from the perspective of each camera.
     */
    void ForwardRenderManager::RenderScene() {
        // render the scene from the perspective of each camera found in ProcessScene()
        for (UInt32 i = 0; i < cameraCount; i++) {
            RenderSceneForCamera(i);
        }
    }

    /*
     * Clear any caches that are set up for the render manager.
     */
    void ForwardRenderManager::ClearCaches() {
        DestroyCachedShadowVolumes();
    }

    /*
     * Render a quad-mesh that covers the entire screen and whose normal is orthogonal to the camera's
     * direction vector. The vertices of the quad will be passed to the shader it the range:
     *
     *   X: [0 .. 1] From left to right
     *   Y: [0 .. 1] From bottom to top
     *
     * [renderTarget] - The render target to which the quad should be rendered.
     * [material] - The material (and shader) to be used for rendering.
     * [clearBuffers] - Should the buffers belonging to [renderTarget] be cleared before rendering?
     *
     */
    void ForwardRenderManager::RenderFullScreenQuad(RenderTargetRef renderTarget, MaterialRef material, Bool clearBuffers) {
        NONFATAL_ASSERT(renderTarget.IsValid(), "ForwardRenderManager::RenderFullScreenQuad -> Invalid render target.", true);
        NONFATAL_ASSERT(material.IsValid(), "ForwardRenderManager::RenderFullScreenQuad -> Invalid material.", true);

        // activate the material, which will switch the GPU's active shader to
        // the one associated with the material
        ActivateMaterial(material, false);

        // send uniforms set for the material to its shader
        SendActiveMaterialUniformsToShader();

        Graphics * graphics = Engine::Instance()->GetGraphicsSystem();

        // save the currently active render target
        RenderTargetRef currentTarget = graphics->GetCurrrentRenderTarget();

        // set [renderTarget] as the current render target
        graphics->ActivateRenderTarget(renderTarget);

        // clear buffers (if necessary)
        if (clearBuffers) {
            IntMask clearMask = IntMaskUtil::CreateMask();
            IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Color);
            IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Depth);
            graphics->ClearRenderBuffers(clearMask);
        }

        fullScreenQuadObject->SetActive(true);

        // set [material] to be the material for the full screen quad mesh
        Renderer * renderer = fullScreenQuadObject->GetRenderer().GetPtr();
        Mesh3DRenderer * meshRenderer = dynamic_cast<Mesh3DRenderer*>(renderer);
        if (meshRenderer->GetMultiMaterialCount() > 0) {
            meshRenderer->GetMultiMaterial(0)->SetMaterial(0, material);
        }
        else meshRenderer->AddMultiMaterial(material);

        ASSERT(meshRenderer->GetSubRendererCount() == fullScreenQuad->GetSubMeshCount(), "ForwardRenderManager::RenderFullScreenQuad -> Sub-renderer count does not equal sub-mesh count.");
        // render the full screen quad mesh
        for (UInt32 i = 0; i < fullScreenQuad->GetSubMeshCount(); i++) {
            meshRenderer->GetSubRenderer(i)->Render();
        }
        fullScreenQuadObject->SetActive(false);

        // activate the old render target
        graphics->ActivateRenderTarget(currentTarget);
    }

    /*
     * Perform all actions that must occur before any rendering.
     */
    void ForwardRenderManager::PreRender() {
        lightCount = 0;
        ambientLightCount = 0;
        cameraCount = 0;
        renderableSceneObjectCount = 0;
        renderQueueManager.ClearAllRenderQueues();

        SceneObjectRef sceneRoot = Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
        ASSERT(sceneRoot.IsValid(), "ForwardRenderManager::Update -> 'sceneRoot' is null.");

        // gather information about the cameras, lights, and renderable meshes in the scene
        PreProcessScene(sceneRoot.GetRef(), 0);
        // perform any pre-transformations and calculations (e.g. vertex skinning)
        PreRenderScene();
        // calculate shadow volumes
        BuildSceneShadowVolumes();
    }

    /*
     * PreProcessScene:
     *
     * For each SceneObject that is visited during this search, this method saves references to:
     *
     *   1. Scene objects that contain instances of Camera -> saved to [sceneCameras]
     *   2. Scene objects that contain instances of Light -> saved to [sceneAmbientLights] or [sceneLights]
     *   3. Scene objects that contain instances of Renderer will be saved to [renderableSceneObjects]
     *
     * Additionally for each instance of Renderer encountered, render queue entries will be created for each sub-renderer.
     *
     * Later, the entire scene is rendered from the perspective of each camera in [sceneCameras] list via RenderSceneFromCamera().
     *
     * For each scene obejct in [renderableSceneObjects], the PreRenderScene() method will call the PreRender() method of each sub-renderer
     * in that scene object's Renderer component. For skinned meshes, this method will (indirectly) perform the vertex skinning transformation.
     */
    void ForwardRenderManager::PreProcessScene(SceneObject& parent, UInt32 recursionDepth) {
        // enforce max recursion depth
        if (recursionDepth >= Constants::MaxObjectRecursionDepth - 1)return;

        for (UInt32 i = 0; i < parent.GetChildrenCount(); i++) {
            SceneObjectRef childRef = parent.GetChildAt(i);

            if (!childRef.IsValid()) {
                Debug::PrintWarning("ForwardRenderManager::PreProcessScene -> Null scene object encountered.");
                continue;
            }

            SceneObject* child = childRef.GetPtr();

            // only process active scene objects
            if (child->IsActive()) {
                CameraRef camera = child->GetCamera();
                if (camera.IsValid() && cameraCount < MAX_CAMERAS) {
                    // add a scene camera from which to render the scene
                    // always make sure to respect the render order index of the camera,
                    // the array is sort in ascending order
                    for (UInt32 i = 0; i <= cameraCount; i++) {
                        if (i == cameraCount || sceneCameras[i]->GetCamera()->GetRenderOrderIndex() > camera->GetRenderOrderIndex()) {
                            for (UInt32 ii = cameraCount; ii > i; ii--) {
                                sceneCameras[ii] = sceneCameras[ii - 1];
                            }
                            sceneCameras[i] = child;
                            cameraCount++;
                            break;
                        }
                    }
                }

                LightRef light = child->GetLight();
                if (light.IsValid() && ambientLightCount + lightCount < Constants::MaxSceneLights) {
                    if (light->GetType() == LightType::Ambient) {
                        // add ambient light
                        sceneAmbientLights[ambientLightCount] = child;
                        ambientLightCount++;
                    }
                    else {
                        // add non-ambient light
                        sceneLights[lightCount] = child;
                        lightCount++;
                    }
                }

                RendererRef baseRenderer = child->GetRenderer();
                Mesh3DFilterRef meshFilter = child->GetMesh3DFilter();
                Mesh3DRef mesh = child->GetMesh3D();

                if (baseRenderer.IsValid()) {
                    Mesh3DRenderer * meshRenderer = dynamic_cast<Mesh3DRenderer*>(baseRenderer.GetPtr());
                    if (meshRenderer != nullptr) {
                        if (meshFilter.IsValid() && mesh.IsValid() && mesh->GetSubMeshCount() == meshRenderer->GetSubRendererCount() && meshRenderer->GetMultiMaterialCount() > 0 && renderableSceneObjectCount < Constants::MaxSceneObjects) {
                            renderableSceneObjects[renderableSceneObjectCount] = child;
                            renderableSceneObjectCount++;
                            UInt32 materialCount = meshRenderer->GetMultiMaterialCount();
                            UInt32 subMeshCount = mesh->GetSubMeshCount();

                            // for each sub mesh in [mesh], create a render queue entry for each material in the
                            // corresponding multi-material
                            for (UInt32 i = 0; i < subMeshCount; i++) {
                                // get the multi material that corresponds to this sub-mesh
                                MultiMaterialRef multiMat = meshRenderer->GetMultiMaterial(i % materialCount);

                                // for each material in [multiMat], create a render queue entry
                                for (UInt32 m = 0; m < multiMat->GetMaterialCount(); m++) {
                                    MaterialRef mat = multiMat->GetMaterial(m);
                                    RenderQueue* targetRenderQueue = renderQueueManager.GetRenderQueueForID(mat->GetRenderQueue());

                                    SceneObjectProcessingDescriptor& processingDesc = child->GetProcessingDescriptor();
                                    Transform * aggregateTransform = &processingDesc.AggregateTransform;
                                    targetRenderQueue->Add(child, mesh->GetSubMesh(i).GetPtr(), meshRenderer->GetSubRenderer(i).GetPtr(), &const_cast<MaterialSharedPtr&>(mat), meshFilter.GetPtr(), aggregateTransform);
                                }
                            }
                        }
                    }
                }

                // continue recursion through child object
                PreProcessScene(*child, recursionDepth + 1);
            }
        }
    }

    /*
     * Iterate through all (active) Renderer components in the scene all call their
     * respective PreRender() methods. This is typically where vertex skinning will
     * happen.
     */
    void ForwardRenderManager::PreRenderScene() {
        Transform model;
        Transform modelInverse;

        // loop through each mesh-containing SceneObject in [sceneMeshObjects]
        for (UInt32 s = 0; s < renderableSceneObjectCount; s++) {
            SceneObject* child = renderableSceneObjects[s];

            SceneObjectProcessingDescriptor& processingDesc = child->GetProcessingDescriptor();
            model.SetTo(processingDesc.AggregateTransform);
            modelInverse.SetTo(model);
            modelInverse.Invert();

            RendererRef baseRenderer = child->GetRenderer();
            Mesh3DRenderer * meshRenderer = dynamic_cast<SkinnedMesh3DRenderer*>(baseRenderer.GetPtr());
            if (meshRenderer == nullptr) {
                meshRenderer = dynamic_cast<Mesh3DRenderer*>(baseRenderer.GetPtr());
            }

            if (meshRenderer != nullptr) {
                // for each sub-renderer, call the PreRender() method
                for (UInt32 r = 0; r < meshRenderer->GetSubRendererCount(); r++) {
                    SubMesh3DRendererRef subRenderer = meshRenderer->GetSubRenderer(r);
                    if (subRenderer.IsValid()) {
                        subRenderer->PreRender(model.GetConstMatrix(), modelInverse.GetConstMatrix());
                    }
                }
            }
        }
    }

    /*
     * Render the entire scene from the perspective of a single camera. Uses [cameraIndex]
     * as an index into the array of cameras [sceneCameras] that has been found by processing the scene.
     */
    void ForwardRenderManager::RenderSceneForCamera(UInt32 cameraIndex) {
        NONFATAL_ASSERT(cameraIndex < cameraCount, "ForwardRenderManager::RenderSceneFromCamera -> cameraIndex out of bounds", true);

        SceneObject* object = sceneCameras[cameraIndex];
        NONFATAL_ASSERT(object != nullptr, "ForwardRenderManager::RenderSceneFromCamera -> Camera's scene object is not valid.", true);

        SceneObjectRef sceneRoot = Engine::Instance()->GetEngineObjectManager()->GetSceneRoot();
        ASSERT(sceneRoot.IsValid(), "ForwardRenderManager::RenderSceneFromCamera -> sceneRoot is null.");

        RenderSceneForCamera(object->GetCamera());
    }

    /*
    * Calculate the relevant transforms and other parameters necessary for rendering the scene from [camera].
    */
    void ForwardRenderManager::GetViewDescriptorForCamera(const Camera& camera, const Transform* altViewTransform, ViewDescriptor& descriptor) {
        SceneObjectProcessingDescriptor& processingDesc = const_cast<Camera&>(camera).GetSceneObject()->GetProcessingDescriptor();
        const Transform& cameraTransform = processingDesc.AggregateTransform;

        descriptor.ClearBufferMask = camera.GetClearBufferMask();
        descriptor.CullingMask = camera.GetCullingMask();

        descriptor.ReverseCulling = camera.GetReverseCulling();

        descriptor.ViewTransform = altViewTransform != nullptr ? *altViewTransform : cameraTransform;
        descriptor.ViewTransformInverse.SetTo(descriptor.ViewTransform);
        descriptor.ViewTransformInverse.Invert();

        Point3 viewOrigin;
        descriptor.ViewTransform.TransformPoint(viewOrigin);
        descriptor.ViewPosition = viewOrigin;

        descriptor.ProjectionTransform = camera.GetProjectionTransform();
        descriptor.ProjectionTransformInverse = camera.GetInverseProjectionTransform();

        descriptor.UniformWorldSceneObjectTransform = camera.GetUniformWorldSceneObjectTransform();
        descriptor.UniformWorldSceneObjectTransformInverse = camera.GetUniformWorldSceneObjectTransform();
        descriptor.UniformWorldSceneObjectTransformInverse.Invert();

        descriptor.LightingEnabled = camera.IsLightingEnabled();
        descriptor.DepthPassEnabled = camera.IsDepthPassEnabled();

        descriptor.SSAOEnabled = camera.IsSSAOEnabled();
        descriptor.SSAOMode = camera.GetSSAORenderMode();

        descriptor.SkyboxEnabled = camera.IsSkyboxSetup() && camera.IsSkyboxEnabled();
        descriptor.SkyboxObject = const_cast<Camera&>(camera).GetSkyboxSceneObject().GetPtr();

        // for now we only support up to one clip plane
        //TODO: Add support for > 1 clip plane
        if (camera.GetClipPlaneCount() > 0) {
            const ClipPlane* clipPlane0 = const_cast<Camera&>(camera).GetClipPlane(0);

            if (clipPlane0 != nullptr) {
                descriptor.ClipPlaneCount = 1;
                descriptor.ClipPlane0Normal = clipPlane0->Normal;
                descriptor.ClipPlane0Offset = clipPlane0->Offset;
            }
        }
        else {
            static const Vector3 defaultVector = Vector3(0.0, 0.0, 0.0);
            descriptor.ClipPlaneCount = 0;
            descriptor.ClipPlane0Normal = defaultVector;
            descriptor.ClipPlane0Offset = 0.0f;
        }
    }

    /*
     * This method will activate the render target belonging to [camera] (which may just be the default render target),
     * and then pass control to ForwardRenderSceneForCameraAndCurrentRenderTarget.
     */
    void ForwardRenderManager::RenderSceneForCamera(CameraRef cameraRef) {
        NONFATAL_ASSERT(cameraRef.IsValid(), "ForwardRenderManager::RenderSceneFromCamera -> Camera is not valid.", true);
        Camera& camera = cameraRef.GetRef();

        SceneObjectRef cameraObject = camera.GetSceneObject();
        NONFATAL_ASSERT(cameraObject.IsValid(), "ForwardRenderManager::RenderSceneForCamera -> Camera is not attached to a scene object.", true);

        // clear stack of activated render targets
        ClearRenderTargetStack();

        // activate camera's render target
        RenderTargetRef cameraRenderTarget = camera.GetRenderTarget();
        NONFATAL_ASSERT(cameraRenderTarget.IsValid(), "ForwardRenderManager::RenderSceneForCamera -> Camera's render target is not valid.", true);
        PushRenderTarget(cameraRenderTarget);

        SetCurrentCamera(cameraRef);

        ViewDescriptor viewDescriptor;
        // get a reference to the engine's graphics system
        Graphics* graphics = Engine::Instance()->GetGraphicsSystem();

        // if the render target is a cube, render 6 times, twice for each axis
        if (cameraRenderTarget->GetColorTexture().IsValid() &&
            cameraRenderTarget->GetColorTexture()->GetAttributes().IsCube) {
            Transform altViewTransform;
            SceneObjectProcessingDescriptor& processingDesc = cameraObject->GetProcessingDescriptor();
            const Transform& cameraTransform = processingDesc.AggregateTransform;

            static const Real orientations[6][4] = { { 0, 1, 0, 0},
                                                     { 0, 1, 0, -180 },
                                                     { 1, 0, 0, 90 },
                                                     { 1, 0, 0, -90 },
                                                     { 0, 1, 0, 90 },
                                                     { 0, 1, 0, -90 } };

            static const CubeTextureSide sides[6] = { CubeTextureSide::Front,
                                                     CubeTextureSide::Back,
                                                     CubeTextureSide::Top,
                                                     CubeTextureSide::Bottom,
                                                     CubeTextureSide::Left,
                                                     CubeTextureSide::Right };

            for (UInt32 i = 0; i < 6; i++) {
                graphics->ActivateCubeRenderTargetSide(sides[i]);
                altViewTransform.SetTo(cameraTransform);
                altViewTransform.Rotate(orientations[i][0], orientations[i][1], orientations[i][2], orientations[i][3], true);
                GetViewDescriptorForCamera(camera, nullptr, viewDescriptor);
                RenderSceneForCurrentRenderTarget(viewDescriptor);
            }
        }
        else {
            GetViewDescriptorForCamera(camera, nullptr, viewDescriptor);
            RenderSceneForCurrentRenderTarget(viewDescriptor);
        }

        RenderTargetRef copyTarget = camera.GetCopyRenderTarget();
        if (copyTarget.IsValid()) {
            RenderTargetRef source = camera.GetRenderTarget();
            graphics->CopyBetweenRenderTargets(source, copyTarget);
        }

        PopRenderTarget();
    }

    /*
    * Clear the 'rendered' status for each scene object found during scene processing, indicating that
    * it has not yet been rendered.
    */
    void ForwardRenderManager::ClearRenderedStatus() {
        for (RenderQueueManager::ConstIterator itr = renderQueueManager.Begin(); itr != renderQueueManager.End(); ++itr) {
            RenderQueueEntry* entry = *itr;

            SceneObject * sceneObject = entry->Container;

            SceneObjectProcessingDescriptor& processingDescriptor = sceneObject->GetProcessingDescriptor();
            processingDescriptor.WillRenderCalled = false;

            renderedSubRenderers.clear();
        }
    }

    /*
     * Render all the meshes in the scene using forward rendering. The camera's inverted transform, which is
     * stored in [viewDescriptor] as "ViewTransformInverse", is used as the view transform. The reason the inverse
     * is used is because on the GPU side of things the view transform is used to move the world relative to the camera,
     * rather than move the camera in the world.
     *
     * Since we are using a forward-rendering approach, meshes are rendered in one of two ways:

        - Each mesh with a 'singlePassMode' of SinglePassMode::None is rendered once for each light and the output from
          each pass is combined with the others using additive blending. This is accomplished through a call to
          RenderSceneForLight(), which then calls RenderMesh(). The additive blending occurs in the RenderMesh() method.

        - Each mesh with a 'singlePassMode' that is not SinglePassMode::None is rendered via a call to RenderSceneSinglePass().
          No additive blending occurs for these meshes because it is assumed the blending will occur in the shader.
     *
     * This method will render to whatever render target is currently active.
     */
    void ForwardRenderManager::RenderSceneForCurrentRenderTarget(const ViewDescriptor& viewDescriptor) {
        // clear 'rendered' status for each scene object
        ClearRenderedStatus();

        // clear the appropriate render buffers
        ClearRenderBuffers(viewDescriptor.ClearBufferMask);

        UInt32 renderQueueCount = renderQueueManager.GetRenderQueueCount();

        // ===========================================
        // BASE PASS (PRE-SSAO AMBIENT-ONLY PASS)
        // ===========================================

        Bool renderedAmbient = false;

        // Render all objects that can receive ambient light in render queues preceeding RenderQueueType::Transparent.
        // These are the objects for which SSAO can be applied, so their ambient pass must be rendered completely
        // before the SSAO effect, which is screen-space. Objects in render queues greater than or equal to 
        // RenderQueueType::Transparent will not get SSAO. 
        if (viewDescriptor.LightingEnabled) {
            for (UInt32 queueIndex = 0; queueIndex < renderQueueCount; queueIndex++) {
                Int32 queueID = renderQueueManager.GetRenderQueueID(queueIndex);

                if (queueID >= (Int32)RenderQueueType::Transparent)break;

                // perform single-pass rendering for objects whose materials have a 'singlePassMode' of SinglePassMode::PreSSAO.
                RenderSceneSinglePass(viewDescriptor, queueID, SinglePassMode::PreSSAO);

                // loop through each ambient light and render objects in render queue [queueID] for it
                for (UInt32 l = 0; l < ambientLightCount; l++) {
                    SceneObject* lightObject = sceneAmbientLights[l];
                    ASSERT(lightObject != nullptr, "ForwardRenderManager::RenderSceneForCurrentRenderTarget -> Base pass: Ambient light's scene object is not valid.");

                    LightRef lightRef = lightObject->GetLight();
                    ASSERT(lightRef.IsValid(), "ForwardRenderManager::RenderSceneForCurrentRenderTarget -> Base pass: Ambient light is not valid.");

                    // render all objects in the scene that have materials the use lighting
                    RenderSceneForLight(lightRef.GetRef(), viewDescriptor, queueID);
                    renderedAmbient = true;
                }
            }
        }

        // =============================
        // DEPTH FILL PASS
        // =============================

        // we need to fill the depth buffer with the scene to allow
        // for proper shadow volume rendering and depth-buffer culling
        if (viewDescriptor.DepthPassEnabled) {
            RenderDepthBuffer(viewDescriptor);
        }

        // =============================
        // SSAO PASS
        // =============================

        // perform the standard screen-space ambient occlusion pass
        if (renderedAmbient && viewDescriptor.SSAOEnabled && viewDescriptor.SSAOMode == SSAORenderMode::Standard) {
            RenderSceneSSAO(viewDescriptor);
        }

        // =============================
        // STANDARD PASS
        // =============================

        // Render all objects (both lit and self-lit) in render queues above or equal to RenderQueueType::Transparent.
        // These are the objects for which SSAO will NOT be applied.
        Bool skyboxPassComplete = false;
        for (UInt32 queueIndex = 0; queueIndex < renderQueueCount; queueIndex++) {
            Int32 queueID = renderQueueManager.GetRenderQueueID(queueIndex);

            // loop through each ambient light and render objects in render queue [queueID] for it
            if (viewDescriptor.LightingEnabled) {
                // perform single-pass rendering for objects whose materials have a 'singlePassMode' of SinglePassMode::Standard.
                RenderSceneSinglePass(viewDescriptor, queueID, SinglePassMode::Standard);

                // render ambient lights again, but only for transparent objects
                if (queueID >= (Int32)RenderQueueType::Transparent) {
                    for (UInt32 l = 0; l < ambientLightCount; l++) {
                        SceneObject* lightObject = sceneAmbientLights[l];
                        ASSERT(lightObject != nullptr, "ForwardRenderManager::RenderSceneForCurrentRenderTarget -> Ambient light's scene object is not valid.");

                        LightRef lightRef = lightObject->GetLight();
                        ASSERT(lightRef.IsValid(), "ForwardRenderManager::RenderSceneForCurrentRenderTarget -> Ambient light is not valid.");

                        // render all objects in the scene that have materials the use lighting
                        RenderSceneForLight(lightRef.GetRef(), viewDescriptor, queueID);
                        renderedAmbient = true;
                    }
                }

                // Render objects in render queue [queueID] for each light for  -> this means everything below RenderQueueType::Transparent
                for (UInt32 l = 0; l < lightCount; l++) {
                    SceneObject* lightObject = sceneLights[l];
                    ASSERT(lightObject != nullptr, "ForwardRenderManager::RenderSceneForCurrentRenderTarget -> Light's scene object is not valid.");

                    LightRef lightRef = lightObject->GetLight();
                    ASSERT(lightRef.IsValid(), "ForwardRenderManager::RenderSceneForCurrentRenderTarget -> Light is not valid.");

                    RenderSceneForLight(lightRef.GetRef(), viewDescriptor, queueID);
                }
            }

            // Render objects in render queue [queueID] for meshes that have materials which DO NOT use lighting.
            RenderSceneWithoutLight(viewDescriptor, NullMaterialRef, true, true, FowardBlendingFilter::OnlyIfRendered, nullptr, queueID);

            if (!skyboxPassComplete && (queueIndex == renderQueueCount - 1 || renderQueueManager.GetRenderQueueID(queueIndex + 1) >= (Int32)RenderQueueType::Skybox)) {
                // perform the screen-space ambient occlusion pass as an outline effect
                if (viewDescriptor.SSAOEnabled && viewDescriptor.SSAOMode == SSAORenderMode::Outline) {
                    RenderSceneSSAO(viewDescriptor);
                }

                skyboxPassComplete = true;
                RenderSkyboxForCamera(viewDescriptor);
            }
        }

        // restore default graphics state
        Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);
    }

    /*
    * Render the skybox for the view specified by [viewDescriptor].
    */
    void ForwardRenderManager::RenderSkyboxForCamera(const ViewDescriptor& viewDescriptor) {
        Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);

        // ensure that a skybox has been enabled for this view
        if (viewDescriptor.SkyboxEnabled) {
            // retrieve the scene object that contains the skybox mesh
            SceneObject* skyboxObject = viewDescriptor.SkyboxObject;
            NONFATAL_ASSERT(skyboxObject != nullptr, "ForwardRenderManager::RenderSkyboxForCamera -> View descriptor has invalid skybox scene object.", true);

            RendererRef renderer = skyboxObject->GetRenderer();
            NONFATAL_ASSERT(renderer.IsValid(), "ForwardRenderManager::RenderSkyboxForCamera -> Skybox renderer is not valid.", true);

            Renderer* baseRenderer = renderer.GetPtr();
            Mesh3DRenderer* meshRenderer = dynamic_cast<Mesh3DRenderer*>(baseRenderer);

            NONFATAL_ASSERT(meshRenderer != nullptr, "ForwardRenderManager::RenderSkyboxForCamera -> Could not find valid renderer for skybox.", true);

            Mesh3DRef mesh = meshRenderer->GetTargetMesh();
            NONFATAL_ASSERT(mesh.IsValid(), "ForwardRenderManager::RenderSkyboxForCamera -> Skybox has invalid mesh.", true);
            NONFATAL_ASSERT(mesh->GetSubMeshCount() > 0, "ForwardRenderManager::RenderSkyboxForCamera -> Skybox has empty mesh.", true);
            NONFATAL_ASSERT(meshRenderer->GetSubRendererCount() > 0, "ForwardRenderManager::RenderSkyboxForCamera -> Skybox has no renderers.", true);
            NONFATAL_ASSERT(meshRenderer->GetMultiMaterialCount() > 0, "ForwardRenderManager::RenderSkyboxForCamera -> Skybox has no material.", true);

            skyboxObject->SetActive(true);

            // set up single render queue entry for skybox
            skyboxEntry.Container = skyboxObject;
            skyboxEntry.Mesh = mesh->GetSubMesh(0).GetPtr();
            skyboxEntry.Renderer = meshRenderer->GetSubRenderer(0).GetPtr();

            MaterialRef skyboxMaterial = meshRenderer->GetMultiMaterial(0)->GetMaterial(0);
            skyboxEntry.RenderMaterial = const_cast<MaterialSharedPtr*>(&skyboxMaterial);

            singleLightDescriptor.UseLighting = false;

            // render the skybox
            RenderMesh(skyboxEntry, singleLightDescriptor, viewDescriptor, NullMaterialRef, true, FowardBlendingFilter::Never);

            skyboxObject->SetActive(false);
        }
    }

    /*
    * Render the scene to only the depth-buffer. Render from the perspective
    * of the view specified by [viewDescriptor].
    */
    void ForwardRenderManager::RenderDepthBuffer(const ViewDescriptor& viewDescriptor) {
        RenderSceneWithoutLight(viewDescriptor, depthOnlyMaterial, false, false, FowardBlendingFilter::Never, nullptr);
    }

    /*
    * Render the screen-space ambient occlusion for the scene.  Render from the perspective
    * of the view specified by [viewDescriptor].
    */
    void ForwardRenderManager::RenderSceneSSAO(const ViewDescriptor& viewDescriptor) {
        static const UniformID uniform_DEPTH_TEXTURE = UniformDirectory::RegisterVarID("DEPTH_TEXTURE");
        static const UniformID uniform_INV_PROJECTION_MATRIX = UniformDirectory::RegisterVarID("INV_PROJECTION_MATRIX");
        static const UniformID uniform_DISTANCE_THRESHHOLD = UniformDirectory::RegisterVarID("DISTANCE_THRESHHOLD");
        static const UniformID uniform_FILTER_RADIUS = UniformDirectory::RegisterVarID("FILTER_RADIUS");
        static const UniformID uniform_SCREEN_WIDTH = UniformDirectory::RegisterVarID("SCREEN_WIDTH");
        static const UniformID uniform_SCREEN_HEIGHT = UniformDirectory::RegisterVarID("SCREEN_HEIGHT");

        Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);
        GraphicsAttributes attributes = Engine::Instance()->GetGraphicsSystem()->GetAttributes();

        // activate the off-screen render target
        PushRenderTarget(depthRenderTarget);

        // clear the relevant buffers in the off-screen render target
        IntMask clearMask = IntMaskUtil::CreateMask();
        if (depthRenderTarget->HasBuffer(RenderBufferType::Color))IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Color);
        if (depthRenderTarget->HasBuffer(RenderBufferType::Depth))IntMaskUtil::SetBitForMask(&clearMask, (UInt32)RenderBufferType::Depth);
        Engine::Instance()->GetGraphicsSystem()->ClearRenderBuffers(clearMask);

        // render the depth values for the scene to the off-screen color texture (filter out non-static objects)
        MaterialRef depthValueMaterialRef = depthValueMaterial;
        RenderSceneWithoutLight(viewDescriptor, depthValueMaterialRef, false, true, FowardBlendingFilter::Never, [=](SceneObject* sceneObject) {
            return !sceneObject->IsStatic();
        });
        // restore previous render target
        PopRenderTarget();

        Matrix4x4 projectionInvMat;
        viewDescriptor.ProjectionTransformInverse.CopyMatrix(projectionInvMat);

        // retrieve the color texture (which contains depth values) from the off-screen render target
        TextureRef depthTexture = depthRenderTarget->GetColorTexture();

        // set SSAO material values
        ssaoOutlineMaterial->SetTexture(depthTexture, uniform_DEPTH_TEXTURE);
        ssaoOutlineMaterial->SetMatrix4x4(projectionInvMat, uniform_INV_PROJECTION_MATRIX);
        ssaoOutlineMaterial->SetUniform1f(.5f, uniform_DISTANCE_THRESHHOLD);
        ssaoOutlineMaterial->SetUniform2f(.3f, .75f, uniform_FILTER_RADIUS);
        ssaoOutlineMaterial->SetUniform1f((Real)depthRenderTarget->GetWidth(), uniform_SCREEN_WIDTH);
        ssaoOutlineMaterial->SetUniform1f((Real)depthRenderTarget->GetHeight(), uniform_SCREEN_HEIGHT);

        FowardBlendingMethod currentFoward = GetForwardBlending();

        // set forward rendering blending to subtractive since we need to darken areas of the
        // scene that are occluded.
        SetForwardBlending(FowardBlendingMethod::Subtractive);
        // rendering scene with SSAO material and filter out non-static objects
        MaterialRef ssaoOutlineMaterialRef = ssaoOutlineMaterial;
        RenderSceneWithoutLight(viewDescriptor, ssaoOutlineMaterialRef, false, true, FowardBlendingFilter::Always, [=](SceneObject* sceneObject) {
            return !sceneObject->IsStatic();
        });
        // restore previous forward rendering blend mode
        SetForwardBlending(currentFoward);
    }


    /*
     * Forward-Render the scene for a single light [light] from the perspective specified in [viewDescriptor].
     *
     * This method performs two passes:
     *
     * Pass 1: If [light] is not ambient, render shadow volumes for all meshes in the scene for [light] into the stencil buffer.
     * Pass 2: Perform actual rendering of all meshes in the scene for [light]. If [light] is not ambient, this pass will
     *         exclude screen pixels that are hidden from [light] based on the stencil buffer contents from pass 0. Is [light]
     *         is ambient, then this pass will perform a standard render of all meshes in the scene.
     *
     * [viewDescriptor.UniformWorldSceneObjectTransform] is pre-multiplied with the transform of each rendered scene object & light
     */
    void ForwardRenderManager::RenderSceneForLight(const Light& light, const ViewDescriptor& viewDescriptor, Int32 queueID) {
        Transform modelView;
        Transform model;
        Transform modelInverse;

        Light& localLight = const_cast<Light&>(light);

        SceneObjectProcessingDescriptor& processingDesc = localLight.GetSceneObject()->GetProcessingDescriptor();
        Transform lightTransform = processingDesc.AggregateTransform;
        lightTransform.PreTransformBy(viewDescriptor.UniformWorldSceneObjectTransform);

        Point3 lightWorldPosition(0, 0, 0);
        lightTransform.TransformPoint(lightWorldPosition);

        Vector3 lightDirection = light.GetDirection();
        lightTransform.TransformVector(lightDirection);

        // set up lighting descriptor for single light
        singleLightDescriptor.LightObjects[0] = &localLight;
        singleLightDescriptor.Positions[0] = lightWorldPosition;
        singleLightDescriptor.Directions[0] = lightDirection;
        singleLightDescriptor.Enabled[0] = true;
        singleLightDescriptor.UseLighting = true;

        RenderMode currentRenderMode = RenderMode::None;

        enum RenderPass {
            ShadowVolumeRender = 0,
            StandardRender = 1,
            _PassCount = 2
        };

        for (Int32 pass = 0; pass < RenderPass::_PassCount; pass++) {
            if (pass == ShadowVolumeRender) // shadow volume pass
            {
                // check if this light can cast shadows; if not we skip this pass
                if (light.GetShadowsEnabled() && light.GetType() != LightType::Ambient) {
                    currentRenderMode = RenderMode::ShadowVolumeRender;
                    Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::ShadowVolumeRender);
                }
                else
                    continue;
            }

            UInt32 minQueue = renderQueueManager.GetMinQueue();
            UInt32 maxQueue = renderQueueManager.GetMaxQueue();

            if (queueID >= 0) {
                minQueue = maxQueue = queueID;
            }

            for (RenderQueueManager::ConstIterator itr = renderQueueManager.Begin(minQueue, maxQueue); itr != renderQueueManager.End(); ++itr) {
                RenderQueueEntry* entry = *itr;
                NONFATAL_ASSERT(entry != nullptr, "ForwardRenderManager::RenderSceneForLight -> Null render queue entry encountered.", true);

                MaterialRef entryMaterial = *entry->RenderMaterial;
                if (entryMaterial->UseLighting() && entryMaterial->GetSinglePassMode() == SinglePassMode::None) {
                    SceneObject* sceneObject = entry->Container;
                    NONFATAL_ASSERT(sceneObject != nullptr, "ForwardRenderManager::RenderSceneForLight -> Null scene object encountered.", true);

                    Mesh3DFilterRef filter = sceneObject->GetMesh3DFilter();

                    // copy the full world transform of the scene object, including those of all ancestors
                    SceneObjectProcessingDescriptor& processingDesc = sceneObject->GetProcessingDescriptor();
                    Transform sceneObjectWorldTransform;
                    sceneObjectWorldTransform.SetTo(processingDesc.AggregateTransform);
                    sceneObjectWorldTransform.PreTransformBy(viewDescriptor.UniformWorldSceneObjectTransform);

                    // make sure the current mesh should not be culled from [light] or from
                    // the current camera, whose culling mask is in [viewDescriptor].
                    if (!ShouldCullByLayer(viewDescriptor.CullingMask, *sceneObject) &&
                        !ShouldCullFromLightByLayer(light, *sceneObject) &&
                        !ShouldCullFromLightByPosition(light, lightWorldPosition, *entry->Mesh, sceneObjectWorldTransform)) {
                        if (pass == ShadowVolumeRender) // shadow volume pass
                        {
                            if (filter->GetCastShadows()) {
                                RenderShadowVolumeForMesh(*entry, light, lightWorldPosition, lightDirection, viewDescriptor);
                            }
                        }
                        else if (pass == StandardRender) // normal rendering pass
                        {
                            // check if this light can cast shadows and the mesh can receive shadows, if not do standard (shadow-less) rendering
                            if (light.GetShadowsEnabled() && light.GetType() != LightType::Ambient && filter->GetReceiveShadows()) {
                                if (currentRenderMode != RenderMode::StandardWithShadowVolumeTest) {
                                    currentRenderMode = RenderMode::StandardWithShadowVolumeTest;
                                    Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::StandardWithShadowVolumeTest);
                                }
                            }
                            else if (currentRenderMode != RenderMode::Standard) {
                                currentRenderMode = RenderMode::Standard;
                                Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);
                            }

                            // Dispatch "WillRender" event
                            if (!processingDesc.WillRenderCalled) {
                                Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::WillRender, *sceneObject);
                                processingDesc.WillRenderCalled = true;
                            }

                            RenderMesh(*entry, singleLightDescriptor, viewDescriptor, NullMaterialRef, true, FowardBlendingFilter::OnlyIfRendered);
                        }
                    }
                }
            }
        }
    }

    /*
    * Forward-Render the scene for all lights from the perspective specified in [viewDescriptor]. This is a
    * special-case function that will render a mesh for all lights in a single pass. The shader attached to
    * the material that will be used to render the mesh must support multiple light uniforms (in array form).
    *
    * Shadows are not supported by this function, so all meshes rendered in this manner will not cast nor
    * receive shadows.
    */
    void ForwardRenderManager::RenderSceneSinglePass(const ViewDescriptor& viewDescriptor, Int32 queueID, SinglePassMode singlePassMode) {
        UInt32 minQueue = renderQueueManager.GetMinQueue();
        UInt32 maxQueue = renderQueueManager.GetMaxQueue();

        if (queueID >= 0) {
            minQueue = maxQueue = queueID;
        }

        Bool renderModeEntered = false;
        for (RenderQueueManager::ConstIterator itr = renderQueueManager.Begin(minQueue, maxQueue); itr != renderQueueManager.End(); ++itr) {
            RenderQueueEntry* entry = *itr;
            NONFATAL_ASSERT(entry != nullptr, "ForwardRenderManager::RenderSceneForMultiLight -> Null render queue entry encountered.", true);

            SubMesh3DRenderer * subRenderer = entry->Renderer;
            NONFATAL_ASSERT(subRenderer != nullptr, "ForwardRenderManager::RenderSceneForMultiLight -> Null sub renderer encountered.", true);

            MaterialRef entryMaterial = *entry->RenderMaterial;
            NONFATAL_ASSERT(entryMaterial.IsValid(), "ForwardRenderManager::RenderSceneForMultiLight -> Null material encountered.", true);

            Bool rendered = renderedSubRenderers[subRenderer->GetObjectID()];

            if (entryMaterial->UseLighting() && entryMaterial->GetSinglePassMode() == singlePassMode && !rendered) {
                SceneObject* sceneObject = entry->Container;
                NONFATAL_ASSERT(sceneObject != nullptr, "ForwardRenderManager::RenderSceneForLight -> Null scene object encountered.", true);

                // set up lighting descriptor for multiple lights
                BuildMultiLightDescriptor();

                // copy the full world transform of the scene object, including those of all ancestors
                SceneObjectProcessingDescriptor& processingDesc = sceneObject->GetProcessingDescriptor();
                Transform sceneObjectWorldTransform;
                sceneObjectWorldTransform.SetTo(processingDesc.AggregateTransform);
                sceneObjectWorldTransform.PreTransformBy(viewDescriptor.UniformWorldSceneObjectTransform);

                if (!ShouldCullByLayer(viewDescriptor.CullingMask, *sceneObject)) {
                    for (UInt32 l = 0; l < multiLightDescriptor.LightCount; l++) {
                        Light& light = *multiLightDescriptor.LightObjects[l];
                        Bool shouldCull = ShouldCullFromLightByLayer(light, *sceneObject) ||
                            ShouldCullFromLightByPosition(light, multiLightDescriptor.Positions[l], *entry->Mesh, sceneObjectWorldTransform);
                        if (shouldCull)multiLightDescriptor.Enabled[l] = 0;
                    }

                    if (!renderModeEntered) {
                        Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);
                        renderModeEntered = true;
                    }

                    // Dispatch "WillRender" event
                    if (!processingDesc.WillRenderCalled) {
                        Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::WillRender, *sceneObject);
                        processingDesc.WillRenderCalled = true;
                    }

                    RenderMesh(*entry, multiLightDescriptor, viewDescriptor, NullMaterialRef, true, FowardBlendingFilter::OnlyIfRendered);
                }
            }
        }
    }

    void ForwardRenderManager::BuildMultiLightDescriptor() {
        // set up lighting descriptor for multiple lights
        multiLightDescriptor.LightCount = GTEMath::Min(Constants::MaxShaderLights, lightCount + ambientLightCount);
        multiLightDescriptor.UseLighting = true;
        for (UInt32 l = 0; l < Constants::MaxShaderLights; l++) {
            multiLightDescriptor.Enabled[l] = 0;
        }

        Real* positionDatas = multiLightDescriptor.PositionDatas;
        Real* directionDatas = multiLightDescriptor.DirectionDatas;
        Real* colorDatas = multiLightDescriptor.ColorDatas;
        for (UInt32 l = 0; l < multiLightDescriptor.LightCount; l++) {
            SceneObject* lightObject = nullptr;

            if (l >= ambientLightCount) {
                lightObject = sceneLights[l - ambientLightCount];
            }
            else {
                lightObject = sceneAmbientLights[l];
            }

            if (lightObject == nullptr) continue;
            LightRef lightRef = lightObject->GetLight();
            if (!lightRef.IsValid())continue;
            Light& light = const_cast<Light&>(lightRef.GetRef());

            SceneObjectProcessingDescriptor& processingDesc = light.GetSceneObject()->GetProcessingDescriptor();
            Transform lightTransform = processingDesc.AggregateTransform;

            multiLightDescriptor.LightObjects[l] = &light;

            multiLightDescriptor.Positions[l].Set(0, 0, 0);
            multiLightDescriptor.Directions[l] = light.GetDirection();
            lightTransform.TransformPoint(multiLightDescriptor.Positions[l]);
            lightTransform.TransformVector(multiLightDescriptor.Directions[l]);

            multiLightDescriptor.Colors[l] = light.GetColor();

            Real* thisPositionDatas = multiLightDescriptor.Positions[l].GetDataPtr();
            Real* thisDirectionDatas = multiLightDescriptor.Directions[l].GetDataPtr();
            Real* thisColorDatas = multiLightDescriptor.Colors[l].GetDataPtr();

            BaseVector4_QuickCopy_IncDest(thisPositionDatas, positionDatas);
            BaseVector4_QuickCopy_IncDest(thisDirectionDatas, directionDatas);
            BaseVector4_QuickCopy_IncDest(thisColorDatas, colorDatas);

            multiLightDescriptor.Types[l] = (Int32)lightRef->GetType();
            multiLightDescriptor.Intensities[l] = lightRef->GetIntensity();
            multiLightDescriptor.Ranges[l] = lightRef->GetRange();
            multiLightDescriptor.Attenuations[l] = lightRef->GetAttenuation();
            multiLightDescriptor.ParallelAngleAttenuations[l] = (Int32)lightRef->GetParallelAngleAttenuationType();
            multiLightDescriptor.OrthoAngleAttenuations[l] = (Int32)lightRef->GetOrthoAngleAttenuationType();

            multiLightDescriptor.Enabled[l] = 1;
        }
    }

    void ForwardRenderManager::RenderSceneWithoutLight(const ViewDescriptor& viewDescriptor, MaterialRef  material, Bool flagRendered, Bool renderMoreThanOnce,
                                                       FowardBlendingFilter blendingFilter, std::function<Bool(SceneObject*)> filterFunction) {
        RenderSceneWithoutLight(viewDescriptor, material, flagRendered, renderMoreThanOnce, blendingFilter, filterFunction, -1);
    }

    /*
    * Render all objects in the scene without lighting. If [material] is a valid Material, then it will be used as a
    * replacement material to render all scene objects. If it is not valid then only meshes with materials that have
    * the "use lighting" flag set to false will be rendered. All objects will be rendered from the perspective
    * of the view specified by [viewDescriptor].
    *
    * [flagRendered] - If true, each mesh will be flagged as rendered after it is rendered, which affects how blending
    *                  works for that mesh on future rendering passes.
    * [renderMoreThanOnce] - If true, meshes can be rendered more than once (meaning in the additive passes).
    * [blendingFilter] - Determines how forward-rendering blending will be applied.
    * [filterFunction] - Used to filter out select scene objects.
    */
    void ForwardRenderManager::RenderSceneWithoutLight(const ViewDescriptor& viewDescriptor, MaterialRef  material, Bool flagRendered, Bool renderMoreThanOnce,
                                                       FowardBlendingFilter blendingFilter, std::function<Bool(SceneObject*)> filterFunction, Int32 queueID) {
        Engine::Instance()->GetGraphicsSystem()->EnterRenderMode(RenderMode::Standard);

        UInt32 minQueue = renderQueueManager.GetMinQueue();
        UInt32 maxQueue = renderQueueManager.GetMaxQueue();

        if (queueID >= 0) {
            minQueue = maxQueue = queueID;
        }

        singleLightDescriptor.UseLighting = false;

        for (RenderQueueManager::ConstIterator itr = renderQueueManager.Begin(minQueue, maxQueue); itr != renderQueueManager.End(); ++itr) {
            RenderQueueEntry* entry = *itr;

            SubMesh3DRenderer * subRenderer = entry->Renderer;
            NONFATAL_ASSERT(subRenderer != nullptr, "ForwardRenderManager::RenderSceneWithoutLight -> Null sub renderer encountered.", true);

            Bool rendered = renderedSubRenderers[subRenderer->GetObjectID()];
            if (rendered && !renderMoreThanOnce)continue;

            if (!material.IsValid() && (*entry->RenderMaterial)->UseLighting()) {
                continue;
            }

            SceneObject* sceneObject = entry->Container;

            // check if [sceneObject] should be rendered based on its layer
            if (ShouldCullByLayer(viewDescriptor.CullingMask, *sceneObject)) {
                continue;
            }

            // execute filter function (if one is specified)
            if (filterFunction != nullptr) {
                Bool filter = filterFunction(sceneObject);
                if (filter)continue;
            }

            // Dispatch "WillRender" event
            SceneObjectProcessingDescriptor& processingDescriptor = sceneObject->GetProcessingDescriptor();
            if (!processingDescriptor.WillRenderCalled) {
                Engine::Instance()->GetEventManager()->DispatchSceneObjectEvent(SceneObjectEvent::WillRender, *sceneObject);
                processingDescriptor.WillRenderCalled = true;
            }

            RenderMesh(*entry, singleLightDescriptor, viewDescriptor, material, flagRendered, blendingFilter);
        }
    }

    /*
     * Forward-Render the mesh attached to [entry].
     *
     * [entry] - RenderQueueEntry object that contains the relevant renderable and transform information.
     * [lightingDescriptor] - Describes the lighting to be used for rendering (if there is any).
     * [viewDescriptor] - Describes the view and perspective transforms for rendering the mesh, as well as several other properties.
     * [materialOverride] - If this is valid, it will be used to render the mesh, rather than the mesh's material.
     * [flagRendered] - If true, each mesh will be flagged as rendered after it is rendered, which affects how blending
     *                  works for that mesh on future rendering passes.
     * [renderMoreThanOnce] - If true, meshes can be rendered more than once (meaning in the additive passes).
     * [blendingFilter] - Determines how forward-rendering blending will be applied.
     */
    void ForwardRenderManager::RenderMesh(RenderQueueEntry& entry, const LightingDescriptor& lightingDescriptor, const ViewDescriptor& viewDescriptor,
                                          MaterialRef  materialOverride, Bool flagRendered, FowardBlendingFilter blendingFilter) {
        Transform modelViewProjection;
        Transform modelView;
        Transform model;

        SceneObject* sceneObject = entry.Container;
        SubMesh3DRenderer* renderer = entry.Renderer;

        NONFATAL_ASSERT(sceneObject != nullptr, "ForwardRenderManager::RenderMesh -> Scene object is not valid.", true);
        NONFATAL_ASSERT(renderer != nullptr, "ForwardRenderManager::RenderMesh -> Null sub renderer encountered.", true);

        // if we have an override material, we use that for every mesh
        Bool doMaterialOvverride = materialOverride.IsValid() ? true : false;
        MaterialRef currentMaterial = doMaterialOvverride ? materialOverride : *entry.RenderMaterial;

        NONFATAL_ASSERT(currentMaterial != nullptr, "ForwardRenderManager::RenderMesh -> Null material encountered.", true);

        if (!ValidateRenderPassForRenderer(*renderer, currentMaterial)) return;

        // Build model, model-view and model-view-projection transforms
        SceneObjectProcessingDescriptor& processingDesc = sceneObject->GetProcessingDescriptor();
        model.SetTo(processingDesc.AggregateTransform);
        model.PreTransformBy(viewDescriptor.UniformWorldSceneObjectTransform);
        modelView.SetTo(model);
        modelView.PreTransformBy(viewDescriptor.ViewTransformInverse);
        modelViewProjection.SetTo(modelView);
        modelViewProjection.PreTransformBy(viewDescriptor.ProjectionTransform);

        // activate the material, which will switch the GPU's active shader to
        // the one associated with [currentMaterial]
        ActivateMaterial(currentMaterial, viewDescriptor.ReverseCulling);

        // send uniforms set for [currentMaterial] to its shader
        SendActiveMaterialUniformsToShader();

        // send light data to the active shader (if it needs it)
        if (lightingDescriptor.UseLighting) {
            if (currentMaterial->GetSinglePassMode() != SinglePassMode::None) {
                currentMaterial->SendLightsToShader(lightingDescriptor.PositionDatas, lightingDescriptor.DirectionDatas, lightingDescriptor.Types,
                                                    lightingDescriptor.ColorDatas, lightingDescriptor.Intensities, lightingDescriptor.Ranges,
                                                    lightingDescriptor.Attenuations, lightingDescriptor.ParallelAngleAttenuations,
                                                    lightingDescriptor.OrthoAngleAttenuations, lightingDescriptor.Enabled, lightingDescriptor.LightCount);
            }
            else {
                currentMaterial->SendLightToShader(*lightingDescriptor.LightObjects[0], lightingDescriptor.Positions[0], &lightingDescriptor.Directions[0]);
            }
        }

        // pass relevant transforms to shader
        SendTransformUniformsToShader(model, modelView, viewDescriptor.ViewTransformInverse, viewDescriptor.ProjectionTransform, modelViewProjection);

        // send view attributes to the active shader
        SendViewAttributesToShader(viewDescriptor);

        // apply additive or subtractive blending ONLY if the material has not specified its own blending mode
        if (currentMaterial->GetBlendingMode() == RenderState::BlendingMode::None) {
            // determine if this mesh has been rendered using [renderer] before
            Bool rendered = renderedSubRenderers[renderer->GetObjectID()];

            // if this sub mesh has already been rendered by this camera, then we want to use
            // additive blending to combine it with the output from other lights. Otherwise
            // turn off blending and render.
            if ((rendered && blendingFilter == FowardBlendingFilter::OnlyIfRendered) || (blendingFilter == FowardBlendingFilter::Always)) {
                if (GetForwardBlending() == FowardBlendingMethod::Subtractive) {
                    Engine::Instance()->GetGraphicsSystem()->SetBlendingEnabled(true);
                    Engine::Instance()->GetGraphicsSystem()->SetBlendingFunction(RenderState::BlendingMethod::Zero, RenderState::BlendingMethod::SrcAlpha);
                }
                else {
                    Engine::Instance()->GetGraphicsSystem()->SetBlendingEnabled(true);
                    Engine::Instance()->GetGraphicsSystem()->SetBlendingFunction(RenderState::BlendingMethod::One, RenderState::BlendingMethod::One);
                }
            }
            else {
                Engine::Instance()->GetGraphicsSystem()->SetBlendingEnabled(false);
            }
        }

        // render the current mesh
        renderer->Render();

        // flag the current mesh & renderer combo as being rendered (at least once)
        if (flagRendered) renderedSubRenderers[renderer->GetObjectID()] = true;
    }

    Bool ForwardRenderManager::ValidateRenderPassForRenderer(SubMesh3DRenderer& renderer, MaterialRef material) {
        Bool rendered = renderedSubRenderers[renderer.GetObjectID()];

        ForwardRenderPass forwardRenderPass = material->GetForwardRenderPass();
        switch (forwardRenderPass) {
            case ForwardRenderPass::All:
            break;
            case ForwardRenderPass::Base:
            if (rendered) return false;
            case ForwardRenderPass::Additive:
            if (!rendered) return false;
        }

        return true;
    }

    /*
     * Render the shadow volumes for the meshs attached to [entry] for [light]. This essentially means altering the
     * stencil buffer to reflect areas of the rendered scene that are shadowed from [light] by the mesh.
     *
     * [entry] - RenderQueueEntry object that contains the relevant renderable and transform information.
     * [lightPosition] - The world space position of [light].
     * [lightDirection] - The world space direction of [light].
     * [viewDescriptor] - Describes the view and perspective transforms for rendering the shadow volume, as well as several other properties.
     */
    void ForwardRenderManager::RenderShadowVolumeForMesh(RenderQueueEntry& entry, const Light& light, const Point3& lightPosition, const Vector3& lightDirection, const ViewDescriptor& viewDescriptor) {
        static const UniformID uniform_EPSILON = UniformDirectory::RegisterVarID("EPSILON");

        Transform modelViewProjection;
        Transform model;
        Transform modelInverse;

        SceneObject* sceneObject = entry.Container;
        SubMesh3DRenderer* renderer = entry.Renderer;
        SubMesh3D* mesh = entry.Mesh;

        NONFATAL_ASSERT(mesh != nullptr, "ForwardRenderManager::RenderShadowVolumeForMesh -> Renderer returned null mesh.", true);
        NONFATAL_ASSERT(sceneObject != nullptr, "ForwardRenderManager::RenderShadowVolumeForMesh -> Scene object is not valid.", true);
        NONFATAL_ASSERT(renderer != nullptr, "ForwardRenderManager::RenderShadowVolumeForMesh -> Null sub renderer encountered.", true);

        // if mesh doesn't have face data, it can't have a shadow volume
        if (!mesh->HasFaces())return;

        Mesh3DFilter* filter = entry.MeshFilter;
        NONFATAL_ASSERT(filter != nullptr, "ForwardRenderManager::RenderShadowVolumeForMesh -> Scene object has null mesh filter.", true);

        // calculate model transform and inverse model transform
        SceneObjectProcessingDescriptor& processingDesc = sceneObject->GetProcessingDescriptor();
        model.SetTo(processingDesc.AggregateTransform);
        model.PreTransformBy(viewDescriptor.UniformWorldSceneObjectTransform);
        modelInverse.SetTo(model);
        modelInverse.Invert();

        // calculate the position and/or direction of [light]
        // in the mesh's local space
        Point3 modelLocalLightPos = lightPosition;
        Vector3 modelLocalLightDir = lightDirection;
        modelInverse.TransformPoint(modelLocalLightPos);
        modelInverse.TransformVector(modelLocalLightDir);

        // build special MVP transform for rendering shadow volumes
        Real scaleFactor = filter->GetUseBackSetShadowVolume() ? .99f : .99f;
        BuildShadowVolumeMVPTransform(model, viewDescriptor.ViewTransformInverse, viewDescriptor.ProjectionTransform, modelViewProjection, scaleFactor, scaleFactor);

        // activate [shadowVolumeMaterial], which will switch the GPU's active shader to
        // the one associated with [shadowVolumeMaterial]
        ActivateMaterial(shadowVolumeMaterial, viewDescriptor.ReverseCulling);

        if (filter->GetUseCustomShadowVolumeOffset()) {
            // set the epsilon offset for the shadow volume shader based on custom value
            shadowVolumeMaterial->SetUniform1f(filter->GetCustomShadowVolumeOffset(), uniform_EPSILON);
        }
        else {
            // set the epsilon offset for the shadow volume shader based on type of shadow volume
            if (filter->GetUseBackSetShadowVolume())shadowVolumeMaterial->SetUniform1f(.00002f, uniform_EPSILON);
            else shadowVolumeMaterial->SetUniform1f(.2f, uniform_EPSILON);
        }

        // send uniforms set for the shadow volume material to its shader
        SendActiveMaterialUniformsToShader();
        // pass special shadow volume model-view-matrix to shader
        SendModelViewProjectionToShader(modelViewProjection);

        // send view attributes to the active shader
        SendViewAttributesToShader(viewDescriptor);

        // send shadow volume uniforms to shader
        shadowVolumeMaterial->SendLightToShader(light, modelLocalLightPos, &modelLocalLightDir);

        // form cache key from sub-renderer's object ID and light's object ID
        ObjectPairKey cacheKey;
        cacheKey.ObjectAID = renderer->GetObjectID();
        cacheKey.ObjectBID = light.GetObjectID();

        const Point3Array * cachedShadowVolume = nullptr;
        cachedShadowVolume = GetCachedShadowVolume(cacheKey);

        // render the shadow volume if it is valid
        if (cachedShadowVolume != nullptr) {
            renderer->RenderShadowVolume(cachedShadowVolume);
        }
    }

    /*
     * Build shadow volumes for all relevant meshes for all shadow casting lights.
     */
    void ForwardRenderManager::BuildSceneShadowVolumes() {
        // loop through each light in [sceneLights]
        for (UInt32 l = 0; l < lightCount; l++) {
            SceneObject* lightObject = sceneLights[l];
            NONFATAL_ASSERT(lightObject != nullptr, "ForwardRenderManager::BuildSceneShadowVolumes -> Light's scene object is not valid.", true);

            // verify the light is active
            if (lightObject->IsActive()) {
                LightRef lightRef = lightObject->GetLight();
                NONFATAL_ASSERT(lightRef.IsValid(), "ForwardRenderManager::BuildSceneShadowVolumes -> Light is not valid.", true);

                // verify that this light casts shadows
                if (lightRef->GetShadowsEnabled()) {
                    Transform lightWorldTransform;
                    SceneObjectProcessingDescriptor& processingDesc = lightRef->GetSceneObject()->GetProcessingDescriptor();
                    lightWorldTransform.SetTo(processingDesc.AggregateTransform);
                    BuildShadowVolumesForLight(lightRef.GetRef(), lightWorldTransform);
                }
            }
        }
    }

    /*
     * Build shadow volumes for all relevant meshes for [light]. Use [lightWorldTransform] as the
     * local-to-world transformation for the light.
     */
    void ForwardRenderManager::BuildShadowVolumesForLight(const Light& light, const Transform& lightWorldTransform) {
        Point3 lightWorldPosition;
        lightWorldTransform.TransformPoint(lightWorldPosition);

        Vector3 lightDirection = light.GetDirection();
        lightWorldTransform.TransformVector(lightDirection);

        for (RenderQueueManager::ConstIterator itr = renderQueueManager.Begin(); itr != renderQueueManager.End(); ++itr) {
            RenderQueueEntry* entry = *itr;
            NONFATAL_ASSERT(entry != nullptr, "ForwardRenderManager::BuildShadowVolumesForLight -> Null render queue entry encountered.", true);

            SceneObject* sceneObject = entry->Container;

            if (sceneObject->IsActive()) {
                // copy the full world transform of the scene object, including those of all ancestors
                SceneObjectProcessingDescriptor& processingDesc = sceneObject->GetProcessingDescriptor();
                Transform sceneObjectWorldTransform;
                sceneObjectWorldTransform.SetTo(processingDesc.AggregateTransform);

                // make sure the current mesh should not be culled from [light].
                if (!ShouldCullFromLightByLayer(light, *sceneObject) &&
                    !ShouldCullFromLightByPosition(light, lightWorldPosition, *entry->Mesh, sceneObjectWorldTransform)) {
                    BuildShadowVolumesForMesh(*entry, light, lightWorldPosition, lightDirection);
                }
            }
        }
    }

    /*
     * Build (and cache) shadow volumes for the meshes attached to [sceneObject] for [light], using
     * [lightPosition] as the light's world position.
     */
    void ForwardRenderManager::BuildShadowVolumesForMesh(RenderQueueEntry& entry, const Light& light, const Point3& lightPosition, const Vector3& lightDirection) {
        SubMesh3DRenderer *renderer = entry.Renderer;
        NONFATAL_ASSERT(renderer != nullptr, "ForwardRenderManager::BuildShadowVolumesForSceneObject -> Null renderer encountered.", true);

        SceneObject * container = entry.Container;
        NONFATAL_ASSERT(container != nullptr, "ForwardRenderManager::BuildShadowVolumesForSceneObject -> Null scene object encountered.", true);

        SubMesh3D * mesh = entry.Mesh;
        Mesh3DFilterRef filter = container->GetMesh3DFilter();

        NONFATAL_ASSERT(mesh != nullptr, "ForwardRenderManager::BuildShadowVolumesForSceneObject -> Null mesh encountere.", true);
        NONFATAL_ASSERT(filter != nullptr, "ForwardRenderManager::BuildShadowVolumesForSceneObject -> Null mesh filter encountered.", true);

        // if mesh doesn't have face data, it can't have a shadow volume
        if (!mesh->HasFaces())return;

        Light& castLight = const_cast<Light&>(light);
        SceneObjectRef lightObject = castLight.GetSceneObject();

        // a shadow volume is dynamic if the light's scene object is not static or the mesh's
        // scene object is not static
        Bool dynamic = !(container->IsStatic()) || !(lightObject->IsStatic());

        ObjectPairKey cacheKey;
        Bool cached = false;

        // form cache key from sub-renderer's object ID and light's object ID
        cacheKey.ObjectAID = renderer->GetObjectID();
        cacheKey.ObjectBID = light.GetObjectID();

        const Point3Array * cachedShadowVolume = GetCachedShadowVolume(cacheKey);

        // check if this shadow volume is already cached
        if (cachedShadowVolume != nullptr) {
            cached = true;
        }

        // if the mesh is dynamic we must always rebuild the shadow volume
        if (cached && !dynamic) {
            return;
        }

        Transform modelViewProjection;
        Transform modelView;
        Transform model;
        Transform modelInverse;

        // calculate model transform and inverse model transform
        SceneObjectProcessingDescriptor& processingDesc = container->GetProcessingDescriptor();
        model.SetTo(processingDesc.AggregateTransform);
        modelInverse.SetTo(model);
        modelInverse.Invert();

        // calculate the position and/or direction of [light]
        // in the mesh's local space
        Point3 modelLocalLightPos = lightPosition;
        Vector3 modelLocalLightDir = lightDirection;
        modelInverse.TransformPoint(modelLocalLightPos);
        modelInverse.TransformVector(modelLocalLightDir);

        Vector3 lightPosDir;
        if (light.GetType() == LightType::Directional) {
            lightPosDir.x = modelLocalLightDir.x;
            lightPosDir.y = modelLocalLightDir.y;
            lightPosDir.z = modelLocalLightDir.z;
        }
        else {
            lightPosDir.x = modelLocalLightPos.x;
            lightPosDir.y = modelLocalLightPos.y;
            lightPosDir.z = modelLocalLightPos.z;
        }

        // calculate shadow volume geometry
        renderer->BuildShadowVolume(lightPosDir, light.GetType() == LightType::Directional || light.GetType() == LightType::Planar, filter->GetUseBackSetShadowVolume());

        // cache shadow volume for later rendering
        CacheShadowVolume(cacheKey, renderer->GetShadowVolumePositions());
    }

    /*
     * Build the model-view-projection matrix that is used when rendering shadow volumes.
     * It is a special matrix that 'narrows' the base shadow volume to avoid Z-fighting artifacts.
     *
     * Multiplying the mesh geometry by this rotation matrix and then scaling X & Y ever so slightly has an
     * effect similar to 'narrowing' the shadow volume around the mesh-to-light vector. This mitigates artifacts where
     * the shadow volume's sides are very close to and parallel to mesh polygons and Z-fighting occurs.
     *
     * [modelTransform] - The transform from model space to world space.
     * [viewTransformInverse] - The inverse of the view transform.
     * [outTransform] - The output model-view-projection Transform.
     * [xScale] - factor by which to scale the shadow volume along the x-axis.
     * [yScale] - factor by which to scale the shadow volume along the y-axis.
     */
    void ForwardRenderManager::BuildShadowVolumeMVPTransform(const Transform& modelTransform, const Transform& viewTransformInverse, const Transform& projectionTransform,
                                                             Transform& outTransform, Real xScale, Real yScale) const {
        Transform modelView;
        Transform shadowVolumeViewTransform;

        shadowVolumeViewTransform.Scale(xScale, yScale, 1.00, true);
        modelView.SetTo(shadowVolumeViewTransform);
        modelView.PreTransformBy(modelTransform);
        modelView.PreTransformBy(viewTransformInverse);
        outTransform.SetTo(modelView);
        outTransform.PreTransformBy(projectionTransform);
    }

    /*
     * Store a copy of a shadow volume in [shadowVolumeCache], keyed by [key].
     */
    void ForwardRenderManager::CacheShadowVolume(const ObjectPairKey& key, const Point3Array * shadowVolume) {
        NONFATAL_ASSERT(shadowVolume != nullptr, "ForwardRenderManager::CacheShadowVolume -> Shadow volume is null.", true);

        Bool needsInit = false;
        Point3Array * target = nullptr;

        if (!HasCachedShadowVolume(key)) {
            needsInit = true;
        }
        else {
            target = shadowVolumeCache[key];
            if (target->GetReservedCount() != shadowVolume->GetReservedCount()) {
                ClearCachedShadowVolume(key);
                needsInit = true;
            }
        }

        if (needsInit) {
            target = new(std::nothrow) Point3Array();
            ASSERT(target != nullptr, "ForwardRenderManager::CacheShadowVolume -> Unable to allocate shadow volume copy.");

            Bool initSuccess = target->Init(shadowVolume->GetReservedCount());
            ASSERT(initSuccess, "ForwardRenderManager::CacheShadowVolume -> Unable to initialize shadow volume copy.");

            target->SetCount(shadowVolume->GetCount());
            shadowVolumeCache[key] = target;
        }

        target->SetCount(shadowVolume->GetCount());
        shadowVolume->CopyTo(target);
    }

    /*
     * Remove the shadow volume cached for [key] (if it exists).
     */
    void ForwardRenderManager::ClearCachedShadowVolume(const ObjectPairKey& key) {
        if (HasCachedShadowVolume(key)) {
            Point3Array* shadowVolume = shadowVolumeCache[key];
            shadowVolumeCache.erase(key);
            if (shadowVolume != nullptr) {
                delete shadowVolume;
            }
        }
    }

    /*
     * Is a shadow volume cached for [key].
     */
    Bool ForwardRenderManager::HasCachedShadowVolume(const ObjectPairKey& key)  const {
        if (shadowVolumeCache.find(key) != shadowVolumeCache.end()) {
            return true;
        }

        return false;
    }

    /*
     * Get cached shadow volume for [key].
     */
    const Point3Array * ForwardRenderManager::GetCachedShadowVolume(const ObjectPairKey& key) {
        if (HasCachedShadowVolume(key)) {
            const Point3Array * shadowVolume = shadowVolumeCache[key];
            return shadowVolume;
        }

        return nullptr;
    }

    /*
     * Remove and delete all cached shadow volumes.
     */
    void ForwardRenderManager::DestroyCachedShadowVolumes() {
        for (unsigned i = 0; i < shadowVolumeCache.bucket_count(); ++i) {
            for (auto iter = shadowVolumeCache.begin(i); iter != shadowVolumeCache.end(i); ++iter) {
                Point3Array * shadowVolume = iter->second;
                delete shadowVolume;
            }
        }
        shadowVolumeCache.clear();
    }

    /*
     * Set the blending method to be used in forward rendering.
     */
    void ForwardRenderManager::SetForwardBlending(FowardBlendingMethod method) {
        forwardBlending = method;
    }

    /*
     * Get the blending method to be used in forward rendering.
     */
    FowardBlendingMethod ForwardRenderManager::GetForwardBlending() const {
        return forwardBlending;
    }

    /*
     * Should the meshes on [sceneObject] be rendered? This is determined by comparing the
     * layer mask of [sceneObject] with [cullingMask].
     */
    Bool ForwardRenderManager::ShouldCullByLayer(IntMask cullingMask, const SceneObject& sceneObject) const {
        // make sure cullingMask includes at least one of layers of [sceneObject]
        return !(Engine::Instance()->GetEngineObjectManager()->GetLayerManager().AtLeastOneLayerInCommon(sceneObject.GetLayerMask(), cullingMask));
    }

    /*
     * Check if [mesh] should be rendered with [light], based on the distance of the center of [mesh] from [lightPosition].
     */
    Bool ForwardRenderManager::ShouldCullFromLightByPosition(const Light& light, const Point3& lightWorldPosition, const SubMesh3D& mesh, const Transform& meshWorldTransform) const {
        if (light.GetType() == LightType::Directional || light.GetType() == LightType::Ambient) {
            return false;
        }

        LightCullType cullType = LightCullType::BoundingBox;

        switch (cullType) {
            case LightCullType::None:
            return false;
            break;
            case LightCullType::BoundingBox:
            return ShouldCullByBoundingBox(light, lightWorldPosition, meshWorldTransform, mesh);
            break;
            case LightCullType::Tiled:
            return ShouldCullByTile(light, lightWorldPosition, meshWorldTransform, mesh);
            break;
            default:
            return false;
            break;
        }

        return false;
    }

    /*
    * Check if [sceneObject] should be rendered with [light], based on the culling mask of the light and the layer to
    * which [sceneObject] belongs.
    */
    Bool ForwardRenderManager::ShouldCullFromLightByLayer(const Light& light, const SceneObject& sceneObject) const {
        // exclude objects that have layer masks that are not compatible
        // with the culling mask of [light].
        IntMask cullingMask = light.GetCullingMask();
        return ShouldCullByLayer(cullingMask, sceneObject);
    }

    /*
     * Cull light based on distance of center of [mesh] from [light]. Each mesh has
     * a bounding box and if that bounding box does not intersect with the sphere that is
     * formed by the light's range, then the mesh should be culled from the light. This method cheats in
     * that it takes the distance to one of the corners of the bounding box and uses that as the radius for
     * a sphere to do bounding sphere culling.
     *
     * [lightPosition] - World space position of the light.
     * [meshWorldTransform] - Used to transform the bounding box of [mesh] from local to world space.
     */
    Bool ForwardRenderManager::ShouldCullByBoundingBox(const Light& light, const Point3& lightPosition, const Transform& meshWorldTransform, const SubMesh3D& mesh) const {
        if (light.GetType() == LightType::Planar || light.GetType() == LightType::Point) {
            Point3 localMeshCenter = mesh.GetCenter();

            Vector3 boundingBox = mesh.GetBoundingBox();
            Point3 corners[8];
            corners[0].Set(boundingBox.x + localMeshCenter.x, boundingBox.y + localMeshCenter.y, boundingBox.z + localMeshCenter.z);
            corners[1].Set(boundingBox.x + localMeshCenter.x, boundingBox.y + localMeshCenter.y, -boundingBox.z + localMeshCenter.z);
            corners[2].Set(boundingBox.x + localMeshCenter.x, -boundingBox.y + localMeshCenter.y, -boundingBox.z + localMeshCenter.z);
            corners[3].Set(boundingBox.x + localMeshCenter.x, -boundingBox.y + localMeshCenter.y, boundingBox.z + localMeshCenter.z);
            corners[4].Set(-boundingBox.x + localMeshCenter.x, boundingBox.y + localMeshCenter.y, boundingBox.z + localMeshCenter.z);
            corners[5].Set(-boundingBox.x + localMeshCenter.x, boundingBox.y + localMeshCenter.y, -boundingBox.z + localMeshCenter.z);
            corners[6].Set(-boundingBox.x + localMeshCenter.x, -boundingBox.y + localMeshCenter.y, -boundingBox.z + localMeshCenter.z);
            corners[7].Set(-boundingBox.x + localMeshCenter.x, -boundingBox.y + localMeshCenter.y, boundingBox.z + localMeshCenter.z);

            // transform the mesh's center into world space
            Point3 meshCenter = localMeshCenter;
            meshWorldTransform.TransformPoint(meshCenter);

            Real maxRadiusSqr = 0.0f;
            Vector3 maxRadiusComponents;
            for (UInt32 i = 0; i < 8; i++) {
                Point3& corner = corners[i];
                meshWorldTransform.TransformPoint(corner);

                // use a corner of the bounding box to form a vector from the bounding box's center, and use the
                // vector as the radius for a bounding sphere to do bounding sphere culling
                Vector3 radius;
                Point3::Subtract(corner, meshCenter, radius);

                // check if this is the largest radius found so far
                Real localRadiusSqr = radius.x * radius.x + radius.y * radius.y + radius.z * radius.z;
                if (localRadiusSqr > maxRadiusSqr) {
                    maxRadiusSqr = localRadiusSqr;
                    maxRadiusComponents = radius;
                }
            }

            if (light.GetType() == LightType::Planar) {
                Vector3 lightDirection = light.GetDirection();

                // project vector from origin to mesh center on light's direction vector
                Real parallelPortion = meshCenter.x * lightDirection.x + meshCenter.y * lightDirection.y + meshCenter.z * lightDirection.z;
                // get sqaured length of projected vector
                Real parallelPortionSqr = parallelPortion * parallelPortion;
                // max distance from the plane is based on attenuation, not the light's range
                Real planarRange = light.GetIntensity() / light.GetAttenuation();
                // get squared planar range
                Real planarRangeSqr = planarRange * planarRange;
                // calculate offset of planar light's position from origin
                Real d = lightPosition.x * lightDirection.x + lightPosition.y * lightDirection.y + lightPosition.z * lightDirection.z;
                // calculate squared offset
                Real dSqr = d * d;
                // check if mesh's center is within +/- (radiusSqr + planarRangeSqr) of the light's plane
                if (GTEMath::QuickSquareRoot(parallelPortionSqr) - GTEMath::QuickSquareRoot(dSqr) >
                    GTEMath::QuickSquareRoot(planarRangeSqr) + GTEMath::QuickSquareRoot(maxRadiusSqr)) {
                    return true;
                }

                // the radial range (max distance from the light's center in the light's plane) is
                // based on the light's range value
                Real radialRange = light.GetRange();
                // calculate squared radial range
                Real radialRangeSqr = radialRange * radialRange;
                // calculate distance from mesh's center to light's position in the light's plane
                Vector3 lightPosToCenter;
                Point3::Subtract(meshCenter, lightPosition, lightPosToCenter);
                // project [lightPosToCenter] on to the light's direction vector
                parallelPortion = lightPosToCenter.x * lightDirection.x + lightPosToCenter.y * lightDirection.y + lightPosToCenter.z * lightDirection.z;
                parallelPortionSqr = parallelPortion * parallelPortion;
                // calculate squared distance of [lightPosToCenter]
                Real lightPosToCenterLengthSqr = lightPosToCenter.x * lightPosToCenter.x + lightPosToCenter.y * lightPosToCenter.y + lightPosToCenter.z + lightPosToCenter.z;
                // get suared projected sistance of [lightPosToCenter] in light's plane
                Real portionPlanarSqr = lightPosToCenterLengthSqr - parallelPortionSqr;
                // check if mesh's center is within +/- (radialRangeSqr + radiusSqr) of the light's position
                if (GTEMath::QuickSquareRoot(portionPlanarSqr) > GTEMath::QuickSquareRoot(radialRangeSqr) + GTEMath::QuickSquareRoot(maxRadiusSqr)) {
                    return true;
                }

                return false;
            }
            else if (light.GetType() == LightType::Point) {
                Real lightRange = light.GetRange();
                Real maxRange = GTEMath::QuickSquareRoot(maxRadiusSqr) + lightRange;

                Vector3 toCenter;
                Point3::Subtract(meshCenter, lightPosition, toCenter);
                Real rangeSquare = toCenter.x * toCenter.x + toCenter.y * toCenter.y + toCenter.z * toCenter.z;
                Real range = GTEMath::QuickSquareRoot(rangeSquare);

                return range > maxRange;
            }
        }

        return false;
    }

    /*
     * TODO: (Eventually) - Implement tile-base culling.
     */
    Bool ForwardRenderManager::ShouldCullByTile(const Light& light, const Point3& lightPosition, const Transform& meshWorldTransform, const SubMesh3D& mesh) const {
        return false;
    }

    /*
     * Send relevant scene transforms the active shader.
     * The binding information stored in the active material holds the shader variable locations for these matrices.
     */
    void ForwardRenderManager::SendTransformUniformsToShader(const Transform& model, const Transform& modelView, const Transform& view, const Transform& projection, const Transform& modelViewProjection) {
        MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
        ASSERT(activeMaterial.IsValid(), "ForwardRenderManager::SendTransformUniformsToShader -> Active material is null.");

        ShaderRef shader = activeMaterial->GetShader();
        ASSERT(shader.IsValid(), "ForwardRenderManager::SendTransformUniformsToShader -> Active material contains null shader.");

        Matrix4x4 modelInverseTranspose = model.GetConstMatrix();
        modelInverseTranspose.Transpose();
        modelInverseTranspose.Invert();

        activeMaterial->SendModelMatrixInverseTransposeToShader(modelInverseTranspose);
        activeMaterial->SendModelMatrixToShader(model.GetConstMatrix());
        activeMaterial->SendModelViewMatrixToShader(modelView.GetConstMatrix());
        activeMaterial->SendViewMatrixToShader(view.GetConstMatrix());
        activeMaterial->SendProjectionMatrixToShader(projection.GetConstMatrix());
        activeMaterial->SendMVPMatrixToShader(modelViewProjection.GetConstMatrix());
    }

    /*
     * Send only the full model-view-projection matrix stored in [modelViewProjection] to the active shader.
     * The binding information stored in the active material holds the shader variable location for this matrix;
     */
    void ForwardRenderManager::SendModelViewProjectionToShader(const Transform& modelViewProjection) {
        MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
        ASSERT(activeMaterial.IsValid(), "ForwardRenderManager::SendModelViewProjectionToShader -> Active material is null.");

        ShaderRef shader = activeMaterial->GetShader();
        ASSERT(shader.IsValid(), "ForwardRenderManager::SendModelViewProjectionToShader -> Active material contains null shader.");

        activeMaterial->SendMVPMatrixToShader(modelViewProjection.GetConstMatrix());
    }

    /*
     * Send the clip plane and view position in [viewDescriptor] to the active shader..
     */
    void ForwardRenderManager::SendViewAttributesToShader(const ViewDescriptor& viewDescriptor) {
        MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
        ASSERT(activeMaterial.IsValid(), "ForwardRenderManager::SendViewAttributesToShader -> Active material is null.");

        ShaderRef shader = activeMaterial->GetShader();
        ASSERT(shader.IsValid(), "ForwardRenderManager::SendViewAttributesToShader -> Active material contains null shader.");

        // for now we only support up to one clip plane
        //TODO: Add support for > 1 clip plane
        if (viewDescriptor.ClipPlaneCount > 0) {
            Engine::Instance()->GetGraphicsSystem()->DeactiveAllClipPlanes();
            Engine::Instance()->GetGraphicsSystem()->AddClipPlane();

            activeMaterial->SendClipPlaneToShader(0, viewDescriptor.ClipPlane0Normal.x, viewDescriptor.ClipPlane0Normal.y, viewDescriptor.ClipPlane0Normal.z, viewDescriptor.ClipPlane0Offset);
            activeMaterial->SendClipPlaneCountToShader(1);
        }
        else {
            Engine::Instance()->GetGraphicsSystem()->DeactiveAllClipPlanes();
            activeMaterial->SendClipPlaneToShader(0, 0, 0, 0, 0);
            activeMaterial->SendClipPlaneCountToShader(0);
        }

        activeMaterial->SendEyePositionToShader(&viewDescriptor.ViewPosition);
    }

    /*
     * Send any custom uniforms specified by the active material to the active shader
     */
    void ForwardRenderManager::SendActiveMaterialUniformsToShader()  const {
        MaterialRef activeMaterial = Engine::Instance()->GetGraphicsSystem()->GetActiveMaterial();
        ASSERT(activeMaterial.IsValid(), "ForwardRenderManager::SendCustomUniformsToShader -> Active material is not valid.");
        activeMaterial->SendAllStoredUniformValuesToShader();
    }

    /*
     * Activate [material], which will switch the GPU's active shader to
     * the one associated with it.
     */
    void ForwardRenderManager::ActivateMaterial(MaterialRef material, Bool reverseFaceCulling) {
        // We MUST notify the graphics system about the change in active material because other
        // components (like Mesh3DRenderer) need to know about the active material
        Engine::Instance()->GetGraphicsSystem()->ActivateMaterial(material, reverseFaceCulling);
    }
}

