#include "MainLayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>

const float radius = 3;

MainLayer::MainLayer(){
	camera().setNearPlane(2.0f);
	camera().setFarPlane(600);
	camera().setFOV(45); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0, -200, 50));
	camera().setRotation(glm::vec3(0, 0, +90));
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
	//renderer.enableTransparency();

	scene = Scene::create("scene");

	sample_A  = Primitives::createBox(20, 2, 15);
	sample_B  = Primitives::createBox(20, 2, 15);
	//interface = Primitives::createBox(20, 1, 15);

	sample_A->translate(glm::vec3(0, 1, 15*0.5));
	sample_B->translate(glm::vec3(0,-1, 15*0.5));

	sample_A->setMaterial("black plastic");  //PLA
	sample_B->setMaterial("cyan plastic"); //TPU
	//interface->setMaterial("cyan plastic"); //TPU


	/*
		black_plastic
		cyan_plastic
		green_plastic
		red_plastic
		gray_plastic
		white_plastic
		yellow_plastic
	*/


	scene->add(sample_A);
	scene->add(sample_B);
	//scene->add(interface);



	/***********************
		Scene decoration
	************************/

	shared<Model> bed = ModelLoader::loadModel("./assets/models/bed.stl");
	bed->translate(glm::vec3(0.75, -0.25, -0.2));
	bed->scale(glm::vec3(1.025, 1.025, 1.0));
	bed->setMaterial("chrome");
	

	//modelShader->Use();
	//modelShader->setVec3("lightPos", glm::vec3(0.0, 10.0, 10));

	shared<Model> bed_surface = Model::create("floorSurface", Primitives::createRectangle(316, 216));
	bed_surface->translate(glm::vec3(0.75, -0.25, 0));
	bed_surface->scale(glm::vec3(1, -1, 1));

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

	TransformObject_Ptr origin = TransformObject::create("origin", 10);
	origin->translate(glm::vec3(-150, -100, 0));
	scene->add(origin);
}

void MainLayer::slice(){
	float z = Settings::layer_height


}

void MainLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_INFO);

	createScene();
}

void MainLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);

	renderer.clear();
	renderer.render(scene, camera());
	renderer.reset();

}


void MainLayer::onImGuiRender(){
	//ImGui::ShowDemoWindow();

	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::Begin("Infos");

	ImGui::LabelText("FPS", std::to_string(fps()).c_str());

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

	ImGui::End();


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
	}
	ImGui::End();


}
