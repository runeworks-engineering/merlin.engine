#include "pch.h"
#include "merlin/physics/analysis/isoSurfaceGenerator.h"

void IsoSurfaceGenerator::init() {
	m_isosurfaceGenerator;
	setConstants();
	m_isosurfaceGenerator->compile();
}

void IsoSurfaceGenerator::setConstants() {

}
