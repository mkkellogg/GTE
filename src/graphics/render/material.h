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
		int AltBinding;

		VertexAttrBufferBinding()
		{
			Attribute = StandardAttribute::_None;
			Buffer = NULL;
			AltBinding = -1;
		}

		VertexAttrBufferBinding(VertexAttrBuffer * buffer, StandardAttribute attribute, int altBinding)
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
		static const int BINDINGS_ARRAY_MAX_LENGTH = 128;

		// the following are values for the sizes of data required by various types of uniforms
		static const int SAMPLER_2D_DATA_SIZE = 64;
		static const int SAMPLER_CUBE_DATA_SIZE = 384;
		static const int MATRIX4X4_DATA_SIZE = 16;

		// string that holds the material's name
		std::string materialName;

		// pointer to this material's shader
		ShaderRef shader;

		// bit masks to hold binding status for standard uniforms and attributes
		StandardAttributeSet standardAttributes;
		StandardUniformSet standardUniforms;

		// ids/locations of shader variables corresponding to standard attributes
		int standardAttributeBindings[BINDINGS_ARRAY_MAX_LENGTH];
		// ids/locations of shader variables corresponding to standard uniforms
		int standardUniformBindings[BINDINGS_ARRAY_MAX_LENGTH];

		// a vector UniformDescriptor objects that describe custom uniforms that are set
		// by the developer and
		std::vector<UniformDescriptor*> setUniforms;

		// have all the attributes been given valid values?
		bool attributesSetAndVerified;

		// length of values for each attribute that has been set
		int * attributesSetValues;

		// map a shader variable ID/location to its index in attributesSetValues
		std::map<int, int> attributeLocationsToVerificationIndex;

		// have all the uniforms been given valid values?
		bool uniformsSetAndVerified;

		// length of values for each uniform that has been set
		int * uniformsSetValues;

		// map a shader variable ID/location to its index in uniformsSetValues
		std::map<int, int> uniformLocationsToVerificationIndex;

		// current highest used sampler unit index
		unsigned int currentSampletUnityIndex;
		// map a texture uniform to its sampler unit
		std::map<std::string, int> textureUniformSamplerUnitIndex;

		// does this material require a light to be rendered?
		bool selfLit;

		unsigned int GetRequiredUniformSize(UniformType uniformType);
		bool allSetUniformsandAttributesVerified;

		void BindStandardVars();
		void ClearStandardBindings();
		bool SetupSetVerifiers();

		bool SetupSetUniforms();
		void DestroySetUniforms();

		void SetStandardAttributeBinding(int varID, StandardAttribute attr);
		int GetStandardAttributeBinding(StandardAttribute attr) const;
		int TestForStandardAttribute(StandardAttribute attr) const;

		int GetUniformIndex(const std::string& uniformName);
		unsigned int GetSamplerUnitForName(const std::string& name);
		void SetStandardUniformBinding(int varID, StandardUniform uniform);
		int GetStandardUniformBinding(StandardUniform uniform) const;
		int TestForStandardUniform(StandardUniform uniform) const;
		bool ValidateUniformName(const std::string& name, int& loc, int& index);

		void SetAttributeSetValue(int varID, int size);
		void SetUniformSetValue(int varID, int size);


	protected:

		Material(const std::string& materialName);
		virtual ~Material();
		bool Init(ShaderRef shader);

	public:

		void ResetVerificationState();

		ShaderRef GetShader() const;

		StandardAttributeSet GetStandardAttributes() const;
		void SendStandardAttributeBufferToShader(StandardAttribute attr, VertexAttrBuffer *buffer);
		void SendAttributeBufferToShader(int varID, VertexAttrBuffer *buffer);

		StandardUniformSet GetStandardUniforms() const;

		void SendSetUniformToShader(unsigned int index);
		void SendAllSetUniformsToShader();
		void SetTexture(TextureRef texture, const std::string& varName);
		void SetMatrix4x4(const Matrix4x4& mat, const std::string& varName);
		void SetUniform1f(float val, const std::string& varName);
		void SetUniform2f(float v1, float v2, const std::string& varName);
		void SetUniform4f(float v1, float v2, float v3, float v4, const std::string& varName);
		void SetColor(Color4 val, const std::string& varName);
		unsigned int GetSetUniformCount() const;

		void SendClipPlaneCountToShader(unsigned int count);
		void SendClipPlaneToShader(unsigned int index, float eq1, float eq2, float eq3, float eq4);
		void SendModelMatrixToShader(const Matrix4x4 * mat);
		void SendModelViewMatrixToShader(const Matrix4x4 * mat);
		void SendProjectionMatrixToShader(const Matrix4x4 * mat);
		void SendMVPMatrixToShader(const Matrix4x4 * mat);
		void SendLightToShader(const Light * light, const Point3 * position, const Vector3 * altDirection);
		void SendEyePositionToShader(const Point3 * position);

		bool VerifySetVars(int vertexCount);

		void SetSelfLit(bool selfLit);
		bool IsSelfLit();
	};
}

#endif
