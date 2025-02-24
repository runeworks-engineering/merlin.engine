#pragma once
#include "merlin/Physics/particleSystem.h"

namespace Merlin {
	class IsoSurfaceGenerator {
	public:

		IsoSurfaceGenerator();

		void init();
		void sort();
		void clean();

		void setCellSize(float size);
		void setDomain(BoundingBox bb);
		void setWkgSize(GLuint size);

	private :
		void setConstants();

	private:

		static ComputeShader_Ptr m_isosurfaceGenerator;
		static ComputeShader_Ptr m_textureGenerator;


	};

}