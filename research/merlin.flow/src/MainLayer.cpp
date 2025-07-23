#include "MainLayer.h"
#include "settings.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <GLFW/glfw3.h>

const bool real_scale = false;

bool finidshed = false;
float array_plan[4] = { 25, 50, 75, 100 };
int index = -1;
float viscosity = 50.0f; //Viscosity of the fluid

MainLayer::MainLayer() {
	camera().setNearPlane(2.0f);
	camera().setFarPlane(800);
	camera().setFOV(45); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0, 0, 150));

	glm::vec3 viewCenter = glm::vec3(0, 0, 0);
	camera().setView(CameraView::Iso, glm::distance(glm::vec3(0), camera().getPosition()), viewCenter);

	glfwSwapInterval(0);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}

MainLayer::~MainLayer(){}

void MainLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_INFO);

	settings.viscosity = viscosity;
	settings.artificialViscosityMultiplier = 60 * 0.01f;

	createBuffers();
	sim.init();
	createShaders();
	createScene();
	createGCode();
	controller.reset();
	sim.reset();


	//controller.setNozzlePosition(glm::vec3(0, 0, 25));
	//sim.setNozzlePosition(glm::vec3(0, 0, 25));		
}

void MainLayer::onDetach(){
	Layer3D::onDetach();
	sim.stop();
}

void MainLayer::onUpdate(Timestep ts) {
	Layer3D::onUpdate(ts);



	GPU_PROFILE(render_time,
		syncUniform();

		if (sim.isRunning()) {
			plotCutView();
			plotIsoSurface();
		}
		
		renderer.clear();
		renderer.render(scene, camera());
		renderer.reset();
	)

	need_sync = false;

	if (last_numParticle != settings.numParticles()) need_sync = true;

	if (!finidshed && sim.isRunning()) {
		controller.update(settings.timestep);
		nozzle->setPosition(controller.getNozzlePosition());
		sim.setNozzlePosition(controller.getNozzlePosition());
		sim.setFlowrate(controller.getFlowrate());

		sim.step(settings.timestep);
		MemoryManager::instance().resetBindings();

		if (controller.lastCommandReached()) {
			if (use_2Dplot) {
				std::string savePath = "./visco" + std::to_string(array_plan[index]);
				texture_debugXY->exportPNG(savePath + "XY.png");
				texture_debugXZ->exportPNG(savePath + "XZ.png");
				texture_debugYZ->exportPNG(savePath + "YZ.png");
			}


			//viscosity = array_plan[index];
			//settings.viscosity = viscosity;
			//settings.artificialViscosityMultiplier = viscosity * 0.01f;
			createGCode();
		}
	}
}


void MainLayer::createGCode(){
	//settings.setTimestep(settings.max_timestep /= float(index+1));
	settings.flow_override = 1.0f;
	controller.clear();
	index++;
	if (index >= 4) {
		finidshed = true;
		return;
	}
	float speed = array_plan[index];
	controller.move(glm::vec3(-80, -80, 10), 0, 100);
	controller.move(glm::vec3(0, -80, 10), 300, speed);
	controller.move(glm::vec3(0, 80, 10), 300, speed);

	controller.reset();
	sim.reset();
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
	//renderer.enableTransparency();

	scene = Scene::create("scene");

	auto& memory = MemoryManager::instance();

	/***********************
		Scene decoration
	************************/

	origin = TransformObject::create("origin", 10);
	origin->setPosition(glm::vec3(-150, -100, 0));

	bed = ModelLoader::loadModel("./assets/models/bed.stl");
	bed->translate(glm::vec3(0.75, -0.25, 0));
	bed->setMaterial("chrome");

	bed_glass = ModelLoader::loadModel("./assets/models/glass.stl");
	bed_glass->setMaterial("glass");
	//bed_glass->setMaterial("black rubber");
	bed_glass->translate(glm::vec3(-8, -8, 5));
	bed_glass->scale(glm::vec3(0.98f, 0.98f, 1.0f));
	bed_glass->translate(glm::vec3(-150, -100, -5.2));
	bed->addChild(bed_glass);

	bed_surface = Model::create("floorSurface", Primitives::createRectangle(316, 216));
	bed_surface->translate(glm::vec3(0.75, -0.25, 0.3));
	bed_surface->scale(glm::vec3(0.98, -0.98, 1));

	shared<PhongMaterial> floorMat2 = createShared<PhongMaterial>("floorMat2");
	floorMat2->setAmbient(glm::vec3(0.02));
	floorMat2->setDiffuse(glm::vec3(0.95));
	floorMat2->setSpecular(glm::vec3(0.99));
	floorMat2->setShininess(0.95);
	floorMat2->loadTexture("assets/textures/bed.png", TextureType::DIFFUSE);

	bed_surface->setMaterial(floorMat2);
	bed->addChild(bed_surface);
	bed->translate(glm::vec3(0, 0, -5.2));

	nozzle = ModelLoader::loadMesh("./assets/models/nozzle.stl");
	//nozzle->setMaterial("glass");
	nozzle->setMaterial("gold");
	if(!real_scale) nozzle->scale(10);

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
	isosurface->mesh()->translate(settings.bb * glm::vec3(0, 0, 0.5));
	isosurface->mesh()->scale(settings.bb * glm::vec3(1.0, 1.0, 0.5));
	

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

	bs->hide();
	//toolpath->hide();
	isosurface->hide();

}

void MainLayer::onImGuiRender() {
	//ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);

	if (ImGui::BeginMainMenuBar()) {
		ImGui::BeginDisabled();
		if (ImGui::BeginMenu("File")) {

			
			if (ImGui::MenuItem("New Project", "Ctrl+N", false)) {
			}

			if (ImGui::MenuItem("Open Project", "Ctrl+O", false)) {
			}

			if (ImGui::BeginMenu("Recent Project")) {

				ImGui::MenuItem("...", "", false);
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Save Project", "Ctrl+S", false)) {
			}

			if (ImGui::MenuItem("Save Project as", "Ctrl+Alt+S", false)) {
			}

			ImGui::Separator();
			if (ImGui::BeginMenu("Import")) {

				ImGui::MenuItem("Import Mesh", "Ctrl+I", false);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Export")) {

				if (ImGui::MenuItem("Export G-Code..", "Ctrl+G", false)) {
				}

				ImGui::MenuItem("Send G-Code..", "Ctrl+Shift+G", false);

				ImGui::MenuItem("Export G-code to SD-Card", "Ctrl+U", false);

				ImGui::Separator();
				ImGui::MenuItem("Export project as STL", "", false);

				ImGui::Separator();
				ImGui::MenuItem("Export Configuration", "", false);

				ImGui::EndMenu();
			}

			ImGui::Separator();
			
			if (ImGui::BeginMenu("Convert")) {

				ImGui::MenuItem("Convert ASCII G-code to Binary", "", false);

				ImGui::MenuItem("Convert Binary G-code to ASCII", "", false);

				ImGui::EndMenu();
			}

			ImGui::Separator();
			ImGui::MenuItem("Exit", "Alt+F4", false);

			ImGui::EndMenu();
		}
		
		
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
		}
		ImGui::EndDisabled();

		if (ImGui::BeginMenu("View")) {
			glm::vec3 viewCenter = glm::vec3(0, 0, 0);

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
		sim.reset();
		controller.reset();
		index = -1;
		finidshed = false;
		createGCode();
		
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
		ImGui::DragInt("Pressure Solver iteration", &settings.pressure_iteration, 1, 1, 50);
		ImGui::DragInt("Visco Solver iteration", &settings.visco_iteration, 1, 1, 50);

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

		float artificialViscosityMultiplier = settings.artificialViscosityMultiplier.value() * 100.0;
		if (ImGui::SliderFloat("XPSH Viscosity", &artificialViscosityMultiplier, 0.0, 200.0)) {
			settings.artificialViscosityMultiplier = artificialViscosityMultiplier * 0.01;
			solver->use();
			settings.artificialViscosityMultiplier.sync(*solver);
		}

		if (ImGui::SliderFloat("Viscosity", &settings.viscosity.value(), 0.0, 100.0)) {
			solver->use();
			solver->setFloat("u_viscosity", settings.viscosity.value());
		}


		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Vizualisation"))
	{	
		static int colorMode = 4;
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

	static int selected_sample_index = -1;
	static int node_clicked = -1;
	static shared<RenderableObject> selected_renderable = nullptr;  // Track selected 3D scene object

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

	// Properties panel (shows either physics or graphics properties)
	ImGui::Begin("Properties");
	if (selected_renderable) {
		selected_renderable->onRenderMenu();  // Assume `drawProperties()` exists for graphics properties
	}
	ImGui::End();

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
	texPlot->setVec3("offset", glm::vec3(0,0,0));
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
	texPlot->setVec3("offset", glm::vec3(0, 0, 0));
	texPlot->dispatch(x, z);
	texPlot->barrier(GL_ALL_BARRIER_BITS);
}

void MainLayer::plotYZ() {
	int y = (settings.tex_size.y - 1) / settings.texWkgSize.y;
	int z = (settings.tex_size.z - 1) / settings.texWkgSize.z;

	texture_debugYZ->bindImage(0);

	texPlot->setFloat("offsetPlane", offsetPlane);
	texPlot->setVec3("axis", glm::vec3(0, 1, 1));
	texPlot->setVec3("offset", glm::vec3(0, 0, 0));
	texPlot->dispatch(y, z);
	texPlot->barrier(GL_ALL_BARRIER_BITS);
}