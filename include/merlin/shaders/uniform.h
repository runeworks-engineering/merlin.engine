#pragma once
#include "shaderBase.h"

namespace Merlin{

	class ShaderBase;
	template <class T>
	class Uniform {
	public:
		Uniform(const std::string& name, T initValue = T()) : m_name(name), m_data(initValue) {}

		inline T& value() {
			return m_data;
		}

		void sync(ShaderBase& shader);

		inline const std::string name() { return m_name; }

		void operator=(T data);
		T& operator()();

	private:
		T m_data;
		std::string m_name;
	};



	template<typename T>
	inline void Uniform<T>::operator=(T data) {
		m_data = data;
	}

	template<typename T>
	inline T& Uniform<T>::operator()() {
		return m_data;
	}

	template<>
	inline void Uniform<int>::sync(ShaderBase& shader) {
		shader.setInt(m_name, m_data);
	}

	template<>
	inline void Uniform<float>::sync(ShaderBase& shader) {
		shader.setFloat(m_name, m_data);
	}

	template<>
	inline void Uniform<double>::sync(ShaderBase& shader) {
		shader.setDouble(m_name, m_data);
	}

	template<>
	inline void Uniform<GLuint>::sync(ShaderBase& shader) {
		shader.setUInt(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::vec2>::sync(ShaderBase& shader) {
		shader.setVec2(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::ivec2>::sync(ShaderBase& shader) {
		shader.setIVec2(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::uvec2>::sync(ShaderBase& shader) {
		shader.setUVec2(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::dvec2>::sync(ShaderBase& shader) {
		shader.setDVec2(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::vec3>::sync(ShaderBase& shader) {
		shader.setVec3(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::ivec3>::sync(ShaderBase& shader) {
		shader.setIVec3(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::uvec3>::sync(ShaderBase& shader) {
		shader.setUVec3(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::dvec3>::sync(ShaderBase& shader) {
		shader.setDVec3(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::vec4>::sync(ShaderBase& shader) {
		shader.setVec4(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::ivec4>::sync(ShaderBase& shader) {
		shader.setIVec4(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::uvec4>::sync(ShaderBase& shader) {
		shader.setUVec4(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::dvec4>::sync(ShaderBase& shader) {
		shader.setDVec4(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::mat4>::sync(ShaderBase& shader) {
		shader.setMat4(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::mat3>::sync(ShaderBase& shader) {
		shader.setMat3(m_name, m_data);
	}

	template<class T>
	using UniformObject = Uniform<T>;




















	template <class T>
	class ConstantUniform {
	public:
		ConstantUniform(const std::string& name, T initValue = T()) : m_name(name), m_data(initValue) {}

		inline T& value() {
			return m_data;
		}

		void set(ShaderBase& shader);

		inline const std::string name() { return m_name; }

		void operator=(T data);
		T& operator()();

	private:
		T m_data;
		std::string m_name;
	};



	template<typename T>
	inline void ConstantUniform<T>::operator=(T data) {
		m_data = data;
	}

	template<typename T>
	inline T& ConstantUniform<T>::operator()() {
		return m_data;
	}

	template<>
	inline void ConstantUniform<int>::set(ShaderBase& shader) {
		shader.setConstInt(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<float>::set(ShaderBase& shader) {
		shader.setConstFloat(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<double>::set(ShaderBase& shader) {
		shader.setConstDouble(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<GLuint>::set(ShaderBase& shader) {
		shader.setConstUInt(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::vec2>::set(ShaderBase& shader) {
		shader.setConstVec2(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::ivec2>::set(ShaderBase& shader) {
		shader.setConstIVec2(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::uvec2>::set(ShaderBase& shader) {
		shader.setConstUVec2(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::dvec2>::set(ShaderBase& shader) {
		shader.setConstDVec2(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::vec3>::set(ShaderBase& shader) {
		shader.setConstVec3(m_name, m_data);
	}
	template<>
	inline void ConstantUniform<glm::ivec3>::set(ShaderBase& shader) {
		shader.setConstIVec3(m_name, m_data);
	}
	template<>
	inline void ConstantUniform<glm::uvec3>::set(ShaderBase& shader) {
		shader.setConstUVec3(m_name, m_data);
	}
	template<>
	inline void ConstantUniform<glm::dvec3>::set(ShaderBase& shader) {
		shader.setConstDVec3(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::vec4>::set(ShaderBase& shader) {
		shader.setConstVec4(m_name, m_data);
	}
	template<>
	inline void ConstantUniform<glm::ivec4>::set(ShaderBase& shader) {
		shader.setConstIVec4(m_name, m_data);
	}
	template<>
	inline void ConstantUniform<glm::uvec4>::set(ShaderBase& shader) {
		shader.setConstUVec4(m_name, m_data);
	}
	template<>
	inline void ConstantUniform<glm::dvec4>::set(ShaderBase& shader) {
		shader.setConstDVec4(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::mat4>::set(ShaderBase& shader) {
		shader.setConstMat4(m_name, m_data);
	}

	template<>
	inline void ConstantUniform<glm::mat3>::set(ShaderBase& shader) {
		shader.setConstMat3(m_name, m_data);
	}

	template<class T>
	using ConstantUniformObject = ConstantUniform<T>;
}