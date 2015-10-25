/*
 * class: Material
 *
 * author: Mark Kellogg
 *
 * A Material represents an instance of a Shader along with a set of values for the custom uniforms
 * that shader exposes.
 *
 * A Material, like other engine objects (such as Shader and Mesh3D), utilizes the notion of standard attributes
 * and standard uniforms. These are shader variables that are very common, sometimes required,
 * and are supplied to a shader by the engine, not explicitly by the developer using the engine.
 * An example is mesh vertex normals. The developer does not explicitly set these each frame; the
 * engine takes them from a mesh and sends them to the shader. Therefore the shader variable's name needs to
 * correspond to a predetermined name that the engine has defined. In the case of vertex normals, the name of
 * the variable is 'NORMAL'.
 */

#ifndef _GTE_MATERIAL_H_
#define _GTE_MATERIAL_H_

#include "graphics/stdattributes.h"
#include "graphics/stduniforms.h"
#include "graphics/color/color4.h"
#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "graphics/shader/uniformdesc.h"
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
		StandardAttribute Attribute;
		Int32 AltBinding;

		VertexAttrBufferBinding()
		{
			Attribute = StandardAttribute::_None;
			Buffer = nullptr;
			AltBinding = -1;
		}

		VertexAttrBufferBinding(VertexAttrBuffer * buffer, StandardAttribute attribute, Int32 altBinding)
		{
			this->Buffer = buffer;
			this->Attribute = attribute;
			this->AltBinding = altBinding;
		}
	};

	class Material : public EngineObject
	{
		// Since this derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		// length of the array that contains binding information for stand attributes & uniforms
		static const Int32 BINDINGS_ARRAY_MAX_LENGTH = 128;

		// the following are values for the sizes of data required by various types of uniforms
		static const Int32 SAMPLER_2D_DATA_SIZE = 64;
		static const Int32 SAMPLER_CUBE_DATA_SIZE = 384;
		static const Int32 MATRIX4X4_DATA_SIZE = 16;

		// string that holds the material's name
		std::string materialName;

		// pointer to this material's shader
		ShaderRef shader;

		// bit masks to hold binding status for standard uniforms and attributes
		StandardAttributeSet standardAttributes;
		StandardUniformSet standardUniforms;

		// ids/locations of shader variables corresponding to standard attributes
		Int32 standardAttributeBindings[BINDINGS_ARRAY_MAX_LENGTH];
		// ids/locations of shader variables corresponding to standard uniforms
		Int32 standardUniformBindings[BINDINGS_ARRAY_MAX_LENGTH];

		// a vector UniformDescriptor objects that describe custom uniforms that are set
		// by the developer and
		std::vector<UniformDescriptor*> setUniforms;

		// have all the attributes been given valid values?
		Bool attributesSetAndVerified;

		// length of values for each attribute that has been set
		Int32 * attributesSetValues;

		// map a shader variable ID/location to its index in attributesSetValues
		std::map<int, int> attributeLocationsToVerificationIndex;

		// have all the uniforms been given valid values?
		Bool uniformsSetAndVerified;

		// length of values for each uniform that has been set
		Int32 * uniformsSetValues;

		// map a shader variable ID/location to its index in uniformsSetValues
		std::map<int, int> uniformLocationsToVerificationIndex;

		// current highest used sampler unit index
		UInt32 currentSampletUnityIndex;
		// map a texture uniform to its sampler unit
		std::map<std::string, int> textureUniformSamplerUnitIndex;

		// does this material require a light to be rendered?
		Bool selfLit;

		UInt32 GetRequiredUniformSize(UniformType uniformType);
		Bool allSetUniformsandAttributesVerified;

		void BindStandardVars();
		void ClearStandardBindings();
		Bool SetupSetVerifiers();

		Bool SetupSetUniforms();
		void DestroySetUniforms();

		void SetStandardAttributeBinding(Int32 varID, StandardAttribute attr);
		Int32 GetStandardAttributeBinding(StandardAttribute attr) const;
		Int32 TestForStandardAttribute(StandardAttribute attr) const;

		Int32 GetUniformIndex(const std::string& uniformName);
		UInt32 GetSamplerUnitForName(const std::string& name);
		void SetStandardUniformBinding(Int32 varID, StandardUniform uniform);
		Int32 GetStandardUniformBinding(StandardUniform uniform) const;
		Int32 TestForStandardUniform(StandardUniform uniform) const;
		Bool ValidateUniformName(const std::string& name, int& loc, int& index);

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
		void SendStandardAttributeBufferToShader(StandardAttribute attr, VertexAttrBuffer *buffer);
		void SendAttributeBufferToShader(Int32 varID, VertexAttrBuffer *buffer);

		StandardUniformSet GetStandardUniforms() const;

		void SendSetUniformToShader(UInt32 index);
		void SendAllSetUniformsToShader();
		void SetTexture(TextureRef texture, const std::string& varName);
		void SetMatrix4x4(const Matrix4x4& mat, const std::string& varName);
		void SetUniform1f(Real val, const std::string& varName);
		void SetUniform2f(Real v1, Real v2, const std::string& varName);
		void SetUniform4f(Real v1, Real v2, Real v3, Real v4, const std::string& varName);
		void SetColor(const Color4& val, const std::string& varName);
		UInt32 GetSetUniformCount() const;

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

		Bool VerifySetVars(Int32 vertexCount);

		void SetSelfLit(Bool selfLit);
		Bool IsSelfLit();
	};
}

#endif
