#include "pch.h"
#include "merlin/graphics/mesh.h"
#include "merlin/memory/ibo.h"
#include "merlin/utils/voxelizer.h"

#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <future>


// Helper function to compare glm::vec3 for hashing
bool operator==(const glm::vec3& a, const glm::vec3& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

// Hash function for glm::vec3
namespace std {
	template<> struct hash<glm::vec3> {
		size_t operator()(const glm::vec3& v) const {
			return ((hash<float>()(v.x)
				^ (hash<float>()(v.y) << 1)) >> 1)
				^ (hash<float>()(v.z) << 1);
		}
	};
}

namespace Merlin {
	Mesh::Mesh(std::string name) : RenderableObject(name) {
		m_drawMode = GL_POINTS;
		m_vertices.push_back({ glm::vec3(0,0,0) });
		m_elementCount = 1;

		m_vao = createShared<VAO>();
		VBO vbo(m_vertices);
		m_vao->addBuffer(vbo, Vertex::getLayout());

		computeBoundingBox();
		Console::trace("Mesh") << "Loaded " << m_vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, GLsizei count, shared<VBO<>> vbo, GLuint mode){
		m_drawMode = mode;
		m_elementCount = count;
		m_vao = createShared<VAO>();
		m_vao->addBuffer(*vbo, Vertex::getLayout());
		computeBoundingBox();
		Console::info("Mesh") << "Loaded " << m_vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, GLsizei count, shared<VBO<>> vbo, shared<IBO> ebo, GLuint mode) {
		m_drawMode = mode;
		m_elementCount = count;
		m_vao = createShared<VAO>();
		m_vao->addBuffer(*vbo, Vertex::getLayout());
		m_vao->bindBuffer(*ebo);
		computeBoundingBox();
		Console::trace("Mesh") << "Loaded " << m_vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, GLuint mode) : RenderableObject(name) {
		m_drawMode = mode;
		m_vertices = vertices;
		m_elementCount = m_vertices.size();
		m_vao = createShared<VAO>();
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao->addBuffer(vbo, Vertex::getLayout());
		m_vao->bindBuffer(ebo);
		computeBoundingBox();
		Console::trace("Mesh") << "Loaded " << m_vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) : RenderableObject(name) {
		m_drawMode = mode;
		//Move vertices data
		m_vertices = vertices;
		m_indices = indices;
		m_elementCount = m_indices.size();
		// Create VAO, VBO, EBO
		m_vao = createShared<VAO>();
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		// Add VBO and EBO to VAO using DSA
		m_vao->addBuffer(vbo, Vertex::getLayout());
		m_vao->bindBuffer(ebo);
		computeBoundingBox();
		Console::trace("Mesh") << "Loaded " << m_vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, VAO_Ptr vao, GLuint count, GLuint mode) : RenderableObject(name) {
		m_drawMode = mode;
		m_vao = vao;
		m_elementCount = count;
	}

	shared<Mesh> Mesh::create(std::string name) {
		return createShared<Mesh>(name);
	}

	shared<Mesh> Mesh::create(std::string name, std::vector<Vertex>& vertices, GLuint mode) {
		return createShared<Mesh>(name, vertices, mode);
	}

	shared<Mesh> Mesh::create(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) {
		return createShared<Mesh>(name, vertices, indices, mode);
	}


	void Mesh::draw() const {
		glBindVertexArray(m_vao->id());
		if (m_indices.size() > 0) glDrawElements(m_drawMode, m_elementCount, GL_UNSIGNED_INT, 0); //draw elements using EBO
		else glDrawArrays(m_drawMode, 0, m_elementCount); //draw
		glBindVertexArray(0);
	}

	void Mesh::drawInstanced(GLsizeiptr instances) const {
		glBindVertexArray(m_vao->id());
		if (m_indices.size() > 0) glDrawElementsInstanced(m_drawMode, m_elementCount, GL_UNSIGNED_INT, nullptr, instances); //draw elements using EBO
		else glDrawArraysInstanced(m_drawMode, 0, m_elementCount, instances); //draw
		glBindVertexArray(0);
	}



	void Mesh::smoothNormals() {
		// Clear any existing normals in the vertex data
		for (auto& vertex : m_vertices) {
			vertex.normal = glm::vec3(0.0f);
		}

		// Use a map to accumulate normals based on vertex positions
		std::unordered_map<glm::vec3, glm::vec3, std::hash<glm::vec3>, std::equal_to<glm::vec3>> normalMap;

		// Accumulate face normals for each vertex
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			GLuint i0 = m_indices[i];
			GLuint i1 = m_indices[i + 1];
			GLuint i2 = m_indices[i + 2];

			glm::vec3 v0 = m_vertices[i0].position;
			glm::vec3 v1 = m_vertices[i1].position;
			glm::vec3 v2 = m_vertices[i2].position;

			// Calculate the face normal
			glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			// Accumulate the face normal into each vertex normal based on position
			normalMap[v0] += faceNormal;
			normalMap[v1] += faceNormal;
			normalMap[v2] += faceNormal;
		}

		// Normalize the accumulated normals and assign them to the vertices
		for (auto& vertex : m_vertices) {
			vertex.normal = glm::normalize(normalMap[vertex.position]);
			vertex.tangent = glm::vec3(0);
			vertex.bitangent = glm::vec3(0);
		}

		// Update the Vertex Array Object (VAO) with the new normals
		updateVAO();
	}

	void Mesh::voxelize(float size, bool sdf) {
		m_voxels = Voxelizer::voxelize(*this, size);
		m_voxels_position = Voxelizer::getVoxelposition(m_voxels, m_bbox, size);
		m_voxels_sdf = std::vector<glm::vec4>(m_voxels.size(), glm::vec4(0));
		Console::print() << "Voxelized " << m_voxels.size() << " voxels." << Console::endl;

		if (sdf) {
			Console::print() << "Computing SDF (this may take a while)" << Console::endl;
			computeSDF(std::thread::hardware_concurrency());
		}
	}

	void Mesh::voxelizeSurface(float size, float thickness) {
		m_voxels = Voxelizer::voxelizeSurface(*this, size, thickness);
	}

	glm::vec4 pointTriangleDistance(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
		using namespace glm;

		vec3 ab = b - a;
		vec3 ac = c - a;
		vec3 ap = p - a;

		float d1 = dot(ab, ap);
		float d2 = dot(ac, ap);
		if (d1 <= 0.0f && d2 <= 0.0f) {
			return glm::vec4(a, length(p - a));
		}

		vec3 bp = p - b;
		float d3 = dot(ab, bp);
		float d4 = dot(ac, bp);
		if (d3 >= 0.0f && d4 <= d3) {
			return glm::vec4(b, length(p - b));
		}

		vec3 cp = p - c;
		float d5 = dot(ab, cp);
		float d6 = dot(ac, cp);
		if (d6 >= 0.0f && d5 <= d6) {
			return glm::vec4(c, length(p - c));
		}

		float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
			float v = d1 / (d1 - d3);
			vec3 closestPoint = a + v * ab;
			return glm::vec4(closestPoint, length(p - closestPoint));
		}

		float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
			float w = d2 / (d2 - d6);
			vec3 closestPoint = a + w * ac;
			return glm::vec4(closestPoint, length(p - closestPoint));
		}

		float va = d3 * d6 - d5 * d4;
		if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 + va) >= 0.0f) {
			float u = (d4 - d3) / ((d4 - d3) + va);
			vec3 closestPoint = b + u * (c - b);
			return glm::vec4(closestPoint, length(p - closestPoint));
		}

		// Inside face region
		vec3 normal = normalize(cross(ab, ac));
		float distToPlane = dot(ap, normal);
		vec3 closestPoint = p - distToPlane * normal;
		return glm::vec4(closestPoint, fabs(distToPlane));
	}


	//return the SDF and gradient of the given point according to the mesh
	glm::vec4 Mesh::sdf(glm::vec3 pointOfInterest) {
		float minDistance = std::numeric_limits<float>::max();
		glm::vec3 closestPoint;

		for (size_t i = 0; i < m_indices.size(); i += 3) {
			GLuint i0 = m_indices[i];
			GLuint i1 = m_indices[i + 1];
			GLuint i2 = m_indices[i + 2];
			glm::vec3 v0 = m_vertices[i0].position;
			glm::vec3 v1 = m_vertices[i1].position;
			glm::vec3 v2 = m_vertices[i2].position;

			glm::vec4 localClosest = pointTriangleDistance(pointOfInterest, v0, v1, v2);
			float distance = localClosest.w;
			if (distance < minDistance) {
				minDistance = distance;
				closestPoint = glm::vec3(localClosest) + m_vertices[i0].normal * 2.0f;
			}
		}

		glm::vec3 gradient = glm::normalize(pointOfInterest - closestPoint);
		return glm::vec4(gradient, minDistance);
	}

	void Mesh::computeSDF(int threadCount) {
		if (threadCount > 1) {
			const int voxelCount = static_cast<int>(m_voxels_position.size());
			m_voxels_sdf.resize(voxelCount);

			std::vector<std::future<void>> futures;
			int batchSize = voxelCount / threadCount;

			for (int t = 0; t < threadCount; ++t) {
				int start = t * batchSize;
				int end = (t == threadCount - 1) ? voxelCount : start + batchSize;

				futures.emplace_back(std::async(std::launch::async, [this, start, end]() {
					for (int i = start; i < end; ++i) {
						m_voxels_sdf[i] = sdf(m_voxels_position[i]);
					}
					}));
			}

			for (auto& f : futures) {
				f.get();
			}
		}
		else {
			//implement here the mono threaded version
		}
	}


	void Mesh::computeBoundingBox() {
		glm::mat4 modelMat = globalTransform();
		glm::vec3 min(FLT_MAX), max(-FLT_MAX);
		for (const auto& vertex : m_vertices) {
			glm::vec4 vec = modelMat * glm::vec4(vertex.position, 1);
			min = glm::min(min, glm::vec3(vec));
			max = glm::max(max, glm::vec3(vec));
		}


		glm::vec3 bbsize = max - min;

		if (bbsize.x * bbsize.y * bbsize.z == 0) {
			min -= glm::vec3(0.5);
			max += glm::vec3(0.5);
		}

		m_bbox.min = min;
		m_bbox.max = max;
		m_bbox.centroid = (min + max) / 2.0f;
		m_bbox.size = glm::abs(m_bbox.max - m_bbox.min);
		m_hasBoundingBox = true;
		Console::trace("Mesh") << "Bounding box is " << m_bbox.max - m_bbox.min << " starting at " << m_bbox.min << " and ending at " << m_bbox.max << Console::endl;
	}

	void Mesh::swapNormals() {
		m_swap_normals = !m_swap_normals;
	}

	void Mesh::flipFace() {
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			GLuint i0 = m_indices[i];
			GLuint i1 = m_indices[i + 1];
			GLuint i2 = m_indices[i + 2];

			glm::vec3 v0 = m_vertices[i0].position;
			glm::vec3 v1 = m_vertices[i1].position;
			glm::vec3 v2 = m_vertices[i2].position;

			m_vertices[i1].position = v2;
			m_vertices[i2].position = v1;
		}

		updateVAO();
	}

	

	void Mesh::computeNormals(){
		// Initialize all normals to zero
		for (auto& vertex : m_vertices) {
			vertex.normal = glm::vec3(0);
		}

		// Calculate the face normal for each triangle and accumulate the normals for each vertex
		Console::info("Mesh") << "Recomputing Mesh normals.." << Console::endl;
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			GLuint i0 = m_indices[i];
			GLuint i1 = m_indices[i + 1];
			GLuint i2 = m_indices[i + 2];

			glm::vec3 v0 = m_vertices[i0].position;
			glm::vec3 v1 = m_vertices[i1].position;
			glm::vec3 v2 = m_vertices[i2].position;

			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;

			glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

			m_vertices[i0].normal += faceNormal;
			m_vertices[i1].normal += faceNormal;
			m_vertices[i2].normal += faceNormal;
			Console::printProgress(float(i) / float(m_indices.size()));
		}
		Console::print() << Console::endl;

		// Normalize the accumulated normals
		for (auto& vertex : m_vertices) {
			vertex.normal = -glm::normalize(vertex.normal);
		}

		//Update VAO, VBO
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao->bindBuffer(ebo);
		m_vao->addBuffer(vbo, Vertex::getLayout());
	}

	void Mesh::calculateIndices() {
		Console::info("Mesh") << "Recomputing Mesh indices and removing duplicate vertices.." << Console::endl;

		Vertices newVertices;
		std::vector<GLuint> newIndices(m_vertices.size());
		int newIndex = 0;

		for (size_t i = 0; i < m_vertices.size(); ++i) {
			const Vertex& v = m_vertices[i];
			bool found = false;
			int foundIndex = -1;
			for (int j = 0; j < newVertices.size(); ++j) {
				if (newVertices[j].position == v.position) {
					found = true;
					foundIndex = j;
					break;
				}
			}

			if (!found) {
				newVertices.push_back(v);
				newIndices[i] = newIndex;
				++newIndex;
			}
			else {
				newIndices[i] = foundIndex;
			}
			Console::printProgress(float(i + 1) / float(m_vertices.size()));
		}

		m_vertices = std::move(newVertices);
		m_indices = std::move(newIndices);

		Console::print() << Console::endl;
	}



	void Mesh::updateVAO() {
		//Update VAO, VBO
		m_vao->bind();
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao->bindBuffer(ebo);
		m_vao->addBuffer(vbo, Vertex::getLayout());
		m_vao->unbind();
	}


	void Mesh::removeUnusedVertices() {
		Console::info("Mesh") << "Removing unused vertices.." << Console::endl;

		std::vector<bool> usedVertices(m_vertices.size(), false);
		for (GLuint index : m_indices) {
			usedVertices[index] = true;
		}

		Vertices newVertices;
		std::vector<GLuint> vertexMapping(m_vertices.size());
		GLuint newIndex = 0;

		for (size_t i = 0; i < m_vertices.size(); ++i) {
			if (usedVertices[i]) {
				newVertices.push_back(m_vertices[i]);
				vertexMapping[i] = newIndex++;
			}
		}

		for (GLuint& index : m_indices) {
			index = vertexMapping[index];
		}

		m_vertices = std::move(newVertices);

		Console::print() << Console::endl;
	}

	void Mesh::applyMeshTransform(){
		for (auto& vertex : m_vertices) {
			vertex.position = transform() * glm::vec4(vertex.position,1.0);
			vertex.normal = glm::normalize(transform() * glm::vec4(vertex.normal,0.0));
		}
		computeBoundingBox();
		updateVAO();
		setTransform(glm::mat4(1));
	}

	void Mesh::centerMeshOrigin() {
		computeBoundingBox();

		// Apply transformation directly to the vertices
		glm::vec3 centroid = m_bbox.centroid;
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), -centroid);

		for (auto& vertex : m_vertices) {
			vertex.position = transform * glm::vec4(vertex.position, 1.0);
			vertex.normal = transform * glm::vec4(vertex.normal, 0.0);
		}

		computeBoundingBox();
		updateVAO();
		setTransform(glm::mat4(1));
	}

	
}