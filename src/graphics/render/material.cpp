#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>
#include <iostream>

#include "material.h"
#include "geometry/matrix4x4.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/shader/shader.h"
#include "graphics/texture/textureattr.h"
#include "graphics/texture/texture.h"
#include "graphics/stdattributes.h"
#include "graphics/shader/uniformdesc.h"
#include "graphics/shader/attributedesc.h"
#include "graphics/render/vertexattrbuffer.h"
#include "graphics/light/light.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "global/assert.h"
#include "base/basevector4.h"
#include "engine.h"
#include <string>

namespace GTE
{
	/*
	* Only constructor - requires a material name
	*/
	Material::Material(const std::string& materialName)
	{
		this->materialName = materialName;

		ClearStandardBindings();

		allSetUniformsandAttributesVerified = false;

		attributesSetAndVerified = false;
		attributesSetValues = nullptr;

		uniformsSetAndVerified = false;
		uniformsSetValues = nullptr;

		selfLit = false;

		currentSampletUnityIndex = 0;
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
		for (UInt32 i = 0; i < setUniforms.size(); i++)
		{
			UniformDescriptor * desc = setUniforms[i];
			SAFE_DELETE(desc);
		}
		setUniforms.clear();
	}

	/*
	 * Get the size of the data for uniform of type [uniformType].
	 */
	UInt32 Material::GetRequiredUniformSize(UniformType uniformType)
	{
		switch (uniformType)
		{
		case UniformType::Sampler2D:
			return SAMPLER_2D_DATA_SIZE;
			break;
		case UniformType::SamplerCube:
			return SAMPLER_CUBE_DATA_SIZE;
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
	Bool Material::Init(ShaderRef shader)
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), " Material::Init -> tried to initialize with invalid shader.", false, true);
		NONFATAL_ASSERT_RTRN(shader->IsLoaded(), " Material::Init -> tried to initialize with unloaded shader.", false, true);

		this->shader = shader;

		// clear any existing bindings to standard attributes and uniforms
		ClearStandardBindings();
		// setup bindings to standard attributes and uniforms for [shader]
		BindStandardVars();

		// SetupSetVerifiers() must allocate memory so it could fail, though incredibly unlikely
		if (!SetupSetVerifiers())return false;

		SetupSetUniforms();

		return true;
	}

	/*
	 * The member arrays [attributesSetAndVerified] and [uniformsSetAndVerified] store the
	 * size of the data that has been set for all attributes and uniforms respectively. This
	 * information is used to determine if an active uniform or attribute has no data set
	 * for it, which could cause rendering artifacts or crashes.
	 */
	Bool Material::SetupSetVerifiers()
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::SetupSetVerifiers -> Shader is invalid.", false, true);

		UInt32 attributeCount = shader->GetAttributeCount();
		UInt32 uniformCount = shader->GetUniformCount();

		attributesSetValues = new(std::nothrow) int[attributeCount];
		ASSERT(attributesSetValues != nullptr, "Material::SetupSetVerifiers -> Could not allocate attributesSetValues.");

		uniformsSetValues = new(std::nothrow) int[uniformCount];
		ASSERT(uniformsSetValues != nullptr, "Material::SetupSetVerifiers -> Could not allocate uniformsSetValues.");

		// initialize all the values in [attributesSetValues] and [uniformsSetValues] to 0
		ResetVerificationState();

		// create map from attribute id/location to index in [attributesSetValues]
		for (UInt32 i = 0; i < attributeCount; i++)
		{
			const AttributeDescriptor * desc = shader->GetAttributeDescriptor(i);
			attributeLocationsToVerificationIndex[desc->ShaderVarID] = i;
		}

		// create map from uniform id/location to index in [uniformsSetValues]
		for (UInt32 i = 0; i < uniformCount; i++)
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
	Bool Material::SetupSetUniforms()
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::SetupSetUniforms -> Shader is invalid.", false, true);

		DestroySetUniforms();

		UInt32 uniformCount = shader->GetUniformCount();
		for (UInt32 i = 0; i < uniformCount; i++)
		{
			const UniformDescriptor * desc = shader->GetUniformDescriptor(i);
			UniformDescriptor *newDesc = new(std::nothrow) UniformDescriptor();

			if (newDesc == nullptr)
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
	void Material::SetAttributeSetValue(Int32 varID, Int32 size)
	{
		Int32 varIndex = attributeLocationsToVerificationIndex[varID];
		if (varIndex >= 0)
		{
			attributesSetValues[varIndex] = size;
		}
	}

	/*
	 * Indicate the set data size for an uniform in [uniformsSetValues]. [varID] is
	 * used to specify the uniform, but is mapped to an index in [uniformsSetValues]
	 */
	void Material::SetUniformSetValue(Int32 varID, Int32 size)
	{
		Int32 varIndex = uniformLocationsToVerificationIndex[varID];
		if (varIndex >= 0)
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
		if (attributesSetValues != nullptr && shader.IsValid())memset(attributesSetValues, 0, sizeof(Int32) * shader->GetAttributeCount());
		if (uniformsSetValues != nullptr && shader.IsValid())memset(uniformsSetValues, 0, sizeof(Int32) * shader->GetUniformCount());
		allSetUniformsandAttributesVerified = false;
	}

	/*
	 * Map a standard attribute [attr] to a shader var ID/location [varID]
	 */
	void Material::SetStandardAttributeBinding(Int32 varID, StandardAttribute attr)
	{
		standardAttributeBindings[(Int32)attr] = varID;
	}

	/*
	 * Get the shader var ID/location for [attr]
	 */
	Int32 Material::GetStandardAttributeBinding(StandardAttribute attr) const
	{
		return standardAttributeBindings[(Int32)attr];
	}

	/*
	 * Check if the standard attribute specified by [attr] is used by
	 * the shader connected to this material
	 */
	Int32 Material::TestForStandardAttribute(StandardAttribute attr) const
	{
		const Char * attrName = StandardAttributes::GetAttributeName(attr);
		Int32 varID = shader->GetAttributeVarID(attrName);

		return varID;
	}

	/*
	 * Get the index in the shader's list of uniforms corresponding
	 * to the uniform named [uniformName]
	 */
	Int32 Material::GetUniformIndex(const std::string& uniformName)
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::GetUniformIndex -> Shader is invalid.", -1, true);

		Int32 foundIndex = -1;
		for (UInt32 i = 0; i < setUniforms.size(); i++)
		{
			if (uniformName == setUniforms[i]->Name)
			{
				foundIndex = (Int32)i;
			}
		}

		return foundIndex;
	}

	UInt32 Material::GetSamplerUnitForName(const std::string& name)
	{
		UInt32 index = 0;

		if (textureUniformSamplerUnitIndex.find(name) == textureUniformSamplerUnitIndex.end())
		{
			textureUniformSamplerUnitIndex[name] = currentSampletUnityIndex;
			index = currentSampletUnityIndex;
			currentSampletUnityIndex++;
		}
		else
		{
			index = textureUniformSamplerUnitIndex[name];
		}

		return index;
	}

	/*
	 * Map a standard uniform to a shader var ID/location
	 */
	void Material::SetStandardUniformBinding(Int32 varID, StandardUniform uniform)
	{
		standardUniformBindings[(Int32)uniform] = varID;
	}

	/*
	 * Get the shader var ID/location for [uniform]
	 */
	Int32 Material::GetStandardUniformBinding(StandardUniform uniform) const
	{
		return standardUniformBindings[(Int32)uniform];
	}

	/*
	 * Check if the standard uniform specified by [uniform] is used by
	 * the shader connected to this material
	 */
	Int32 Material::TestForStandardUniform(StandardUniform uniform) const
	{
		const Char * uniformName = StandardUniforms::GetUniformName(uniform);
		Int32 loc = shader->GetUniformVarID(uniformName);
		return loc;
	}

	/*
	 * Validate the existence and binding of the shader variable specified by [name].
	 * Store the shader variable location in [loc] and mapped index in [setUniforms] of
	 * that variable in [index].
	 */
	Bool Material::ValidateUniformName(const std::string& name, int& loc, int& index)
	{
		loc = shader->GetUniformVarID(name);
		if (loc < 0)
		{
			std::string str = std::string("Material::ValidateUniformName -> Could not find shader var:") +
				name + std::string(" for material: ") + materialName;

			Debug::PrintError(str);
			return false;
		}

		// get the index in [setUniforms] that has the UniformDescriptor for the
		// uniform named [varName]
		index = GetUniformIndex(name);

		if (index < 0)
		{
			std::string err = std::string("Material::SetUniform1f -> Invalid uniform specified: ") + name;
			Debug::PrintError(err);
			return false;
		}
		return true;
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
		for (Int32 i = 0; i < (Int32)StandardAttribute::_Last; i++)
		{
			StandardAttribute attr = (StandardAttribute)i;

			Int32 varID = TestForStandardAttribute(attr);
			if (varID >= 0)
			{
				SetStandardAttributeBinding(varID, attr);
				StandardAttributes::AddAttribute(&standardAttributes, attr);
			}
		}

		standardUniforms = StandardUniforms::CreateUniformSet();
		for (Int32 i = 0; i < (Int32)StandardUniform::_Last; i++)
		{
			StandardUniform uniform = (StandardUniform)i;

			Int32 varID = TestForStandardUniform(uniform);
			if (varID >= 0)
			{
				SetStandardUniformBinding(varID, uniform);
				StandardUniforms::AddUniform(&standardUniforms, uniform);
			}
		}
	}

	/*
	 * Remove existing bindings for standard attributes and uniforms.
	 */
	void Material::ClearStandardBindings()
	{
		for (Int32 i = 0; i < BINDINGS_ARRAY_MAX_LENGTH; i++)standardAttributeBindings[i] = -1;
		for (Int32 i = 0; i < BINDINGS_ARRAY_MAX_LENGTH; i++)standardUniformBindings[i] = -1;
	}

	/*
	 * Get a pointer to the shader to which this material is connected.
	 */
	ShaderRef Material::GetShader() 
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
		NONFATAL_ASSERT(buffer != nullptr, "Material::SendStandardAttributeBufferToShader -> 'buffer' is null.", true);

		Int32 varID = GetStandardAttributeBinding(attr);
		if (varID >= 0)
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
	void Material::SendAttributeBufferToShader(Int32 varID, VertexAttrBuffer *buffer)
	{
		NONFATAL_ASSERT(buffer != nullptr, "Material::SendAttributeBufferToShader -> 'buffer' is null.", true);
		NONFATAL_ASSERT(varID >= 0, "Material::SendAttributeBufferToShader -> 'varID' cannot be less than 0.", true);

		if (varID >= 0)
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
	void Material::SendSetUniformToShader(UInt32 index)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendSetUniformToShader -> 'shader' is null.", true);

		if (index < setUniforms.size())
		{
			UniformDescriptor * desc = setUniforms[index];
			NONFATAL_ASSERT(desc != nullptr, "Material::SendSetUniformToShader -> Uniform descriptor is null.", true);

			if (desc->IsSet)
			{
				if (desc->Type == UniformType::Sampler2D)
				{
					shader->SendUniformToShader(desc->ShaderVarID, desc->SamplerUnitIndex, desc->SamplerData);
					SetUniformSetValue(desc->ShaderVarID, GetRequiredUniformSize(UniformType::Sampler2D));
				}
				if (desc->Type == UniformType::SamplerCube)
				{
					shader->SendUniformToShader(desc->ShaderVarID, desc->SamplerUnitIndex, desc->SamplerData);
					SetUniformSetValue(desc->ShaderVarID, GetRequiredUniformSize(UniformType::SamplerCube));
				}
				else if (desc->Type == UniformType::Float)
				{
					shader->SendUniformToShader(desc->ShaderVarID, desc->BasicFloatData[0]);
					SetUniformSetValue(desc->ShaderVarID, GetRequiredUniformSize(UniformType::Float));
				}
				else if (desc->Type == UniformType::Float2)
				{
					shader->SendUniformToShader2(desc->ShaderVarID, desc->BasicFloatData[0], desc->BasicFloatData[1]);
					SetUniformSetValue(desc->ShaderVarID, GetRequiredUniformSize(UniformType::Float2));
				}
				else if (desc->Type == UniformType::Float3)
				{
					shader->SendUniformToShader3(desc->ShaderVarID, desc->BasicFloatData[0], desc->BasicFloatData[1], desc->BasicFloatData[2]);
					SetUniformSetValue(desc->ShaderVarID, GetRequiredUniformSize(UniformType::Float3));
				}
				else if (desc->Type == UniformType::Float4)
				{
					shader->SendUniformToShader4(desc->ShaderVarID, desc->BasicFloatData[0], desc->BasicFloatData[1], desc->BasicFloatData[2], desc->BasicFloatData[3]);
					SetUniformSetValue(desc->ShaderVarID, GetRequiredUniformSize(UniformType::Float4));
				}
				else if (desc->Type == UniformType::Matrix4x4)
				{
					shader->SendUniformToShader(desc->ShaderVarID, &desc->MatrixData);
					SetUniformSetValue(desc->ShaderVarID, GetRequiredUniformSize(UniformType::Matrix4x4));
				}
			}
		}
	}

	/*
	 * Send all uniforms for which values have been set to the shader.
	 */
	void Material::SendAllSetUniformsToShader()
	{
		for (UInt32 i = 0; i < GetSetUniformCount(); i++)
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
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetTexture -> Shader is null.", true);
		NONFATAL_ASSERT(texture.IsValid(), "Material::SetTexture -> 'texture' is null.", true);

		Int32 loc, foundIndex;
		Bool success = ValidateUniformName(varName, loc, foundIndex);
		if (!success)return;

		TextureAttributes textureAttributes = texture->GetAttributes();

		UniformDescriptor * desc = setUniforms[foundIndex];
		desc->ShaderVarID = loc;
		if (textureAttributes.IsCube)desc->Type = UniformType::SamplerCube;
		else desc->Type = UniformType::Sampler2D;
		desc->SamplerData = texture;
		desc->SamplerUnitIndex = GetSamplerUnitForName(varName);
		desc->IsSet = true;
	}

	/*
	 * Find a uniform with the name specified by [shaderVarName] and set its
	 * value to [val]
	 */
	void Material::SetMatrix4x4(const Matrix4x4& mat, const std::string& varName)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetMatrix4x4 -> Shader is null.", true);

		Int32 loc, foundIndex;
		Bool success = ValidateUniformName(varName, loc, foundIndex);
		if (!success)return;

		UniformDescriptor * desc = setUniforms[foundIndex];
		desc->ShaderVarID = loc;
		desc->Type = UniformType::Matrix4x4;
		desc->MatrixData = mat;
		desc->IsSet = true;
	}


	/*
	 * Find a uniform with the name specified by [shaderVarName] and set its
	 * value to [val]
	 */
	void Material::SetUniform1f(Real val, const std::string& varName)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetUniform1f -> 'shader' is null.", true);

		Int32 loc, foundIndex;
		Bool success = ValidateUniformName(varName, loc, foundIndex);
		if (!success)return;

		UniformDescriptor * desc = setUniforms[foundIndex];
		desc->ShaderVarID = loc;
		desc->Type = UniformType::Float;
		desc->BasicFloatData[0] = val;
		desc->IsSet = true;
	}

	/*
	 * Find a uniform with the name specified by [shaderVarName] and set its
	 * value to [val]
	 */
	void Material::SetUniform2f(Real v1, Real v2, const std::string& varName)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetUniform2f -> 'shader' is null.", true);

		Int32 loc, foundIndex;
		Bool success = ValidateUniformName(varName, loc, foundIndex);
		if (!success)return;

		UniformDescriptor * desc = setUniforms[foundIndex];
		desc->ShaderVarID = loc;
		desc->Type = UniformType::Float2;
		desc->BasicFloatData[0] = v1;
		desc->BasicFloatData[1] = v2;
		desc->IsSet = true;
	}

	/*
	 * Find a uniform with the name specified by [shaderVarName] and set its
	 * value to [val]
	 */
	void Material::SetUniform4f(Real v1, Real v2, Real v3, Real v4, const std::string& varName)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetUniform4f -> 'shader' is null.", true);

		Int32 loc, foundIndex;
		Bool success = ValidateUniformName(varName, loc, foundIndex);
		if (!success)return;

		UniformDescriptor * desc = setUniforms[foundIndex];
		desc->ShaderVarID = loc;
		desc->Type = UniformType::Float4;
		desc->BasicFloatData[0] = v1;
		desc->BasicFloatData[1] = v2;
		desc->BasicFloatData[2] = v3;
		desc->BasicFloatData[3] = v4;
		desc->IsSet = true;
	}

	/* Find a uniform with the name specified by [shaderVarName] and set its
	* value to [val]
	*/
	void Material::SetColor(const Color4& val, const std::string& varName)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetColor -> 'shader' is null.", true);

		Int32 loc, foundIndex;
		Bool success = ValidateUniformName(varName, loc, foundIndex);
		if (!success)return;

		UniformDescriptor * desc = setUniforms[foundIndex];
		desc->ShaderVarID = loc;
		desc->Type = UniformType::Float4;
		desc->BasicFloatData[0] = val.r;
		desc->BasicFloatData[1] = val.g;
		desc->BasicFloatData[2] = val.b;
		desc->BasicFloatData[3] = val.a;
		desc->IsSet = true;
	}

	/*
	 * Get the number of set uniforms...this should for the most part be
	 * equal to the total number of uniforms exposed by this material's shader.
	 */
	UInt32 Material::GetSetUniformCount() const
	{
		return (UInt32)setUniforms.size();
	}

	/*
	 * Send the number of active clip planes to the material's shader.
	 */
	void Material::SendClipPlaneCountToShader(UInt32 count)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendClipPlaneToShader -> 'shader' is null.", true);

		Int32 varID = GetStandardUniformBinding(StandardUniform::ClipPlaneCount);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, (Int32)count);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Int));
		}
	}

	/*
	 * Send the clip plane specified collectively by [eq1], [eq2], [eq3], and [eq4]
	 * to this material's shader for clip number [index].
	 */
	void Material::SendClipPlaneToShader(UInt32 index, Real eq1, Real eq2, Real eq3, Real eq4)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendClipPlaneToShader -> 'shader' is null.", true);

		Int32 varID = GetStandardUniformBinding((StandardUniform)((UInt32)StandardUniform::ClipPlane0 + index));
		if (varID >= 0)
		{
			shader->SendUniformToShader4(varID, eq1, eq2, eq3, eq4);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}
	}

	/*
	 * Send the 4x4 matrix data in [mat] to this material's shader via the
	 * standard uniform ModelMatrix.
	 */
	void Material::SendModelMatrixToShader(const Matrix4x4 * mat)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendModelMatrixToShader -> 'shader' is null.", true);

		Int32 varID = GetStandardUniformBinding(StandardUniform::ModelMatrix);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, mat);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Matrix4x4));
		}
	}

	/*
	 * Send the 4x4 matrix data in [mat] to this material's shader via the
	 * standard uniform ModelViewMatrix.
	 */
	void Material::SendModelViewMatrixToShader(const Matrix4x4 * mat)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendModelViewMatrixToShader -> 'shader' is null.", true);

		Int32 varID = GetStandardUniformBinding(StandardUniform::ModelViewMatrix);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, mat);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Matrix4x4));
		}
	}

	/*
	 * Send the 4x4 matrix data in [mat] to this material's shader via the
	 * standard uniform ProjectionMatrix.
	 */
	void Material::SendProjectionMatrixToShader(const Matrix4x4 * mat)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendProjectionMatrixToShader -> 'shader' is null.", true);

		Int32 varID = GetStandardUniformBinding(StandardUniform::ProjectionMatrix);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, mat);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Matrix4x4));
		}
	}

	/*
	 * Send the 4x4 matrix data in [mat] to this material's shader via the
	 * standard uniform ModelViewProjectionMatrix.
	 */
	void Material::SendMVPMatrixToShader(const Matrix4x4 * mat)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendMVPMatrixToShader -> 'shader' is null.", true);

		Int32 varID = GetStandardUniformBinding(StandardUniform::ModelViewProjectionMatrix);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, mat);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Matrix4x4));
		}
	}

	/*
	 * Send the light data in [light] to this material's shader using
	 * several standard uniforms: LIGHT_POSITION, LIGHT_DIRECTION, LIGHT_ATTENUATION, and LIGHT_COLOR
	 */
	void Material::SendLightToShader(const Light * light, const Point3 * position, const Vector3 * altDirection)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendLightToShader -> 'shader' is null.", true);
		NONFATAL_ASSERT(light != nullptr, "Material::SendLightToShader -> 'light' is null.", true);
		NONFATAL_ASSERT(position != nullptr, "Material::SendLightToShader -> 'position' is null.", true);

		Int32 varID = GetStandardUniformBinding(StandardUniform::LightType);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, (Int32)light->GetType());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float));
		}

		varID = GetStandardUniformBinding(StandardUniform::LightPosition);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, position);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}

		varID = GetStandardUniformBinding(StandardUniform::LightDirection);
		if (varID >= 0)
		{
			if (altDirection != nullptr)shader->SendUniformToShader(varID, altDirection);
			else shader->SendUniformToShader(varID, light->GetDirectionPtr());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}

		varID = GetStandardUniformBinding(StandardUniform::LightColor);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, light->GetColorPtr());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}

		varID = GetStandardUniformBinding(StandardUniform::LightIntensity);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, light->GetIntensity());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float));
		}

		varID = GetStandardUniformBinding(StandardUniform::LightAttenuation);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, light->GetAttenuation());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float));
		}
	}

	void Material::SendEyePositionToShader(const Point3 * position)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendViewPositionToShader -> 'shader' is null.", true);

		Int32 varID = GetStandardUniformBinding(StandardUniform::EyePosition);
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, position);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}
	}

	/*
	 * Verify that all uniforms and attributes exposed by this material's shader have values
	 * set for them and that those values are of the correct size.
	 */
	Bool Material::VerifySetVars(Int32 vertexCount)
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::VerifySetVars -> 'shader' is null.", false, true);
		if (allSetUniformsandAttributesVerified == true)return true;

		for (UInt32 i = 0; i < shader->GetAttributeCount(); i++)
		{
			const AttributeDescriptor * desc = shader->GetAttributeDescriptor(i);
			if (attributesSetValues[i] != vertexCount)
			{
				std::string msg = "Material::VerifySetVars -> Attribute '";
				msg += desc->Name + std::string("' set incorrectly: size is ") + std::to_string(attributesSetValues[i]);
				msg += std::string(" instead of ") + std::to_string(vertexCount);

				Debug::PrintError(msg);
				return false;
			}
		}

		for (UInt32 i = 0; i < shader->GetUniformCount(); i++)
		{
			const UniformDescriptor * desc = shader->GetUniformDescriptor(i);
			Int32 requiredSize = GetRequiredUniformSize(desc->Type);

			if (uniformsSetValues[i] != requiredSize)
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

	/*
	 * Specify whether this material is self-lit or not.
	 */
	void Material::SetSelfLit(Bool selfLit)
	{
		this->selfLit = selfLit;
	}

	/*
	 * Is this material self-lit?
	 */
	Bool Material::IsSelfLit()
	{
		return selfLit;
	}
}


