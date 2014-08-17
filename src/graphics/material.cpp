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

void Material::SetBinding(int location, Attribute attr)
{
	varBindings[(int)attr] = location;
}

void Material::BindVars()
{
	for(int i=0; i<(int)Attribute::_Last; i++)
	{
		Attribute attr = (Attribute)i;

		int loc = TestForAttribue(attr);
		if(loc >= 0)
		{
			SetBinding(loc, attr);
		}
	}
}

int Material::TestForAttribue(Attribute attr)
{
	const char * attrName = Attributes::GetAttributeName(attr);
	int loc = shader->GetVariableLocation(attrName);

	return loc;
}


