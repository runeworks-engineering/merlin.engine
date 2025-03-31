#pragma once
#include "merlin/core/core.h"

#include "merlin/memory/vao.h"
#include "merlin/memory/ssbo.h"
#include "merlin/shaders/shader.h"
#include "merlin/textures/texture.h"
#include "merlin/shaders/computeShader.h"
#include "merlin/core/timestep.h"
#include "merlin/graphics/mesh.h"

#include "glm/gtc/random.hpp"
#include <set>

namespace Merlin {

	enum class ParticleSystemDisplayMode {
		MESH,
		//SPRITE,
		POINT_SPRITE,
		POINT_SPRITE_SHADED
	};

	class ParticleSystem : public RenderableObject {
	public:
		ParticleSystem(const std::string& name, size_t count = 1);
		void draw() const; //draw the mesh
		void setInstancesCount(size_t count);
		void setActiveInstancesCount(size_t count);

		inline GLuint count() { return m_instancesCount; }
		inline GLuint activeParticles() { return m_active_instancesCount; }

		//void addProgram(ComputeShader_Ptr program);
		//bool hasProgram(const std::string& name) const;

		bool hasPositionBuffer() const;
		AbstractBufferObject_Ptr getPositionBuffer() const;
		void setPositionBuffer(AbstractBufferObject_Ptr buffer);

		void setShader(Shader_Ptr shader);
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }
		Shader_Ptr getShader() const;
		inline const std::string& getShaderName() const { return m_shaderName; }

		bool hasShader() const;

		inline void setMaterial(shared<MaterialBase> material) { m_material = material; }
		inline void setMaterial(std::string materialName) { m_materialName = materialName; }
		inline bool hasMaterial() const { return m_material != nullptr; }
		inline const shared<MaterialBase> getMaterial() const { return m_material; }
		inline const std::string& getMaterialName() const { return m_materialName; }


		inline void setMesh(shared<Mesh> geometry) { m_geometry = geometry; }
		inline shared<Mesh> getMesh() const { return m_geometry; }

		inline void setDisplayMode(ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline ParticleSystemDisplayMode getDisplayMode() const { return m_displayMode; }


		static shared<ParticleSystem> create(const std::string&, size_t count = 1);

		//inline void setPositionBuffer(AbstractBufferObject_Ptr buffer) { if(buffer) m_position_buffer = buffer; }


	protected:
		//Rendering
		std::string m_materialName = "default";
		shared<MaterialBase> m_material = nullptr;

		Shader_Ptr m_shader = nullptr;
		std::string m_shaderName = "default";

		AbstractBufferObject_Ptr m_position_buffer = nullptr;

		Mesh_Ptr m_geometry = nullptr;
		size_t m_instancesCount = 1;
		size_t m_active_instancesCount = 1; //for rendering
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;
	};

	typedef shared<ParticleSystem> ParticleSystem_Ptr;

}
