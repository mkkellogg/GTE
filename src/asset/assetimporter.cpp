#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#include "graphics/stdattributes.h"
#include <IL/il.h>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "assetimporter.h"
#include "importutil.h"
#include "geometry/matrix4x4.h"
#include "object/engineobjectmanager.h"
#include "object/shadermanager.h"
#include "object/sceneobjectcomponent.h"
#include "object/sceneobject.h"
#include "object/enginetypes.h"
#include "graphics/render/submesh3Drenderer.h"
#include "graphics/object/submesh3D.h"
#include "graphics/render/mesh3Drenderer.h"
#include "graphics/object/mesh3D.h"
#include "geometry/sceneobjecttransform.h"
#include "graphics/uv/uv2array.h"
#include "graphics/render/material.h"
#include "graphics/image/rawimage.h"
#include "filesys/filesystem.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/color/color4.h"
#include "graphics/uv/uv2.h"
#include "geometry/point/point3array.h"
#include "geometry/vector/vector3array.h"
#include "graphics/color/color4array.h"
#include "graphics/uv/uv2array.h"
#include "base/longmask.h"
#include "graphics/render/skinnedmesh3Dattrtransformer.h"
#include "modelimporter.h"
#include "global/global.h"
#include "ui/debug.h"

AssetImporter::AssetImporter()
{

}

AssetImporter::~AssetImporter()
{

}

SceneObjectRef AssetImporter::LoadModelDirect(const std::string& filePath, float importScale)
{
	ModelImporter importer;
	return importer.LoadModelDirect(filePath, importScale);
}


