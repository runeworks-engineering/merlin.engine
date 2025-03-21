#pragma once
#include "merlin/core/core.h"
#include "merlin/shaders/shaderBase.h"
#include "uniform.h"

namespace Merlin{

	class ComputeShader : public ShaderBase {
	public:
		ComputeShader(const std::string& n, const std::string& file_path = "", bool compile = true, ShaderType type = ShaderType::COMPUTE_SHADER);
		~ComputeShader();

		void destroy() override;
		void compile();

		void readFile(const std::string& file_path);
		void compileFromFile(const std::string& file_path);
		void compileFromSrc(const std::string& src);
		
		void dispatch(); //execute using the default WorkgroupLayout
		void dispatch(GLuint width, GLuint height = 1, GLuint layers = 1); //execute using the given WorkgroupLayout
		void SetWorkgroupLayout(GLuint width, GLuint height = 1, GLuint layers = 1); // Set current workgroup Layout
		void SetWorkgroupLayout(glm::uvec3); // Set current workgroup Layout

		void wait();
		void barrier(GLbitfield barrier = GL_ALL_BARRIER_BITS);

		void printLimits();

		static shared<ComputeShader> create(const std::string& n, const std::string& file_path, bool compile = true, ShaderType type = ShaderType::COMPUTE_SHADER) {
			return createShared<ComputeShader>(n, file_path, compile, type); 
		}


	protected:
		glm::uvec3 m_wkgrpLayout;
		GLuint m_shaderID = 0;
		std::string m_shaderSrc; 
	};

	class StagedComputeShader : public ComputeShader {
	public:
		StagedComputeShader(const std::string& n, const std::string& file_path, GLuint numberOfStage, bool compile = true);

		void executeAll();
		void execute(GLuint stage);
		void execute(GLuint start, GLuint end);
		void step();

		inline void setStageCount(GLuint n) { m_stageCount = n; }
		inline void setStage(GLuint n) { m_stage = n; }
		inline void reset() { m_stage = 0; }
		inline GLuint getStage() const { return m_stage; }
		inline GLuint getStageCount() const { return m_stageCount; }

		static shared<StagedComputeShader> create(const std::string& n, const std::string& file_path, GLuint numberOfStage, bool compile = true) {
			return createShared<StagedComputeShader>(n, file_path, numberOfStage, compile);
		};

	protected:
		GLuint m_stage = 0;
		GLuint m_stageCount = 0;

	};

	typedef shared<ComputeShader> ComputeShader_Ptr;
	typedef shared<StagedComputeShader> StagedComputeShader_Ptr;
}


