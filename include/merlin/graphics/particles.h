#pragma once
#include "merlin/graphics/renderableObject.h"

#include "merlin/core/core.h"

#include "merlin/memory/vao.h"
#include "merlin/memory/ssbo.h"
#include "merlin/shaders/shader.h"
#include "merlin/textures/texture.h"
#include "merlin/shaders/computeShader.h"
#include "merlin/core/timestep.h"
#include "merlin/graphics/mesh.h"


namespace Merlin {

	enum class ParticlesDisplayMode {
		MESH,
		//SPRITE,
		POINT_SPRITE,
		POINT_SPRITE_SHADED
	};



	class Particles : public RenderableObject {

	public:
		Particles(const std::string& name, size_t count = 1);
		void draw() const; //draw the mesh

		void setInstancesCount(size_t count);
		void setActiveInstancesCount(size_t count);

		void setShader(Shader_Ptr shader) {	m_shader = shader;}
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }
		inline void setMaterial(shared<MaterialBase> material) { m_material = material; }
		inline void setMaterial(std::string materialName) { m_materialName = materialName; }
		inline void setMesh(shared<Mesh> geometry) { m_geometry = geometry; }
		inline void setDisplayMode(ParticlesDisplayMode mode) { m_displayMode = mode; }

		Shader_Ptr getShader() const {return m_shader;}
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline bool hasShader() const { return m_shader != nullptr; }
		inline const shared<MaterialBase> getMaterial() const { return m_material; }
		inline const std::string& getMaterialName() const { return m_materialName; }
		inline bool hasMaterial() const { return m_material != nullptr; }

		inline shared<Mesh> getMesh() const { return m_geometry; }
		inline ParticlesDisplayMode getDisplayMode() const { return m_displayMode; }
		
		

		static shared<Particles> create(const std::string&, size_t count = 1);



	protected:
		//Rendering
		std::string m_materialName = "default";
		shared<MaterialBase> m_material = nullptr;

		Shader_Ptr m_shader = nullptr;
		std::string m_shaderName = "default";

		Mesh_Ptr m_geometry = nullptr;

		size_t m_instancesCount = 1;
		size_t m_active_instancesCount = 1; //for rendering
		ParticlesDisplayMode m_displayMode = ParticlesDisplayMode::POINT_SPRITE;

		AbstractBufferObject_Ptr m_position; //Buffer to store particles fields (vec4, position + scalar)
	};

}