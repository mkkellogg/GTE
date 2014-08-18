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
	memset(varBindings,0,sizeof(int)*VAR_BINDINGS_SIZE);
}

void Material::SetBinding(unsigned int location, Attribute attr)
{
	varBindings[(int)attr] = location;
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
			SetBinding(loc, attr);
			Attributes::AddAttribute(&attributeSet,attr);
		}
	}
}

unsigned int Material::TestForAttribute(Attribute attr)
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

unsigned int Material::GetAttributeShaderVarLocation(Attribute attr)
{
	return  Material::varBindings[(int)attr];
}
