#ifndef _MATERIAL_H_
#define _MATERIAL_H_

//forward declarations
class Graphics;
class Shader;
class VertexAttrBuffer;

#include "graphics/attributes.h"
#include "graphics/uniforms.h"
#include "object/engineobject.h"
#include "graphics/texture/texture.h"
#include "graphics/render/uniformdesc.h"
#include <vector>

class Material : EngineObject
{
	friend class EngineObjectManager;

	static const int VAR_BINDINGS_SIZE=64;

	char materialName[32];
	Shader * shader;
	AttributeSet standardAttributes;
	UniformSet standardUniforms;

	int standardAttributeBindings[VAR_BINDINGS_SIZE];
	int standardUniformBindings[VAR_BINDINGS_SIZE];
	std::vector<UniformDescriptor*> customUniforms;
	int currentSamplerLoc;

	void BindVars();
	void ClearBindings();

	void SetStandardAttributeBinding( int location, Attribute attr);
    int GetStandardAttributeBinding(Attribute attr) const;
	int TestForStandardAttribute(Attribute attr) const;

	void SetStandardUniformBinding( int location, Uniform uniform);
	int GetStandardUniformBinding(Uniform uniform) const;
	int TestForStandardUniform(Uniform uniform) const;

    protected:

    Material(const char * materialName);
    virtual ~Material();
    bool Init(Shader * shader);

    public:

    Shader * GetShader() const;

    int GetStandardAttributeShaderVarLocation(Attribute attr) const;
    AttributeSet GetStandardAttributes() const;
    void SendStandardAttributeBufferToShader(Attribute attr, VertexAttrBuffer *buffer);

    int GetStandardUniformShaderVarLocation(Uniform uniform) const;
    UniformSet GetStandardUniforms() const;

    void AddTexture(Texture * texture, const char *shaderVarName);
    unsigned int GetCustomUniformCount();
    UniformDescriptor * GetCustomUniform(unsigned int index);

};

#endif
