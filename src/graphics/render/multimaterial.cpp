#include "multimaterial.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"

namespace GTE
{
	MultiMaterial::MultiMaterial()
	{

	}

	MultiMaterial::~MultiMaterial()
	{

	}

	UInt32 MultiMaterial::GetMaterialCount()
	{
		return (UInt32)materials.size();
	}

	void MultiMaterial::AddMaterial(MaterialRef material)
	{
		materials.push_back(material);
	}

	void MultiMaterial::SetMaterial(UInt32 index, MaterialRef material)
	{
		NONFATAL_ASSERT(index < GetMaterialCount(), "MultiMaterial::GetMaterial -> 'shader' is null.", true);

		materials[index] = material;
	}

	MaterialRef MultiMaterial::GetMaterial(UInt32 index)
	{
		NONFATAL_ASSERT_RTRN(index < GetMaterialCount(), "MultiMaterial::GetMaterial -> 'shader' is null.", NullMaterialRef, true);

		return materials[index];
	}
}

