#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>
#include <iostream>

#include "material.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/shader/shader.h"
#include "graphics/stdattributes.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "graphics/shader/uniformdesc.h"
#include "graphics/shader/attributedesc.h"
#include "graphics/render/vertexattrbuffer.h"
#include "graphics/light/light.h"
#include "base/basevector4.h"
#include <string>

/*
 * Only constructor - requires a material name
 */
Material::Material(const std::string& materialName)
{
	this->materialName = materialName;

	ClearStandardBindings();

	allSetUniformsandAttributesVerified = false;

	attributesSetAndVerified = false;
	attributesSetValues = NULL;

	uniformsSetAndVerified = false;
	uniformsSetValues = NULL;
}

/*
 * Clean up
 */
Material::~Material()
{
	SAFE_DELETE(attributesSetValues);
	SAFE_DELETE(uniformsSetValues);
	DestroySetUniforms();
}

/*
 * Clean up set uniform data
 */
void Material::DestroySetUniforms()
{
	for(unsigned int i=0; i < setUniforms.size(); i++)
	{
		UniformDescriptor * desc = setUniforms[i];
		SAFE_DELETE(desc);
	}
	setUniforms.clear();
}

/*
 * Get the size of the data for uniform of type [uniformType].
 */
unsigned int Material::GetRequiredUniformSize(UniformType uniformType)
{
	switch(uniformType)
	{
		case UniformType::Sampler2D:
			return SAMPLER_2D_DATA_SIZE;
		break;
		case UniformType::Matrix4x4:
			return MATRIX4X4_DATA_SIZE;
		break;
		case UniformType::Float4:
			return 4;
		break;
		case UniformType::Float3:
			return 3;
		break;
		case UniformType::Float2:
			return 2;
		break;
		case UniformType::Float:
			return 1;
		case UniformType::Int:
			return 1;
		default:
			return -1;
		break;
	}

	return -1;
}

/*
 * Once a material has been instantiated, it must then be initialized. The Init() method
 * connects the material to [shader] and does all allocation & initialization of
 * dynamic data structures linked to that shader.
 *
 * This method will return true if initialization succeeds, otherwise it returns false.
 */
bool Material::Init(ShaderRef shader)
{
	ASSERT(shader.IsValid()," Material::Init(Shader *) -> tried to Init with NULL shader.", false);

	if(!shader->IsLoaded())
	{
		Debug::PrintError(" Material::Init(Shader *) -> tried to Init with unloaded shader.");
		return false;
	}

	this->shader = shader;

	// clear any existing bindings to standard attributes and uniforms
	ClearStandardBindings();
	// setup bindings to standard attributes and uniforms for [shader]
	BindStandardVars();

	// SetupSetVerifiers() must allocate memory so it could fail, though incredibly unlikely
	if(!SetupSetVerifiers())return false;

	SetupSetUniforms();

	return true;
}

/*
 * The member arrays [attributesSetAndVerified] and [uniformsSetAndVerified] store the
 * size of the data that has been set for all attributes and uniforms respectively. This
 * information is used to determine if an active uniform or attribute has no data set
 * for it, which could cause rendering artifacts or crashes.
 */
bool Material::SetupSetVerifiers()
{
	ASSERT(shader.IsValid(),"Material::SetupSetVerifiers -> shader is NULL", false);

	unsigned int attributeCount = shader->GetAttributeCount();
	unsigned int uniformCount = shader->GetUniformCount();

	attributesSetValues = new int[attributeCount];
	ASSERT(attributesSetValues != NULL,"Material::SetupSetVerifiers -> could not allocate attributesSetValues", false);

	uniformsSetValues = new int[uniformCount];
	ASSERT(uniformsSetValues != NULL,"Material::SetupSetVerifiers -> could not allocate uniformsSetValues", false);

	// initialize all the values in [attributesSetValues] and [uniformsSetValues] to 0
	ResetVerificationState();

	// create map from attribute id/location to index in [attributesSetValues]
	for(unsigned int i=0; i < attributeCount; i ++)
	{
		const AttributeDescriptor * desc = shader->GetAttributeDescriptor(i);
		attributeLocationsToVerificationIndex[desc->ShaderVarID] = i;
	}

	// create map from uniform id/location to index in [uniformsSetValues]
	for(unsigned int i=0; i < uniformCount; i ++)
	{
		const UniformDescriptor * desc = shader->GetUniformDescriptor(i);
		uniformLocationsToVerificationIndex[desc->ShaderVarID] = i;
	}

	return true;
}

/*
 * Set up an array of UniformDescriptor objects corresponding to each uniform
 * exposed by this material's shader.
 */
bool Material::SetupSetUniforms()
{
	ASSERT(shader.IsValid(),"Material::SetupSetUniforms -> shader is NULL", false);

	DestroySetUniforms();

	unsigned int uniformCount = shader->GetUniformCount();
	for(unsigned int i=0; i < uniformCount; i ++)
	{
		const UniformDescriptor * desc = shader->GetUniformDescriptor(i);
		UniformDescriptor *newDesc = new UniformDescriptor();

		if(newDesc == NULL)
		{
			DestroySetUniforms();
			Debug::PrintError("Material::SetupSetUniforms -> could not allocate UniformDescriptor");
			return false;
		}

		*newDesc = *desc;
		setUniforms.push_back(newDesc);
	}

	return true;
}

/*
 * Indicate the set data size for an attribute in [attributesSetValues]. [varID] is
 * used to specify the attribute, but is mapped to an index in [attributesSetValues]
 */
void Material::SetAttributeSetValue(int varID, int size)
{
	int varIndex = attributeLocationsToVerificationIndex[varID];
	if(varIndex >=0)
	{
		attributesSetValues[varIndex] = size;
	}
}

/*
 * Indicate the set data size for an uniform in [uniformsSetValues]. [varID] is
 * used to specify the uniform, but is mapped to an index in [uniformsSetValues]
 */
void Material::SetUniformSetValue(int varID, int size)
{
	int varIndex = uniformLocationsToVerificationIndex[varID];
	if(varIndex >=0)
	{
		uniformsSetValues[varIndex] = size;
	}
}

/*
 * Initialize all the values in [attributesSetValues] and [uniformsSetValues] to 0.
 * Also clear the [allSetUniformsandAttributesVerified] flag, which indicates that the
 * current values set for all uniforms and attributes have passed verification.
 */
void Material::ResetVerificationState()
{
	if(attributesSetValues != NULL && shader.IsValid())memset(attributesSetValues, 0, sizeof(int) * shader->GetAttributeCount());
	if(uniformsSetValues != NULL && shader.IsValid())memset(uniformsSetValues, 0, sizeof(int) * shader->GetUniformCount());
	allSetUniformsandAttributesVerified = false;
}

/*
 * Map a standard attribute [attr] to a shader var ID/location [varID]
 */
void Material::SetStandardAttributeBinding(int varID, StandardAttribute attr)
{
	standardAttributeBindings[(int)attr] = varID;
}

/*
 * Get the shader var ID/location for [attr]
 */
int Material::GetStandardAttributeBinding(StandardAttribute attr) const
{
	return standardAttributeBindings[(int)attr];
}

/*
 * Check if the standard attribute specified by [attr] is used by
 * the shader connected to this material
 */
int Material::TestForStandardAttribute(StandardAttribute attr) const
{
	const char * attrName = StandardAttributes::GetAttributeName(attr);
	int varID = shader->GetAttributeVarID(attrName);

	return varID;
}

/*
 * Get the index in the shader's list of uniforms corresponding
 * to the uniform named [uniformName]
 */
int Material::GetUniformIndex(const std::string& uniformName)
{
	ASSERT(shader.IsValid(),"Material::GetUniformIndex -> is NULL",-1);

	int foundIndex = -1;
	for(unsigned int i=0; i< setUniforms.size(); i++)
	{
		if(uniformName == setUniforms[i]->Name)
		{
			foundIndex = (int)i;
		}
	}

	return foundIndex;
}

/*
 * Map a standard uniform to a shader var ID/location
 */
void Material::SetStandardUniformBinding( int varID, StandardUniform uniform)
{
	standardUniformBindings[(int)uniform] = varID;
}

/*
 * Get the shader var ID/location for [uniform]
 */
int Material::GetStandardUniformBinding(StandardUniform uniform) const
{
	return standardUniformBindings[(int)uniform];
}

/*
 * Check if the standard uniform specified by [uniform] is used by
 * the shader connected to this material
 */
int Material::TestForStandardUniform(StandardUniform uniform) const
{
	const char * uniformName = StandardUniforms::GetUniformName(uniform);
	int loc = shader->GetUniformVarID(uniformName);
	return loc;
}

/*
 * Loop through all standard attributes and all standard uniforms and
 * for each:
 *
 *     1. Check if the shader attached to this material uses it
 *     2. If (1) is true:
 *        a. Get the var ID/location and set the binding using either
 *           SetStandardAttributeBinding() or SetStandardUniformBinding().
 *        b. Set the appropriate bit in [standardAttributes] and [standardUniforms]
 *           to indicate usage of the attribute or uniform.
 */
void Material::BindStandardVars()
{
	standardAttributes = StandardAttributes::CreateAttributeSet();
	for(int i=0; i<(int)StandardAttribute::_Last; i++)
	{
		StandardAttribute attr = (StandardAttribute)i;

		int varID = TestForStandardAttribute(attr);
		if(varID >= 0)
		{
			SetStandardAttributeBinding(varID, attr);
			StandardAttributes::AddAttribute(&standardAttributes,attr);
		}
	}

	standardUniforms = StandardUniforms::CreateUniformSet();
	for(int i=0; i<(int)StandardUniform::_Last; i++)
	{
		StandardUniform uniform = (StandardUniform)i;

		int varID = TestForStandardUniform(uniform);
		if(varID >= 0)
		{
			SetStandardUniformBinding(varID, uniform);
			StandardUniforms::AddUniform(&standardUniforms,uniform);
		}
	}
}

/*
 * Remove existing bindings for standard attributes and uniforms.
 */
void Material::ClearStandardBindings()
{
	for(int i=0; i < BINDINGS_ARRAY_MAX_LENGTH; i++)standardAttributeBindings[i] = -1;
	for(int i=0; i < BINDINGS_ARRAY_MAX_LENGTH; i++)standardUniformBindings[i] = -1;
}

// =====================================================
//  Public methods
// =====================================================

/*
 * Get a pointer to the shader to which this material is connected.
 */
ShaderRef Material::GetShader() const
{
	return shader;
}

/*
 * Get a bit mask that indicates which standard attributes are used by this material's shader
 */
StandardAttributeSet Material::GetStandardAttributes() const
{
	return standardAttributes;
}

/*
 * Send an array of vertex attributes held in [buffer] to this material's shader. This method
 * maps the standard attribute specified by [attr] to the corresponding shader var ID/location
 * (if a binding exists) and sends the data to that variable in the shader.
 *
 */
void Material::SendStandardAttributeBufferToShader(StandardAttribute attr, VertexAttrBuffer *buffer)
{
	ASSERT_RTRN(buffer != NULL, "Material::SendStandardAttributeBufferToShader -> buffer is NULL.");

	int varID = GetStandardAttributeBinding(attr);
	if(varID >= 0)
	{
		shader->SendBufferToShader(varID, buffer);
		SetAttributeSetValue(varID, buffer->GetVertexCount());
	}
}

/*
 * Send an array of vertex attributes held in [buffer] to this material's shader.
 * This method skips the step of mapping a standard attribute to shader variable,
 * as in SendStandardAttributeBufferToShader(), and simply uses [varID] as the
 * shader variable.
 *
 */
void Material::SendAttributeBufferToShader(int varID, VertexAttrBuffer *buffer)
{
	ASSERT_RTRN(buffer != NULL, "Material::SendAttributeBufferToShader -> buffer is NULL.");
	ASSERT_RTRN(varID >= 0, "Material::SendAttributeBufferToShader -> varID cannot be less than 0.");

	if(varID >= 0)
	{
		shader->SendBufferToShader(varID, buffer);
		SetAttributeSetValue(varID, buffer->GetVertexCount());
	}
}

/*
 * Get a bit mask that indicates which standard uniforms are used by this material's shader
 */
StandardUniformSet Material::GetStandardUniforms() const
{
	return standardUniforms;
}

/*
 * Send a uniform that already has its value set to this material's shader. The parameter
 * [index] corresponds to an index in the vector of uniforms for which values have been
 * set [setUniforms] (which usually correspond to custom uniforms specified by the
 * developer). This method simply takes the value stored for that uniform and sends
 * it to the shader.
 */
void Material::SendSetUniformToShader(unsigned int index)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SendSetUniformToShader -> shader is NULL");

	if(index < setUniforms.size())
	{
		UniformDescriptor * desc = setUniforms[index];
		ASSERT_RTRN(desc != NULL, "Material::SendSetUniformToShader -> uniform descriptor is NULL");

		if(desc->IsSet)
		{
			if(desc->Type == UniformType::Sampler2D)
			{
				shader->SendUniformToShader(desc->SamplerUnitIndex, desc->SamplerData);
				SetUniformSetValue(desc->ShaderVarID, SAMPLER_2D_DATA_SIZE);
			}
			else if(desc->Type == UniformType::Float)
			{
				shader->SendUniformToShader(desc->SamplerUnitIndex, desc->BasicFloatData[0]);
				SetUniformSetValue(desc->ShaderVarID, 1);
			}
		}
	}
}

/*
 * Send all uniforms for which values have been set to the shader.
 */
void Material::SendAllSetUniformsToShader()
{
	for(unsigned int i=0; i < GetSetUniformCount(); i++)
	{
		SendSetUniformToShader(i);
	}
}

/*
 * Find a uniform with the name specified by [shaderVarName] and set its
 * value to the sampler data held by [texture]
 */
void Material::SetTexture(TextureRef texture, const std::string& varName)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SetTexture -> shader is NULL");

	int loc = shader->GetUniformVarID(varName);
	if(loc < 0)
	{
		std::string str = std::string("Material::SetTexture -> Could not find shader sampler var:" ) +
				varName + std::string("for material: ") + materialName;

		Debug::PrintError(str);
		return;
	}

	// get the index in [setUniforms] that has the UniformDescriptor for the
	// uniform named [varName]
	int foundIndex = GetUniformIndex(varName);

	if(foundIndex >= 0)
	{
		UniformDescriptor * desc = setUniforms[foundIndex];

		desc->ShaderVarID = loc;
		desc->Type = UniformType::Sampler2D;
		desc->SamplerData = texture;
		desc->IsSet = true;
	}
	else
	{
		std::string err = std::string("Material::SetTexture -> Invalid uniform specified: ") + varName;
		Debug::PrintError(err);
	}
}

/*
 * Find a uniform with the name specified by [shaderVarName] and set its
 * value to [val]
 */
void Material::SetUniform1f(float val, const std::string& varName)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SetUniform1f -> shader is NULL");

	int loc = shader->GetUniformVarID(varName);
	if(loc < 0)
	{
		std::string str = std::string("Material::SetUniform1f -> Could not find shader sampler var:" ) +
				varName + std::string("for material: ") + materialName;

		Debug::PrintError(str);
		return;
	}

	// get the index in [setUniforms] that has the UniformDescriptor for the
	// uniform named [varName]
	int foundIndex = GetUniformIndex(varName);

	if(foundIndex >= 0)
	{
		UniformDescriptor * desc = setUniforms[foundIndex];

		desc->ShaderVarID = loc;
		desc->Type = UniformType::Float;
		desc->BasicFloatData[0] = val;
		desc->IsSet = true;
	}
	else
	{
		std::string err = std::string("Material::SetUniform1f -> Invalid uniform specified: ") + varName;
		Debug::PrintError(err);
	}
}

/*
 * Get the number of set uniforms...this should for the most part be
 * equal to the total number of uniforms exposed by this material's shader.
 */
unsigned int Material::GetSetUniformCount() const
{
	return setUniforms.size();
}

/*
 * Send the 4x4 matrix data in [mat] to this material's shader via the
 * standard uniform ModelMatrix.
 */
void Material::SendModelMatrixToShader(const Matrix4x4 * mat)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SendModelMatrixToShader -> shader is NULL");

	int varID = GetStandardUniformBinding(StandardUniform::ModelMatrix);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, mat);
		SetUniformSetValue(varID, MATRIX4X4_DATA_SIZE);
	}
}

/*
 * Send the 4x4 matrix data in [mat] to this material's shader via the
 * standard uniform ModelViewMatrix.
 */
void Material::SendModelViewMatrixToShader(const Matrix4x4 * mat)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SendModelViewMatrixToShader -> shader is NULL");

	int varID = GetStandardUniformBinding(StandardUniform::ModelViewMatrix);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, mat);
		SetUniformSetValue(varID, MATRIX4X4_DATA_SIZE);
	}
}

/*
 * Send the 4x4 matrix data in [mat] to this material's shader via the
 * standard uniform ProjectionMatrix.
 */
void Material::SendProjectionMatrixToShader(const Matrix4x4 * mat)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SendProjectionMatrixToShader -> shader is NULL");

	int varID = GetStandardUniformBinding(StandardUniform::ProjectionMatrix);
	if(varID >= 0)
	{
		shader->SendUniformToShader(varID, mat);
		SetUniformSetValue(varID, MATRIX4X4_DATA_SIZE);
	}
}

/*
 * Send the 4x4 matrix data in [mat] to this material's shader via the
 * standard uniform ModelViewProjectionMatrix.
 */
void Material::SendMVPMatrixToShader(const Matrix4x4 * mat)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SendMVPMatrixToShader -> shader is NULL");

	int varID = GetStandardUniformBinding(StandardUniform::ModelViewProjectionMatrix);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, mat);
		SetUniformSetValue(varID, MATRIX4X4_DATA_SIZE);
	}
}

/*
 * Send the light data in [light] to this material's shader using
 * several standard uniforms: LIGHT_POSITION, LIGHT_DIRECTION, LIGHT_ATTENUATION, and LIGHT_COLOR
 */
void Material::SendLightToShader(const Light * light, const Point3 * position,  const Vector3 * altDirection)
{
	ASSERT_RTRN(shader.IsValid(),"Material::SendLightToShader -> shader is NULL");
	ASSERT_RTRN(light != NULL,"Material::SendLightToShader -> light is NULL");
	ASSERT_RTRN(position != NULL,"Material::SendLightToShader -> position is NULL");

	int varID = GetStandardUniformBinding(StandardUniform::LightType);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, (int)light->GetType());
		SetUniformSetValue(varID, 1);
	}

	varID = GetStandardUniformBinding(StandardUniform::LightPosition);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, position);
		SetUniformSetValue(varID, 4);
	}

	varID = GetStandardUniformBinding(StandardUniform::LightDirection);
	if(varID >=0 )
	{
		if(altDirection != NULL)shader->SendUniformToShader(varID, altDirection);
		else shader->SendUniformToShader(varID, light->GetDirectionPtr());
		SetUniformSetValue(varID, 4);
	}

	varID = GetStandardUniformBinding(StandardUniform::LightColor);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, light->GetColorPtr());
		SetUniformSetValue(varID, 4);
	}

	varID = GetStandardUniformBinding(StandardUniform::LightIntensity);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, light->GetIntensity());
		SetUniformSetValue(varID, 1);
	}

	varID = GetStandardUniformBinding(StandardUniform::LightAttenuation);
	if(varID >=0 )
	{
		shader->SendUniformToShader(varID, light->GetAttenuation());
		SetUniformSetValue(varID, 1);
	}
}

/*
 * Verify that all uniforms and attributes exposed by this material's shader have values
 * set for them and that those values are of the correct size.
 */
bool Material::VerifySetVars(int vertexCount)
{
	ASSERT(shader.IsValid(), "Material::VerifySetVars -> shader is NULL", false);
	if(allSetUniformsandAttributesVerified == true)return true;

	for(unsigned int i =0; i< shader->GetAttributeCount(); i++)
	{
		const AttributeDescriptor * desc = shader->GetAttributeDescriptor(i);
		if(attributesSetValues[i] != vertexCount)
		{
			std::string msg = "Material::VerifySetVars -> Attribute '";
			msg += desc->Name + std::string("' set incorrectly: size is ") + std::to_string(attributesSetValues[i]);
			msg += std::string(" instead of ") + std::to_string(vertexCount);

			Debug::PrintError(msg);
			return false;
		}
	}

	for(unsigned int i =0; i< shader->GetUniformCount(); i++)
	{
		const UniformDescriptor * desc = shader->GetUniformDescriptor(i);
		int requiredSize = GetRequiredUniformSize(desc->Type);

		if(uniformsSetValues[i] != requiredSize)
		{
			std::string msg = "Material::VerifySetVars -> Uniform '";
			msg += desc->Name + std::string("' set incorrectly: size is ") + std::to_string(uniformsSetValues[i]);
			msg += std::string(" instead of ") + std::to_string(requiredSize);

			Debug::PrintError(msg);
			return false;
		}
	}

	allSetUniformsandAttributesVerified = true;

	return true;
}


