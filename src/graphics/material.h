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
	int varBindings[VAR_BINDINGS_SIZE];

	void BindVars();
	void ClearBindings();
	void SetVarBinding( int location, Attribute attr);
    int GetVarBinding(Attribute attr) const;
	int TestForAttribute(Attribute attr) const;

    protected:

    Material();
    virtual ~Material();
    void Init(Shader * shader);

    public:

    int GetAttributeShaderVarLocation(Attribute attr) const;
    AttributeSet GetAttributeSet();
    Shader * GetShader();
    void SendAttributeBufferToShader(Attribute attr, VertexAttrBuffer *buffer);
};

#endif
