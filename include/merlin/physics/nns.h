#pragma once
#include "merlin/core/core.h"
#include "merlin/physics/particleSystem.h"
#include "merlin/physics/physicsEnum.h"

namespace Merlin {

	struct Bin {
		GLuint count; //particle count in the bin
		GLuint sum;   //global sum
		GLuint csum;  //local sum
		GLuint index; //bin index
	};

	class NNS {
	public:
		NNS();

		void init(BoundingBox domain, float cellsize);
		void clean();
		void sort(StagedComputeShader_Ptr solver);
		
		void solveLink(shared<ShaderBase> shader);
		void detach(shared<ShaderBase> shader);

		void setShader(Shader_Ptr);

		void setWkgSize(GLuint size);

		float getCellSize();
		glm::uvec3 getBound();
		GLuint getBinCount();

		ParticleSystem_Ptr getParticleSystem();
		SSBO_Ptr<Bin> getBuffer();

	private:
		void computeThreadLayout();
		void warmUnstagedChanges();
		void errorNotReady();
		void setConstants();

	private:
		bool m_active = true;
		bool m_ready = false;

		GLuint bWkgSize = 512; //Number of thread per workgroup
		GLuint m_thread = 0; //Total number of element (thread)
		GLuint m_blockSize = 0; //PrefixSum block size
		GLuint m_blocks = 0;
		GLuint m_bWkgCount = 0;
		glm::uvec3 m_maxBound;

		float m_cell_size = 10;
		BoundingBox m_domain;
		ParticleSystem_Ptr m_bins = nullptr;
		SSBO_Ptr<Bin> m_bin_buffer;

		StagedComputeShader_Ptr m_prefixSum;
	};

	typedef shared<NNS> NNS_Ptr;
}