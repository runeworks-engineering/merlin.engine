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

}


void ExampleLayer::onImGuiRender()
{
	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);
	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::list<shared<RenderableObject>>&)> traverseNodes = [&](const std::list<shared<RenderableObject>>& nodes){
		for (auto& node : nodes){
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open){
				if (node != nullptr){
					ImGui::Text(node->name().c_str());
				}
				// draw the node's children
				traverseNodes(node->children());
				ImGui::TreePop();
			}
		}
	};

	std::vector<PhysicsEntity_Ptr> entities = solver.getEntities();
	for (const auto& entity : entities) {
		ImGui::Begin(entity->name().c_str());
		entity->onRenderMenu();
		ImGui::End();
	}

	// draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene->nodes());
	ImGui::End();
	


}
