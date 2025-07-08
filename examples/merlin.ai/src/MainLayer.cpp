#include "MainLayer.h"
#include "settings.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <GLFW/glfw3.h>


const bool real_scale = false;

const float radius = 3;

MainLayer::MainLayer() : camera_output(img_res, img_res) {
	camera().setNearPlane(2.0f);
	camera().setFarPlane(800);
	camera().setFOV(45); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0, 0, 150));

	glm::vec3 viewCenter = glm::vec3(150, 100, 0);
	camera().setView(CameraView::Top, glm::distance(glm::vec3(0), camera().getPosition()), viewCenter);

	glfwSwapInterval(0);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}

MainLayer::~MainLayer(){
	gym.stop();
}

void MainLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_INFO);

	min_size_= 20;
	max_size_= 50;

	default_props.name = "sample";
	default_props.comment = "Specimen 0";
	default_props.x_position = 0;
	default_props.y_position = 0;
	default_props.length = 30;
	default_props.width = 20;
	default_props.height = 0.15f * 10;
	default_props.layer_height = 0.2 * 10;
	default_props.line_width = 0.4 * 10;
	default_props.flow = 1.0f;
	default_props.retract = 1.0f;
	default_props.feedrate = 1050;

	createBuffers();
	sim.init();
	createShaders();
	sim.reset();
	createScene();
	
	createCamera();
	createGym();
	gym.start();
	createRandomSample();
	slice();
}

void MainLayer::onDetach(){
	Layer3D::onDetach();
	sim.stop();
	gym.stop();
}


void debugToolPath(std::vector<ToolPath>& vec) {
	Console::info("Vector") << " : " << Console::endl << "[";
	for (GLuint i = 0; i < ((vec.size() > 100) ? 100 : vec.size() - 1); i++) {
		Console::print() << vec[i].start << "|" << vec[i].start << ", ";
	}
	if (vec.size() > 100) Console::print() << "..., ";
	Console::print() << vec[vec.size() - 1].start << "|" << vec[vec.size() - 1].start << "]" << Console::endl << Console::endl;
}

void MainLayer::onUpdate(Timestep ts) {
	Layer3D::onUpdate(ts);
	
	std::lock_guard<std::mutex> lock(sim.mutex());

	GPU_PROFILE(render_time,

		if (real_scale) 	nozzle->setPosition(sim.getNozzlePosition() * 0.1f);
		else nozzle->setPosition(sim.getNozzlePosition());

		syncUniform();

		if (sim.isRunning()) {
			plotCutView();
			plotIsoSurface();
		}

		//MemoryManager::instance().resetBindings();
		
		renderer.clear();
		renderer.render(scene, camera());
		renderer.reset();

		gym.setCurrentImage(captureCurrentImage());
		//MemoryManager::instance().resetBindings();
	)

		need_sync = false;

	if (last_numParticle != settings.numParticles()) need_sync = true;

	if (!sim.hasReset()) {
		sim.reset();
		//createRandomGoal();
		//gym.setGoalImage(captureGoalImage());
	}

	if (!sim.hasPhaseChanged()) {
		sim.phase();
		createRandomSample();
		slice();

		gym.setGoalImage(captureGoalImage());
		gym.setGoalDepthImage(captureGoalDepthImage());
		gym.setCurrentImage(captureCurrentImage());
	}

	sim.api_step();
	sim.run(ts);
	MemoryManager::instance().resetBindings();
	
}




void MainLayer::slice(){
	slicer.clear();
	//for (auto& s : samples) {
	//	if (!s.enabled) continue;
	//	slicer.generateSample(s.getProperties());
	//}

	if(sample_obj.enabled)
		slicer.generateSample(sample_obj.getProperties());
	slicer.slice();

	toolpath_buffer->bind();
	auto& data = slicer.getToolPath();

	if (toolpath_buffer->elements() < data.size()) {
		toolpath_buffer->setElements(data.size());
		toolpath_buffer->allocateBuffer(data.size() * sizeof(ToolPath), data.data(), BufferUsage::StaticDraw);
	}
	else toolpath_buffer->writeBuffer(data.size() * sizeof(ToolPath), data.data());
	toolpath->setInstancesCount(data.size());
	current_layer = data.size();
	toolpath_buffer->unbind();

	sim.setGCode(slicer.get_gcode());
	//slicer.export_gcode("./samples.gcode");
}


void MainLayer::createCamera() {
	camera_fbo = FBO::create(img_res, img_res);

	camera_rbo = RBO::create();
	camera_rbo->reserve(img_res, img_res, GL_DEPTH24_STENCIL8);

	camera_texture = Texture2D::create(img_res, img_res, 4, 8, TextureType::ALBEDO);
	camera_texture->bind();
	camera_texture->setInterpolationMode(GL_LINEAR, GL_LINEAR);
	camera_texture->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	camera_texture->setBorderColor4f(1, 1, 1, 1);
	camera_texture->unbind();

	camera_fbo->bind();
	camera_texture->bind();
	camera_fbo->attachColorTexture(camera_texture);
	camera_rbo->bind();
	camera_fbo->attachDepthStencilRBO(camera_rbo);
	camera_fbo->unbind();
	camera_rbo->unbind();
	camera_texture->unbind();

	camera_goal_texture = Texture2D::create(img_res, img_res, 4, 8, TextureType::ALBEDO);
	camera_goal_texture->bind();
	camera_goal_texture->setInterpolationMode(GL_LINEAR, GL_LINEAR);
	camera_goal_texture->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	camera_goal_texture->setBorderColor4f(1, 1, 1, 1);
	camera_goal_texture->unbind();

	camera_output.setView(CameraView::Top, 100, glm::vec3(150, 100, 0));

	//camera_output(img_res, img_res)
	static float nearPlane = 95;
	static float farPlane = 110;

	camera_output.setNearPlane(nearPlane);
	camera_output.setFarPlane(farPlane);

}


void MainLayer::createSample(SampleProperty props) {
	sample_obj = SampleObject(props);
	if (sample_mesh)
		scene->remove(sample_mesh);

	sample_mesh = sample_obj.getMesh();

	if(sample_mesh)
		scene->add(sample_mesh);
}

void MainLayer::createRandomSample() {

	std::uniform_real_distribution<float> size_dist(min_size_, max_size_);
	float w = size_dist(rng_);
	float l = size_dist(rng_);

	float hw = w / 2.0f, hh = l / 2.0f;
	std::uniform_real_distribution<float> xpos(-50.0f + hw, 50.0f - hw);
	std::uniform_real_distribution<float> ypos(-50.0f + hh, 50.0f - hh);
	float cx = xpos(rng_) + 150, cy = ypos(rng_) + 100;

	std::uniform_real_distribution<float> ang_dist(0.0f, 360.0f);
	float angle_deg = ang_dist(rng_);

	SampleProperty props = default_props;
	
	props.width = w;
	props.length = l;
	
	props.x_position = cx;
	props.y_position = cy;
	props.rotation_z = glm::radians(angle_deg);

	if (sample_mesh) 
		scene->removeChild(sample_mesh);
	
	createSample(props);
}

void MainLayer::createBuffers(){
	auto& memory = MemoryManager::instance();
	//Particle buffers
	memory.createBuffer<glm::vec4>("last_position_buffer", settings.max_pThread);
	memory.createBuffer<glm::vec4>("position_buffer", settings.max_pThread);
	memory.createBuffer<glm::vec4>("predicted_position_buffer", settings.max_pThread);
	memory.createBuffer<glm::vec4>("correction_buffer", settings.max_pThread);
	memory.createBuffer<glm::vec4>("velocity_buffer", settings.max_pThread);
	memory.createBuffer<glm::vec2>("temperature_buffer", settings.max_pThread);
	memory.createBuffer<float>("density_buffer", settings.max_pThread);
	memory.createBuffer<float>("lambda_buffer", settings.max_pThread);
	memory.createBuffer<glm::uvec4>("meta_buffer", settings.max_pThread);
	memory.createBuffer<CopyContent>("copy_buffer", settings.max_pThread);

	//Particle Emitter buffers
	memory.createBuffer<glm::vec4>("emitter_position_buffer", 1);

	//Bin Buffer
	memory.createBuffer<Bin>("bin_buffer", settings.bThread);

	//Toolpath viz
	memory.createBuffer<ToolPath>("toolpath_buffer", 1);

}

void MainLayer::createScene() {
	renderer.initialize();
	renderer.enableSampleShading();
	//renderer.setEnvironmentGradientColor(1.0, 1.0, 1.0);
	renderer.setEnvironmentGradientColor(0.9, 0.9, 0.9);
	renderer.enableEnvironment();
	renderer.disableShadows();
	//renderer.showLights();
	renderer.disableFaceCulling();
	renderer.enableTransparency();

	scene = Scene::create("scene");

	auto& memory = MemoryManager::instance();

	/***********************
		Toolpath Viz
	************************/

	toolpath = ParticleSystem::create("toolpath", 1);
	toolpath->setDisplayMode(ParticleSystemDisplayMode::MESH);
	toolpath->setMesh(Primitives::createLine(1, glm::vec3(1, 0, 0)));
	toolpath_buffer = memory.getBuffer("toolpath_buffer");
	toolpath->setPositionBuffer(toolpath_buffer);
	toolpath->setShader(toolpath_shader);
	//toolpath->translate(glm::vec3(150, 100, 0));

	/***********************
		Scene decoration
	************************/

	origin = TransformObject::create("origin", 10);

	bed = ModelLoader::loadModel("./assets/models/bed.stl");
	bed->translate(glm::vec3(0.75, -0.25, 0));
	bed->setMaterial("chrome");

	bed_glass = ModelLoader::loadModel("./assets/models/glass.stl");
	bed_glass->setMaterial("glass");
	bed_glass->translate(glm::vec3(-8, -8, 5));
	bed_glass->scale(glm::vec3(0.98f, 0.98f, 1.0f));
	bed_glass->translate(glm::vec3(-150, -100, -5.2));
	bed->addChild(bed_glass);

	bed_surface = Model::create("floorSurface", Primitives::createRectangle(316, 216));
	bed_surface->translate(glm::vec3(0.75, -0.25, 0.3));
	bed_surface->scale(glm::vec3(0.98, 0.98, 1));
	bed_surface->scale(glm::vec3(1, -1, 1));

	shared<PhongMaterial> floorMat2 = createShared<PhongMaterial>("floorMat2");
	floorMat2->setAmbient(glm::vec3(0.02));
	floorMat2->setDiffuse(glm::vec3(0.95));
	floorMat2->setSpecular(glm::vec3(0.99));
	floorMat2->setShininess(0.95);
	floorMat2->loadTexture("assets/textures/bed.png", TextureType::DIFFUSE);

	bed_surface->setMaterial(floorMat2);
	bed->addChild(bed_surface);
	bed->translate(glm::vec3(150, 100, -5.2));

	nozzle = ModelLoader::loadMesh("./assets/models/nozzle.stl");
	nozzle->setMaterial("gold");
	//std::static_pointer_cast<PhongMaterial>(nozzle->getMaterial())->setAlphaBlending(0.1);
	//nozzle->smoothNormals();
	//nozzle->translate(glm::vec3(0, 0, 20*6));
	//nozzle->rotate(glm::vec3(180*DEG_TO_RAD, 0, 0));
	if(!real_scale) nozzle->scale(10);
	//nozzle->applyMeshTransform();
	//nozzle->enableWireFrameMode();
	if (real_scale) {
		nozzle->translate(glm::vec3(0, 0, -0.13));
	}
	nozzle->applyMeshTransform();


	/***********************
		Setup Textures
	************************/

	texture_debugXZ = Texture2D::create(settings.tex_size.x, settings.tex_size.z, 4, 16);
	texture_debugXZ->setUnit(0);
	texture_debugXY = Texture2D::create(settings.tex_size.x, settings.tex_size.y, 4, 16);
	texture_debugXY->setUnit(0);
	texture_debugYZ = Texture2D::create(settings.tex_size.y, settings.tex_size.z, 4, 16);
	texture_debugYZ->setUnit(0);



	/***********************
		Setup Iso-Surface
	************************/

	volume = Texture3D::create(settings.volume_size.x, settings.volume_size.y, settings.volume_size.z, 4, 16);
	volume->setUnit(0);
	isosurface = IsoSurface::create("isosurface", volume);
	isosurface->mesh()->setShader(isosurface_shader);
	isosurface->mesh()->translate(settings.bb * glm::vec3(0, 0, 0.5) + glm::vec3(150,100,0));
	isosurface->mesh()->scale(settings.bb * glm::vec3(1.0, 1.0, 0.5));
	if (real_scale) {
		isosurface->mesh()->scale(0.1);
		
	}
	

	/****************************
		Setup Particle System
	*****************************/

	ps = sim.getParticles();
	bs = sim.getBins();

	ps->setShader(particle_shader);
	bs->setShader(bin_shader);

	ps->setPositionBuffer(memory.getBuffer("position_buffer"));

	if (real_scale) {
		ps->scale(0.1);
		bs->scale(0.1);
	}

	/****************************
		Scene assembly
	*****************************/

	scene->add(ps);
	scene->add(bs);
	
	scene->add(nozzle);
	scene->add(isosurface);

	scene->add(bed);
	scene->add(origin);
	scene->add(toolpath);

	bs->hide();
	//toolpath->hide();
	isosurface->hide();

}

void MainLayer::createGym(){
	gym.setSim(&sim);
}

std::vector<uint8_t> MainLayer::captureCurrentImage(){
	bool particleHidden = ps->isHidden();
	bool nozzleHidden = nozzle->isHidden();
	bool isosurfaceHidden = isosurface->isHidden();
	bool toolpathHidden = toolpath->isHidden();
	nozzle->hide();
	isosurface->hide();
	toolpath->hide();
	ps->show();

	ps->setDisplayMode(ParticleSystemDisplayMode::MESH);

	particle_shader->use();
	particle_shader->setInt("colorCycle", 0);

	camera_texture->bind();
	renderer.renderTo(camera_fbo);
	renderer.activateTarget();
	renderer.clear();
	renderer.render(scene, camera_output);
	renderer.reset();

	int width = camera_texture->width();
	int height = camera_texture->height();
	
	std::vector<uint8_t> pixels(width * height * 3);

	camera_texture->bind();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
	camera_texture->unbind();
	
	ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
	if (!nozzleHidden) nozzle->show();
	if (!isosurfaceHidden) isosurface->show();
	if (!toolpathHidden) toolpath->show();
	if (particleHidden) ps->hide();

	particle_shader->use();
	particle_shader->setInt("colorCycle", colorMode);
	
	return pixels; // RGB format, 8-bit per channel
}

std::vector<uint8_t> MainLayer::captureGoalImage(){

	sample_mesh->show();
	bool nozzleHidden = nozzle->isHidden();
	nozzle->hide();

	renderer.renderTo(camera_fbo);
	renderer.activateTarget();
	renderer.clear();
	renderer.render(scene, camera_output);
	renderer.reset();


	int width = camera_texture->width();
	int height = camera_texture->height();


	glCopyImageSubData(camera_texture->id(), GL_TEXTURE_2D, 0, 0, 0, 0,
		camera_goal_texture->id(), GL_TEXTURE_2D, 0, 0, 0, 0,
		width, height, 1);



	std::vector<uint8_t> pixels(width * height * 3);

	camera_goal_texture->bind();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
	camera_goal_texture->unbind();


	sample_mesh->hide();
	if(!nozzleHidden) nozzle->show();

	return pixels; // RGB format, 8-bit per channel
}

std::vector<uint8_t> MainLayer::captureGoalDepthImage() {

	sample_mesh->show();
	sample_mesh->setRenderMode(RenderMode::DEPTH);
	bool nozzleHidden = nozzle->isHidden();
	nozzle->hide();

	renderer.renderTo(camera_fbo);
	renderer.activateTarget();
	renderer.clear();
	renderer.render(scene, camera_output);
	renderer.reset();


	int width = camera_texture->width();
	int height = camera_texture->height();


	glCopyImageSubData(camera_texture->id(), GL_TEXTURE_2D, 0, 0, 0, 0,
		camera_goal_texture->id(), GL_TEXTURE_2D, 0, 0, 0, 0,
		width, height, 1);



	std::vector<uint8_t> pixels(width * height * 3);

	camera_goal_texture->bind();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
	camera_goal_texture->unbind();


	sample_mesh->hide();
	sample_mesh->setRenderMode(RenderMode::LIT);
	if (!nozzleHidden) nozzle->show();

	return pixels; // RGB format, 8-bit per channel
}



void MainLayer::onImGuiRender() {
	//ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {

			if (ImGui::MenuItem("New Project", "Ctrl+N", false)) {
				newProject();
			}

			if (ImGui::MenuItem("Open Project", "Ctrl+O", false)) {
				importProject();
			}
			/*
			if (ImGui::BeginMenu("Recent Project")) {

				ImGui::MenuItem("...", "", false);

				ImGui::EndMenu();
			}
			*/
			if (ImGui::MenuItem("Save Project", "Ctrl+S", false)) {
				saveProject();
			}

			if (ImGui::MenuItem("Save Project as", "Ctrl+Alt+S", false)) {
				current_project_path = "";
				saveProject();
			}

			ImGui::Separator();
			if (ImGui::BeginMenu("Import")) {

				ImGui::MenuItem("Import Mesh", "Ctrl+I", false);

				//ImGui::MenuItem("Import Configuration", "", false);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Export")) {

				if (ImGui::MenuItem("Export G-Code..", "Ctrl+G", false)) {
					std::string gcode_path = Dialog::saveFileDialog(Dialog::FileType::DATA);
					if (gcode_path.size() != 0)
						slicer.export_gcode(gcode_path);
				}

				//ImGui::MenuItem("Send G-Code..", "Ctrl+Shift+G", false);

				//ImGui::MenuItem("Export G-code to SD-Card", "Ctrl+U", false);

				//ImGui::Separator();
				//ImGui::MenuItem("Export project as STL", "", false);

				//ImGui::Separator();
				//ImGui::MenuItem("Export Configuration", "", false);

				ImGui::EndMenu();
			}

			ImGui::Separator();
			/*
			if (ImGui::BeginMenu("Convert")) {

				ImGui::MenuItem("Convert ASCII G-code to Binary", "", false);

				ImGui::MenuItem("Convert Binary G-code to ASCII", "", false);

				ImGui::EndMenu();
			}*/

			ImGui::Separator();
			ImGui::MenuItem("Exit", "Alt+F4", false);

			ImGui::EndMenu();
		}

		/*
		if (ImGui::BeginMenu("Edit")) {

			ImGui::MenuItem("Select All", "Ctrl+A", false);

			ImGui::MenuItem("Deselect All", "Esc", false);

			ImGui::Separator();
			ImGui::MenuItem("Delete Selected", "Del", false);

			ImGui::Separator();
			ImGui::MenuItem("Delete All", "Ctrl+Del", false);

			ImGui::MenuItem("Undo", "Ctrl+Z", false);

			ImGui::MenuItem("Redo", "Ctrl+Y", false);

			ImGui::Separator();
			ImGui::MenuItem("Copy", "Ctrl+C", false);

			ImGui::MenuItem("Paste", "Ctrl+V", false);

			ImGui::EndMenu();
		}*/

		if (ImGui::BeginMenu("View")) {
			glm::vec3 viewCenter = glm::vec3(150, 100, 0);

			if (ImGui::MenuItem("Iso", "0", false)) camera().setView(CameraView::Iso, glm::distance(origin->position(), camera().getPosition()), viewCenter);

			ImGui::Separator();
			if (ImGui::MenuItem("Top", "1", false)) camera().setView(CameraView::Top, glm::distance(origin->position(), camera().getPosition()), viewCenter);

			if (ImGui::MenuItem("Bottom", "2", false)) camera().setView(CameraView::Bottom, glm::distance(origin->position(), camera().getPosition()), viewCenter);

			if (ImGui::MenuItem("Front", "3", false)) camera().setView(CameraView::Front, glm::distance(origin->position(), camera().getPosition()), viewCenter);

			if (ImGui::MenuItem("Rear", "4", false)) camera().setView(CameraView::Rear, glm::distance(origin->position(), camera().getPosition()), viewCenter);

			if (ImGui::MenuItem("Left", "5", false)) camera().setView(CameraView::Left, glm::distance(origin->position(), camera().getPosition()), viewCenter);

			if (ImGui::MenuItem("Right", "6", false)) camera().setView(CameraView::Right, glm::distance(origin->position(), camera().getPosition()), viewCenter);

			//ImGui::Separator();
			//if (ImGui::MenuItem("FullScreen", "F11", false)) camera().setView(CameraView::Iso, viewCenter, glm::distance(origin->position(), camera().getPosition()));

			ImGui::EndMenu();
		}

		ImGui::MenuItem("Configuration", "", false);

		if (ImGui::BeginMenu("Help")) {

			ImGui::MenuItem("About", "", false);

			ImGui::MenuItem("Report an Issue", "", false);

			ImGui::MenuItem("Documentation", "", false);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}



	ImGui::Begin("General");

	ImGui::LabelText("FPS", std::to_string(fps()).c_str());
	ImGui::LabelText(std::to_string(settings.numParticles()).c_str(), "particles");
	ImGui::LabelText(std::to_string(settings.bThread).c_str(), "bins");

	if (ImGui::TreeNode("Statistics")) {
		ImGui::LabelText("FPS", std::to_string(fps()).c_str());

		ImGui::Text("Nearest Neighbor Search %.1f ms", nns_time);
		ImGui::Text("Substep solver %.1f ms", solver_substep_time - nns_time);
		ImGui::Text("Jacobi iteration %.1f ms", jacobi_time);
		ImGui::Text("Total physics time %.1f ms", solver_total_time);
		ImGui::Text("Render time %.1f ms", render_time);
		ImGui::Text("Total frame time %.1f ms", total_time);
		ImGui::TreePop();
	}

	ImGui::End();



	ImGui::Begin("Simulation");

	if (sim.isRunning()) {
		if (ImGui::SmallButton("Stop simulation")) {
			sim.stop();
		}
	}
	else {
		if (ImGui::ArrowButton("Pause simulation", 1)) {
			sim.start();
		}
	}

	if (ImGui::SmallButton("Reset simulation")) {
		sim.api_reset();
	}

	ImGui::DragFloat("Flow override", &settings.flow_override, 1.0, 0.0f, 100.0f);

	if (ImGui::TreeNode("Graphics"))
	{
		static bool transparency = true;
		if (ImGui::Checkbox("Particle transparency", &transparency)) {
			if (transparency) ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
			else ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE);
		}

		ImGui::Checkbox("Show 2D plot", &use_2Dplot);

		if (ImGui::Checkbox("Show Isosurface", &use_isosurface)) {
			if (use_isosurface) isosurface->show();
			else isosurface->hide();
		}

		static bool Nozzlestate = true;
		if (ImGui::Checkbox("Show Nozzle", &Nozzlestate)) {
			if (Nozzlestate) nozzle->show();
			else nozzle->hide();
		}

		static bool Pstate = true;
		if (ImGui::Checkbox("Show Particles", &Pstate)) {
			if (Pstate) ps->show();
			else ps->hide();
		}

		static bool Bstate = false;
		if (ImGui::Checkbox("Show Bins", &Bstate)) {
			if (Bstate) bs->show();
			else bs->hide();
		}

		static bool BBstate = false;
		if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
			particle_shader->use();
			particle_shader->setInt("showBoundary", BBstate);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Physics")) {

		//ImGui::SliderFloat("Emitter delay (ms)", &settings.emitterDelay, 0.0, 100.0f);
		ImGui::Checkbox("Use emitter", &settings.use_emitter);
		ImGui::Checkbox("Use emitter init", &settings.use_emitter_init);

		ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
		ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 200);


		ImGui::InputFloat("Time step", &settings.timestep, 0.0, 0.02f);


		if (ImGui::SliderFloat("Fluid particle mass", &settings.particleMass.value(), 0.1, 2.0)) {
			solver->use();
			settings.particleMass.sync(*solver);
		}
		if (ImGui::SliderFloat("Rest density", &settings.restDensity.value(), 0.0, 2.0)) {
			solver->use();
			settings.restDensity.sync(*solver);
			particle_shader->use();
			settings.restDensity.sync(*particle_shader);
		}

		static float artificialPressureMultiplier = settings.artificialPressureMultiplier.value() * 1000.0;
		if (ImGui::SliderFloat("Pressure multiplier", &artificialPressureMultiplier, 0.0, 10.0)) {
			settings.artificialPressureMultiplier.value() = artificialPressureMultiplier * 0.001;
			solver->use();
			settings.artificialPressureMultiplier.sync(*solver);
		}

		static float artificialViscosityMultiplier = settings.artificialViscosityMultiplier.value() * 100.0;
		if (ImGui::SliderFloat("XPSH Viscosity", &artificialViscosityMultiplier, 0.0, 200.0)) {
			settings.artificialViscosityMultiplier.value() = artificialViscosityMultiplier * 0.01;
			solver->use();
			settings.artificialViscosityMultiplier.sync(*solver);
		}

		if (ImGui::SliderFloat("Viscosity", &settings.viscosity.value(), 0.0, 10.0)) {
			solver->use();
			solver->setFloat("viscosity", settings.viscosity.value());
		}


		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Vizualisation"))
	{
		static const char* options[] = { "Solid color", "Bin index", "Density", "Temperature", "Velocity", "Mass", "Neighbors" };
		if (ImGui::ListBox("Colored field", &colorMode, options, 7)) {
			particle_shader->use();
			particle_shader->setInt("colorCycle", colorMode);
			bin_shader->use();
			bin_shader->setInt("colorCycle", colorMode);
			texPlot->use();
			texPlot->setInt("colorMode", colorMode);
			isosurface_shader->use();
			isosurface_shader->setInt("colorMode", colorMode);
		}

		static int particleTest = 50;
		static int binTest = 1459;
		if (colorMode == 6) {
			if (ImGui::DragInt("Particle to test", &particleTest)) {
				particle_shader->use();
				particle_shader->setUInt("particleTest", particleTest);
				bin_shader->use();
				bin_shader->setUInt("particleTest", particleTest);
			}

			ImGui::LabelText("Bin selector", "");

			bool changed = false;

			ImGui::LabelText("Current Bin", std::to_string(binTest).c_str());

			if (ImGui::SmallButton("X+")) { binTest++; changed = true; }
			if (ImGui::SmallButton("X-")) { binTest--; changed = true; }
			if (ImGui::SmallButton("Y+")) { binTest += (settings.bb.x / settings.bWidth); changed = true; }
			if (ImGui::SmallButton("Y-")) { binTest -= (settings.bb.x / settings.bWidth); changed = true; }
			if (ImGui::SmallButton("Z+")) { binTest += int(settings.bb.y / settings.bWidth) * int(settings.bb.x / settings.bWidth); changed = true; }
			if (ImGui::SmallButton("Z-")) { binTest -= int(settings.bb.y / settings.bWidth) * int(settings.bb.x / settings.bWidth); changed = true; }

			if (changed) {
				bin_shader->use();
				bin_shader->setUInt("binTest", binTest);
			}
		}
		ImGui::TreePop();
	}

	ImGui::End();



	ImGui::Begin("ColorMap");
	static int colorMap = 4;
	static const char* optionsMap[] = { "inferno", "blackbody", "plasma", "viridis", "warmcool", "parula", "jet" };
	if (ImGui::ListBox("ColorMap field", &colorMap, optionsMap, 7)) {
		texPlot->use();
		texPlot->setInt("colorMapSelection", colorMap);
		isosurface_shader->use();
		isosurface_shader->setInt("colorMapSelection", colorMap);
	}
	ImGui::End();



	ImGui::Begin("Slicer");
	if (ImGui::Button("Slice")) {
		slice();
	}


	if (ImGui::Checkbox("Show Rapid Toolpath", &showG0)) {
		toolpath_shader->use();
		toolpath_shader->setInt("showG0", showG0);
	}

	static int colorMode = 1;
	static const char* options[] = { "Tool index", "Feedrate", "Temperature" };
	if (ImGui::ListBox("Colored field", &colorMode, options, 3)) {
		toolpath_shader->use();
		toolpath_shader->setInt("colorMode", colorMode);
	}

	ImGui::End();




	if (use_2Dplot) {
		static bool first_frame = true;
		static int debug_layer = 0;
		float scale = 0.2;


		ImGui::Begin("Debug XY");
		ImGui::Image((void*)(intptr_t)texture_debugXY->id(), ImVec2(settings.tex_size.x * scale, settings.tex_size.y * scale), ImVec2(1, 1), ImVec2(0, 0));
		ImGui::End();

		ImGui::Begin("Debug XZ");
		ImGui::Image((void*)(intptr_t)texture_debugXZ->id(), ImVec2(settings.tex_size.x * scale, settings.tex_size.z * scale), ImVec2(1, 1), ImVec2(0, 0));
		ImGui::End();

		ImGui::Begin("Debug YZ");
		ImGui::SliderFloat("Layer", &offsetPlane, -40, 40);
		if (ImGui::Button("Save picture")) {
			std::string savePath = Dialog::saveFileDialog(Dialog::FileType::IMAGE);
			if (!savePath.size() == 0) {
				texture_debugXY->exportPNG(savePath + "XY");
				texture_debugXZ->exportPNG(savePath + "XZ");
				texture_debugYZ->exportPNG(savePath + "YZ");
			}
		}





		ImGui::Image((void*)(intptr_t)texture_debugYZ->id(), ImVec2(settings.tex_size.y * scale, settings.tex_size.z * scale), ImVec2(1, 1), ImVec2(0, 0));

		ImGui::End();
	}

	
	ImGui::Begin("Camera");

	camera_texture->bind();
	ImGui::Image((void*)(intptr_t)camera_texture->id(), ImVec2(img_res,img_res), ImVec2(0, 1), ImVec2(1, 0));
	camera_texture->unbind();

	//camera_output(img_res, img_res)
	static float nearPlane = 95;
	static float farPlane = 110;

	bool state = false;
	state = ImGui::DragFloat("Near plane", &nearPlane, 50);
	state |= ImGui::DragFloat("Far plane", &farPlane, 300);
	

	if (state) {
		camera_output.setNearPlane(nearPlane);
		camera_output.setFarPlane(farPlane);
	}
	ImGui::End();

		
	ImGui::Begin("Goal Image");
	camera_goal_texture->bind();
	ImGui::Image((void*)(intptr_t)camera_goal_texture->id(), ImVec2(img_res,img_res), ImVec2(0, 1), ImVec2(1, 0));
	camera_goal_texture->unbind();

	if (ImGui::Button("New Goal")) {
		createRandomSample();
		gym.setGoalImage(captureGoalImage());
	}

	ImGui::End();





	static int selected_sample_index = -1;
	static int node_clicked = -1;
	static shared<RenderableObject> selected_renderable = nullptr;  // Track selected 3D scene object


	ImGui::Begin("Sample List");

	if (ImGui::Button("New Sample")) {
		/*
		if (selected_sample_index != -1 && selected_sample_index < samples.size()) {
			createSample(samples[selected_sample_index].getProperties());
		}
		else 
		*/
		createRandomSample();
	}

	/*
	if (ImGui::Button("Remove Selected Sample")) {
		if (selected_sample_index != -1 && selected_sample_index < samples.size()) {
			samples.erase(samples.begin() + selected_sample_index);
			selected_sample_index = -1;  // Reset selection after removal
			ImGui::End();
			return;
		}
	}*/

	for (int i = 0; i < 1 /*samples.size()*/; i++) {

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

		if (selected_sample_index == i)
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string label = "Sample " + std::to_string(i);
		bool open = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", label.c_str());


		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			selected_sample_index = i;
			selected_renderable = nullptr;
		}

		if (open) {

			ImGui::TreePop();
		}
	}

	ImGui::End();

	// 3D Scene Graph selection logic
	std::function<void(const std::list<shared<RenderableObject>>&)> traverseNodes = [&](const std::list<shared<RenderableObject>>& nodes) {
		for (auto& node : nodes) {
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (!node) continue;
			if (selected_renderable == node) {
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool node_open = ImGui::TreeNodeEx(node->name().c_str(), node_flags);

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
				selected_renderable = node;
			}

			if (node_open) {
				traverseNodes(node->children());
				ImGui::TreePop();
			}
		}
		};

	// Draw the scene graph starting from the root node
	ImGui::Begin("3D Scene Graph");
	traverseNodes(scene->nodes());
	ImGui::End();

	ImGui::Begin("Layers");

	//if (ImGui::VSliderInt("layer", ImVec2(30, 1000), &current_layer, 0, slicer.getLayerCount())) {
		//toolpath_shader->use();
		//toolpath_shader->setInt("layer", current_layer);
	//}
	ImGui::End();

	// Properties panel (shows either physics or graphics properties)
	ImGui::Begin("Properties");
	if (selected_renderable) {
		selected_renderable->onRenderMenu();  // Assume `drawProperties()` exists for graphics properties
	}else if(selected_sample_index != -1){
		//samples[selected_sample_index].renderMenu();  // Render sample properties
		if (sample_obj.renderMenu()) {  // Render sample properties
			scene->remove(sample_mesh);
			sample_mesh = sample_obj.getMesh();
			scene->add(sample_mesh);
		}
	}
	ImGui::End();





}

void MainLayer::saveProject(){
	if (current_project_path == "") {
		current_project_path = Dialog::saveFileDialog(Dialog::FileType::DATA);
	}
	if (current_project_path.size() == 0) return;


	std::ofstream file(current_project_path);
	if (!file.is_open()) {
		std::cerr << "Failed to open file for XML export: " << current_project_path << std::endl;
		return;
	}

	file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	file << "<Samples>\n";

	//for (auto& s : samples) {
		//file << s.toXML();
	//}
	file << sample_obj.toXML();  // Render sample properties

	file << "</Samples>\n";
	file.close();

}

void MainLayer::importProject() {
	using namespace tinyxml2;

	int answer = Dialog::messageBox("Save current project", "Discard", Dialog::MessageType::YES_NO_CANCEL);
	/* 0 for cancel/no , 1 for ok/yes , 2 for no in yesnocancel */
	if (answer == 0) return;
	if (answer == 1) saveProject();
	if (answer == 2) ;//just import

	std::string path = Dialog::openFileDialog(Dialog::FileType::DATA);

	XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != XML_SUCCESS) {
		std::cerr << "Failed to load XML file: " << path << std::endl;
		return;
	}

	//samples.clear(); // optional: reset current project

	XMLElement* root = doc.FirstChildElement("Samples");
	if (!root) {
		std::cerr << "Invalid XML format (missing <Samples>)" << std::endl;
		return;
	}

	//samples.clear();

	for (XMLElement* elem = root->FirstChildElement("Sample"); elem != nullptr; elem = elem->NextSiblingElement("Sample")) {
		SampleProperty s = SampleObject::fromXML(elem);
		createSample(s);
	}
}


void MainLayer::createShaders() {

	solver = sim.getSolver();

	particle_shader = Shader::create("particle", "assets/shaders/particle.vert", "assets/shaders/particle.frag", "", false);
	settings.setConstants(*particle_shader);
	particle_shader->compile();
	particle_shader->use();
	particle_shader->setVec3("lightPos", glm::vec3(0, -200, 1000));
	particle_shader->setInt("colorCycle", 4);

	bin_shader = Shader::create("bins", "assets/shaders/bin.vert", "assets/shaders/bin.frag", "", false);
	settings.setConstants(*bin_shader);
	bin_shader->compile();
	bin_shader->use();
	bin_shader->setInt("colorCycle", 4);

	renderer.addShader(particle_shader);
	renderer.addShader(bin_shader);

	isoGen = ComputeShader::create("isoGen", "assets/shaders/solver/isoGen.comp", false);
	settings.setConstants(*isoGen);
	isoGen->compile();
	isoGen->setWorkgroupLayout(settings.iWkgCount);

	texPlot = ComputeShader::create("texPlot", "assets/shaders/solver/texturePlot.comp", false);
	settings.setConstants(*texPlot);
	texPlot->compile();

	isosurface_shader = Shader::create("isosurface", "assets/shaders/isosurface.vert", "assets/shaders/isosurface.frag", "", false);
	isosurface_shader->supportShadows(false);
	isosurface_shader->supportMaterial(false);
	isosurface_shader->supportLights(false);
	isosurface_shader->supportEnvironment(false);
	isosurface_shader->supportTexture(false);
	settings.setConstants(*isosurface_shader);
	isosurface_shader->compile();

	toolpath_shader = Shader::create("toolpathShader", "./assets/shaders/toolpath.vert", "./assets/shaders/toolpath.frag");
	toolpath_shader->supportEnvironment(false);
	toolpath_shader->supportLights(false);
	toolpath_shader->supportMaterial(false);
	toolpath_shader->supportShadows(false);
	toolpath_shader->supportTexture(false);


}

void MainLayer::syncUniform(){
	if (need_sync) {
		
		particle_shader->use();
		settings.numParticles.sync(*particle_shader);
		settings.restDensity.sync(*particle_shader);

		isosurface_shader->use();
		settings.numParticles.sync(*isosurface_shader);
		settings.particleMass.sync(*isosurface_shader);
	}
}

void MainLayer::newProject() {
	using namespace tinyxml2;

	int answer = Dialog::messageBox("Save current project", "Discard", Dialog::MessageType::YES_NO_CANCEL);
	/* 0 for cancel/no , 1 for ok/yes , 2 for no in yesnocancel */
	if (answer == 0) return;
	if (answer == 1) saveProject();
	if (answer == 2);//just import

	//samples.clear(); // optional: reset current project
}

void MainLayer::plotIsoSurface(){

	if (use_isosurface) {
		if (isosurface->isHidden()) isosurface->show();
		isoGen->bindBuffer();
		volume->bindImage(0);
		isoGen->use();
		
		if (need_sync) {
			settings.numParticles.sync(*isoGen);
			settings.particleMass.sync(*isoGen);
		}

		isoGen->dispatch();
		isoGen->barrier(GL_ALL_BARRIER_BITS);
		isosurface->setIsoLevel(0.5);
		isosurface->compute();
	}else if (!isosurface->isHidden()) 
		isosurface->hide();

}

void MainLayer::plotCutView(){
	if (use_2Dplot) {
		texPlot->bindBuffer();
		texPlot->use();

		settings.numParticles.sync(*texPlot);
		settings.particleMass.sync(*texPlot);

		plotXY();
		plotXZ();
		plotYZ();
	}
}

void MainLayer::plotXY() {
	int x = (settings.tex_size.x - 1) / settings.texWkgSize.x;
	int y = (settings.tex_size.y - 1) / settings.texWkgSize.y;

	texture_debugXY->bindImage(0);
	texPlot->setFloat("offsetPlane", offsetPlane);
	texPlot->setVec3("offset", glm::vec3(150,100,0));
	texPlot->setVec3("axis", glm::vec3(1, 1, 0));
	texPlot->dispatch(x, y);
	texPlot->barrier(GL_ALL_BARRIER_BITS);

}

void MainLayer::plotXZ() {
	int x = (settings.tex_size.x - 1) / settings.texWkgSize.x;
	int z = (settings.tex_size.z - 1) / settings.texWkgSize.z;

	texture_debugXZ->bindImage(0);
	texPlot->setFloat("offsetPlane", offsetPlane);
	texPlot->setVec3("axis", glm::vec3(1, 0, 1));
	texPlot->setVec3("offset", glm::vec3(150, 100, 0));
	texPlot->dispatch(x, z);
	texPlot->barrier(GL_ALL_BARRIER_BITS);
}

void MainLayer::plotYZ() {
	int y = (settings.tex_size.y - 1) / settings.texWkgSize.y;
	int z = (settings.tex_size.z - 1) / settings.texWkgSize.z;

	texture_debugYZ->bindImage(0);

	texPlot->setFloat("offsetPlane", offsetPlane);
	texPlot->setVec3("axis", glm::vec3(0, 1, 1));
	texPlot->setVec3("offset", glm::vec3(150, 100, 0));
	texPlot->dispatch(y, z);
	texPlot->barrier(GL_ALL_BARRIER_BITS);
}