#pragma once
#include "merlin/core/core.h"
#include "merlin/shaders/shader.h"
#include "merlin/graphics/material.h"
#include "merlin/memory/vao.h"
#include "merlin/graphics/renderableObject.h"

namespace Merlin {

	struct BoundingBox {
		glm::vec3 min, max, centroid;
	};

	class Mesh : public RenderableObject {
	public:
		Mesh(std::string name);
		Mesh(std::string name, GLsizei count, Shared<VBO<>>, GLuint mode = GL_TRIANGLES);
		Mesh(std::string name, GLsizei count, Shared<VBO<>>, Shared<IBO> = nullptr, GLuint mode = GL_TRIANGLES);
		Mesh(std::string name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		Mesh(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);
		Mesh(std::string name, VAO_Ptr, GLuint count, GLuint mode = GL_TRIANGLES);

		void draw() const;
		void drawInstanced(GLsizeiptr instanced) const;

		void voxelize(float size);
		void voxelizeSurface(float size, float thickness);

		void computeBoundingBox();
		void computeNormals();
		void smoothNormals();
		void calculateIndices();
		void removeUnusedVertices();
		void applyMeshTransform();
		void centerMeshOrigin();
		void updateVAO();

		inline void setDrawMode(GLuint mode) { m_drawMode = mode; }
		inline void setShader(Shared<Shader> shader) { m_shader = shader; }
		inline void setMaterial(Shared<MaterialBase> material) { m_material = material; }
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }
		inline void setMaterial(std::string materialName) { m_materialName = materialName; m_material = nullptr; }

		inline bool hasIndices() const { return m_indices.size() > 0; }
		inline bool hasShader() const { return m_shader != nullptr; }
		inline bool hasMaterial() const { return m_material != nullptr; }

		inline GLuint getDrawMode() const { return m_drawMode; }
		inline const std::vector<int>& getVoxels() const { return m_voxels;  }
		inline const std::vector<Vertex>& getVertices() const { return m_vertices;  }
		inline const std::vector<GLuint>& getIndices() const{ return m_indices; }
		inline const glm::mat4& getTransform() const { return m_transform; }
		inline const Shared<Shader> getShader() const { return m_shader; }
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline const Shared<MaterialBase> getMaterial() const { return m_material; }
		inline const std::string& getMaterialName() const { return m_materialName; }

		inline BoundingBox getBoundingBox() const { return m_bbox; }

		static Shared<Mesh> create(std::string name);
		static Shared<Mesh> create(std::string name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		static Shared<Mesh> create(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

	private:
		VAO_Ptr m_vao;
		GLuint m_drawMode;

		GLuint m_elementCount = 0;
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		std::vector<int> m_voxels;

		BoundingBox m_bbox = { glm::vec3(), glm::vec3() };

		std::string m_materialName = "default";
		std::string m_shaderName = "default";

		Shared<MaterialBase> m_material = nullptr;
		Shared<Shader> m_shader = nullptr;

		

		bool debugnormal = true;
	};

	typedef Shared<Mesh> Mesh_Ptr;
}