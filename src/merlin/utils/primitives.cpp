#include "pch.h"
#include "merlin/utils/primitives.h"
#include "merlin/memory/ibo.h"

namespace Merlin {

	shared<Mesh> Primitives::createCircle(float r, int res) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		// Center vertex
		vertices.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

		for (int i = 0; i < res; ++i) {
			float angle = 2.0f * glm::pi<float>() * i / res;
			float x = r * cos(angle);
			float y = r * sin(angle);

			vertices.push_back(Vertex(glm::vec3(x, y, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2((cos(angle) + 1.0f) * 0.5f, (sin(angle) + 1.0f) * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

			// Connect vertices to form triangles
			if (i > 0) {
				indices.push_back(0); // Center
				indices.push_back(i);
				indices.push_back(i + 1);
			}
		}

		// Close the circle
		indices.push_back(0); // Center
		indices.push_back(res);
		indices.push_back(1);

		return Mesh::create("Circle", vertices, indices, GL_TRIANGLES);
	}

	shared<Mesh> Primitives::createOutlinedCircle(float r, int res) {
		std::vector<Vertex> vertices;

		for (int i = 0; i < res; ++i) {
			float angle = 2.0f * glm::pi<float>() * i / res;
			float x = r * cos(angle);
			float y = r * sin(angle);

			vertices.push_back(Vertex(glm::vec3(x, y, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2((cos(angle) + 1.0f) * 0.5f, (sin(angle) + 1.0f) * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		vertices.push_back(Vertex(glm::vec3(r, 0, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2((2.0f) * 0.5f, (1.0f) * 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

		return Mesh::create("Circle", vertices, GL_LINE_STRIP);
	}

	shared<Mesh> Primitives::createRectangle(float x, float y) {
		Vertex v0 = Vertex{ glm::vec3(-x / 2.0f,-y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		Vertex v1 = Vertex{ glm::vec3(x / 2.0f, -y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		Vertex v2 = Vertex{ glm::vec3(x / 2.0f, y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		Vertex v3 = Vertex{ glm::vec3(-x / 2.0f,y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f,  1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };

		Vertices v = { v0, v1, v2, v3 };

		Indices i = { 0, 1, 2, 0, 2, 3 };
		return Mesh::create("Rectangle", v, i, GL_TRIANGLES);
	}


	shared<Mesh> Primitives::createQuadRectangle(float x, float y, bool centered) {

		float x_lo;
		float x_up;
		float y_lo;
		float y_up;

		if (centered) {
			x_lo = -x / 2.0f;
			x_up = x / 2.0f;
			y_lo = -y / 2.0f;
			y_up = y / 2.0f;
		}
		else {
			x_lo = 0;
			x_up = x;
			y_lo = 0;
			y_up = y;
		}

		Vertex v0 = { glm::vec3(x_lo,y_lo,0),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		Vertex v1 = { glm::vec3(x_up,y_lo,0),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		Vertex v2 = { glm::vec3(x_up,y_up,0),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		Vertex v3 = { glm::vec3(x_lo,y_up,0),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };


		Vertices v = { v0, v1, v2, v3 };
		Mesh_Ptr m = Mesh::create("Cube", v, GL_QUADS);
		m->castShadow(false);
		return m;
	}

	shared<Mesh> Primitives::createFloor(const int groundNumTiles, const float groundTileSize) {

		glm::vec2 squareVerts[] = { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1) };

		float r = groundNumTiles / 2.0 * groundTileSize;

		std::vector<float> groundVerts;
		groundVerts.resize(3 * 4 * groundNumTiles * groundNumTiles, 0);
		std::vector<float> groundColors;
		groundColors.resize(3 * 4 * groundNumTiles * groundNumTiles, 0);

		for (int xi(0); xi < groundNumTiles; xi++) {
			for (int yi(0); yi < groundNumTiles; yi++) {
				float x = (-groundNumTiles / 2.0 + xi) * groundTileSize;
				float y = (-groundNumTiles / 2.0 + yi) * groundTileSize;
				float p = xi * groundNumTiles + yi;
				for(int i(0); i < 4; i++) {
					float q = 4 * p + i;
					float px = x + squareVerts[i][0] * groundTileSize;
					float py = y + squareVerts[i][1] * groundTileSize;
					groundVerts[3 * q] = px;
					groundVerts[3 * q + 1] = py;
					float col = 0.0;

					if((xi + yi) % 2 == 1){
						col = 0.9;
						float pr = sqrt(px * px + py * py);
						float d = std::max(0.0, 1.2 - pr / r);
						col = col * d*2;
						for(int j(0); j < 3; j++)
							groundColors[3 * q + j] = col;
						
					}
				}
			}
		}

		Vertices v;
		Vertex v0;
		Vertex v2;

		int localIndex = 0;

		for (int i(0); i < 4 * groundNumTiles * groundNumTiles; i++) {
			Vertex vert;
			vert.position = glm::vec3(groundVerts[i * 3], groundVerts[i * 3 + 1], groundVerts[i * 3 + 2]);
			vert.normal = glm::vec3(0, 0, 1);
			vert.color = glm::vec3(groundColors[i * 3], groundColors[i * 3 + 1], groundColors[i * 3 + 2]);

			float tx = (i / 4) / 3;
			float ty = (i / 4) % 3;

			vert.texCoord = glm::vec2(tx / groundNumTiles * groundTileSize, ty / groundNumTiles * groundTileSize);
			vert.tangent = glm::vec3(1, 0, 0);
			vert.bitangent = glm::vec3(0, 1, 0);

			switch (localIndex) {
			case 0:
				v0 = vert;
				v.push_back(vert);
				localIndex++;
				break;
			case 1:
				v.push_back(vert);
				localIndex++;
				break;
			case 2:
				v2 = vert;
				v.push_back(vert);
				localIndex++;
				break;
			case 3 :
				v.push_back(v0);
				v.push_back(v2);
				v.push_back(vert);
				localIndex = 0;
				break;
			}
		}

		Mesh_Ptr result = Mesh::create("Floor", v, GL_TRIANGLES);
		result->useVertexColors(true);
		return result;
	}

	shared<Mesh> Primitives::createPoint() {
		Vertices v = {
			Vertex{glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)}
		};
		return Mesh::create("Point", v, GL_POINTS);
	}

	shared<Mesh> Primitives::createLine(float length, glm::vec3 dir) {
		Vertices v = {
			Vertex{glm::vec3(0,0,0)},
			Vertex{glm::normalize(dir) * length}
		};

		return Mesh::create("Line", v, GL_LINES);
	}

	shared<Mesh> Primitives::createCoordSystem() {
		Vertices v = {
			Vertex{ glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
		};

		return Mesh::create("CoordinateSystem", v, GL_LINES);
	}

	shared<Mesh> Primitives::createQuadCube(float w, bool centered) {
		return createQuadBox(w, w, w, centered);
	}

	shared<Mesh> Primitives::createQuadBox(float x, float y, float z, bool centered) {

		float x_lo;
		float x_up;
		float y_lo;
		float y_up;
		float z_lo;
		float z_up;

		if (centered) {
			x_lo = -x / 2.0f;
			x_up = x / 2.0f;
			y_lo = -y / 2.0f;
			y_up = y / 2.0f;
			z_lo = -z / 2.0f;
			z_up = z / 2.0f;
		}else{
			x_lo = 0;
			x_up = x;
			y_lo = 0;
			y_up = y;
			z_lo = 0;
			z_up = z;
		}

		std::vector<Vertex> v = {
			//   Coordinates
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)}, // X-
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},

			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)}, // X+
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}, // Y-
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},

			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)}, // Y+
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},

			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)}, // Z-
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},

			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)}, // Z+
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)}

		};
		Mesh_Ptr m = Mesh::create("Cube", v, GL_QUADS);
		m->castShadow(false);
		return m;
	}

	shared<Mesh> Primitives::createCube(float w) { return createBox(w, w, w); }

	shared<Mesh> Primitives::createBox(float x, float y, float z) {

		float x_lo = -x / 2.0f;
		float x_up = x / 2.0f;
		float y_lo = -y / 2.0f;
		float y_up = y / 2.0f;
		float z_lo = -z / 2.0f;
		float z_up = z / 2.0f;

		std::vector<Vertex> v = {
			//   Coordinates
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)}
		};

		return Mesh::create("Cube", v);
	}

	shared<Mesh> Primitives::createHollowCube(float w, float th) { return createHollowBox(w, w, w, th); }

	shared<Mesh> Primitives::createHollowBox(float x, float y, float z, float thickness) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		// Helper function to create a beam (box) between two points
		auto createBeam = [&](glm::vec3 start, glm::vec3 end, float thickness) {
			glm::vec3 direction = glm::normalize(end - start);
			start = start - direction * thickness * 0.5f;
			end = end + direction * thickness * 0.5f;

			glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
			if (glm::abs(glm::dot(direction, up)) > 0.99f) {
				up = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			glm::vec3 right = glm::normalize(glm::cross(direction, up));
			up = glm::normalize(glm::cross(right, direction));

			glm::vec3 halfThickness = glm::vec3(thickness) * 0.5f;

			std::vector<glm::vec3> beamVertices = {
				start - halfThickness * right - halfThickness * up, // 0
				start + halfThickness * right - halfThickness * up, // 1
				start + halfThickness * right + halfThickness * up, // 2
				start - halfThickness * right + halfThickness * up, // 3
				end - halfThickness * right - halfThickness * up,   // 4
				end + halfThickness * right - halfThickness * up,   // 5
				end + halfThickness * right + halfThickness * up,   // 6
				end - halfThickness * right + halfThickness * up    // 7
			};

			for (const auto& v : beamVertices) {
				vertices.push_back(Vertex(v, glm::vec3(0.0f), glm::vec3(1.0f)));
			}

			std::vector<GLuint> beamIndices = {
				0, 1, 2, 0, 2, 3, // Front face
				4, 5, 6, 4, 6, 7, // Back face
				0, 1, 5, 0, 5, 4, // Bottom face
				1, 2, 6, 1, 6, 5, // Right face
				2, 3, 7, 2, 7, 6, // Top face
				3, 0, 4, 3, 4, 7  // Left face
			};

			GLuint offset = vertices.size() - 8;
			for (const auto& index : beamIndices) {
				indices.push_back(offset + index);
			}
			};

		// Define the 8 corners of the box
		glm::vec3 corners[8] = {
			{-x / 2, -y / 2, -z / 2}, 
			{ x / 2, -y / 2, -z / 2},
			{ x / 2, y / 2, -z / 2}, 
			{-x / 2,  y / 2, -z / 2},
			{-x / 2, -y / 2,  z / 2}, 
			{ x / 2, -y / 2,  z / 2},
			{ x / 2,  y / 2,  z / 2}, 
			{-x / 2,  y / 2,  z / 2}
		};

		glm::vec3 offsetX = glm::vec3(thickness * 0.5f,0,0) ;

		// Create beams for the 12 edges of the box
		createBeam(corners[0], corners[1], thickness);
		createBeam(corners[1], corners[2], thickness);
		createBeam(corners[2], corners[3], thickness);
		createBeam(corners[3], corners[0], thickness);
		createBeam(corners[4], corners[5], thickness);
		createBeam(corners[5], corners[6], thickness);
		createBeam(corners[6], corners[7], thickness);
		createBeam(corners[7], corners[4], thickness);
		createBeam(corners[0], corners[4], thickness);
		createBeam(corners[1], corners[5], thickness);
		createBeam(corners[2], corners[6], thickness);
		createBeam(corners[3], corners[7], thickness);

		Mesh_Ptr box = Mesh::create("HollowBox", vertices, indices, GL_TRIANGLES);
		box->useFlatShading();
		box->computeNormals();
		return box;
	}


	


	shared<Mesh> Primitives::createCone(float r, float h, int res) {
		const float angleStep = glm::two_pi<float>() / res;

		// Generate vertices
		Vertices vertices;
		vertices.reserve(res + 2);

		Vertex v = { glm::vec3(h, 0.0f, 0.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.5f, 1.0f) };
		vertices.emplace_back(v);
		v = { glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(-1.0f, 1.0f, 1.0f)), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.5f, 0.0f) };
		vertices.emplace_back(v);
		for (int i = 0; i < res; ++i) {
			const float angle = i * angleStep;
			const glm::vec3 position(0.0f, r * sin(angle), r * cos(angle));
			const glm::vec3 normal = glm::normalize(glm::vec3(r / sqrt(r * r + h * h), sin(angle), cos(angle)));
			const glm::vec2 texCoord(angle / glm::two_pi<float>(), 0.0f);

			v = { position, normal, glm::vec3(1.0f, 1.0f, 1.0f), texCoord };

			vertices.emplace_back(v);
		}

		// Generate indices
		Indices indices;
		indices.reserve(res * 4);
		for (int i = 0; i < res; ++i) {
			const int i0 = 2 + i;
			const int i1 = 2 + (i + 1) % res;
			indices.push_back(0);
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(1);
			indices.push_back(i1);
			indices.push_back(i0);
		}

		return createShared<Mesh>("Cone", vertices, indices, GL_TRIANGLES);
	}

	shared<Mesh> Primitives::createCylinder(float r, float h, int res) {
		const float angleStep = glm::two_pi<float>() / res;

		// Generate indices
		Indices indices;
		indices.reserve(res * 12);
		for (int i = 0; i < res; ++i) {
			const int i0 = i * 2;
			const int i1 = i0 + 1;
			const int i2 = (i + 1) % res * 2;
			const int i3 = i2 + 1;
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i2);
			indices.push_back(i1);
			indices.push_back(i3);

			// Indices for top face
			const int iTopCenter = (res + 1) * 2;
			indices.push_back(i2);
			indices.push_back(i0);
			indices.push_back(iTopCenter);

			// Indices for bottom face
			const int iBottomCenter = (res + 1) * 2 + 1;
			indices.push_back(i1);
			indices.push_back(i3);
			indices.push_back(iBottomCenter);
		}

		// Generate vertices
		Vertices vertices;
		vertices.reserve((res + 1) * 2 + 2);
		for (int i = 0; i <= res; ++i) {
			const float angle = i * angleStep;
			const glm::vec2 texCoordTop(angle / glm::two_pi<float>(), 1.0f);
			const glm::vec2 texCoordBottom(angle / glm::two_pi<float>(), 0.0f);
			vertices.emplace_back(glm::vec3(h, r * sin(angle), r * cos(angle)), glm::normalize(glm::vec3(0.0f, sin(angle), cos(angle))), glm::vec3(1.0f), texCoordTop);
			vertices.emplace_back(glm::vec3(0.0f, r * sin(angle), r * cos(angle)), glm::normalize(glm::vec3(0.0f, sin(angle), cos(angle))), glm::vec3(1.0f), texCoordBottom);
		}
		// Add the center vertices for the top and bottom faces
		vertices.emplace_back(glm::vec3(h, 0.0f, 0.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(1.0f), glm::vec2(0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f)), glm::vec3(1.0f), glm::vec2(0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		Mesh_Ptr mesh = createShared<Mesh>("Cylinder", vertices, indices, GL_TRIANGLES);

		return mesh;
	}

	shared<Model> Primitives::createArrow(float size, glm::vec3 color, int res){
		Mesh_Ptr axis_cyl;
		Mesh_Ptr axis_cone;

		float cone_height = size / 10;
		float cone_radius = size / 25;
		float cylinder_radius = size / 40;
		float cylinder_height = size;

		axis_cyl = Primitives::createCylinder(cylinder_radius, cylinder_height, res);
		axis_cone = Primitives::createCone(cone_radius, cone_height, res);

		axis_cone->translate(glm::vec3(size, 0, 0));
		//axis_cone->rotate(glm::vec3(0, 0, -glm::pi<float>()*0.5));
		//axis_cone->applyMeshTransform();
		axis_cone->computeNormals();
		axis_cone->smoothNormals();

		Model_Ptr x_axis = Model::create("x_axis", { axis_cyl , axis_cone });

		shared<PhongMaterial> xMaterial = createShared<PhongMaterial>("xMaterial");
		xMaterial->setAmbient(color);
		xMaterial->setDiffuse(color);

		x_axis->setMaterial(xMaterial);
		x_axis->setRenderMode(RenderMode::UNLIT);

		return x_axis;
	}


	shared<Mesh> Primitives::createBoundingBox(BoundingBox aabb) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		float thickness = aabb.size.length()*0.01;

		// Helper function to create a beam (box) between two points
		auto createBeam = [&](glm::vec3 start, glm::vec3 end, float thickness) {
			glm::vec3 direction = glm::normalize(end - start);
			start = start - direction * thickness * 0.5f;
			end = end + direction * thickness * 0.5f;

			glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
			if (glm::abs(glm::dot(direction, up)) > 0.99f) {
				up = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			glm::vec3 right = glm::normalize(glm::cross(direction, up));
			up = glm::normalize(glm::cross(right, direction));

			glm::vec3 halfThickness = glm::vec3(thickness) * 0.5f;

			std::vector<glm::vec3> beamVertices = {
				start - halfThickness * right - halfThickness * up, // 0
				start + halfThickness * right - halfThickness * up, // 1
				start + halfThickness * right + halfThickness * up, // 2
				start - halfThickness * right + halfThickness * up, // 3
				end - halfThickness * right - halfThickness * up,   // 4
				end + halfThickness * right - halfThickness * up,   // 5
				end + halfThickness * right + halfThickness * up,   // 6
				end - halfThickness * right + halfThickness * up    // 7
			};

			for (const auto& v : beamVertices) {
				vertices.push_back(Vertex(v, glm::vec3(0.0f), glm::vec3(1.0f)));
			}

			std::vector<GLuint> beamIndices = {
				0, 1, 2, 0, 2, 3, // Front face
				4, 5, 6, 4, 6, 7, // Back face
				0, 1, 5, 0, 5, 4, // Bottom face
				1, 2, 6, 1, 6, 5, // Right face
				2, 3, 7, 2, 7, 6, // Top face
				3, 0, 4, 3, 4, 7  // Left face
			};

			GLuint offset = vertices.size() - 8;
			for (const auto& index : beamIndices) {
				indices.push_back(offset + index);
			}
			};

		// Define the 8 corners of the box
		glm::vec3 corners[8] = {
			{aabb.min.x, aabb.min.y, aabb.min.z},
			{aabb.max.x, aabb.min.y, aabb.min.z},
			{aabb.max.x, aabb.max.y, aabb.min.z},
			{aabb.min.x, aabb.max.y, aabb.min.z},
			{aabb.min.x, aabb.min.y, aabb.max.z},
			{aabb.max.x, aabb.min.y, aabb.max.z},
			{aabb.max.x, aabb.max.y, aabb.max.z},
			{aabb.min.x, aabb.max.y, aabb.max.z}
		};

		glm::vec3 offsetX = glm::vec3(thickness * 0.5f, 0, 0);

		// Create beams for the 12 edges of the box
		createBeam(corners[0], corners[1], thickness);
		createBeam(corners[1], corners[2], thickness);
		createBeam(corners[2], corners[3], thickness);
		createBeam(corners[3], corners[0], thickness);
		createBeam(corners[4], corners[5], thickness);
		createBeam(corners[5], corners[6], thickness);
		createBeam(corners[6], corners[7], thickness);
		createBeam(corners[7], corners[4], thickness);
		createBeam(corners[0], corners[4], thickness);
		createBeam(corners[1], corners[5], thickness);
		createBeam(corners[2], corners[6], thickness);
		createBeam(corners[3], corners[7], thickness);

		Mesh_Ptr box = Mesh::create("HollowBox", vertices, indices, GL_TRIANGLES);
		box->useFlatShading();
		box->computeNormals();
		return box;
	}

	/*
	shared<Mesh> Primitives::createSphere(float r, int hres, int vres) {

		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> tex;

		
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / r;    // vertex normal
		float s, t;                                     // vertex texCoord

		float sectorStep = 2 * glm::pi<float>() / hres;
		float stackStep = glm::pi<float>() / vres;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= vres; ++i)
		{
			stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = r * cosf(stackAngle);             // r * cos(u)
			z = r * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= hres; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

				position.push_back(glm::vec3(x, y, z));

				// normalized vertex normal (nx, ny, nz)
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				normals.push_back(glm::vec3(nx, ny, nz));

				// vertex tex coord (s, t) range between [0, 1]
				s = (float)j / hres;
				t = (float)i / vres;
				tex.push_back(glm::vec2(s, t));
			}
		}
		
		std::vector<int> indices;
		int k1, k2;
		for (int i = 0; i < vres; ++i)
		{
			k1 = i * (hres + 1);     // beginning of current stack
			k2 = k1 + hres + 1;      // beginning of next stack

			for (int j = 0; j < hres; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (vres - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}


		

		
		position.resize(hres * vres * 6);
		normals.resize(hres * vres * 6);

		float d_h = 2 * glm::pi<float>() / ((float)hres);
		float d_v = glm::pi<float>() / ((float)vres);
		int n = 0;
		// Vertices are created inside this loop.
		for (int i = 0; i < hres; i++) {
			float h = i * d_h;
			float hn = h + d_h;
			for (int j = 0; j < vres; j++) {
				float v = j * d_v;
				float vn = v + d_v;

				// The sphere is consists of multiple triangles where 2 triangles make a quad.
				// These 4 points are the corners of said plane. To create a triangle 3 of these corners are
				// used counterclockwise with the 2nd triangle's first point being the 1st last point.
				// Normal vectors are the same as the points without the radius multiplied.
				glm::vec4 p0(glm::cos(h) * glm::sin(v), glm::sin(h) * glm::sin(v),
					glm::cos(v), 1.0f);
				glm::vec4 p1(glm::cos(h) * glm::sin(vn), glm::sin(h) * glm::sin(vn),
					glm::cos(vn), 1.0f);
				glm::vec4 p2(glm::cos(hn) * glm::sin(v), glm::sin(hn) * glm::sin(v),
					glm::cos(v), 1.0f);
				glm::vec4 p3(glm::cos(hn) * glm::sin(vn), glm::sin(hn) * glm::sin(vn),
					glm::cos(vn), 1.0f);
				position[n] = p0 * r;
				normals.at(n++) = glm::vec3(p0);
				position.at(n) = p1 * r;
				normals.at(n++) = glm::vec3(p1);
				position.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				position.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				position.at(n) = p2 * r;
				normals.at(n++) = glm::vec3(p2);
				position.at(n) = p0 * r;
				normals.at(n++) = glm::vec3(p0);

				// vertex tex coord (s, t) range between [0, 1]
				float s = (float)j / hres;
				float t = (float)i / vres;
				tex.push_back(glm::vec2(s, t));
			}
		}

		Vertices v;
		for (int i(0); i < position.size(); i++) {
			v.push_back({ position[i], normals[i], glm::vec3(1) });
		}
		return Mesh::create("Sphere", v);
		

		
		Vertices v;
		for (int i(0); i < position.size(); i++) {
			v.push_back({ position[i], normals[i], glm::vec3(1), tex[i]});
		}

		Indices i;
		for (int j(0); j < indices.size(); j++) {
			i.push_back(indices[j]);
		}

		return Mesh::create("Sphere",v, i);
		
	}
	*/



//hres sector
//vres stack
	shared<Mesh> Primitives::createSphere(float r, int hres, int vres) {
		float PI = 3.14159265359;
		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> tex;
		std::vector<GLuint> indices;

		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / r;    // normal
		float s, t;                                     // texCoord

		float sectorStep = 2 * PI / hres;
		float stackStep = PI / vres;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= vres; ++i)
		{
			stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = r * cosf(stackAngle);             // r * cos(u)
			z = r * sinf(stackAngle);              // r * sin(u)

			for (int j = 0; j <= hres; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
				position.push_back(glm::vec3(x, z, y));

				// normalized vertex normal
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				normals.push_back(glm::vec3(nx, nz, ny));

				// vertex tex coord between [0, 1]
				s = (float)j / hres;
				t = (float)i / vres;
				tex.push_back(glm::vec2(s, t));
			}
		}

		for (int i = 0; i < vres ; ++i){
			auto k1 = i * (hres + 1);
			auto k2 = k1 + hres + 1;
			for (int j = 0; j < hres;  ++j, ++k1, ++k2){
				if (i != 0){
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != vres-1) {
					indices.push_back(k1+1);
					indices.push_back(k2);
					indices.push_back(k2+1);
				}
			}
			// vertical lines for all stacks
		}
		


		Vertices v;
		for (int i(0); i < position.size(); i++) {
			v.push_back({ position[i], normals[i], glm::vec3(1), tex[i]});
		}

		Indices i;
		for (int j(0); j < indices.size(); j++) {
			i.push_back(indices[j]);
		}

		return Mesh::create("Sphere",v, i);

	}
		

	shared<Mesh> Primitives::createFromQuad(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) {
		std::vector<GLuint> indices_buffer;
		GLuint a; GLuint b;
		GLuint c; GLuint d;

		for (int i(0); i < indices.size(); i += 4) {
			a = indices_buffer[0]; b = indices_buffer[1];
			c = indices_buffer[2]; d = indices_buffer[3];

			indices_buffer.push_back(a - 1);//first triangle
			indices_buffer.push_back(b - 1);
			indices_buffer.push_back(c - 1);

			indices_buffer.push_back(a - 1);//second triangle
			indices_buffer.push_back(c - 1);
			indices_buffer.push_back(d - 1);
		}
		return Mesh::create("QuadMesh", vertices, indices_buffer);
	}

}