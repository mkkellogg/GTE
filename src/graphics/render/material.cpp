#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "material.h"
#include "graphics/shader/shader.h"
#include "graphics/stdattributes.h"
#include "ui/debug.h"
#include "global/global.h"
#include "graphics/shader/uniformdesc.h"
#include "graphics/shader/attributedesc.h"
#include "graphics/render/vertexattrbuffer.h"

Material::Material(const char * materialName)
{
	if(materialName == NULL)materialName = "_UnnamedMaterial";
	int index =0;
	while(materialName != NULL && index < MATERIAL_NAME_MAX_LENGTH-1)
	{
		this->materialName[index] = materialName[0];
		index++;
		materialName++;
	}

	shader = NULL;
	ClearStandardBindings();

	attributesSetAndVerified = false;
	attributesSetValues = NULL;

	uniformsSetAndVerified = false;
	uniformsSetValues = NULL;
}

Material::~Material()
{
	for(unsigned int i=0; i < setUniforms.size(); i++)
	{
		UniformDescriptor * desc = setUniforms[i];
		if(desc != NULL)
		{
			delete desc;
		}
	}
	setUniforms.clear();

	SAFE_DELETE(attributesSetValues);
	SAFE_DELETE(uniformsSetValues);
}

bool Material::Init(Shader * shader)
{
	if(shader == NULL)
	{
		Debug::PrintError(" Material::Init(Shader *) -> tried to Init with NULL shader.");
		return false;
	}

	if(!shader->IsLoaded())
	{
		Debug::PrintError(" Material::Init(Shader *) -> tried to Init with unloaded shader.");
		return false;
	}

	this->shader = shader;

	ClearStandardBindings();
	BindStandardVars();

	if(!SetupSetVerifiers())return false;

	return true;
}

bool Material::SetupSetVerifiers()
{
	NULL_CHECK(shader,"Material::SetupSetVerifiers -> shader is NULL", false);

	unsigned int attributeCount = shader->GetAttributeCount();
	unsigned int uniformCount = shader->GetUniformCount();

	attributesSetValues = new int[attributeCount];
	NULL_CHECK(attributesSetValues,"Material::SetupSetVerifiers -> could not allocate attributesSetValues", false);

	uniformsSetValues = new int[uniformCount];
	NULL_CHECK(uniformsSetValues,"Material::SetupSetVerifiers -> could not allocate uniformsSetValues", false);

	ClearSetVerifiers();

	for(unsigned int i=0; i < attributeCount; i ++)
	{
		const AttributeDescriptor * desc = shader->GetAttributeDescriptor(i);
		attributeLocationsToVerificationIndex[desc->ShaderVarID] = i;
	}

	for(unsigned int i=0; i < uniformCount; i ++)
	{
		const UniformDescriptor * desc = shader->GetUniformDescriptor(i);
		uniformLocationsToVerificationIndex[desc->ShaderVarID] = i;
	}

	return true;
}

void Material::SetAttributeSetValue(int varID, int size)
{
	int varIndex = attributeLocationsToVerificationIndex[varID];
	if(varIndex >=0)
	{
		attributesSetValues[varIndex] = size;
	}
}

void Material::SetUniformSetValue(int varID, int size)
{
	int varIndex = uniformLocationsToVerificationIndex[varID];
	if(varIndex >=0)
	{
		uniformsSetValues[varIndex] = size;
	}
}

void Material::ClearSetVerifiers()
{
	if(attributesSetValues != NULL && shader != NULL)memset(attributesSetValues, 0, sizeof(int) * shader->GetAttributeCount());
	if(uniformsSetValues != NULL && shader != NULL)memset(uniformsSetValues, 0, sizeof(int) * shader->GetUniformCount());
}

void Material::ClearStandardBindings()
{
	for(int i=0; i < VAR_BINDINGS_SIZE; i++)standardAttributeBindings[i] = -1;
	for(int i=0; i < VAR_BINDINGS_SIZE; i++)standardUniformBindings[i] = -1;
}

void Material::SetStandardAttributeBinding(int varID, StandardAttribute attr)
{
	standardAttributeBindings[(int)attr] = varID;
}

int Material::GetStandardAttributeBinding(StandardAttribute attr) const
{
	return standardAttributeBindings[(int)attr];
}

void Material::BindStandardVars()
{
	standardAttributes = StandardAttributes::CreateAttributeSet();
	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;

		int varID = TestForStandardAttribute(attr);
		if(varID >= 0)
		{
			SetStandardAttributeBinding(varID, attr);
			StandardAttributes::AddAttribute(&standardAttributes,attr);
		}
	}

	standardUniforms = StandardUniforms::CreateUniformSet();
	for(int i=0; i<(int)StandardUniform::_Last; i++)
	{
		StandardUniform uniform = (StandardUniform)i;

		int varID = TestForStandardUniform(uniform);
		if(varID >= 0)
		{
			SetStandardUniformBinding(varID, uniform);
			StandardUniforms::AddUniform(&standardUniforms,uniform);
		}
	}
}

int Material::TestForStandardAttribute(StandardAttribute attr) const
{
	const char * attrName = StandardAttributes::GetAttributeName(attr);
	int varID = shader->GetAttributeVarID(attrName);

	return varID;
}

void Material::SetStandardUniformBinding( int varID, StandardUniform uniform)
{
	standardUniformBindings[(int)uniform] = varID;
}

int Material::GetStandardUniformBinding(StandardUniform uniform) const
{
	return standardUniformBindings[(int)uniform];
}

int Material::TestForStandardUniform(StandardUniform uniform) const
{
	const char * uniformName = StandardUniforms::GetUniformName(uniform);
	int loc = shader->GetUniformVarID(uniformName);

	return loc;
}

StandardAttributeSet Material::GetStandardAttributes() const
{
	return standardAttributes;
}

Shader * Material::GetShader() const
{
	return shader;
}

int Material::GetStandardAttributeShaderVarLocation(StandardAttribute attr) const
{
	return GetStandardAttributeBinding(attr);
}

void Material::SendStandardAttributeBufferToShader(StandardAttribute attr, VertexAttrBuffer *buffer)
{
	int varID = GetStandardAttributeBinding(attr);
	if(varID >= 0)
	{
		shader->SendBufferToShader(varID, buffer);
		SetUniformSetValue(varID, buffer->GetVertexCount() * (buffer->GetComponentCount() + buffer->GetStride()));
	}
}

int Material::GetStandardUniformShaderVarLocation(StandardUniform uniform) const
{
	return  GetStandardUniformBinding(uniform);
}

StandardUniformSet Material::GetStandardUniforms() const
{
	return standardUniforms;
}

void Material::SendSetUniformToShader(unsigned int index)
{
	if(index >=0 && index < setUniforms.size())
	{
		UniformDescriptor * desc = setUniforms[index];
		NULL_CHECK_RTRN(desc, "Material::SendSetUniformToShader -> uniform descriptor is NULL");

		if(desc->Type == UniformType::Sampler2D)
		{
			shader->SendUniformToShader(desc->ShaderVarID, desc->SamplerData);
			SetUniformSetValue(desc->ShaderVarID, SAMPLER_2D_DATA_SIZE);
		}
	}
}

void Material::SendAllSetUniformsToShader()
{
	for(unsigned int i=0; i < GetSetUniformCount(); i++)
	{
		SendSetUniformToShader(i);
	}
}

void Material::SendModelViewMatrixToShader(const Matrix4x4 * mat)
{
	int varID = GetStandardUniformBinding(StandardUniform::ModelViewMatrix);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, mat);
		SetUniformSetValue(varID, 16);
	}
}

void Material::SendProjectionMatrixToShader(const Matrix4x4 * mat)
{
	int varID = GetStandardUniformBinding(StandardUniform::ProjectionMatrix);
	if(varID)
	{
		shader->SendUniformToShader(varID, mat);
		SetUniformSetValue(varID, 16);
	}
}

void Material::SendMVPMatrixToShader(const Matrix4x4 * mat)
{
	int varID = GetStandardUniformBinding(StandardUniform::ModelViewProjectionMatrix);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, mat);
		SetUniformSetValue(varID, 16);
	}
}

void Material::SetTexture(Texture * texture, const char *shaderVarName)
{
	int loc = shader->GetUniformVarID(shaderVarName);
	if(loc < 0)
	{
		char msg[128];
		sprintf(msg, "Could not find shader sampler var: '%s' for material: '%s'", shaderVarName, materialName);
		Debug::PrintError(msg);
		return;
	}

	UniformDescriptor * desc = new UniformDescriptor();
	if(desc == NULL)
	{
		Debug::PrintError("Material::AddTexture -> could not allocate UniformDescriptor");
		return;
	}

	desc->ShaderVarID = loc;
	desc->Type = UniformType::Sampler2D;
	desc->SamplerData = texture;
	setUniforms.push_back(desc);
}

unsigned int Material::GetSetUniformCount() const
{
	return setUniforms.size();
}

UniformDescriptor * Material::GetSetUniform(unsigned int index)
{
	if(index >=0 && index < GetSetUniformCount())
	{
		return setUniforms[index];
	}
	return NULL;
}
