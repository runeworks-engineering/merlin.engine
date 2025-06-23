#include "MainLayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <string>
#include <GLFW/glfw3.h>

const float radius = 3;

MainLayer::MainLayer(){
	camera().setNearPlane(2.0f);
	camera().setFarPlane(600);
	camera().setFOV(45); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0, -200, 50));
	camera().setRotation(glm::vec3(0, 0, +90));

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}

MainLayer::~MainLayer(){}

void MainLayer::createScene() {
	renderer.initialize();
	renderer.enableSampleShading();
	//renderer.setEnvironmentGradientColor(1.0, 1.0, 1.0);
	renderer.setEnvironmentGradientColor(0.5, 0.5, 0.5);
	renderer.enableEnvironment();
	renderer.disableShadows();
	//renderer.showLights();
	renderer.disableFaceCulling();
	renderer.enableTransparency();

	scene = Scene::create("scene");

	default_props.name = "Sample 0";
	default_props.comment = "Specimen 0";
	default_props.x_offset = 230.0f;
	default_props.y_offset = 120.0f;
	default_props.width = 30.0f;
	default_props.height = 3.0f;
	default_props.thickness = 4.0f;
	default_props.layer_height = 0.2;
	default_props.line_width = 0.4;
	default_props.tool_a = 0;
	default_props.tool_b = 1;
	default_props.flow_a = 0.9f;
	default_props.flow_b = 1.4f;
	default_props.retract_a = 1.0f;
	default_props.retract_b = 1.0f;
	default_props.feedrate_a = 1050;
	default_props.feedrate_b = 1050;
	default_props.temperature_a = 220.0f;
	default_props.temperature_b = 230.0f;
	default_props.overlap = 2;
	default_props.overlap_flow_modifier = 1.3f;
	default_props.use_purge_tower = false;
	default_props.use_alternate_sweep = true;
	default_props.use_in_to_out = true;

	default_props.name = "Sample 0";
	default_props.comment = "Specimen 0";
	default_props.x_offset = 150;
	default_props.y_offset = 100;
	default_props.overlap = 2;
	default_props.feedrate_a = 600;
	default_props.feedrate_b = 600;

	createSample(default_props);

	default_props.name = "Sample 1";
	default_props.comment = "Specimen 1";
	default_props.x_offset += -60;
	default_props.y_offset += -60;
	default_props.overlap = 1;
	default_props.feedrate_a = 300;
	default_props.feedrate_b = 300;

	createSample(default_props);

	default_props.name = "Sample 2";
	default_props.comment = "Specimen 2";
	default_props.x_offset += +120;
	default_props.y_offset += 0;
	default_props.overlap = 1;
	default_props.feedrate_a = 900;
	default_props.feedrate_b = 900;

	createSample(default_props);

	default_props.name = "Sample 3";
	default_props.comment = "Specimen 2";
	default_props.x_offset += 0;
	default_props.y_offset += 120;
	default_props.overlap = 3;
	default_props.feedrate_a = 900;
	default_props.feedrate_b = 900;

	createSample(default_props);

	default_props.name = "Sample 4";
	default_props.comment = "Specimen 2";
	default_props.x_offset += -120;
	default_props.y_offset += 0;
	default_props.overlap = 3;
	default_props.feedrate_a = 300;
	default_props.feedrate_b = 300;

	createSample(default_props);

	/***********************
		Toolpath Viz
	************************/

	toolpath = ParticleSystem::create("toolpath");
	toolpath->setDisplayMode(ParticleSystemDisplayMode::MESH);
	toolpath->setMesh(Primitives::createLine(1, glm::vec3(1,0,0)));
	toolpath_shader = Shader::create("toolpathShader", "./assets/shaders/toolpath.vert", "./assets/shaders/toolpath.frag");
	toolpath_shader->supportEnvironment(false);
	toolpath_shader->supportLights(false);
	toolpath_shader->supportMaterial(false);
	toolpath_shader->supportShadows(false);
	toolpath_shader->supportTexture(false);

	toolpath_buffer = SSBO<ToolPath>::create("toolpath_buffer");
	toolpath_shader->attach(toolpath_buffer);

	toolpath->setShader(toolpath_shader);
	toolpath->translate(glm::vec3(150, 100, 0));

	/***********************
		Scene decoration
	************************/

	bed = ModelLoader::loadModel("./assets/models/bed.stl");
	bed->translate(glm::vec3(0.75, -0.25, 0));
	//bed->scale(glm::vec3(1.025, 1.025, 1.0));
	bed->setMaterial("chrome");
	//bed->setShader("debug.normals");
	bed->translate(glm::vec3(150, 100, -5.2));

	bed_glass = ModelLoader::loadModel("./assets/models/glass.stl");
	bed_glass->setMaterial("water");
	bed_glass->translate(glm::vec3(-4, -4, -5));
	bed_glass->scale(glm::vec3(0.98f, 0.98f, 1.0f));
	
	//modelShader->Use();
	//modelShader->setVec3("lightPos", glm::vec3(0.0, 10.0, 10));

	bed_surface = Model::create("floorSurface", Primitives::createRectangle(316, 216));
	bed_surface->translate(glm::vec3(0.75, -0.25, 0));
	bed_surface->scale(glm::vec3(1, -1, 1));
	//bed_surface->scale(glm::vec3(1.0, 1.0, 1.0));
	bed_surface->translate(glm::vec3(150, -100, -5.1));

	shared<PhongMaterial> floorMat2 = createShared<PhongMaterial>("floorMat2");
	floorMat2->setAmbient(glm::vec3(0.02));
	floorMat2->setDiffuse(glm::vec3(0.95));
	floorMat2->setSpecular(glm::vec3(0.99));
	floorMat2->setShininess(0.95);
	floorMat2->setAlphaBlending(0.8);
	floorMat2->loadTexture("assets/textures/bed.png", TextureType::DIFFUSE);
	bed_surface->setMaterial(floorMat2);
	
	scene->add(bed);
	scene->add(bed_surface);
	scene->add(bed_glass);
	scene->add(toolpath);

	origin = TransformObject::create("origin", 10);
	scene->add(origin);
}

void MainLayer::slice(){
	slicer.clear();
	for (auto& s : samples) {
		slicer.generateSample(s.getProperties());
	}
	slicer.export_gcode("./samples.gcode");
}


void MainLayer::createSample(Sample props){
	samples.emplace_back(props);
}


void MainLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_INFO);

	createScene();
}

void MainLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);

	scene->clear();

	scene->add(bed);
	scene->add(bed_surface);
	scene->add(bed_glass);
	scene->add(origin);
	scene->add(toolpath);

	for (auto& s : samples) {
		scene->add(s.getMeshA());
		scene->add(s.getMeshB());
	}

	renderer.clear();
	renderer.render(scene, camera());
	renderer.reset();

}


void MainLayer::onImGuiRender(){
	//ImGui::ShowDemoWindow();

	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");

	ImGui::LabelText("FPS", std::to_string(fps()).c_str());

	if (ImGui::Button("Slice")) {
		slice();
	}

	ImGui::End();

	static int selected_sample_index = -1;

	ImGui::Begin("Sample List");

	if (ImGui::Button("Create Sample")) {
		if (selected_sample_index != -1 && selected_sample_index < samples.size()) {
			createSample(samples[selected_sample_index].getProperties());
		}
		else createSample(default_props);
	}

	if (ImGui::Button("Remove Selected Sample")) {
		if (selected_sample_index != -1 && selected_sample_index < samples.size()) {
			samples.erase(samples.begin() + selected_sample_index);
			selected_sample_index = -1;  // Reset selection after removal
			ImGui::End();
			return;
		}
	}

	for (int i = 0; i < samples.size(); i++) {

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

		if (selected_sample_index == i)
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string label = "Sample " + std::to_string(i);
		bool open = ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", label.c_str());


		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			selected_sample_index = i;
		}

		if (open) {

			ImGui::TreePop();
		}
	}

	ImGui::End();

	/*
	if (ImGui::Checkbox("Show Isosurface", &use_isosurface)) {
		if (use_isosurface) isosurface->show();
		else isosurface->hide();
	}
	*/

	/*
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

		//User OK, do stuff here
	}*/

	/*
	static bool Pstate = true;
	if (ImGui::Checkbox("Show Particles", &Pstate)) {
		if (Pstate) solver.getParticles()->show();
		else solver.getParticles()->hide();
	}

	static bool Bstate = false;
	if (ImGui::Checkbox("Show Bins", &Bstate)) {
		if (Bstate) solver.getBins()->show();
		else solver.getBins()->hide();
	}*/


	/*
	static bool BBstate = false;
	if (ImGui::Checkbox("Show Boundaries", &BBstate)) {
		particleShader->use();
		particleShader->setInt("showBoundary", BBstate);
	}
	/**/

	


	static int node_clicked = -1;
	static PhysicsEntity_Ptr selected_entity = nullptr;
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
	}else if(selected_sample_index != -1){
		samples[selected_sample_index].renderMenu();  // Render sample properties
	}
	ImGui::End();


}

SampleObject::SampleObject(const Sample& props){

	this->props = props;

	static int i = 0;
	mesh_A = Primitives::createBox(props.width, props.thickness*0.5, props.height);
	mesh_A->rename("s_" + std::to_string(i) + "PLA");
	mesh_A->translate(glm::vec3(0, props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
	mesh_A->setMaterial("black plastic");  //PLA


	mesh_B = Primitives::createBox(props.width, props.thickness*0.5, props.height);
	mesh_B->rename("s_" + std::to_string(i) + "TPU");
	mesh_B->translate(glm::vec3(0, -props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
	mesh_B->setMaterial("cyan plastic");  //TPU

	i++;

}

void SampleObject::renderMenu(){
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGui::Text((std::string("name : ") + props.name).c_str());

	bool changed = false;

	ImGui::Checkbox("Enabled", &enabled);

	changed |= ImGui::DragFloat("X Offset", &props.x_offset);
	changed |= ImGui::DragFloat("Y Offset", &props.y_offset);
	changed |= ImGui::DragFloat("Width", &props.width);
	changed |= ImGui::DragFloat("Height", &props.height);
	changed |= ImGui::DragFloat("Thickness", &props.thickness);

	ImGui::InputInt("Tool A", &props.tool_a);
	ImGui::InputInt("Tool B", &props.tool_b);

	ImGui::InputFloat("Flow A", &props.flow_a);
	ImGui::InputFloat("Flow B", &props.flow_b);

	ImGui::InputFloat("Retract A", &props.retract_a);
	ImGui::InputFloat("Retract B", &props.retract_b);

	ImGui::InputFloat("Feedrate A", &props.feedrate_a);
	ImGui::InputFloat("Feedrate B", &props.feedrate_b);

	ImGui::InputFloat("Line Width", &props.line_width);

	ImGui::InputFloat("Temperature A", &props.temperature_a);
	ImGui::InputFloat("Temperature B", &props.temperature_b);

	ImGui::InputInt("Overlap lines", &props.overlap);
	ImGui::InputFloat("Overlap Flow Modifier", &props.overlap_flow_modifier);

	ImGui::Checkbox("Use Purge Tower", &props.use_purge_tower);
	ImGui::Checkbox("Use Alternate Sweep", &props.use_alternate_sweep);
	ImGui::Checkbox("Use In to Out", &props.use_in_to_out);

	char buffer[256];
	strncpy(buffer, props.comment.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	if (ImGui::InputText("Comment", buffer, sizeof(buffer))) {
		props.comment = buffer;
	}

	if (ImGui::Button("Apply changes") || changed) {
		std::string name_A = mesh_A->name();
		mesh_A = Primitives::createBox(props.width, props.thickness * 0.5, props.height);
		mesh_A->rename(name_A);
		mesh_A->translate(glm::vec3(0, props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
		mesh_A->setMaterial("black plastic");  //PLA

		std::string name_B = mesh_B->name();
		mesh_B = Primitives::createBox(props.width, props.thickness * 0.5, props.height);
		mesh_B->rename(name_B);
		mesh_B->translate(glm::vec3(0, -props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
		mesh_B->setMaterial("cyan plastic");  //TPU

	}
}
