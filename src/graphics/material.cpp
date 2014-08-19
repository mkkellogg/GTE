#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "material.h"
#include "shader/shader.h"
#include "attributes.h"

Material::Material()
{
	shader = NULL;
	ClearBindings();
}

Material::~Material()
{

}

void Material::Init(Shader * shader)
{
	this->shader = shader;
	ClearBindings();
	BindVars();
}

void Material::ClearBindings()
{
	for(int i=0; i < VAR_BINDINGS_SIZE; i++)varBindings[i] = -1;
}

void Material::SetVarBinding(int location, Attribute attr)
{
	varBindings[(int)attr] = location;
}

int Material::GetVarBinding(Attribute attr) const
{
	return varBindings[(int)attr];
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
			SetVarBinding(loc, attr);
			Attributes::AddAttribute(&attributeSet,attr);
		}
	}
}

int Material::TestForAttribute(Attribute attr) const
{
	const char * attrName = Attributes::GetAttributeName(attr);
	unsigned int loc = shader->GetVariableLocation(attrName);

	return loc;
}

AttributeSet Material::GetAttributeSet()
{
	return attributeSet;
}

Shader * Material::GetShader()
{
	return shader;
}

int Material::GetAttributeShaderVarLocation(Attribute attr) const
{
	return  Material::varBindings[(int)attr];
}

void Material::SendAttributeBufferToShader(Attribute attr, VertexAttrBuffer *buffer)
{
	int loc = GetVarBinding(attr);
	shader->SendBufferToShader(loc, buffer);
}
