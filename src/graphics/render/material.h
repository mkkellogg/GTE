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
#include <map>


class Material : EngineObject
{
	friend class EngineObjectManager;

	static const int MATERIAL_NAME_MAX_LENGTH = 64;
	static const int VAR_BINDINGS_SIZE=64;
	static const int SAMPLER_2D_DATA_SIZE=64;

	char materialName[MATERIAL_NAME_MAX_LENGTH];
	Shader * shader;
	StandardAttributeSet standardAttributes;
	StandardUniformSet standardUniforms;

	int standardAttributeBindings[VAR_BINDINGS_SIZE];
	int standardUniformBindings[VAR_BINDINGS_SIZE];

	std::vector<UniformDescriptor*> setUniforms;

	bool attributesSetAndVerified;
	int * attributesSetValues;
	std::map<int,int> attributeLocationsToVerificationIndex;

	bool uniformsSetAndVerified;
	int * uniformsSetValues;
	std::map<int,int> uniformLocationsToVerificationIndex;

	void BindStandardVars();
	void ClearStandardBindings();
	bool SetupSetVerifiers();

	void SetStandardAttributeBinding( int varID, StandardAttribute attr);
    int GetStandardAttributeBinding(StandardAttribute attr) const;
	int TestForStandardAttribute(StandardAttribute attr) const;

	void SetStandardUniformBinding( int varID, StandardUniform uniform);
	int GetStandardUniformBinding(StandardUniform uniform) const;
	int TestForStandardUniform(StandardUniform uniform) const;

    protected:

    Material(const char * materialName);
    virtual ~Material();
    bool Init(Shader * shader);

    void SetAttributeSetValue(int varID, int size);
    void SetUniformSetValue(int varID, int size);

    public:

    void ClearSetVerifiers();

    Shader * GetShader() const;

    int GetStandardAttributeShaderVarLocation(StandardAttribute attr) const;
    StandardAttributeSet GetStandardAttributes() const;
    void SendStandardAttributeBufferToShader(StandardAttribute attr, VertexAttrBuffer *buffer);

    int GetStandardUniformShaderVarLocation(StandardUniform uniform) const;
    StandardUniformSet GetStandardUniforms() const;
    void SendSetUniformToShader(unsigned int index);
    void SendAllSetUniformsToShader();

    void SendModelViewMatrixToShader(const Matrix4x4 * mat);
    void SendProjectionMatrixToShader(const Matrix4x4 * mat);
    void SendMVPMatrixToShader(const Matrix4x4 * mat);

    void SetTexture(Texture * texture, const char *shaderVarName);
    unsigned int GetSetUniformCount() const ;
    UniformDescriptor * GetSetUniform(unsigned int index);

};

#endif
