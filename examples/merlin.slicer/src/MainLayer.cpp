#include "MainLayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <GLFW/glfw3.h>

const float radius = 3;

MainLayer::MainLayer(){
	camera().setNearPlane(2.0f);
	camera().setFarPlane(600);
	camera().setFOV(45); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(150, -200, 200));
	camera().setRotation(glm::vec3(0, 40, +90));

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}

MainLayer::~MainLayer(){}

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

	default_props.name = "Sample 0";
	default_props.comment = "Specimen 0";
	default_props.x_offset = 230.0f;
	default_props.y_offset = 120.0f;
	default_props.width = 35;
	default_props.height = 3.0f;
	default_props.thickness = 4.0f;
	default_props.layer_height = 0.2;
	default_props.line_width = 0.4;
	default_props.tool_a = 0;
	default_props.tool_b = 1;
	default_props.flow_a = 1.0f;
	default_props.flow_b = 1.0f;
	default_props.retract_a = 1.0f;
	default_props.retract_b = 1.0f;
	default_props.feedrate_a = 1050;
	default_props.feedrate_b = 1050;
	default_props.temperature_a = 220.0f;
	default_props.temperature_b = 235.0f;
	default_props.overlap = 2;
	default_props.overlap_flow_modifier = 1.0f;
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
	default_props.comment = "Specimen 3";
	default_props.x_offset += 0;
	default_props.y_offset += 120;
	default_props.overlap = 3;
	default_props.feedrate_a = 900;
	default_props.feedrate_b = 900;

	createSample(default_props);

	default_props.name = "Sample 4";
	default_props.comment = "Specimen 4";
	default_props.x_offset += -120;
	default_props.y_offset += 0;
	default_props.overlap = 3;
	default_props.feedrate_a = 300;
	default_props.feedrate_b = 300;

	createSample(default_props);

	/***********************
		Toolpath Viz
	************************/

	toolpath = ParticleSystem::create("toolpath", 1);
	toolpath->setDisplayMode(ParticleSystemDisplayMode::MESH);
	toolpath->setMesh(Primitives::createLine(1, glm::vec3(1,0,0)));

	toolpath_shader = Shader::create("toolpathShader", "./assets/shaders/toolpath.vert", "./assets/shaders/toolpath.frag");
	toolpath_shader->supportEnvironment(false);
	toolpath_shader->supportLights(false);
	toolpath_shader->supportMaterial(false);
	toolpath_shader->supportShadows(false);
	toolpath_shader->supportTexture(false);

	toolpath_buffer = SSBO<ToolPath>::create("toolpath_buffer", 1);
	toolpath_shader->attach(toolpath_buffer);
	toolpath->setPositionBuffer(toolpath_buffer);

	toolpath->setShader(toolpath_shader);
	//toolpath->translate(glm::vec3(150, 100, 0));

	/***********************
		Scene decoration
	************************/

	bed = ModelLoader::loadModel("./assets/models/bed.stl");
	bed->translate(glm::vec3(0.75, -0.25, 0));
	bed->setMaterial("chrome");
	

	bed_glass = ModelLoader::loadModel("./assets/models/glass.stl");
	bed_glass->setMaterial("water");
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

	bed_neotech = ModelLoader::loadModel("./assets/models/bed.neotech.stl");
	bed_neotech->setMaterial("black plastic");
	Mesh_Ptr bed_surface_neotech = Primitives::createCircle(50, 50);
	bed_surface_neotech->setMaterial("gray plastic");
	bed_surface_neotech->translate(glm::vec3(0, 0, 1));
	bed_neotech->translate(glm::vec3(0, 0, -1));
	bed_neotech->addChild(bed_surface_neotech);

	origin = TransformObject::create("origin", 10);
}

void MainLayer::slice(){
	slicer.clear();
	for (auto& s : samples) {
		slicer.generateSample(s.getProperties());
	}
	toolpath_buffer->bind();
	auto& data = slicer.getToolPath();

	if (toolpath_buffer->elements() < data.size()) {
		toolpath_buffer->allocateBuffer(data.size() * sizeof(ToolPath), data.data(), BufferUsage::StaticDraw);
	}
	else toolpath_buffer->writeBuffer(data.size() * sizeof(ToolPath), data.data());
	toolpath->setInstancesCount(data.size());
	current_layer = data.size();

	slicer.export_gcode("./samples.gcode");
}


void MainLayer::createSample(SampleProperty props){
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

	if (current_machine == Machine::NEOTECH) {
		scene->add(bed_neotech);
	}
	else if (current_machine == Machine::TOOLHANGER) {
		scene->add(bed);
	}
	
	scene->add(origin);
	scene->add(toolpath);

	for (auto& s : samples) {
		scene->add(s.getMeshA());
		scene->add(s.getMeshB());
	}

	toolpath_shader->use();
	toolpath_shader->setInt("layer", current_layer);
	toolpath_shader->setInt("showG0", showG0);

	renderer.clear();
	renderer.render(scene, camera());
	renderer.reset();

}


void MainLayer::onImGuiRender(){
	//ImGui::ShowDemoWindow();
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {

			ImGui::MenuItem("New Project", "Ctrl+N", false);

			ImGui::MenuItem("Open Project", "Ctrl+O", false);

			if (ImGui::BeginMenu("Recent Project")) {

				ImGui::MenuItem("...", "", false);

				ImGui::EndMenu();
			}

			ImGui::MenuItem("Save Project", "Ctrl+S", false);

			ImGui::MenuItem("Save Project as", "Ctrl+Alt+S", false);

			ImGui::Separator();
			if (ImGui::BeginMenu("Import")) {

				ImGui::MenuItem("Import Mesh", "Ctrl+I", false);

				ImGui::MenuItem("Import Configuration", "", false);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Export")) {

				ImGui::MenuItem("Export G-Code..", "Ctrl+G", false);

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

		if (ImGui::BeginMenu("Tool")) {

			ImGui::MenuItem("Nozzle ID tool", "", false);

			ImGui::MenuItem("3D Object Gallery", "", false);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {

			ImGui::MenuItem("Iso", "0", false);

			ImGui::Separator();
			ImGui::MenuItem("Top", "1", false);

			ImGui::MenuItem("Bottom", "2", false);

			ImGui::MenuItem("Front", "3", false);

			ImGui::MenuItem("Rear", "4", false);

			ImGui::MenuItem("Left", "5", false);

			ImGui::MenuItem("Right", "6", false);

			ImGui::Separator();
			ImGui::MenuItem("FullScreen", "F11", false);

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



	ImGui::Begin("Infos");

	ImGui::LabelText("FPS", std::to_string(fps()).c_str());

	static int selectedMachine = 1;
	static const char* items[]{ "Neotech 15XBT","Toolchanger" };

	if (ImGui::Combo("Machine", &selectedMachine, items, 2)) {
		current_machine = Machine(selectedMachine);
		
		if(current_machine == Machine::TOOLHANGER) camera().setPosition(glm::vec3(150, -200, 200));
		else camera().setPosition(glm::vec3(0, -200, 200));

	}

	if (ImGui::Button("Slice")) {
		slice();
	}

	if (ImGui::Button("Save Project")) {

		std::string savePath = Dialog::saveFileDialog(Dialog::FileType::DATA);
		if (!savePath.size() == 0) {
			saveProject(savePath);
			Console::info("Exported sample to " + savePath);
		}

	}

	if (ImGui::Button("Import Project")) {

		std::string savePath = Dialog::saveFileDialog(Dialog::FileType::DATA);
		if (!savePath.size() == 0) {
			saveProject(savePath);
			Console::info("Exported sample to " + savePath);
		}

	}

	ImGui::Checkbox("Show Rapid Toolpath", &showG0);

	ImGui::End();

	static int selected_sample_index = -1;
	static int node_clicked = -1;
	static shared<RenderableObject> selected_renderable = nullptr;  // Track selected 3D scene object


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
	ImGui::VSliderInt("layer", ImVec2(30, 1000), &current_layer, 0, slicer.getLayer());
	ImGui::End();

	// Properties panel (shows either physics or graphics properties)
	ImGui::Begin("Properties");
	if (selected_renderable) {
		selected_renderable->onRenderMenu();  // Assume `drawProperties()` exists for graphics properties
	}else if(selected_sample_index != -1){
		samples[selected_sample_index].renderMenu();  // Render sample properties
	}
	ImGui::End();


}

void MainLayer::saveProject(std::string path){
	std::ofstream file(path);
	if (!file.is_open()) {
		std::cerr << "Failed to open file for XML export: " << path << std::endl;
		return;
	}

	file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	file << "<Samples>\n";

	for (auto& s : samples) {
		file << s.toXML();
	}

	file << "</Samples>\n";
	file.close();

}

void MainLayer::importProject(std::string path) {
	using namespace tinyxml2;

	std::string answer = Dialog::inputBox("Save current project", "Save", "Yes");

	if (answer == "Yes") {
		std::string savePath = Dialog::saveFileDialog(Dialog::FileType::DATA);
		if (!savePath.size() == 0) {
			saveProject(savePath);
			Console::info("Exported sample to " + savePath);
		}
	}

	XMLDocument doc;
	if (doc.LoadFile(path.c_str()) != XML_SUCCESS) {
		std::cerr << "Failed to load XML file: " << path << std::endl;
		return;
	}

	samples.clear(); // optional: reset current project

	XMLElement* root = doc.FirstChildElement("Samples");
	if (!root) {
		std::cerr << "Invalid XML format (missing <Samples>)" << std::endl;
		return;
	}

	samples.clear();

	for (XMLElement* elem = root->FirstChildElement("Sample"); elem != nullptr; elem = elem->NextSiblingElement("Sample")) {
		SampleProperty s = SampleObject::fromXML(elem);
		createSample(s);
	}
}