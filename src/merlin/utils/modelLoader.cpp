#include "pch.h"
#include "merlin/utils/modelLoader.h"
#include "merlin/core/core.h"
#include "merlin/scene/model.h"
#include "merlin/memory/ibo.h"
#include "merlin/memory/vertex.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Merlin::Utils;

namespace Merlin {


    Texture2D_Ptr loadMaterialTexture(const std::string& path, aiMaterial* mat, aiTextureType type, const std::string& typeName) {
        
        if (mat->GetTextureCount(type) > 0) {
            aiString str;
            mat->GetTexture(type, 0, &str);
            
            std::string filepath = getFileFolder(path) + std::string(str.C_Str()); // Adjust path as needed
            
            TextureType t;
            switch (type) {
                case aiTextureType::aiTextureType_DIFFUSE : 
                    t = TextureType::DIFFUSE;
                    break;
                case aiTextureType::aiTextureType_SPECULAR :
                    t = TextureType::SPECULAR;
                    break;
                case aiTextureType::aiTextureType_NORMALS:
                    t = TextureType::NORMAL;
                    break;
                case aiTextureType::aiTextureType_DISPLACEMENT:
                    t = TextureType::DISPLACMENT ;
                    break;
                case aiTextureType::aiTextureType_EMISSIVE:
                    t = TextureType::EMISSION;
                    break;
                case aiTextureType::aiTextureType_REFLECTION:
                    t = TextureType::REFLECTION;
                    break;
                case aiTextureType::aiTextureType_SHININESS :
                    //t = TextureType::ROUGHNESS ;
                    Console::error("ModelLoader") << "Shininess map are not supported yet" << Console::endl;
                    break;
            }
                          
            return Texture2D::create(filepath, t);
        }
        return nullptr;
    }


    shared<Mesh> processMesh(const std::string& path, aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }

            // Process vertex tangents
            if (mesh->HasTangentsAndBitangents()) {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangent = vector;
                // Process vertex bitangents
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.bitangent = vector;
            }

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoord = vec;
            }
            else {
                vertex.texCoord = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process material
        shared<MaterialBase> material = nullptr;
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

            PhongMaterial* phongMaterial = new PhongMaterial(mesh->mName.C_Str());
            phongMaterial->setDiffuseTexture(loadMaterialTexture(path, mat, aiTextureType_DIFFUSE, "texture_diffuse"));
            phongMaterial->setSpecularTexture(loadMaterialTexture(path, mat, aiTextureType_SPECULAR, "texture_specular"));
            phongMaterial->setNormalTexture(loadMaterialTexture(path, mat, aiTextureType_NORMALS, "texture_normal"));
            
            

            // Extract color information
            aiColor3D color(0.f, 0.f, 0.f);
            float shininess;
            mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
            phongMaterial->setAmbient(glm::vec3(color.r, color.g, color.b));
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            phongMaterial->setDiffuse(glm::vec3(color.r, color.g, color.b));
            mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
            phongMaterial->setSpecular(glm::vec3(color.r, color.g, color.b));
            mat->Get(AI_MATKEY_SHININESS, shininess);
            phongMaterial->setShininess(shininess/128.0);

            material = shared<MaterialBase>(phongMaterial);
        }

        shared<Mesh> newMesh = Mesh::create(mesh->mName.C_Str(), vertices, indices);
        if (!mesh->HasNormals()) {
            newMesh->computeNormals();
        }
        //newMesh->calculateNormals();
        if (material) {
            newMesh->setMaterial(material);
        }
        return newMesh;
    }

    void processNode(const std::string& path, aiNode* node, const aiScene* scene, std::vector<shared<Mesh>>& meshes) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(path, mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(path, node->mChildren[i], scene, meshes);
        }
    }

    // Load a model from the specified file and return a pointer to a new Mesh object
    shared<Mesh> ModelLoader::loadMesh(const std::string& file_path) {

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FixInfacingNormals
            | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;

            //fallback to custom made importer
            FileType ft = getFileType(file_path);
            if (ft == FileType::OBJ || ft == FileType::STL || ft == FileType::GEOM) {
                Vertices vertices;
                Indices indices;
                if (!parseMesh(file_path, vertices, indices)) {
                    throw std::runtime_error("Unknown file type: " + file_path);
                }
                // create a Mesh object using the parsed vertex and index data
                auto mesh = Mesh::create(getFileName(file_path), vertices, indices);
                //mesh->calculateNormals();

                return mesh;
            }
            return nullptr;
        }

        std::vector<shared<Mesh>> meshes;
        processNode(file_path, scene->mRootNode, scene, meshes);

        return meshes[0];
    }

	// Load a model from the specified file and return a pointer to a new Mesh object
    shared<Model> ModelLoader::loadModel(const std::string& file_path) {

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;

            //fallback to custom made importer
            FileType ft = getFileType(file_path);
            if (ft == FileType::OBJ || ft == FileType::STL || ft == FileType::GEOM) {
                Vertices vertices;
                Indices indices;
                if (!parseMesh(file_path, vertices, indices)) {
                    throw std::runtime_error("Unknown file type: " + file_path);
                }
                // create a Mesh object using the parsed vertex and index data
                auto mesh = Mesh::create(getFileName(file_path) + "_mesh", vertices, indices);
                //mesh->calculateNormals();

                return Model::create(getFileName(file_path), mesh);
            }
            return nullptr;
        }

        std::vector<shared<Mesh>> meshes;
        processNode(file_path, scene->mRootNode, scene, meshes);

        shared<Model> mdl = Model::create(getFileName(file_path), meshes);
       

        return mdl;
	}





    /*
    void loadMTL(shared<Model> mdl) {

    }*/


/*
    shared<Model> ModelLoader::LoadGLTF(const std::string& file_path) {
        tinygltf::Model gltf_model;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = loader.LoadBinaryFromFile(&gltf_model, &err, &warn, file_path);
        if (!warn.empty()) {
            std::cout << "Warning: " << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << "Error: " << err << std::endl;
            return nullptr;
        }
        if (!ret) {
            std::cerr << "Failed to load GLTF model from file: " << file_path << std::endl;
            return nullptr;
        }


        // Process materials
        std::vector<shared<Material>> materials;
        for (const auto& gltf_material : gltf_model.materials) {
            shared<Material> material = std::make_shared<Material>(gltf_material.name);

            // Parse PBR properties
            const auto& pbr = gltf_material.pbrMetallicRoughness;

            // Convert base color factor to diffuse
            glm::vec3 diffuse = glm::vec3(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2]);
            material->setDiffuse(diffuse);
            material->setSpecular(diffuse);
            material->setAmbient(diffuse);

            // Set shininess (specular is not directly available in glTF 2.0)
            material->setShininess(32.0f); // Default value, as glTF 2.0 does not have a shininess value

            
            // Load textures
            if (pbr.baseColorTexture.index >= 0) {
                const auto& texture = gltf_model.textures[pbr.baseColorTexture.index];
                const auto& image = gltf_model.images[texture.source];
                
                shared<Texture> tex = createShared<Texture>();

                // Load the texture and add it to the material
                material->setTexture(tex);
            }
            
            
            materials.push_back(material);
        }


        shared<Model> model = Model::create(getFileName(file_path));

        for (const auto& mesh : gltf_model.meshes) {
            for (const auto& primitive : mesh.primitives) {
                std::vector<Vertex> vertices;
                std::vector<GLuint> indices;


                const auto& position_accessor = gltf_model.accessors[primitive.attributes.at("POSITION")];
                const auto& position_buffer_view = gltf_model.bufferViews[position_accessor.bufferView];
                const auto& position_buffer = gltf_model.buffers[position_buffer_view.buffer];
                const float* position_data = reinterpret_cast<const float*>(&position_buffer.data[position_buffer_view.byteOffset + position_accessor.byteOffset]);

                for (size_t i = 0; i < position_accessor.count; ++i) {
                    Vertex vertex;
                    vertex.position = glm::make_vec3(&position_data[i * 3]);
                    vertices.push_back(vertex);
                }

                const auto& index_accessor = gltf_model.accessors[primitive.indices];
                const auto& index_buffer_view = gltf_model.bufferViews[index_accessor.bufferView];
                const auto& index_buffer = gltf_model.buffers[index_buffer_view.buffer];
                const auto index_data = &index_buffer.data[index_buffer_view.byteOffset + index_accessor.byteOffset];

                // Handle different index formats
                if (index_accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE) {
                    for (size_t i = 0; i < index_accessor.count; ++i) {
                        indices.push_back(static_cast<GLuint>(index_data[i]));
                    }
                }
                else if (index_accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
                    const uint16_t* index_data_ushort = reinterpret_cast<const uint16_t*>(index_data);
                    for (size_t i = 0; i < index_accessor.count; ++i) {
                        indices.push_back(static_cast<GLuint>(index_data_ushort[i]));
                    }
                }
                else if (index_accessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
                    const GLuint* index_data_uint = reinterpret_cast<const GLuint*>(index_data);
                    for (size_t i = 0; i < index_accessor.count; ++i) {
                        indices.push_back(index_data_uint[i]);
                    }
                }
                else {
                    std::cerr << "Unsupported index component type: " << index_accessor.componentType << std::endl;
                    return nullptr;
                }
                

                shared<Mesh> mesh_instance = Mesh::create(mesh.name, vertices, indices);
                if (primitive.material >= 0) {
                    mesh_instance->setMaterial(materials[primitive.material]);
                }
                model->addMesh(mesh_instance);
            }
        }

        return model;
    }*/



    Vertex ModelLoader::parseVertex(const std::string& vertexString, const ModelData& objData) {
        // Use a stringstream to parse the vertex data
        std::stringstream ss(vertexString);
        std::string item;
        Vertex vertex;
        unsigned int index;

        // Extract the vertex index
        std::getline(ss, item, '/');
        index = std::stoul(item) - 1;
        vertex.position = objData.vertices[index];

        // Extract the texture coordinate index (if present)
        if (std::getline(ss, item, '/')) {
            index = std::stoul(item) - 1;
            vertex.texCoord = objData.texCoords[index];
        }

        // Extract the normal index (if present)
        if (std::getline(ss, item)) {
            index = std::stoul(item) - 1;
            vertex.normal = objData.normals[index];
        }

        return vertex;
    }




    // Parse an OBJ file and extract the data
    bool ModelLoader::parseOBJ(const std::string& file_path, Vertices& vertices, Indices& indices) {
        // Open the OBJ file
        std::ifstream infile(file_path, std::ios::binary);
        if (!infile) {
            Console::error("ModelLoader") << "Failed to open OBJ file: " << file_path << Console::endl;
            return false;
        }

        // Determine the file size
        infile.seekg(0, std::ios::end);
        std::streamsize file_size = infile.tellg();
        infile.seekg(0, std::ios::beg);

        // Read the entire file into a string
        std::string file_content(file_size, ' ');
        infile.read(&file_content[0], file_size);
        infile.close();

        // Temporary storage for the extracted data
        std::vector<glm::vec3> tempVertices;
        std::vector<glm::vec2> tempTexCoords;
        std::vector<glm::vec3> tempNormals;

        // Reserve space to avoid multiple reallocations
        tempVertices.reserve(1000);
        tempTexCoords.reserve(1000);
        tempNormals.reserve(1000);
        vertices.reserve(1000);
        indices.reserve(1000);

        std::istringstream file_stream(file_content);
        std::string line;
        while (std::getline(file_stream, line)) {
            if (line.empty() || line[0] == '#') {
                // Skip empty lines and comments
                continue;
            }

            // Check the prefix and process accordingly
            if (line.substr(0, 2) == "v ") {
                // Extract vertex data
                glm::vec3 vertex;
                std::sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                tempVertices.push_back(vertex);
            }
            else if (line.substr(0, 3) == "vt ") {
                // Extract texture coordinate data
                glm::vec2 texCoord;
                std::sscanf(line.c_str(), "vt %f %f", &texCoord.x, &texCoord.y);
                tempTexCoords.push_back(texCoord);
            }
            else if (line.substr(0, 3) == "vn ") {
                // Extract normal data
                glm::vec3 normal;
                std::sscanf(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);
                tempNormals.push_back(normal);
            }
            else if (line.substr(0, 2) == "f ") {
                // Extract face data
                unsigned int vertex_indices[3], texcoord_indices[3], normal_indices[3];
                int matches = std::sscanf(line.c_str(),
                    "f %u/%u/%u %u/%u/%u %u/%u/%u",
                    &vertex_indices[0], &texcoord_indices[0], &normal_indices[0],
                    &vertex_indices[1], &texcoord_indices[1], &normal_indices[1],
                    &vertex_indices[2], &texcoord_indices[2], &normal_indices[2]);

                if (matches != 9) {
                    Console::error("ModelLoader") << "Failed to parse face: " << line << Console::endl;
                    return false;
                }

                // Create vertices
                for (int i = 0; i < 3; i++) {
                    Vertex vertex;
                    vertex.position = tempVertices[vertex_indices[i] - 1];
                    vertex.texCoord = tempTexCoords[texcoord_indices[i] - 1];
                    vertex.normal = tempNormals[normal_indices[i] - 1];
                    vertices.push_back(vertex);
                    indices.push_back(indices.size());
                }
            }
        }

        return true;
    }

    bool ModelLoader::parseSTL(const std::string& filepath, Vertices& vertices, Indices& indices) {
        // Open the file for reading
        std::ifstream file(filepath, std::ios::binary);

        // Check if the file was successfully opened
        if (!file.is_open()) {
            Console::error("ModelLoader") << "File not found " << filepath << " or no read access " << Console::endl;
            return false;
        }

        // Read the first 80 bytes of the file (the size of the header in a binary STL file)
        char header[81];
        file.read(header, 80);
        header[80] = '\0';

        // Close the file
        file.close();

        // Check if the first 5 characters of the header are "solid"
        if (std::strncmp(header, "solid", 5) == 0) {
            // The file is an ASCII STL file
            return parseSTL_ASCII(filepath, vertices, indices);
        }
        else {
            // The file is a binary STL file
            return parseSTL_BINARY(filepath, vertices, indices);
        }
    }

    glm::vec3 computeFacetNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        // Uses p2 as a new origin for p1,p3
        auto u = p2 - p1;
        auto v = p3 - p1;

        auto x = u.y * v.z - u.z * v.y;
        auto y = u.z * v.x - u.x * v.z;
        auto z = u.x * v.y - u.y * v.x;
        return glm::normalize(glm::vec3(x, y, z));
    }

    float angleBetween( glm::vec3 a, glm::vec3 b) {
        glm::vec3 da = glm::normalize(a);
        glm::vec3 db = glm::normalize(b);

        if (da == db) return 0.0;

        return glm::acos(glm::dot(da, db));
    }

	// Parse an STL file and extract the data
	bool ModelLoader::parseSTL_BINARY(const std::string& file_path, Vertices& vertices, Indices& indices) {
        // Open the file in binary mode
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            // Failed to open the file
            return false;
        }

        // Read the header of the STL file
        uint8_t header[80];
        file.read((char*)&header, 80);

        // Read the number of triangles in the file
        uint32_t num_triangles;
        file.read((char*)&num_triangles, sizeof(num_triangles));

        // Reserve space in the vertex and index vectors
        vertices.reserve(num_triangles * 3);
        indices.reserve(num_triangles * 3);

        Console::info("ModelLoader") << "Loading Binary model..." << Console::endl;

        // Read the triangle data
        for (uint32_t i = 0; i < num_triangles; ++i) {
            // Read the normal vector
            float bn[3] = {0,0,0};
            file.read((char*)bn, sizeof(bn));

            // Read the three vertices
            float bv[9] = { 0,0,0 , 0,0,0, 0,0,0};
            file.read((char*)&bv, sizeof(bv));

            glm::vec3 v1, v2, v3;
            v1 = glm::vec3(bv[0], bv[1], bv[2]);
            v2 = glm::vec3(bv[3], bv[4], bv[5]);
            v3 = glm::vec3(bv[6], bv[7], bv[8]);

            glm::vec3 normal = computeFacetNormal(v1, v2, v3);

            Vertex vA, vB, vC;
            vA.position = v1;
            vB.position = v2; 
            vC.position = v3;
            vA.normal = normal;
            vB.normal = normal;
            vC.normal = normal;

            // Add the vertices to the vertex vector and update the indices
            unsigned int index = vertices.size();
            vertices.push_back(vA);
            vertices.push_back(vB);
            vertices.push_back(vC);
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);

            // Read the attribute byte count (not used)
            int attr_byte_count;
            file.read((char*)&attr_byte_count, 2);
        }

        return true;
	}

    // Parse an STL file and extract the data
    bool ModelLoader::parseSTL_ASCII(const std::string& file_path, Vertices& vertices, Indices& indices) {
        // Open the file in binary mode
        std::ifstream file(file_path, std::ios::in | std::ios::binary);
        if (!file) return false;
       
        // Reserve space for vertices and indices if you can estimate their size
        vertices.reserve(1024);
        indices.reserve(1024);

        std::vector<Facet> facets;
        facets.reserve(1024);

        Console::info("Model loader") << "Loading ascii model..." << Console::endl;


        // Read the file line by line
        std::string line; 
        Facet facetBuffer;

        float normal[3] = {0,0,0};

        int index = 0;
        while (std::getline(file, line)) {
            std::istringstream line_stream(line);
            std::string keyword;
            line_stream >> keyword;

            if (keyword[0] == 'f') { // facet
                // Read the normal vector
                std::string normal_keyword;
                line_stream >> normal_keyword;
                line_stream >> normal[0] >> normal[1] >> normal[2];
                
            }
            else if (keyword[0] == 'v') { // vertex
                Vertex v;
                v.color = glm::vec3(1);
                line_stream >> v.position.x >> v.position.y >> v.position.z;
                v.normal = glm::vec3(normal[0], normal[1], normal[2]);
                vertices.push_back(v);
                indices.push_back(index++);
   
            }
        }



        return true;
    }
	

	// Parse a any file and extract the data
	bool ModelLoader::parseMesh(const std::string& file_path, Vertices& vertices, Indices& indices) {
		FileType file_type = getFileType(file_path);
		switch (file_type) {
		case FileType::OBJ:
			return parseOBJ(file_path, vertices, indices);
            break;
		case FileType::STL:
			return parseSTL(file_path, vertices, indices);
            break;
		default:
			// Unknown file type
			return false;
            break;
		}
	}



}