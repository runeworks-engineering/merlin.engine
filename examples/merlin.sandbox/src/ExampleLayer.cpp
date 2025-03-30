#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>

const float radius = 3;

ExampleLayer::ExampleLayer(){
	camera().setNearPlane(0.1f);
	camera().setFarPlane(1000.0f);
	camera().setFOV(75); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0.7, -7, 2.4));
	camera().setRotation(glm::vec3(0, 0, +90));
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::createScene() {
	renderer.initialize();
	renderer.enableSampleShading();
	//renderer.setEnvironmentGradientColor(1.0, 1.0, 1.0);
	renderer.setEnvironmentGradientColor(0.5, 0.5, 0.5);
	renderer.enableEnvironment();
	renderer.disableShadows();
	//renderer.showLights();
	renderer.disableFaceCulling();
	//renderer.enableTransparency();

	//renderer.setRenderMode(RenderMode::UNLIT);

	domain = Primitives::createHollowCube(100, 0.1);
	domain->setMaterial("white rubber");
	domain->setRenderMode(RenderMode::UNLIT);

	//fluid = Primitives::createCube(20);
	fluid = ModelLoader::loadMesh("./assets/common/models/bunny.stl");
	fluid->centerMeshOrigin();
	fluid->scale(5);
	fluid->computeBoundingBox();

	fluid_boundingbox = Primitives::createBoundingBox(fluid->getBoundingBox());
	fluid_boundingbox->setMaterial("white rubber");
	fluid_boundingbox->setRenderMode(RenderMode::UNLIT);

	scene = Scene::create("scene");

	scene->add(domain);
	//scene->add(fluid);
	scene->add(fluid_boundingbox);
	//scene->add(TransformObject::create("origin", 10));
}

void ExampleLayer::createPhysics(){
	ParticleSampler_Ptr sampler = createShared<VoxelSampler>(1);
	PhysicsEntity_Ptr fluid_entity = createShared<PhysicsEntity>("fluid", sampler);
	fluid_entity->setMesh(fluid);

	PhysicsModifier_Ptr fluid_modifer = createShared<FluidModifier>();
	fluid_entity->addModifier(fluid_modifer);

	solver.add(fluid_entity);
	solver.setDomain(domain->getBoundingBox());
	solver.init();

	Console::info("ExampleLayer") << "Particles: " << solver.getParticles()->count() << Console::endl;

	scene->add(solver.getParticles());
	//scene->add(solver.getBins());
}

void ExampleLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_INFO);

	createScene();
	createPhysics();
}

void ExampleLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);

	solver.attachGraphics();
	renderer.clear();
	renderer.render(scene, camera());
	renderer.reset();
	solver.detachGraphics();

	if (!paused) {
		solver.update(ts);
	}
}


void ExampleLayer::onImGuiRender(){
	//ImGui::ShowDemoWindow();

	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");

	ImGui::LabelText(std::to_string(solver.getParticles()->count()).c_str(), "particles");
	ImGui::LabelText(std::to_string(solver.getBins()->count()).c_str(), "bins");
	ImGui::LabelText("FPS", std::to_string(fps()).c_str());

	ImGui::LabelText("time", std::to_string(solver.getTime()).c_str());

	if (paused) {
		if (ImGui::ArrowButton("Run simulation", 1)) {
			paused = !paused;
		}
	}
	else {
		if (ImGui::SmallButton("Pause simulation")) {
			paused = !paused;
		}
	}
	/*
	if (ImGui::Checkbox("Show Isosurface", &use_isosurface)) {
		if (use_isosurface) isosurface->show();
		else isosurface->hide();
	}
	*/

	if (ImGui::SmallButton("Reset simulation")) {
		solver.reset();
	}

	if (ImGui::SmallButton("Restart simulation")) {
		ImGui::OpenPopup("Confirmation");

		bool open = true;
		if (ImGui::BeginPopupModal("Confirmation", &open)){
			ImGui::Text("Are you sure you want to restart ?");
			ImGui::Text("This will erase the simulation data and re-allocate buffers");
			ImGui::Text("This may take a while...");
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		solver.init();
	}

	static bool Pstate = true;
	if (ImGui::Checkbox("Show Particles", &Pstate)) {
		if (Pstate) solver.getParticles()->show();
		else solver.getParticles()->hide();
	}

	static bool Bstate = false;
	if (ImGui::Checkbox("Show Bins", &Bstate)) {
		if (Bstate) solver.getBins()->show();
		else solver.getBins()->hide();
	}
	/*
	static bool BBstate = false;
	if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
		particleShader->use();
		particleShader->setInt("showBoundary", BBstate);
	}
	/**/

	/*
	ImGui::DragInt("Solver substep", &settings.solver_substep, 1, 1, 200);
	ImGui::DragInt("Solver iteration", &settings.solver_iteration, 1, 1, 200);

	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera().setPosition(model_matrix_translation);
	}

	if (ImGui::InputFloat("Time step", &settings.timestep, 0.0, 0.02f)) {
		solver->use();
		solver->setFloat("dt", settings.timestep);
	}
	
	static float prev_time = settings.timestep;
	if (ImGui::Checkbox("Real Time", &use_real_time)) {
		if (!use_real_time) settings.timestep = prev_time;
		else prev_time = settings.timestep;
	}

	ImGui::SliderFloat("Emitter delay (ms)", &settings.emitterDelay, 0.0, 100.0f);
	ImGui::Checkbox("Use emitter", &use_emitter);


	if (ImGui::SliderFloat("Fluid particle mass", &settings.particleMass.value(), 0.1, 2.0)) {
		solver->use();
		settings.particleMass.sync(*solver);
	}
	if (ImGui::SliderFloat("Rest density", &settings.restDensity.value(), 0.0, 2.0)) {
		solver->use();
		settings.restDensity.sync(*solver);
		particleShader->use();
		settings.restDensity.sync(*particleShader);
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

	if (ImGui::SliderFloat("Viscosity", &settings.viscosity.value(), 0.0, 100.0)) {
		solver->use();
		solver->setFloat("viscosity", settings.viscosity.value());
	}
	
	static int colorMode = 4;
	static const char* options[] = { "Solid color", "Bin index", "Density", "Temperature", "Velocity", "Mass", "Neighbors" };
	if (ImGui::ListBox("Colored field", &colorMode, options, 7)) {
		particleShader->use();
		particleShader->setInt("colorCycle", colorMode);
		binShader->use();
		binShader->setInt("colorCycle", colorMode);
	}

	static int particleTest = 50;
	static int binTest = 1459;
	if (colorMode == 6) {
		if (ImGui::DragInt("Particle to test", &particleTest)) {
			particleShader->use();
			particleShader->setUInt("particleTest", particleTest);
			binShader->use();
			binShader->setUInt("particleTest", particleTest);
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
			binShader->use();
			binShader->setUInt("binTest", binTest);
		}
	}
	*/
	ImGui::End();



	/*
	ImGui::Begin("Performance");
	ImGui::Text("Nearest Neighbor Search %.1f ms", nns_time);
	ImGui::Text("Substep solver %.1f ms", solver_substep_time - nns_time);
	ImGui::Text("Jacobi iteration %.1f ms", jacobi_time);
	ImGui::Text("Total physics time %.1f ms", solver_total_time);
	ImGui::Text("Render time %.1f ms", render_time);
	ImGui::Text("Total frame time %.1f ms", total_time);
	ImGui::End();
	*/


	/*
	ImGui::Begin("Compute Shader");
	ImGui::LabelText("Solver", "");
	static int stepSolver = 0;
	ImGui::SliderInt("Solver Step", &stepSolver, 0.0, 5.0f);
	if (ImGui::Button("Execute Solver")) {
		solver->use();
		solver->execute(stepSolver);
	}

	ImGui::LabelText("PrefixSum", "");
	static int stepPrefix = 0;
	ImGui::LabelText("Solver", "");
	ImGui::SliderInt("PrefixSum Step", &stepPrefix, 0.0, 3.0f);
	if (ImGui::Button("Execute PrefixSum Step")) {
		prefixSum->use();
		prefixSum->execute(stepPrefix);
	}

	if (ImGui::Button("Step Simulation")) {
		GPU_PROFILE(solver_total_time,
			Simulate(0.016);
			)
	}

	static bool first_frame = true;
	static int debug_layer = 0;

	ImGui::Begin("Debug");
	if (ImGui::SliderInt("Layer", (int*)&debug_layer, 0, settings.volume_size.z - 1) || first_frame)
	{
		// Copy one layer of the 3D volume texture into the "debug" 2D texture for display
		glCopyImageSubData(volume->id(), GL_TEXTURE_3D, 0, 0, 0, debug_layer,
			texture_debug->id(), GL_TEXTURE_2D, 0, 0, 0, 0,
			settings.volume_size.x, settings.volume_size.y, 1);

		first_frame = false;
	}
	ImGui::Image((void*)(intptr_t)texture_debug->id(), ImVec2(settings.volume_size.x, settings.volume_size.y), ImVec2(1, 1), ImVec2(0, 0));
	ImGui::End();


	ImGui::Begin("Debug Laser");
	laser_map->bind();

	ImGui::Image((void*)(intptr_t)laser_map->id(), ImVec2(settings.volume_size.x, settings.volume_size.y), ImVec2(1, 1), ImVec2(0, 0));
	ImGui::End();


	if (ImGui::Button("Debug")) {
		throw("DEBUG");
		Console::info() << "DEBUG" << Console::endl;
	}
	ImGui::End();
	*/



	static int node_clicked = -1;
	static PhysicsEntity_Ptr selected_entity = nullptr;
	static shared<RenderableObject> selected_renderable = nullptr;  // Track selected 3D scene object

	ImGui::Begin("Physics Scene Graph");
	std::vector<PhysicsEntity_Ptr> entities = solver.getEntities();
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	for (int i = 0; i < entities.size(); i++) {
		PhysicsEntity_Ptr entity = entities[i];
		ImGuiTreeNodeFlags node_flags = base_flags;

		// Ensure selection is persistent
		if (node_clicked == i) {
			node_flags |= ImGuiTreeNodeFlags_Selected;
		}

		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, entity->name().c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			node_clicked = i;
			selected_entity = entity;
			selected_renderable = nullptr; // Deselect renderable objects when selecting physics entities
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
				selected_entity = nullptr;
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
	if (selected_entity) {
		selected_entity->onRenderMenu();  // Render physics properties
	}else if (selected_renderable) {
		selected_renderable->onRenderMenu();  // Assume `drawProperties()` exists for graphics properties
	}
	ImGui::End();
	
	ImGui::Begin("Solver Properties");
	solver.onRenderMenu();
	ImGui::End();

}
