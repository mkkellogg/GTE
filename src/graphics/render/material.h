/*
 * class: Material
 *
 * author: Mark Kellogg
 *
 * A Material represents an instance of a Shader along with a set of values for the uniforms
 * that shader exposes.
 *
 * A Material, like other engine objects (such as Shader and Mesh3D), understands  the notion of standard attributes
 * and standard uniforms. These are shader variables that are very common, sometimes required,
 * and are supplied to a shader by the engine, not explicitly by the developer using the engine.
 * An example is mesh vertex normals. The developer does not explicitly set these each frame; the
 * engine takes them from a mesh and sends them to the shader. Therefore the shader variable's name needs to
 * correspond to a predetermined name that the engine has defined. In the case of vertex normals, the name of
 * the variable is 'NORMAL'.
 *
 * An instance of Material is not restricted to standard attributes and standard uniforms, it supports any attribute
 * or uniform defined by the developer.
 */

#ifndef _GTE_MATERIAL_H_
#define _GTE_MATERIAL_H_

#include "engine.h"
#include "graphics/stdattributes.h"
#include "graphics/renderstate.h"
#include "graphics/stduniforms.h"
#include "graphics/materialvardirectory.h"
#include "graphics/color/color4.h"
#include "object/engineobject.h"
#include "graphics/shader/uniformdesc.h"
#include "graphics/shader/attributedesc.h"
#include "rendermanager.h"

#include <vector>
#include <map>
#include <string>

namespace GTE
{
	//forward declarations
	class Graphics;
	class Shader;
	class VertexAttrBuffer;
	class Light;
	class Texture;
	class Point3;
	class Vector3;

	class VertexAttrBufferBinding
	{
		public:

		VertexAttrBuffer * Buffer;
		AttributeID RegisteredAttributeID;

		VertexAttrBufferBinding()
		{
			Buffer = nullptr;
			RegisteredAttributeID = AttributeDirectory::VarID_Invalid;
		}

		VertexAttrBufferBinding(VertexAttrBuffer * buffer, AttributeID id)
		{
			this->Buffer = buffer;
			this->RegisteredAttributeID = id;
		}
	};

	class Material : public EngineObject
	{
		// Since this derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		// the following are values for the sizes of data required by various types of uniforms
		static const Int32 SAMPLER_2D_DATA_SIZE = 64;
		static const Int32 SAMPLER_CUBE_DATA_SIZE = 384;
		static const Int32 MATRIX4X4_DATA_SIZE = 16;

		// string that holds the material's name
		std::string materialName;

		// pointer to this material's shader
		ShaderSharedPtr shader;

		// bit masks to hold binding status for standard uniforms and attributes
		StandardAttributeSet standardAttributes;
		StandardUniformSet standardUniforms;

		// a vector of UniformDescriptor objects that describe uniforms exposed by this
		// material's shader
		std::vector<UniformDescriptor> localUniformDescriptors;

		// a vector of AttributeDescriptor objects that describe attributes exposed by this
		// material's shader
		std::vector<AttributeDescriptor> localAttributeDescriptors;

		// have all the attributes been given valid values?
		Bool attributesSetAndVerified;

		// have all the uniforms been given valid values?
		Bool uniformsSetAndVerified;

		// current highest used sampler unit index
		UInt32 currentSamplerUnitIndex;
		// map a texture uniform to its sampler unit
		std::map<UniformID, int> textureUniformSamplerUnitIndex;

		// does this material require a light to be rendered?
		Bool useLighting;

		UInt32 renderQueueID;

		RenderState::BlendingMode blendingMode;
		RenderState::BlendingMethod sourceBlendingMethod;
		RenderState::BlendingMethod destBlendingMethod;
		RenderState::FaceCulling faceCulling;
		Bool depthBufferWriteEnabled;
		RenderState::DepthBufferFunction depthBufferFunction;

		UInt32 GetRequiredUniformSize(UniformType uniformType);
		Bool allSetUniformsandAttributesVerified;

		void BindVars();

		Bool InitializeUniformDescriptors();
		void DestroyUniformDescriptors();

		Bool InitializeAttributeDescriptors();
		void DestroyAttributeDescriptors();

		Int32 GetLocalAttributeDescriptorIndexByShaderVarID(UInt32 shaderVarID) const;
		Int32 GetLocalAttributeDescriptorIndexByAttributeID(AttributeID attribute) const;
		void SetAttributeBinding(Int32 varID, AttributeID attribute);
		Int32 GetAttributeBinding(AttributeID attribute) const;
		Int32 TestForAttribute(AttributeID attribute) const;

		Int32 GetLocalUniformDescriptorIndexByName(const std::string& uniformName) const;
		Int32 GetLocalUniformDescriptorIndexByUniformID(UniformID uniform) const;
		Int32 GetLocalUniformDescriptorIndexByShaderVarID(UInt32 shaderVarID) const;
		UInt32 ReserveSamplerUnitForUniform(UniformID uniform);
		void SetUniformBinding(Int32 varID, UniformID uniform);
		Int32 GetUniformBinding(UniformID uniform) const;
		Int32 TestForUniform(UniformID uniform) const;
		Bool ValidateUniformName(const std::string& name, int& loc, int& index);
		void SendStoredUniformValueToShader(UInt32 index);

		void SetAttributeSetValue(Int32 varID, Int32 size);
		void SetUniformSetValue(Int32 varID, Int32 size);


	protected:

		Material(const std::string& materialName);
		virtual ~Material();
		Bool Init(ShaderRef shader);

	public:

		void ResetVerificationState();

		ShaderRef GetShader() ;

		StandardAttributeSet GetStandardAttributes() const;
		void SendAttributeBufferToShader(AttributeID, VertexAttrBuffer *buffer);

		StandardUniformSet GetStandardUniforms() const;
		void SendClipPlaneCountToShader(UInt32 count);
		void SendClipPlaneToShader(UInt32 index, Real eq1, Real eq2, Real eq3, Real eq4);
		void SendModelMatrixInverseTransposeToShader(const Matrix4x4 * mat);
		void SendModelMatrixToShader(const Matrix4x4 * mat);
		void SendModelViewMatrixToShader(const Matrix4x4 * mat);
		void SendViewMatrixToShader(const Matrix4x4 * mat);
		void SendProjectionMatrixToShader(const Matrix4x4 * mat);
		void SendMVPMatrixToShader(const Matrix4x4 * mat);
		void SendLightToShader(const Light * light, const Point3 * position, const Vector3 * altDirection);
		void SendEyePositionToShader(const Point3 * position);

		void SendAllStoredUniformValuesToShader();

		void SetTexture(TextureRef texture, const std::string& varName);
		void SetTexture(TextureRef texture, UniformID uniformID);
		void SetMatrix4x4(const Matrix4x4& mat, const std::string& varName);
		void SetMatrix4x4(const Matrix4x4& mat, UniformID uniformID);
		void SetUniform1f(Real val, const std::string& varName);
		void SetUniform1f(Real val, UniformID uniformID);
		void SetUniform2f(Real v1, Real v2, const std::string& varName);
		void SetUniform2f(Real v1, Real v2, UniformID uniformID);
		void SetUniform3f(Real v1, Real v2, Real v3, const std::string& varName);
		void SetUniform3f(Real v1, Real v2, Real v3, UniformID uniformID);
		void SetUniform4f(Real v1, Real v2, Real v3, Real v4, const std::string& varName);
		void SetUniform4f(Real v1, Real v2, Real v3, Real v4, UniformID uniformID);
		void SetColor(const Color4& color, const std::string& varName);
		void SetColor(const Color4& color, UniformID uniformID);

		Bool VerifySetVars(UInt32 vertexCount);

		void SetUseLighting(Bool selfLit);
		Bool UseLighting();
		void SetRenderQueue(RenderQueueType queue);
		void SetRenderQueue(UInt32 queue);
		UInt32 GetRenderQueue();
		void SetBlendingMode(RenderState::BlendingMode mode);
		RenderState::BlendingMode GetBlendingMode();
		void SetSourceBlendingMethod(RenderState::BlendingMethod method);
		RenderState::BlendingMethod GetSourceBlendingMethod();
		void SetDestBlendingMethod(RenderState::BlendingMethod method);
		RenderState::BlendingMethod GetDestBlendingMethod();
		void SetFaceCulling(RenderState::FaceCulling mode);
		RenderState::FaceCulling GetFaceCulling();
		void SetDepthBufferWriteEnabled(Bool enabled);
		Bool GetDepthBufferWriteEnabled();
		void SetDepthBufferFunction(RenderState::DepthBufferFunction function);
		RenderState::DepthBufferFunction GetDepthBufferFunction();
	};
}

#endif
