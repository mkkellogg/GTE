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
#include "graphics/stduniforms.h"
#include "graphics/materialvardirectory.h"
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

		allSetUniformsandAttributesVerified = false;
		attributesSetAndVerified = false;
		uniformsSetAndVerified = false;

		useLighting = true;
		renderQueueType = RenderQueueType::Geometry;
		blendingMode = RenderState::BlendingMode::None;
		sourceBlendingMethod = RenderState::BlendingMethod::SrcAlpha;
		destBlendingMethod = RenderState::BlendingMethod::OneMinusSrcAlpha;
		faceCulling = RenderState::FaceCulling::Back;
		depthBufferWriteEnabled = true;
		depthBufferFunction = RenderState::DepthBufferFunction::LessThanOrEqual;

		currentSamplerUnitIndex = 0;
	}

	/*
	 * Clean up
	 */
	Material::~Material()
	{
		DestroyUniformDescriptors();
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

		InitializeUniformDescriptors();
		InitializeAttributeDescriptors();
	
		// setup bindings to standard attributes and uniforms for [shader]
		BindVars();

		// reset verification of all attributes and uniforms for [shader]
		ResetVerificationState();

		return true;
	}

	/*
	 * Set up an array of UniformDescriptor objects corresponding to each uniform
	 * exposed by this material's shader.
	 */
	Bool Material::InitializeUniformDescriptors()
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::InitializeUniformDescriptors -> Shader is invalid.", false, true);

		DestroyUniformDescriptors();

		UInt32 uniformCount = shader->GetUniformCount();
		for (UInt32 i = 0; i < uniformCount; i++)
		{
			const UniformDescriptor * desc = shader->GetUniformDescriptor(i);
			localUniformDescriptors.push_back(*desc);
		}

		return true;
	}

	/*
	* Clean up uniform descriptors.
	*/
	void Material::DestroyUniformDescriptors()
	{
		localUniformDescriptors.clear();
	}

	/*
	* Set up an array of AttributeDescriptor objects corresponding to each attribute
	* exposed by this material's shader.
	*/
	Bool Material::InitializeAttributeDescriptors()
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::InitializeAttributeDescriptors -> Shader is invalid.", false, true);

		DestroyAttributeDescriptors();

		UInt32 attributeCount = shader->GetAttributeCount();
		for(UInt32 i = 0; i < attributeCount; i++)
		{
			const AttributeDescriptor * desc = shader->GetAttributeDescriptor(i);
			localAttributeDescriptors.push_back(*desc);
		}

		return true;
	}

	/*
	* Clean up attribute descriptors.
	*/
	void Material::DestroyAttributeDescriptors()
	{
		localAttributeDescriptors.clear();
	}

	/*
	 * Indicate the set data size [size] for the attribute corresponding to [varID].
	 */
	void Material::SetAttributeSetValue(Int32 varID, Int32 size)
	{
		Int32 varIndex = GetLocalAttributeDescriptorIndexByShaderVarID(varID);
		if (varIndex >= 0)
		{
			AttributeDescriptor& desc = localAttributeDescriptors[varIndex];
			desc.IsSet = true;
			desc.SetSize = size;
		}
	}

	/*
	 * Indicate the set data size [size] for the uniform corresponding to [varID].
	 */
	void Material::SetUniformSetValue(Int32 varID, Int32 size)
	{
		Int32 varIndex = GetLocalUniformDescriptorIndexByShaderVarID(varID);
		if (varIndex >= 0)
		{			
			UniformDescriptor& desc = localUniformDescriptors[varIndex];
			desc.IsSet = true;
			desc.SetSize = size;
		}
	}

	/*
	 * Reset the "set state" for all descriptors for uniforms and attributes (in [localUniformDescriptors] and 
	 * [localAttributeDescriptors] respectively). This means the "IsSet" and "SetSize" parameters are reset to
	 * indicate that a value for the uniform or attribute has not yet been set.
	 */
	void Material::ResetVerificationState()
	{
		for(UInt32 i = 0; i < localAttributeDescriptors.size(); i++)
		{
			AttributeDescriptor& desc = localAttributeDescriptors[i];
			desc.IsSet = false;
			desc.SetSize = 0;
		}

		for(UInt32 i = 0; i < localUniformDescriptors.size(); i++)
		{
			UniformDescriptor& desc = localUniformDescriptors[i];
			desc.IsSet = false;
			desc.SetSize = 0;
		}

		allSetUniformsandAttributesVerified = false;
	}

	/*
	* Get the index in the local list of attribute descriptors corresponding
	* to [shaderVarID].
	*/
	Int32 Material::GetLocalAttributeDescriptorIndexByShaderVarID(UInt32 shaderVarID) const
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::GetLocalAttributeDescriptorIndexByShaderVarID -> Shader is invalid.", -1, true);

		Int32 foundIndex = -1;
		for(UInt32 i = 0; i < localAttributeDescriptors.size(); i++)
		{
			const AttributeDescriptor& desc = localAttributeDescriptors[i];
			if(shaderVarID == desc.ShaderVarID)
			{
				foundIndex = (Int32)i;
			}
		}

		return foundIndex;
	}

	/*
	* Get the index in the local list of attribute descriptors corresponding
	* to [attribute].
	*/
	Int32 Material::GetLocalAttributeDescriptorIndexByAttributeID(AttributeID attribute) const
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::GetLocalAttributeDescriptorIndexByAttributeID -> Shader is invalid.", -1, true);

		Int32 foundIndex = -1;
		for(UInt32 i = 0; i < localAttributeDescriptors.size(); i++)
		{
			const AttributeDescriptor& desc = localAttributeDescriptors[i];
			if(attribute == desc.RegisteredAttributeID)
			{
				foundIndex = (Int32)i;
			}
		}

		return foundIndex;
	}

	/*
	 * Map an attribute [attribute] to a shader var ID/location [varID].
	 */
	void Material::SetAttributeBinding(Int32 varID, AttributeID attribute)
	{
		Int32 index = GetLocalAttributeDescriptorIndexByAttributeID(attribute);
		NONFATAL_ASSERT(index >= 0, "Material::SetAttributeBinding -> Attribute not found.", true);

		if((UInt32)index < localAttributeDescriptors.size())
		{
			AttributeDescriptor& desc = localAttributeDescriptors[index];
			desc.ShaderVarID = varID;
		}
	}

	/*
	 * Get the shader var ID/location for [attribute].
	 */
	Int32 Material::GetAttributeBinding(AttributeID attribute) const
	{
		Int32 index = GetLocalAttributeDescriptorIndexByAttributeID(attribute);
		if(index < 0)return -1;

		if((UInt32)index < localAttributeDescriptors.size())
		{
			const AttributeDescriptor& desc = localAttributeDescriptors[index];
			return desc.ShaderVarID;
		}

		return -1;
	}

	/*
	 * Check if the attribute specified by [attribute] is used by
	 * the shader connected to this material.
	 */
	Int32 Material::TestForAttribute(AttributeID attribute) const
	{
		const std::string* attrName = AttributeDirectory::GetVarName(attribute);
		if(attrName == nullptr)return 0;
		Int32 varID = shader->GetAttributeVarID(*attrName);

		return varID;
	}

	/*
	* Get the index in the local list of uniform descriptors corresponding
	* to [uniformName].
	 */
	Int32 Material::GetLocalUniformDescriptorIndexByName(const std::string& uniformName) const
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::GetLocalUniformDescriptorIndexByName -> Shader is invalid.", -1, true);

		Int32 foundIndex = -1;
		for (UInt32 i = 0; i < localUniformDescriptors.size(); i++)
		{
			const UniformDescriptor& desc = localUniformDescriptors[i];
			if (uniformName == desc.Name)
			{
				foundIndex = (Int32)i;
			}
		}

		return foundIndex;
	}

	/*
	* Get the index in the local list of uniform descriptors corresponding
	* to [uniform].
	*/
	Int32 Material::GetLocalUniformDescriptorIndexByUniformID(UniformID uniform) const
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::GetLocalUniformDescriptorIndexByUniformID -> Shader is invalid.", -1, true);

		Int32 foundIndex = -1;
		for(UInt32 i = 0; i < localUniformDescriptors.size(); i++)
		{
			const UniformDescriptor& desc = localUniformDescriptors[i];
			if(uniform == desc.RegisteredUniformID)
			{
				foundIndex = (Int32)i;
			}
		}

		return foundIndex;
	}

	/*
	* Get the index in the local list of uniform descriptors corresponding
	* to [uniform].
	*/
	Int32 Material::GetLocalUniformDescriptorIndexByShaderVarID(UInt32 shaderVarID) const
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::GetLocalUniformDescriptorIndexByShaderVarID -> Shader is invalid.", -1, true);

		Int32 foundIndex = -1;
		for(UInt32 i = 0; i < localUniformDescriptors.size(); i++)
		{
			const UniformDescriptor& desc = localUniformDescriptors[i];
			if(shaderVarID == desc.ShaderVarID)
			{
				foundIndex = (Int32)i;
			}
		}

		return foundIndex;
	}

	/*
	* Get the texture sampler unit that is to be used by [uniform]. This should only be
	* called on texture sampler uniforms.
	*/
	UInt32 Material::ReserveSamplerUnitForUniform(UniformID uniform)
	{
		UInt32 index = 0;

		auto result = textureUniformSamplerUnitIndex.find(uniform);

		if (result == textureUniformSamplerUnitIndex.end())
		{
			textureUniformSamplerUnitIndex[uniform] = currentSamplerUnitIndex;
			index = currentSamplerUnitIndex;
			currentSamplerUnitIndex++;
		}
		else
		{
			index = (*result).second;
		}

		return index;
	}

	/*
	 * Map a uniform to a shader var ID/location.
	 */
	void Material::SetUniformBinding(Int32 varID, UniformID uniform)
	{
		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		NONFATAL_ASSERT(index >= 0, "Material::SetUniformBinding -> Uniform not found.", true);

		if((UInt32)index < localUniformDescriptors.size())
		{
			UniformDescriptor& desc = localUniformDescriptors[index];
			desc.ShaderVarID = varID;
		}
	}

	/*
	 * Get the shader var ID/location for [uniform].
	 */
	Int32 Material::GetUniformBinding(UniformID uniform) const
	{
		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		if(index < 0)return -1;

		if((UInt32)index < localUniformDescriptors.size())
		{
			const UniformDescriptor& desc = localUniformDescriptors[index];
			return desc.ShaderVarID;
		}

		return -1;
	}

	/*
	 * Check if the uniform specified by [uniform] is used by
	 * the shader connected to this material.
	 */
	Int32 Material::TestForUniform(UniformID uniform) const
	{
		const std::string* uniformName = UniformDirectory::GetVarName(uniform);

		if(uniformName == nullptr)return -1;

		Int32 loc = shader->GetUniformVarID(*uniformName);
		return loc;
	}

	/*
	 * Validate the existence and binding of the shader variable specified by [name].
	 * Store the shader variable location in [loc] and mapped index in [localUniformDescriptors] of
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

		// get the index in [localUniformDescriptors] that has the UniformDescriptor for the
		// uniform named [varName]
		index = GetLocalUniformDescriptorIndexByName(name);

		if (index < 0)
		{
			std::string err = std::string("Material::ValidateUniformName -> Invalid uniform specified: ") + name;
			Debug::PrintError(err);
			return false;
		}
		return true;
	}

	/*
	 * Link all uniforms and attributes exposed by this material to their counterparts in
	 * the global Uniform directory and Attribute directory respectively. Also populate the
	 * bit masks [standardUniforms] and [standardAttributes] to indicate which standard uniforms
	 * and which standard attributes are utilized by this material.
	 *
	 */
	void Material::BindVars()
	{
		standardAttributes = StandardAttributes::CreateAttributeSet();
		for(UInt32 i = 0; i < localAttributeDescriptors.size(); i++)
		{
			AttributeDescriptor& desc = localAttributeDescriptors[i];
			desc.RegisteredAttributeID = AttributeDirectory::RegisterVarID(desc.Name);

			StandardAttribute attr = AttributeDirectory::GetStandardVar(desc.RegisteredAttributeID);
			if(attr != StandardAttribute::_None)
			{
				StandardAttributes::AddAttribute(&standardAttributes, attr);
			}
		}

		standardUniforms = StandardUniforms::CreateUniformSet();
		for(UInt32 i = 0; i < localUniformDescriptors.size(); i++)
		{
			UniformDescriptor& desc = localUniformDescriptors[i];
			desc.RegisteredUniformID = UniformDirectory::RegisterVarID(desc.Name);

			StandardUniform uniform = UniformDirectory::GetStandardVar(desc.RegisteredUniformID);
			if(uniform != StandardUniform::_None)
			{
				StandardUniforms::AddUniform(&standardUniforms, uniform);
			}
		}
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
	void Material::SendAttributeBufferToShader(AttributeID attribute, VertexAttrBuffer *buffer)
	{
		NONFATAL_ASSERT(buffer != nullptr, "Material::SendAttributeBufferToShader -> 'buffer' is null.", true);

		Int32 varID = GetAttributeBinding(attribute);
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
	 * set [localUniformDescriptors]. This method simply takes the value stored for that uniform and sends
	 * it to the shader.
	 */
	void Material::SendStoredUniformValueToShader(UInt32 index)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendStoredUniformValueToShader -> 'shader' is null.", true);

		if (index < localUniformDescriptors.size())
		{
			UniformDescriptor& desc = localUniformDescriptors[index];
		
			if (desc.Type == UniformType::Sampler2D)
			{
				shader->SendUniformToShader(desc.ShaderVarID, desc.SamplerUnitIndex, desc.SamplerData);
				SetUniformSetValue(desc.ShaderVarID, GetRequiredUniformSize(UniformType::Sampler2D));
			}
			if (desc.Type == UniformType::SamplerCube)
			{
				shader->SendUniformToShader(desc.ShaderVarID, desc.SamplerUnitIndex, desc.SamplerData);
				SetUniformSetValue(desc.ShaderVarID, GetRequiredUniformSize(UniformType::SamplerCube));
			}
			else if (desc.Type == UniformType::Float)
			{
				shader->SendUniformToShader(desc.ShaderVarID, desc.BasicFloatData[0]);
				SetUniformSetValue(desc.ShaderVarID, GetRequiredUniformSize(UniformType::Float));
			}
			else if (desc.Type == UniformType::Float2)
			{
				shader->SendUniformToShader2(desc.ShaderVarID, desc.BasicFloatData[0], desc.BasicFloatData[1]);
				SetUniformSetValue(desc.ShaderVarID, GetRequiredUniformSize(UniformType::Float2));
			}
			else if (desc.Type == UniformType::Float3)
			{
				shader->SendUniformToShader3(desc.ShaderVarID, desc.BasicFloatData[0], desc.BasicFloatData[1], desc.BasicFloatData[2]);
				SetUniformSetValue(desc.ShaderVarID, GetRequiredUniformSize(UniformType::Float3));
			}
			else if (desc.Type == UniformType::Float4)
			{
				shader->SendUniformToShader4(desc.ShaderVarID, desc.BasicFloatData[0], desc.BasicFloatData[1], desc.BasicFloatData[2], desc.BasicFloatData[3]);
				SetUniformSetValue(desc.ShaderVarID, GetRequiredUniformSize(UniformType::Float4));
			}
			else if (desc.Type == UniformType::Matrix4x4)
			{
				shader->SendUniformToShader(desc.ShaderVarID, &desc.MatrixData);
				SetUniformSetValue(desc.ShaderVarID, GetRequiredUniformSize(UniformType::Matrix4x4));
			}
		}
	}

	/*
	 * Send all uniforms for which values have been set to the shader.
	 */
	void Material::SendAllStoredUniformValuesToShader()
	{
		for (UInt32 i = 0; i < localUniformDescriptors.size(); i++)
		{
			SendStoredUniformValueToShader(i);
		}
	}

	/*
	 * Find a uniform with the name specified by [varName] and set its
	 * value to the sampler data held by [texture]
	 */
	void Material::SetTexture(TextureRef texture, const std::string& varName)
	{
		UniformID uniform = UniformDirectory::RegisterVarID(varName);
		SetTexture(texture, uniform);
	}

	/*
	* Find a uniform with the name specified by [uniform] and set its
	* value to the sampler data held by [texture].
	*/
	void Material::SetTexture(TextureRef texture, UniformID uniform)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetTexture -> Shader is null.", true);
		NONFATAL_ASSERT(texture.IsValid(), "Material::SetTexture -> 'texture' is null.", true);

		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		NONFATAL_ASSERT(index >= 0, "Material::SetTexture -> Invalid uniform.", true);

		TextureAttributes textureAttributes = texture->GetAttributes();

		UniformDescriptor& desc = localUniformDescriptors[index];
		if(textureAttributes.IsCube)desc.Type = UniformType::SamplerCube;
		else desc.Type = UniformType::Sampler2D;
		desc.SamplerData = texture;
		desc.SamplerUnitIndex = ReserveSamplerUnitForUniform(uniform);
		desc.IsSet = true;
	}

	/*
	 * Find a uniform with the name specified by [varName] and set its
	 * value to the 4x4 matrix [val].
	 */
	void Material::SetMatrix4x4(const Matrix4x4& mat, const std::string& varName)
	{
		UniformID uniform = UniformDirectory::RegisterVarID(varName);
		SetMatrix4x4(mat, uniform);
	}

	/*
	* Find a uniform with the name specified by [uniform] and set its
	* value to the 4x4 matrix [val].
	*/
	void Material::SetMatrix4x4(const Matrix4x4& mat, UniformID uniform)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetMatrix4x4 -> Shader is null.", true);

		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		NONFATAL_ASSERT(index >= 0, "Material::SetMatrix4x4 -> Invalid uniform.", true);

		UniformDescriptor& desc = localUniformDescriptors[index];
		desc.Type = UniformType::Matrix4x4;
		desc.MatrixData = mat;
		desc.IsSet = true;
	}

	/*
	 * Find a uniform with the name specified by [varName] and set its
	 * value to the floating point value [val].
	 */
	void Material::SetUniform1f(Real val, const std::string& varName)
	{
		UniformID uniform = UniformDirectory::RegisterVarID(varName);
		SetUniform1f(val, uniform);
	}

	/*
	* Find a uniform with the name specified by [uniform] and set its
	* value to the floating point value [val]
	*/
	void Material::SetUniform1f(Real val, UniformID uniform)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetUniform1f -> 'shader' is null.", true);

		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		NONFATAL_ASSERT(index >= 0, "Material::SetUniform1f -> Invalid uniform.", true);

		UniformDescriptor & desc = localUniformDescriptors[index];
		desc.Type = UniformType::Float;
		desc.BasicFloatData[0] = val;
		desc.IsSet = true;
	}

	/*
	 * Find a uniform with the name specified by [varName] and set its
	 * value to the vector made up of v1 & v2.
	 */
	void Material::SetUniform2f(Real v1, Real v2, const std::string& varName)
	{
		UniformID uniform = UniformDirectory::RegisterVarID(varName);
		SetUniform2f(v1, v2, uniform);
	}

	/*
	* Find a uniform with the name specified by [uniform] and set its
	* value to the vector made up of v1 & v2.
	*/
	void Material::SetUniform2f(Real v1, Real v2, UniformID uniform)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetUniform2f -> 'shader' is null.", true);

		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		NONFATAL_ASSERT(index >= 0, "Material::SetUniform2f -> Invalid uniform.", true);

		UniformDescriptor& desc = localUniformDescriptors[index];
		desc.Type = UniformType::Float2;
		desc.BasicFloatData[0] = v1;
		desc.BasicFloatData[1] = v2;
		desc.IsSet = true;
	}

	/*
	 * Find a uniform with the name specified by [varName] and set its
	 * value to the vector made up of v1 & v2 & v3 &v4.
	 */
	void Material::SetUniform4f(Real v1, Real v2, Real v3, Real v4, const std::string& varName)
	{
		UniformID uniform = UniformDirectory::RegisterVarID(varName);
		SetUniform4f(v1, v2, v3, v4, uniform);
	}

	/*
	* Find a uniform with the name specified by [uniform] and set its
	* value to the vector made up of v1 & v2 & v3 &v4.
	*/
	void Material::SetUniform4f(Real v1, Real v2, Real v3, Real v4, UniformID uniform)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetUniform4f -> 'shader' is null.", true);

		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		NONFATAL_ASSERT(index >= 0, "Material::SetUniform4f -> Invalid uniform.", true);

		UniformDescriptor& desc = localUniformDescriptors[index];
		desc.Type = UniformType::Float4;
		desc.BasicFloatData[0] = v1;
		desc.BasicFloatData[1] = v2;
		desc.BasicFloatData[2] = v3;
		desc.BasicFloatData[3] = v4;
		desc.IsSet = true;
	}

	/* Find a uniform with the name specified by [varName] and set its
	* value to the color [color].
	*/
	void Material::SetColor(const Color4& color, const std::string& varName)
	{
		UniformID uniform = UniformDirectory::RegisterVarID(varName);
		SetColor(color, uniform);
	}

	/* Find a uniform with the name specified by [uniform] and set its
	* value to the color [color].
	*/
	void Material::SetColor(const Color4& color, UniformID uniform)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SetColor -> 'shader' is null.", true);

		Int32 index = GetLocalUniformDescriptorIndexByUniformID(uniform);
		NONFATAL_ASSERT(index >= 0, "Material::SetUniform4f -> Invalid uniform.", true);

		UniformDescriptor& desc = localUniformDescriptors[index];
		desc.Type = UniformType::Float4;
		desc.BasicFloatData[0] = color.r;
		desc.BasicFloatData[1] = color.g;
		desc.BasicFloatData[2] = color.b;
		desc.BasicFloatData[3] = color.a;
		desc.IsSet = true;
	}

	/*
	 * Send the number of active clip planes to the material's shader.
	 */
	void Material::SendClipPlaneCountToShader(UInt32 count)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendClipPlaneToShader -> 'shader' is null.", true);

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::ClipPlaneCount));
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

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID((StandardUniform)((UInt32)StandardUniform::ClipPlane0 + index)));
		if (varID >= 0)
		{
			shader->SendUniformToShader4(varID, eq1, eq2, eq3, eq4);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}
	}

	/*
	 * Send the 4x4 matrix data in [mat] to this material's shader via the
	 * standard uniform ModelMatrixInverseTranspose.
	 */
	void Material::SendModelMatrixInverseTransposeToShader(const Matrix4x4 * mat)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendModelMatrixInverseTransposeToShader -> 'shader' is null.", true);

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::ModelMatrixInverseTranspose));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, mat);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Matrix4x4));
		}
	}

	/*
	 * Send the 4x4 matrix data in [mat] to this material's shader via the
	 * standard uniform ModelMatrix.
	 */
	void Material::SendModelMatrixToShader(const Matrix4x4 * mat)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendModelMatrixToShader -> 'shader' is null.", true);

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::ModelMatrix));
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

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::ModelViewMatrix));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, mat);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Matrix4x4));
		}
	}

	/*
	* Send the 4x4 matrix data in [mat] to this material's shader via the
	* standard uniform ViewMatrix.
	*/
	void Material::SendViewMatrixToShader(const Matrix4x4 * mat)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendViewMatrixToShader -> 'shader' is null.", true);

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::ViewMatrix));
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

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::ProjectionMatrix));
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

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::ModelViewProjectionMatrix));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, mat);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Matrix4x4));
		}
	}

	/*
	 * Send the light data in [light] to this material's shader using
	 * several standard uniforms: LIGHT_POSITION, LIGHT_DIRECTION, LIGHT_ATTENUATION, and LIGHT_COLOR.
	 */
	void Material::SendLightToShader(const Light * light, const Point3 * position, const Vector3 * altDirection)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendLightToShader -> 'shader' is null.", true);
		NONFATAL_ASSERT(light != nullptr, "Material::SendLightToShader -> 'light' is null.", true);
		NONFATAL_ASSERT(position != nullptr, "Material::SendLightToShader -> 'position' is null.", true);

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightType));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, (Int32)light->GetType());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightPosition));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, position);
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightDirection));
		if (varID >= 0)
		{
			if (altDirection != nullptr)shader->SendUniformToShader(varID, altDirection);
			else shader->SendUniformToShader(varID, light->GetDirectionPtr());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightColor));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, light->GetColorPtr());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float4));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightIntensity));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, light->GetIntensity());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightRange));
		if(varID >= 0)
		{
			shader->SendUniformToShader(varID, light->GetRange());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightAttenuation));
		if (varID >= 0)
		{
			shader->SendUniformToShader(varID, light->GetAttenuation());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Float));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightParallelAngleAttenuation));
		if(varID >= 0)
		{
			shader->SendUniformToShader(varID, (Int32)light->GetParallelAngleAttenuationType());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Int));
		}

		varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::LightOrthoAngleAttenuation));
		if(varID >= 0)
		{
			shader->SendUniformToShader(varID, (Int32)light->GetOrthoAngleAttenuationType());
			SetUniformSetValue(varID, GetRequiredUniformSize(UniformType::Int));
		}
	}

	void Material::SendEyePositionToShader(const Point3 * position)
	{
		NONFATAL_ASSERT(shader.IsValid(), "Material::SendViewPositionToShader -> 'shader' is null.", true);

		Int32 varID = GetUniformBinding(UniformDirectory::GetStandardVarID(StandardUniform::EyePosition));
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
	Bool Material::VerifySetVars(UInt32 vertexCount)
	{
		NONFATAL_ASSERT_RTRN(shader.IsValid(), "Material::VerifySetVars -> 'shader' is null.", false, true);
		if (allSetUniformsandAttributesVerified == true)return true;

		for (UInt32 i = 0; i < shader->GetAttributeCount(); i++)
		{
			const AttributeDescriptor& desc = localAttributeDescriptors[i];
			if (desc.SetSize != vertexCount)
			{
				std::string msg = "Material::VerifySetVars -> Attribute '";
				msg += desc.Name + std::string("' set incorrectly: size is ") + std::to_string(desc.SetSize);
				msg += std::string(" instead of ") + std::to_string(vertexCount);

				Debug::PrintError(msg);
				return false;
			}
		}

		for (UInt32 i = 0; i < shader->GetUniformCount(); i++)
		{
			const UniformDescriptor& desc = localUniformDescriptors[i];
			UInt32 requiredSize = GetRequiredUniformSize(desc.Type);

			if (desc.SetSize != requiredSize)
			{
				std::string msg = "Material::VerifySetVars -> Uniform '";
				msg += desc.Name + std::string("' set incorrectly: size is ") + std::to_string(desc.SetSize);
				msg += std::string(" instead of ") + std::to_string(requiredSize);

				Debug::PrintError(msg);
				return false;
			}
		}

		allSetUniformsandAttributesVerified = true;

		return true;
	}

	/*
	 * Specify whether this material makes use of scene lights or not.
	 */
	void Material::SetUseLighting(Bool useLighting)
	{
		this->useLighting = useLighting;
	}

	/*
	 * Does this material make use of scene lights?
	 */
	Bool Material::UseLighting()
	{
		return useLighting;
	}

	/*
	* Set the render queue for this material
	*/
	void Material::SetRenderQueue(RenderQueueType queue)
	{
		renderQueueType = queue;
	}

	/*
	* Get the render queue for this material
	*/
	RenderQueueType Material::GetRenderQueueType()
	{
		return renderQueueType;
	}

	/*
	* Set the blending mode to be used when rendering this material.
	*/
	void Material::SetBlendingMode(RenderState::BlendingMode mode)
	{
		blendingMode = mode;
	}

	/*
	* Get the blending mode to be used when rendering this material.
	*/
	RenderState::BlendingMode Material::GetBlendingMode()
	{
		return blendingMode;
	}

	/*
	* Set the source blending method to be used when rendering this material.
	*/
	void Material::SetSourceBlendingMethod(RenderState::BlendingMethod method)
	{
		sourceBlendingMethod = method;
	}

	/*
	* Get the source blending method to be used when rendering this material.
	*/
	RenderState::BlendingMethod Material::GetSourceBlendingMethod()
	{
		return sourceBlendingMethod;
	}

	/*
	* Set the destination blending method to be used when rendering this material.
	*/
	void Material::SetDestBlendingMethod(RenderState::BlendingMethod method)
	{
		destBlendingMethod = method;
	}

	/*
	* Get the destination blending method to be used when rendering this material.
	*/
	RenderState::BlendingMethod Material::GetDestBlendingMethod()
	{
		return destBlendingMethod;
	}

	/*
	* Set the face culling method to be used when rendering this material.
	*/
	void Material::SetFaceCulling(RenderState::FaceCulling method)
	{
		faceCulling = method;
	}

	/*
	* Get the face culling method to be used when rendering this material.
	*/
	RenderState::FaceCulling Material::GetFaceCulling()
	{
		return faceCulling;
	}

	/*
	* Set whether or not writing to the depth buffer is enabled.
	*/
	void Material::SetDepthBufferWriteEnabled(Bool enabled)
	{
		depthBufferWriteEnabled = enabled;
	}

	/*
	* Get whether or not writing to the depth buffer is enabled.
	*/
	Bool Material::GetDepthBufferWriteEnabled()
	{
		return depthBufferWriteEnabled;
	}

	/*
	* Set the depth buffer write test.
	*/
	void Material::SetDepthBufferFunction(RenderState::DepthBufferFunction function)
	{
		depthBufferFunction = function;
	}

	/*
	* Get the depth buffer write test.
	*/
	RenderState::DepthBufferFunction Material::GetDepthBufferFunction()
	{
		return depthBufferFunction;
	}
}


