#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "material.h"
#include "graphics/shader/shader.h"
#include "graphics/attributes.h"
#include "ui/debug.h"
#include "graphics/render/uniformdesc.h"

Material::Material(const char * materialName)
{
	currentSamplerLoc = 0;
	if(materialName == NULL)materialName = "_UnnamedMaterial";
	int index =0;
	while(materialName != NULL && index < 31)
	{
		this->materialName[index] = materialName[0];
		index++;
		materialName++;
	}

	shader = NULL;
	ClearBindings();
}

Material::~Material()
{
	for(unsigned int i=0; i < customUniforms.size(); i++)
	{
		UniformDescriptor * desc = customUniforms[i];
		if(desc != NULL)
		{
			delete desc;
		}
	}
	customUniforms.clear();
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
	ClearBindings();
	BindVars();

	return true;
}

void Material::ClearBindings()
{
	for(int i=0; i < VAR_BINDINGS_SIZE; i++)standardAttributeBindings[i] = -1;
	for(int i=0; i < VAR_BINDINGS_SIZE; i++)standardUniformBindings[i] = -1;
}

void Material::SetStandardAttributeBinding(int location, StandardAttribute attr)
{
	standardAttributeBindings[(int)attr] = location;
}

int Material::GetStandardAttributeBinding(StandardAttribute attr) const
{
	return standardAttributeBindings[(int)attr];
}

void Material::BindVars()
{
	standardAttributes = StandardAttributes::CreateAttributeSet();
	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;

		int loc = TestForStandardAttribute(attr);
		if(loc >= 0)
		{
			SetStandardAttributeBinding(loc, attr);
			StandardAttributes::AddAttribute(&standardAttributes,attr);
		}
	}

	standardUniforms = StandardUniforms::CreateUniformSet();
	for(int i=0; i<(int)StandardUniform::_Last; i++)
	{
		StandardUniform uniform = (StandardUniform)i;

		int loc = TestForStandardUniform(uniform);
		if(loc >= 0)
		{
			SetStandardUniformBinding(loc, uniform);
			StandardUniforms::AddUniform(&standardUniforms,uniform);
		}
	}
}

int Material::TestForStandardAttribute(StandardAttribute attr) const
{
	const char * attrName = StandardAttributes::GetAttributeName(attr);
	int loc = shader->GetAttributeVarLocation(attrName);

	return loc;
}

void Material::SetStandardUniformBinding( int location, StandardUniform uniform)
{
	standardUniformBindings[(int)uniform] = location;
}

int Material::GetStandardUniformBinding(StandardUniform uniform) const
{
	return standardUniformBindings[(int)uniform];
}

int Material::TestForStandardUniform(StandardUniform uniform) const
{
	const char * uniformName = StandardUniforms::GetUniformName(uniform);
	int loc = shader->GetUniformVarLocation(uniformName);

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
	int loc = GetStandardAttributeBinding(attr);
	shader->SendBufferToShader(loc, buffer);
}

int Material::GetStandardUniformShaderVarLocation(StandardUniform uniform) const
{
	return  GetStandardUniformBinding(uniform);
}

StandardUniformSet Material::GetStandardUniforms() const
{
	return standardUniforms;
}

void Material::AddTexture(Texture * texture, const char *shaderVarName)
{
	int loc = shader->GetUniformVarLocation(shaderVarName);
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
	desc->Type = UniformType::Sampler;
	desc->SamplerData = texture;
	customUniforms.push_back(desc);
	currentSamplerLoc++;
}

unsigned int Material::GetCustomUniformCount()
{
	return customUniforms.size();
}

UniformDescriptor * Material::GetCustomUniform(unsigned int index)
{
	if(index >=0 && index < GetCustomUniformCount())
	{
		return customUniforms[index];
	}
	return NULL;
}
