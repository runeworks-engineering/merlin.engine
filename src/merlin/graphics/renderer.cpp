#include "pch.h"
#include "merlin/core/core.h"
#include "merlin/graphics/renderer.h"




namespace Merlin {

	void Renderer::gatherLights(const shared<RenderableObject>& object) {
		if (const auto light = std::dynamic_pointer_cast<Light>(object)) {
			light->applyRenderTransform(m_currentTransform);
			m_activeLights.push_back(light);
		}
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			BoundingBox bb = mesh->getBoundingBox();
			

			// Ajouter les sommets de la bounding box
			std::vector<glm::vec3> corners = {
				bb.min,
				glm::vec3(bb.min.x, bb.min.y, bb.max.z),
				glm::vec3(bb.min.x, bb.max.y, bb.min.z),
				glm::vec3(bb.min.x, bb.max.y, bb.max.z),
				glm::vec3(bb.max.x, bb.min.y, bb.min.z),
				glm::vec3(bb.max.x, bb.min.y, bb.max.z),
				glm::vec3(bb.max.x, bb.max.y, bb.min.z),
				bb.max
			};

			for (const auto& corner : corners) {
				addPoint(corner);
			}

		}

		for (const auto& child : object->children()) {
			if (!child->isHidden()) gatherLights(child);
		}
	}

	void Renderer::renderScene(const Scene& scene, const Camera& camera) {
		if (debug)Console::info() << "Rendering scene" << Console::endl;

		//Gather lights
		for (const auto& node : scene.nodes()) {
			if (!node->isHidden()) gatherLights(node);
		}

		if(debug)Console::info() << "Rendering scene shadows" << Console::endl;

		bool hasLights = m_activeLights.size() != 0;
		if (!hasLights || use_default_light) {
			m_activeLights.push_back(m_defaultAmbient);
			m_activeLights.push_back(m_defaultDirLight);
			m_activeLights.push_back(m_defaultDirLight2);
			m_activeLights.push_back(m_defaultDirLight3);
		}

		if (use_shadows) {
			if (useFaceCulling()) glDisable(GL_CULL_FACE);
			for (const auto& light : m_activeLights) {
				if (!light->castShadow()) continue;
				castShadow(light, scene);
			}
			if (useFaceCulling()) glEnable(GL_CULL_FACE);
		}

		camera.restoreViewport();
		activateTarget();
		
		

		//Render the scene
		if (debug)Console::info() << "Rendering scene objects" << Console::endl;
		
		transparent_pass = false;

		// Render opaque objects first
		enableDepthTest();
		//enableDepthWrite();
		disableTransparency();

		for (const auto& node : scene.nodes()) {
			render(node, camera);
		}

		// Now render transparent objects
		transparent_pass = true;

		//disableDepthWrite();     // Avoid writing to depth buffer
		enableTransparency();    // Enable blending

		// Ideally, sort transparent nodes back-to-front here

		for (const auto& node : scene.nodes()) {
			render(node, camera); // Should only render if node is transparent
		}

		enableDepthWrite();      // Re-enable depth writes for future frames

		//Render default lights
		if (display_lights && (!hasLights || use_default_light)) {
			render(m_defaultAmbient, camera);
			render(m_defaultDirLight, camera);
			render(m_defaultDirLight2, camera);
			render(m_defaultDirLight3, camera);
		}

		if (scene.hasEnvironment()) {
			m_currentEnvironment = scene.getEnvironment();
			renderEnvironment(*m_currentEnvironment, camera);
		}
		else {
			renderEnvironment(*m_defaultEnvironment, camera);
		}
	}


	void Renderer::renderEnvironment(const Environment& env, const Camera& camera){
		if(debug)Console::info() << "Rendering Environment" << Console::endl;
		if (!use_environment) return;
		shared<Shader> shader = getShader("default.skybox");
		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}
		glDepthFunc(GL_LEQUAL);
		if(useFaceCulling())glDisable(GL_CULL_FACE);
		shader->use();
		Texture2D::resetTextureUnits();
		env.attach(*shader);
		shader->setVec3("environment.ambient", env.gradientColor());
		shader->setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix()))); //sync model matrix with GPU
		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		env.draw();
		if (useFaceCulling())glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		env.detach();
	}



	void Renderer::renderDepth(const shared<RenderableObject>& object, shared<Shader> shader){
		if (debug)Console::info() << "Rendering Depth" << Console::endl;
		pushMatrix();
		m_currentTransform *= object->transform();

		//The object is a mesh
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			if (mesh->castShadow()) {
				shader->setMat4("model", m_currentTransform); //sync model matrix with GPU
				mesh->draw();
			}
		}for (auto node : object->children()) {
			renderDepth(node, shader);//Propagate to childrens
		}

		popMatrix();
	}

	void Renderer::renderLight(const Light& li, const Camera& camera){
		if (debug)Console::info() << "Rendering Light" << Console::endl;
		shared<Shader> shader = getShader("default.light");
		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}
		//Texture2D::resetTextureUnits();
		shader->use();
		shader->setVec3("light_color", li.diffuse() + li.ambient() + li.specular());

		shader->setMat4("model", m_currentTransform); //sync model matrix with GPU

		shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		li.draw();
	}

	void Renderer::renderMesh(const Mesh& mesh, const Camera& camera) {
		if (debug)Console::info() << "Rendering Mesh" << Console::endl;
		Material_Ptr mat = nullptr;
		Shader_Ptr shader = nullptr;

		if (mesh.hasMaterial()) mat = mesh.getMaterial();
		else mat = getMaterial(mesh.getMaterialName());

		//render transparent object
		if (mat->isTransparent() && !transparent_pass) {
			return;
		}
		if (!mat->isTransparent() && transparent_pass) {
			return;
		}

		if (mesh.hasShader())
			shader = mesh.getShader();
		else {
			if (mesh.getShaderName() == "default") {
				if (mat->type() == MaterialType::PHONG) shader = getShader("default.phong");
				else if (mat->type() == MaterialType::PBR) shader = getShader("default.pbr");
				else {
					Console::error("Renderer") << "This material has no suitable shader. Please bind it manually" << Console::endl;
				}
			}
			else shader = getShader(mesh.getShaderName());
		}

		if (!shader) {
			Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
			return;
		}

		Texture2D::resetTextureUnits();
		shader->use();

		if (shader->name() == "default.phong" || shader->name() == "default.pbr") {
			switch (m_renderMode) {
			case RenderMode::UNLIT:
				shader->setInt("renderMode", 0);
				break;
			case RenderMode::LIT:
				shader->setInt("renderMode", 1);
				break;
			case RenderMode::NORMALS:
				shader->setInt("renderMode", 2);
				break;
			case RenderMode::DEPTH:
				shader->setInt("renderMode", 3);
				shader->setFloat("camera_near", camera.nearPlane());
				shader->setFloat("camera_far", camera.farPlane());
				break;
			case RenderMode::POSITION:
				shader->setInt("renderMode", 4);
				break;
			case RenderMode::TEXCOORDS:
				shader->setInt("renderMode", 5);
				break;
			case RenderMode::SHADOW:
				shader->setInt("renderMode", 6);
				break;
			default:
				break;
			}
		}

		if (shader->supportLights()) {
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->attach(i, *shader);
			}
		}

		mat->attach(*shader);

		if(m_currentEnvironment != nullptr)
			m_currentEnvironment->attach(*shader);
		else m_defaultEnvironment->attach(*shader);

		if (shader->supportLights()) shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
		shader->setMat4("model", m_currentTransform); //sync model matrix with GPU
		shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
		if(shader->supportLights()) shader->setInt("use_flat_shading", mesh.useFlatShading());
		if(shader->supportShadows()) shader->setInt("useShadows", use_shadows);
		
		

		if (shader->supportShadows()) shader->setInt("swap_normals", mesh.hasInvertedNormals());
		if (shader->supportMaterial()) shader->setInt("use_normal_map", mesh.useNormalMap());

		shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
		if (shader->supportLights()) shader->setInt("numLights", m_activeLights.size());

		if (shader->supportMaterial()) shader->setInt("use_vertex_color", mesh.useVertexColors());

		mesh.draw();
		mat->detach();

		if (shader->supportEnvironment()) {
			if (m_currentEnvironment != nullptr)
				m_currentEnvironment->detach();
			else m_defaultEnvironment->detach();
		}

		if(shader->supportLights()) {
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->detach();
			}
		}
	}


	void Renderer::castShadow(shared<Light> light, const Scene& scene) {
		if (debug)Console::info() << "Cast Shadow" << Console::endl;
		shared<FrameBuffer> fbo;
		shared<Shader> shader;

		if (light->type() == LightType::Directional || light->type() == LightType::Spot) shader = getShader("shadow.depth");
		else if (light->type() == LightType::Point) shader = getShader("shadow.omni");
		else return;

		fbo = light->shadowFBO();

		if (!shader || !fbo) {
			Console::error("Renderer") << "Renderer failed to gather ressoureces for shadows (shader, framebuffer or texture)" << Console::endl;
			return;
		}

		glViewport(0, 0, light->shadowResolution(), light->shadowResolution());
		fbo->bind();
		
		glClear(GL_DEPTH_BUFFER_BIT);

		Texture2D::resetTextureUnits();
		shader->use();
		light->attachShadow(*shader, getScenePoints());
		
		for (const auto& node : scene.nodes()) {
			if (!node->isHidden()) {
				renderDepth(node, shader);
			};
		}
		fbo->unbind();
	}


	
	void Renderer::renderParticleSystem(const ParticleSystem& ps, const Camera& camera) {
		if (debug)Console::info() << "Rendering Particle System" << Console::endl;
		if (ps.getDisplayMode() != ParticleSystemDisplayMode::MESH) {
			Shader_Ptr shader = nullptr;

			if (ps.hasShader())
				shader = ps.getShader();
			else {
				if (ps.getShaderName() == "default") {
					shader = getShader("instanced.sprite");
				}
				else shader = getShader(ps.getShaderName());
			}

			if (!shader) {
				Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
				return;
			}

			if (ps.hasPositionBuffer() && !shader->hasBuffer(ps.getPositionBuffer()->name())) {
				shader->attach(ps.getPositionBuffer());
			}

			shader->use();
			shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
			shader->setMat4("model", m_currentTransform); //sync model matrix with GPU
			shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
			shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU

			
			
			ps.draw();

		} else {
			Mesh& mesh = *ps.getMesh();
			Material_Ptr mat = mesh.getMaterial();
			Shader_Ptr shader;

			if (ps.hasShader()) shader = ps.getShader();
			else shader = getShader("instanced.phong");
			if (mesh.hasMaterial()) mat = mesh.getMaterial();
			else mat = getMaterial(mesh.getMaterialName());

			if (!shader) {
				Console::error("Renderer") << "Renderer failed to gather materials and shaders" << Console::endl;
				return;
			}

			Texture2D::resetTextureUnits();
			shader->use();

			if(shader->supportLights())
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->attach(i, *shader);
			}

			mat->attach(*shader);

			if (shader->supportEnvironment()) {
				if (m_currentEnvironment != nullptr)
					m_currentEnvironment->attach(*shader);
				else m_defaultEnvironment->attach(*shader);
			}

			if (shader->supportLights()) shader->setVec3("viewPos", camera.getPosition()); //sync model matrix with GPU
			shader->setMat4("model", m_currentTransform); //sync model matrix with GPU
			shader->setMat4("view", camera.getViewMatrix()); //sync model matrix with GPU
			if (shader->supportShadows())shader->setInt("useShadows", use_shadows);

			shader->setMat4("projection", camera.getProjectionMatrix()); //sync model matrix with GPU
			if (shader->supportLights()) shader->setInt("numLights", m_activeLights.size());

			if (ps.hasPositionBuffer() && !shader->hasBuffer(ps.getPositionBuffer()->name())) {
				AbstractBufferObject_Ptr pos = ps.getPositionBuffer();
				shader->attach(pos);
			}

			ps.draw();
			mat->detach();

			if (shader->supportEnvironment()) {
				if (m_currentEnvironment != nullptr)
					m_currentEnvironment->detach();
				else m_defaultEnvironment->detach();
			}

			if (shader->supportLights())
			for (int i = 0; i < m_activeLights.size(); i++) {
				m_activeLights[i]->detach();
			}
		}
	}

	void Renderer::renderTransformObject(const TransformObject& obj, const Camera& camera) {
		if (debug)Console::info() << "Rendering TransformObject" << Console::endl;
		render(obj.getXAxisModel(), camera);
		render(obj.getYAxisModel(), camera);
		render(obj.getZAxisModel(), camera);
		render(obj.getSphereMesh(), camera);
	}

	void Renderer::render(const shared<RenderableObject>& object, const Camera& camera) {
		if (object->isHidden()) return;

		if(m_renderModeOverride == RenderMode::DEFAULT)
			m_renderMode = object->renderMode();
		else
			m_renderMode = m_renderModeOverride;

		pushMatrix();
		m_currentTransform *= object->transform();

		if (object->isWireFrame()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//The object is a mesh
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			renderMesh(*mesh, camera);
		}//The object is a model
		else if (const auto ps = std::dynamic_pointer_cast<ParticleSystem>(object)) {
			renderParticleSystem(*ps, camera);
		}
		else if (const auto iso = std::dynamic_pointer_cast<IsoSurface>(object)) {
			render(iso->mesh(), camera);
		}
		else if (const auto li = std::dynamic_pointer_cast<Light>(object)) {
			if(display_lights) renderLight(*li, camera);
		}
		else if (const auto scene = std::dynamic_pointer_cast<Scene>(object)) {
			renderScene(*scene, camera); //Propagate to childrens
		}//The object is a scene node
		else if (const auto ps = std::dynamic_pointer_cast<TransformObject>(object)) {
			renderTransformObject(*ps, camera); //Propagate to childrens
		}

		for (auto node : object->children()) {
			render(node, camera);//Propagate to childrens
		}

		popMatrix();
	}


}