#ifndef _MATERIAL_H_
#define _MATERIAL_H_

//forward declarations
class Graphics;
class Shader;
class VertexAttrBuffer;

#include "graphics/stdattributes.h"
#include "graphics/stduniforms.h"
#include "object/engineobject.h"
#include "graphics/texture/texture.h"
#include "graphics/shader/uniformdesc.h"
#include <vector>

class Material : EngineObject
{
	friend class EngineObjectManager;

	static const int VAR_BINDINGS_SIZE=64;

	char materialName[32];
	Shader * shader;
	StandardAttributeSet standardAttributes;
	StandardUniformSet standardUniforms;

	int standardAttributeBindings[VAR_BINDINGS_SIZE];
	int standardUniformBindings[VAR_BINDINGS_SIZE];

	std::vector<UniformDescriptor*> customUniforms;

	void BindVars();
	void ClearBindings();

	void SetStandardAttributeBinding( int location, StandardAttribute attr);
    int GetStandardAttributeBinding(StandardAttribute attr) const;
	int TestForStandardAttribute(StandardAttribute attr) const;

	void SetStandardUniformBinding( int location, StandardUniform uniform);
	int GetStandardUniformBinding(StandardUniform uniform) const;
	int TestForStandardUniform(StandardUniform uniform) const;

    protected:

    Material(const char * materialName);
    virtual ~Material();
    bool Init(Shader * shader);

    public:

    Shader * GetShader() const;

    int GetStandardAttributeShaderVarLocation(StandardAttribute attr) const;
    StandardAttributeSet GetStandardAttributes() const;
    void SendStandardAttributeBufferToShader(StandardAttribute attr, VertexAttrBuffer *buffer);

    int GetStandardUniformShaderVarLocation(StandardUniform uniform) const;
    StandardUniformSet GetStandardUniforms() const;

    void SendModelViewMatrixToShader(const Matrix4x4 * mat);
    void SendProjectionMatrixToShader(const Matrix4x4 * mat);
    void SendMVPMatrixToShader(const Matrix4x4 * mat);

    void AddTexture(Texture * texture, const char *shaderVarName);
    unsigned int GetCustomUniformCount();
    UniformDescriptor * GetCustomUniform(unsigned int index);

};

#endif
