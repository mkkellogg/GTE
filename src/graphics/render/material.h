#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class Material;

#include "graphics/shader/shader.h"
#include "graphics/attributes.h"
#include "graphics/uniforms.h"
#include "graphics/graphics.h"

class Material
{
	friend class Graphics;

	static const int VAR_BINDINGS_SIZE=64;

	Shader * shader;
	AttributeSet attributeSet;
	UniformSet uniformSet;

	int attributeBindings[VAR_BINDINGS_SIZE];
	int uniformBindings[VAR_BINDINGS_SIZE];

	void BindVars();
	void ClearBindings();

	void SetAttributeBinding( int location, Attribute attr);
    int GetAttributeBinding(Attribute attr) const;
	int TestForAttribute(Attribute attr) const;

	void SetUniformBinding( int location, Uniform uniform);
	int GetUniformBinding(Uniform uniform) const;
	int TestForUniform(Uniform uniform) const;

    protected:

    Material();
    virtual ~Material();
    void Init(Shader * shader);

    public:

    Shader * GetShader() const;

    int GetAttributeShaderVarLocation(Attribute attr) const;
    AttributeSet GetAttributeSet() const;
    void SendAttributeBufferToShader(Attribute attr, VertexAttrBuffer *buffer);

    int GetUniformShaderVarLocation(Uniform uniform) const;
    UniformSet GetUniformSet() const;

   // void SendUniformToShader(Uniform attr, VertexAttrBuffer *buffer);
};

#endif
