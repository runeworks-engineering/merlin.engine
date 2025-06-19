#include "pch.h"
#include "merlin/physics/analysis/isoSurfaceGenerator.h"

namespace Merlin
{
	void IsoSurfaceGenerator::init() {
		m_isosurfaceGenerator;
		setConstants();
		m_isosurfaceGenerator->compile();
	}

	void IsoSurfaceGenerator::setConstants() {

	}
}