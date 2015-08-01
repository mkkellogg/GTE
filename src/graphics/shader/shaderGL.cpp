
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "graphics/gl_include.h"
#include "object/enginetypes.h"
#include "shader.h"
#include "shaderGL.h"
#include "shadersource.h"
#include "graphics/render/vertexattrbuffer.h"
#include "graphics/render/vertexattrbufferGL.h"
#include "geometry/matrix4x4.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"
#include "graphics/texture/texture.h"
#include "graphics/texture/textureGL.h"
#include "graphics/color/color4.h"
#include "debug/gtedebug.h"
#include "global/global.h"
#include "uniformdesc.h"
#include "attributedesc.h"
#include <string>

namespace GTE
{
	/*
	* Only constructor.
	*
	* [shaderSource] - Container for the shader's source code
	*/
	ShaderGL::ShaderGL(const ShaderSource& shaderSource) : Shader(shaderSource)
	{
		ready = false;

		programID = 0;
		vertexShaderID = 0;
		fragmentShaderID = 0;

		attributeCount = 0;
		uniformCount = 0;

		attributes = nullptr;
		uniforms = nullptr;
	}

	/*
	 * Clean up
	 */
	ShaderGL::~ShaderGL()
	{
		DestroyComponents();
	}

	/*
	 * Use OpenGL functions to delete the compiled vertex and fragment shaders.
	 */
	void ShaderGL::DestroyShaders()
	{
		if (vertexShaderID > 0)
		{
			glDeleteShader(vertexShaderID);
			vertexShaderID = 0;
		}

		if (fragmentShaderID > 0)
		{
			glDeleteShader(fragmentShaderID);
			fragmentShaderID = 0;
		}
	}

	/*
	 * Use OpenGL to delete the linked program.
	 */
	void ShaderGL::DestroyProgram()
	{
		if (programID > 0)
		{
			glDeleteProgram(programID);
			programID = 0;
		}
	}

	/*
	 * Destroy everything associated with this shader: vertex and fragment shaders, the program object,
	 * and all uniform and attribute descriptors
	 */
	void ShaderGL::DestroyComponents()
	{
		DestroyShaders();
		DestroyProgram();
		DestroyUniformAndAttributeInfo();
	}

	/*
	 * Destroy all uniform and attribute descriptors
	 */
	void ShaderGL::DestroyUniformAndAttributeInfo()
	{
		if (attributes != nullptr)
		{
			for (UInt32 i = 0; i < attributeCount; i++)
			{
				if (attributes[i] != nullptr)
				{
					delete attributes[i];
					attributes[i] = nullptr;
				}
			}
			delete attributes;
		}
		attributes = nullptr;

		if (uniforms != nullptr)
		{
			for (UInt32 i = 0; i < uniformCount; i++)
			{
				if (uniforms[i] != nullptr)
				{
					delete uniforms[i];
					uniforms[i] = nullptr;
				}
			}
			delete uniforms;
		}
		uniforms = nullptr;
	}

	/*
	 * This method performs all the work of loading shader source, compiling
	 * the shaders, and linking them to form the final program. It also calls
	 * functions to analyze the compiled shader to get information about all
	 * attributes and uniforms.
	 *
	 * The function will return true if the loading, compilation, and linking
	 * are successful. Otherwise it will return false.
	 */
	Bool ShaderGL::Load()
	{
		// attempt to load the shaders' source code
		Bool shaderSourceLoaded = shaderSource.IsLoaded();

		if (!shaderSourceLoaded)
		{
			shaderSourceLoaded = shaderSource.Load();
		}
		ASSERT(shaderSourceLoaded == true, "ShaderGL::Load -> Unable to load shader source.");

		// Create the OpenGL objects that will hold each shader
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		ASSERT(vertexShaderID != 0, "Unable to create GL vertex shader.");

		if (fragmentShaderID == 0)
		{
			DestroyShaders();
			Debug::PrintError("Unable to create GL fragment shader.");
			return false;
		}

		const Char * vertexSourceString = shaderSource.GetVertexSourceString().c_str();
		const Char * fragmentSourceString = shaderSource.GetFragmentSourceString().c_str();

		// point OpenGL to the source for each shader
		glShaderSource(vertexShaderID, 1, &vertexSourceString, nullptr);
		glShaderSource(fragmentShaderID, 1, &fragmentSourceString, nullptr);

		// compile vertex shader
		glCompileShader(vertexShaderID);
		if (!CheckCompilation(vertexShaderID, ShaderType::Vertex))
		{
			DestroyShaders();
			return false;
		}

		// compile fragment shader
		glCompileShader(fragmentShaderID);
		if (!CheckCompilation(fragmentShaderID, ShaderType::Fragment))
		{
			DestroyShaders();
			return false;
		}

		// create the OpenGL program object
		programID = glCreateProgram();
		if (programID == 0)
		{
			DestroyShaders();
			Debug::PrintError("Unable to create GL program.");
			return false;
		}

		// attach shaders to the OpenGL program and link them together
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);
		glLinkProgram(programID);

		GLint programLinked;
		glGetProgramiv(programID, GL_LINK_STATUS, &programLinked);

		if (programLinked == GL_FALSE)
		{
			Debug::PrintError("Error linking program: ");
			Char * info = GetProgramLog(programID);
			Debug::PrintError(info != nullptr ? info : "(no error info provided).");
			return false;
		}

		// get information about all uniforms and attributes in the shaders
		if (!StoreUniformAndAttributeInfo())
		{
			return false;
		}

		ready = true;
		return true;
	}

	/*
	 * Are the vertex and fragment shaders successfully loaded, compiled, and linked?
	 */
	Bool ShaderGL::IsLoaded() const
	{
		return ready;
	}

	/*
	 * Check the compilation status of a vertex or fragment shader.
	 *
	 * [shaderID] - The OpenGL shader object id.
	 * [shaderType] - Type of shader (either vertex or fragment)
	 */
	Bool ShaderGL::CheckCompilation(Int32 shaderID, ShaderType shaderType)
	{
		GLint shaderCompiled;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);

		if (shaderCompiled == GL_FALSE)
		{
			if (shaderType == ShaderType::Vertex)
				Debug::PrintError("Error compiling vertex shader: ");
			else if (shaderType == ShaderType::Fragment)
				Debug::PrintError("Error compiling fragment shader: ");
			Char * info = GetShaderLog(shaderID);
			Debug::PrintError(info != nullptr ? info : "(no error info provided).");
			return false;
		}

		return true;
	}

	/*
	 * Get a detailed description about the status of a program.
	 *
	 * [program] - The OpenGL program to retrieve status for.
	 */
	Char * ShaderGL::GetProgramLog(GLuint program)
	{
		Int32 infologLength = 0;

		Int32 CharsWritten = 0;
		Char *infoLog;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 0)
		{
			infoLog = (Char *)malloc(infologLength);
			glGetProgramInfoLog(program, infologLength, &CharsWritten, infoLog);

			return infoLog;
		}

		return nullptr;
	}

	/*
	 * Get a detailed description about the status of a shader.
	 *
	 * [shader] - The OpenGL shader to retrieve status for.
	 */
	Char * ShaderGL::GetShaderLog(GLuint shader)
	{
		Int32 infologLength = 0;

		Int32 CharsWritten = 0;
		Char *infoLog;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 0)
		{
			infoLog = (Char *)malloc(infologLength);
			glGetShaderInfoLog(shader, infologLength, &CharsWritten, infoLog);

			return infoLog;
		}

		return nullptr;
	}

	/*
	 * Create UniformDescriptor objects for each uniform exposed by
	 * either shader, and AttributeDescriptor objects for each attribute
	 * exposed by the vertex shader.
	 */
	Bool ShaderGL::StoreUniformAndAttributeInfo()
	{
		Int32 totalUniforms = -1;
		glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &totalUniforms);
		if (totalUniforms > 0)
		{
			// allocate array for UniformDescriptor objects
			uniforms = new(std::nothrow)  UniformDescriptor*[totalUniforms];
			ASSERT(uniforms != nullptr, "Unable to allocate shader uniform descriptors.");

			// initialize UniformDescriptor array
			memset(uniforms, 0, sizeof(UniformDescriptor*)*totalUniforms);

			UInt32 samplerUnitIndex = 0;
			// loop through each uniform and query OpenGL for information
			// about that uniform
			for (Int32 i = 0; i < totalUniforms; i++)
			{
				Int32 nameLen = -1;
				Int32 size = -1;
				GLenum type = GL_ZERO;
				Char name[126];

				glGetActiveUniform(programID, GLuint(i), 125, &nameLen, &size, &type, name);
				GLuint loc = glGetUniformLocation(programID, name);

				UniformDescriptor * desc = new(std::nothrow)  UniformDescriptor();
				uniforms[i] = desc;

				desc->ShaderVarID = loc;
				desc->Size = size;
				desc->Type = UniformType::Unknown;

				// convert OpenGL uniform type to our own definition (UniformType)
				switch (type)
				{
				case GL_SAMPLER_CUBE:
					desc->Type = UniformType::SamplerCube;
					break;
				case GL_SAMPLER_2D:
					desc->Type = UniformType::Sampler2D;
					break;
				case GL_FLOAT_MAT4:
					desc->Type = UniformType::Matrix4x4;
					break;
				case GL_FLOAT_VEC4:
					desc->Type = UniformType::Float4;
					break;
				case GL_FLOAT_VEC3:
					desc->Type = UniformType::Float3;
					break;
				case GL_FLOAT_VEC2:
					desc->Type = UniformType::Float2;
					break;
				case GL_FLOAT:
					desc->Type = UniformType::Float;
					break;
				case GL_INT:
					desc->Type = UniformType::Int;
					break;
				}

				if (desc->Type == UniformType::SamplerCube || desc->Type == UniformType::Sampler2D)
				{
					desc->SamplerUnitIndex = samplerUnitIndex;
					samplerUnitIndex++;
				}

				strcpy(desc->Name, name);
			}

			uniformCount = totalUniforms;
		}

		Int32 totalAttributes = -1;
		glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &totalAttributes);
		if (totalAttributes > 0)
		{
			// allocate array for AttributeDescriptor objects
			attributes = new(std::nothrow)  AttributeDescriptor*[totalAttributes];
			ASSERT(attributes != nullptr, "Unable to allocate shader attribute descriptors.");

			// initialize AttributeDescriptor array
			memset(attributes, 0, sizeof(AttributeDescriptor*)*totalAttributes);

			// loop through each attribute and query OpenGL for information
			// about that attribute
			for (Int32 i = 0; i < totalAttributes; i++)
			{
				Int32 nameLen = -1;
				Int32 size = -1;
				GLenum type = GL_ZERO;
				Char name[126];

				glGetActiveAttrib(programID, GLuint(i), 125, &nameLen, &size, &type, name);
				GLuint loc = glGetAttribLocation(programID, name);

				AttributeDescriptor * desc = new(std::nothrow)  AttributeDescriptor();
				attributes[i] = desc;

				desc->ShaderVarID = loc;
				desc->Size = size;
				desc->Type = AttributeType::Unknown;

				// convert OpenGL attribute type to our own definition (AttributeType)
				switch (type)
				{
				case GL_FLOAT_MAT4:
					desc->Type = AttributeType::Matrix4x4;
					break;
				case GL_FLOAT_VEC4:
					desc->Type = AttributeType::Float4;
					break;
				case GL_FLOAT_VEC3:
					desc->Type = AttributeType::Float3;
					break;
				case GL_FLOAT_VEC2:
					desc->Type = AttributeType::Float2;
					break;
				case GL_FLOAT:
					desc->Type = AttributeType::Float;
					break;
				}

				strcpy(desc->Name, name);
			}

			attributeCount = totalAttributes;
		}

		return true;
	}

	/*
	 * Get the shader var ID/location of attribute corresponding to [varName]
	 */
	Int32 ShaderGL::GetAttributeVarID(const std::string& varName) const
	{
		GLint varID = glGetAttribLocation(programID, varName.c_str());
		return (Int32)varID;
	}

	/*
	 * Get the shader var ID/location of uniform corresponding to [varName]
	 */
	Int32 ShaderGL::GetUniformVarID(const std::string& varName) const
	{
		GLint varID = glGetUniformLocation(programID, varName.c_str());
		return (Int32)varID;
	}

	/*
	 * Get the OpengGL program ID for this shader
	 */
	GLuint ShaderGL::GetProgramID() const
	{
		return programID;
	}

	/*
	 * Set the value for a shader attribute.
	 *
	 * [varID] - shader var ID/location of the attribute for which the value is to be set.
	 * [buffer] - attribute data to be sent
	 */
	void ShaderGL::SendBufferToShader(Int32 varID, const VertexAttrBuffer * buffer)
	{
		if (varID < 0)return;

		const VertexAttrBufferGL * bufferGL = dynamic_cast<const VertexAttrBufferGL *>(buffer);
		ASSERT(bufferGL != nullptr, "ShaderGL::SendBufferToShader -> buffer is not VertexAttrBufferGL !!");

		const Real * data = bufferGL->GetDataPtr();
		Int32 componentCount = bufferGL->GetComponentCount();
		Int32 stride = bufferGL->GetStride();

		//TODO: move this so it is not called every single draw call
		glEnableVertexAttribArray((GLuint)varID);

		if (bufferGL->IsGPUBuffer())
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferGL->GetGPUBufferID());
			glVertexAttribPointer(varID, componentCount, GL_FLOAT, 0, stride, 0);
		}
		else
		{
			glVertexAttribPointer(varID, componentCount, GL_FLOAT, GL_FALSE, stride, data);
		}
		glEnableVertexAttribArray(0);
	}

	/*
	 * Set the value for a sampler uniform.
	 *
	 * [varID] - shader var ID/location of the uniform for which the value is to be set.
	 * [texture] - Holds sampler data to be sent
	 */
	void ShaderGL::SendUniformToShader(Int32 varID, UInt32 samplerUnitIndex, const TextureRef texture)
	{
		ASSERT(texture.IsValid(), "ShaderGL::SendUniformToShader(UInt32, Texture *) -> 'texture' is null.");

		Texture* texturePtr = ((TextureRef)texture).GetPtr();
		TextureGL * texGL = dynamic_cast<TextureGL *>(texturePtr);

		ASSERT(texGL != nullptr, "ShaderGL::SendUniformToShader(UInt32, Texture *) -> texture is not TextureGL !!");

		GLenum textureUnit;

		if (samplerUnitIndex == 0)textureUnit = GL_TEXTURE0;
		else if (samplerUnitIndex == 1)textureUnit = GL_TEXTURE1;
		else if (samplerUnitIndex == 2)textureUnit = GL_TEXTURE2;
		else if (samplerUnitIndex == 3)textureUnit = GL_TEXTURE3;

		if (samplerUnitIndex <= 3)
		{
			glActiveTexture(textureUnit);
			if (texGL->GetAttributes().IsCube)
				glBindTexture(GL_TEXTURE_CUBE_MAP, texGL->GetTextureID());
			else
				glBindTexture(GL_TEXTURE_2D, texGL->GetTextureID());
			SendUniformToShader(varID, (Int32)samplerUnitIndex);
		}

		glActiveTexture(GL_TEXTURE0);
	}

	/*
	 * Set the value for a uniform 4x4 matrix.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [mat] - Holds 4x4 matrix data to be sent.
	 */
	void ShaderGL::SendUniformToShader(Int32 varID, const Matrix4x4 * mat)
	{
		glUniformMatrix4fv(varID, 1, GL_FALSE, mat->GetDataPtr());
	}

	/*
	 * Set the value for a position uniform (corresponds 3-component vector).
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [point] - Holds position data to be sent.
	 */
	void ShaderGL::SendUniformToShader(Int32 varID, const Point3 * point)
	{
		glUniform4f(varID, point->x, point->y, point->z, 1);
	}

	/*
	 * Set the value for a vector uniform (corresponds 3-component vector).
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [vector] - Holds vector data to be sent.
	 */
	void ShaderGL::SendUniformToShader(Int32 varID, const Vector3 * vector)
	{
		glUniform4f(varID, vector->x, vector->y, vector->z, 0);
	}

	/*
	 * Set the value for a color uniform (corresponds 4-component vector).
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [color] - Holds color data to be sent.
	 */
	void ShaderGL::SendUniformToShader(Int32 varID, const Color4 * color)
	{
		glUniform4f(varID, color->r, color->g, color->b, color->a);
	}

	/*
	 * Set the value for a 4-component vector uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Holds vector data to be sent.
	 */
	void ShaderGL::SendUniformToShader4v(Int32 varID, const Real * data)
	{
		glUniform4fv(varID, 1, data);
	}

	/*
	 * Set the value for a 3-component vector uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Holds vector data to be sent.
	 */
	void ShaderGL::SendUniformToShader3v(Int32 varID, const Real * data)
	{
		glUniform3fv(varID, 1, data);
	}

	/*
	 * Set the value for a 2-component vector uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Holds vector data to be sent.
	 */
	void ShaderGL::SendUniformToShader2v(Int32 varID, const Real * data)
	{
		glUniform2fv(varID, 1, data);
	}

	/*
	 * Set the value for a 4-component vector uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Holds vector data to be sent.
	 */
	void ShaderGL::SendUniformToShader4(Int32 varID, Real x, Real y, Real z, Real w)
	{
		glUniform4f(varID, x, y, z, w);
	}

	/*
	 * Set the value for a 3-component vector uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Holds vector data to be sent.
	 */
	void ShaderGL::SendUniformToShader3(Int32 varID, Real x, Real y, Real z)
	{
		glUniform3f(varID, x, y, z);
	}

	/*
	 * Set the value for a 2-component vector uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Holds vector data to be sent.
	 */
	void ShaderGL::SendUniformToShader2(Int32 varID, Real x, Real y)
	{
		glUniform2f(varID, x, y);
	}

	/*
	 * Set the value for a single Real uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Uniform data to be sent.
	 */
	void ShaderGL::SendUniformToShader(Int32 varID, Real  data)
	{
		glUniform1f(varID, data);
	}

	/*
	 * Set the value for a single integer uniform.
	 *
	 * [varID] - ID/location of the shader uniform for which the value is to be set.
	 * [data] - Uniform data to be sent.
	 */
	void ShaderGL::SendUniformToShader(Int32 varID, Int32  data)
	{
		glUniform1i(varID, data);
	}

	/*
	 * Get number of uniforms exposed by this shader
	 */
	UInt32 ShaderGL::GetUniformCount() const
	{
		return uniformCount;
	}

	/*
	 * Get UniformDescriptor object at a specific index in [uniforms].
	 * [index] is not the same as a shader variable ID/location. It is just
	 * the index in [uniforms], which is an array of UniformDescriptor objects.
	 *
	 * A variable ID/location is the unique identifier assigned to a uniform by OpengL.
	 * The field [ShaderVarID] in UniformDescriptor holds that value.
	 */
	const UniformDescriptor * ShaderGL::GetUniformDescriptor(UInt32 index) const
	{
		if (index < uniformCount)
		{
			return (const UniformDescriptor *)uniforms[index];
		}

		return nullptr;
	}

	/*
	 * Get number of attributes exposed by this shader
	 */
	UInt32 ShaderGL::GetAttributeCount() const
	{
		return attributeCount;
	}

	/*
	 * Get AttributeDescriptor object at a specific index in [attributes].
	 * [index] is not the same as a shader variable ID/location. It is just
	 * the index in [attributes], which is an array of AttributeDescriptor objects.
	 *
	 * A shader variable ID/location is the unique identifier assigned to an attribute by OpengL.
	 * The field [ShaderVarID] in AttributeDescriptor holds that value.
	 */
	const AttributeDescriptor * ShaderGL::GetAttributeDescriptor(UInt32 index) const
	{
		if (index < attributeCount)
		{
			const AttributeDescriptor * desc = (const AttributeDescriptor *)attributes[index];
			return desc;
		}
		return nullptr;
	}
}
