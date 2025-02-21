#include "pch.h"
#include "merlin/graphics/particles.h"
#include "merlin/utils/primitives.h"
#include "merlin/graphics/ressourceManager.h"

namespace Merlin {

	shared<Particles> Particles::create(const std::string& name, size_t count) {
		return std::make_shared<Particles>(name, count);
	}

	Particles::Particles(const std::string& name, size_t count) : RenderableObject(name), m_instancesCount(count), m_active_instancesCount(count) {
		m_geometry = Merlin::Primitives::createPoint();
	}

	void Particles::draw() const {

		switch (m_displayMode) {
		case ParticlesDisplayMode::MESH:
			if (m_geometry) m_geometry->drawInstanced(m_active_instancesCount);
			break;
		case ParticlesDisplayMode::POINT_SPRITE:
			glEnable(GL_PROGRAM_POINT_SIZE);
			if (m_geometry) m_geometry->drawInstanced(m_active_instancesCount);
			glDisable(GL_PROGRAM_POINT_SIZE);
			break;
		case ParticlesDisplayMode::POINT_SPRITE_SHADED:
			glEnable(GL_PROGRAM_POINT_SIZE);
			glEnable(0x8861);//Point shading
			if (m_geometry) m_geometry->drawInstanced(m_active_instancesCount);
			glDisable(GL_PROGRAM_POINT_SIZE);
			glDisable(0x8861);
			break;
		}
	} //draw the mesh

	void Particles::setInstancesCount(size_t count) {
		if (m_active_instancesCount > count) m_active_instancesCount = count;
		m_instancesCount = count;
	}

	void Particles::setActiveInstancesCount(size_t count) {
		if (count <= m_instancesCount)
			m_active_instancesCount = count;
		else {
			Console::error() << "Active instance count is greater than the total instance count" << Console::endl;
		}

	}



}