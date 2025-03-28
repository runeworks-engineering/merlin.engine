#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/bufferObject.h"

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace Merlin {
	class Merlin::AbstractBufferObject;

	enum class ShaderType {
		ABSTRACT,
		GRAPHICS, // Vertex, fragment or geometry
		COMPUTE_SHADER,
		STAGED_COMPUTE_SHADER
	};

	class ShaderBase {
	public:
		ShaderBase(ShaderType);
		ShaderBase(std::string name, ShaderType);
		virtual ~ShaderBase();

		virtual void use() const;
		virtual void destroy();

		GLuint getUniformLocation(const char*) const;

		void setInt(const std::string name, GLint value) const;
		void setUInt(const std::string name, GLuint value) const;
		void setFloat(const std::string name, GLfloat value) const;
		void setDouble(const std::string name, GLdouble value) const;
		void setVec2(const std::string name, glm::vec2 value) const;
		void setVec3(const std::string name, glm::vec3 value) const;
		void setVec4(const std::string name, glm::vec4 value) const;
		void setUVec2(const std::string name, glm::uvec2 value) const;
		void setUVec3(const std::string name, glm::uvec3 value) const;
		void setUVec4(const std::string name, glm::uvec4 value) const;
		void setIVec2(const std::string name, glm::ivec2 value) const;
		void setIVec3(const std::string name, glm::ivec3 value) const;
		void setIVec4(const std::string name, glm::ivec4 value) const;
		void setDVec2(const std::string name, glm::dvec2 value) const;
		void setDVec3(const std::string name, glm::dvec3 value) const;
		void setDVec4(const std::string name, glm::dvec4 value) const;
		void setMat3(const std::string name, glm::mat3 mat) const;
		void setMat4(const std::string name, glm::mat4 mat) const;

		void setIntArray(const std::string name, GLint* values, uint32_t count) const;


		void setConstInt(const std::string name, GLint value);
		void setConstUInt(const std::string name, GLuint value);
		void setConstFloat(const std::string name, GLfloat value);
		void setConstDouble(const std::string name, GLdouble value);
		void setConstVec2(const std::string name, glm::vec2 value);
		void setConstVec3(const std::string name, glm::vec3 value);
		void setConstVec4(const std::string name, glm::vec4 value);
		void setConstUVec2(const std::string name, glm::uvec2 value);
		void setConstUVec3(const std::string name, glm::uvec3 value);
		void setConstUVec4(const std::string name, glm::uvec4 value);
		void setConstIVec2(const std::string name, glm::ivec2 value);
		void setConstIVec3(const std::string name, glm::ivec3 value);
		void setConstIVec4(const std::string name, glm::ivec4 value);
		void setConstDVec2(const std::string name, glm::dvec2 value);
		void setConstDVec3(const std::string name, glm::dvec3 value);
		void setConstDVec4(const std::string name, glm::dvec4 value);
		void setConstMat3(const std::string name, glm::mat3 mat);
		void setConstMat4(const std::string name, glm::mat4 mat);
		void setConstDMat3(const std::string name, glm::dmat3 mat);
		void setConstDMat4(const std::string name, glm::dmat4 mat);

		void define(const std::string& name, const std::string& value);

		bool hasConstant(const std::string&) const;

		void attach(AbstractBufferObject& buf);
		void detach(AbstractBufferObject& buf);

		inline const GLuint id() const { return m_programID; }
		inline void setID(GLuint _id_) { m_programID = _id_; };

		inline const std::string name() const { return m_name; }
		inline const bool isCompiled() const { return m_compiled; }

		//static std::shared_ptr<ShaderBase> create(const std::string& name);
		static std::string readSrc(const std::string& filename);
	
	protected:
		void precompileSrc(std::string& src);
		static bool compileShader(const std::string& name, const std::string& src, GLuint id);
		std::string updateConstants(const std::string& originalSrc);
		std::string updateDefines(const std::string& originalSrc);


		std::string m_name;
		bool m_compiled = false;
		std::unordered_map<std::string, std::string> m_constants;
		std::unordered_map<std::string, std::string> m_defines;

		static int shader_instances;

	private:
		GLuint m_programID = 0;
		ShaderType m_type = ShaderType::ABSTRACT;
	};

	typedef shared<ShaderBase> GenericShader_Ptr;	

}