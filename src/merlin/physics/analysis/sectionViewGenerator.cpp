#include "pch.h"
#include "merlin/physics/analysis/sectionViewGenerator.h"

namespace Merlin {
	void SectionViewGenerator::init() {
		m_textureGenerator = ComputeShader::create("texPlot", "assets/shaders/solver/texturePlot.comp", false);
		//setConstants();
		m_textureGenerator->compile();
	}

	void SectionViewGenerator::setConstant() {

	}
}