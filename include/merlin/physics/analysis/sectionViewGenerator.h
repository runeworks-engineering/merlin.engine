#pragma once
#include "merlin/Physics/particleSystem.h"

namespace Merlin {
	class SectionViewGenerator {
	public:

		SectionViewGenerator();

		void init();
		void sort();
		void clean();

		void setCellSize(float size);
		void setDomain(BoundingBox bb);
		void setWkgSize(GLuint size);

	private:
		void setConstant();

	private:
		static ComputeShader_Ptr m_textureGenerator;
		Texture2D_Ptr texture_output;
		glm::vec3 plane_normal = glm::vec3(0,1,0);
		float plane_offset = 0;

	};

}