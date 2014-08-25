#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "material.h"
#include "graphics/shader/shader.h"
#include "graphics/attributes.h"
#include "ui/debug.h"

Material::Material()
{
	shader = NULL;
	ClearBindings();
}

Material::~Material()
{

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
	for(int i=0; i < VAR_BINDINGS_SIZE; i++)attributeBindings[i] = -1;
	for(int i=0; i < VAR_BINDINGS_SIZE; i++)uniformBindings[i] = -1;
}

void Material::SetAttributeBinding(int location, Attribute attr)
{
	attributeBindings[(int)attr] = location;
}

int Material::GetAttributeBinding(Attribute attr) const
{
	return attributeBindings[(int)attr];
}

void Material::BindVars()
{
	attributeSet = Attributes::CreateAttributeSet();
	for(int i=0; i<(int)Attribute::_Last; i++)
	{
		Attribute attr = (Attribute)i;

		int loc = TestForAttribute(attr);
		if(loc >= 0)
		{
			SetAttributeBinding(loc, attr);
			Attributes::AddAttribute(&attributeSet,attr);
		}
	}

	uniformSet = Uniforms::CreateUniformSet();
	for(int i=0; i<(int)Uniform::_Last; i++)
	{
		Uniform uniform = (Uniform)i;

		int loc = TestForUniform(uniform);
		if(loc >= 0)
		{
			SetUniformBinding(loc, uniform);
			Uniforms::AddUniform(&uniformSet,uniform);
		}
	}
}

int Material::TestForAttribute(Attribute attr) const
{
	const char * attrName = Attributes::GetAttributeName(attr);
	unsigned int loc = shader->GetAttributeVarLocation(attrName);

	return loc;
}

void Material::SetUniformBinding( int location, Uniform uniform)
{
	uniformBindings[(int)uniform] = location;
}

int Material::GetUniformBinding(Uniform uniform) const
{
	return uniformBindings[(int)uniform];
}

int Material::TestForUniform(Uniform uniform) const
{
	const char * uniformName = Uniforms::GetUniformName(uniform);
	unsigned int loc = shader->GetUniformVarLocation(uniformName);

	return loc;
}

AttributeSet Material::GetAttributeSet() const
{
	return attributeSet;
}

Shader * Material::GetShader() const
{
	return shader;
}

int Material::GetAttributeShaderVarLocation(Attribute attr) const
{
	return GetAttributeBinding(attr);
}

void Material::SendAttributeBufferToShader(Attribute attr, VertexAttrBuffer *buffer)
{
	int loc = GetAttributeBinding(attr);
	shader->SendBufferToShader(loc, buffer);
}

int Material::GetUniformShaderVarLocation(Uniform uniform) const
{
	return  GetUniformBinding(uniform);
}

UniformSet Material::GetUniformSet() const
{
	return uniformSet;
}