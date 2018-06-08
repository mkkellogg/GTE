#include "rendermanager.h"
#include "global/global.h"
#include "global/assert.h"
#include "global/constants.h"
#include "debug/gtedebug.h"

namespace GTE {
    RenderManager::RenderManager() {

    }

    RenderManager::~RenderManager() {

    }

    void RenderManager::SetCurrentCamera(CameraRef camera) {
        currentCamera = camera;
    }

    CameraRef RenderManager::GetCurrentCamera() {
        return currentCamera;
    }
}
