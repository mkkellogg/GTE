#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class Material;

#include "shader/shader.h"
#include "attributes.h"
#include "graphics.h"

class Material
{
	friend class Graphics;

	static const int VAR_BINDINGS_SIZE=64;

	Shader * shader;
	AttributeSet attributeSet;
	unsigned int varBindings[VAR_BINDINGS_SIZE];

	void BindVars();
	void ClearBindings();
	void SetBinding(unsigned int location, Attribute attr);
	unsigned int TestForAttribute(Attribute attr);

    protected:

    Material();
    virtual ~Material();
    void Init(Shader * shader);

    public:

    unsigned int GetAttributeShaderVarLocation(Attribute attr);
    AttributeSet GetAttributeSet();
    Shader * GetShader();
};

#endif
