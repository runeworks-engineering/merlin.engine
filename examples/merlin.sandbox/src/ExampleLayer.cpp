#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>

const float radius = 3;

ExampleLayer::ExampleLayer(){
	camera().setNearPlane(0.1f);
	camera().setFarPlane(1000.0f);
	camera().setFOV(45); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0.7, -7, 2.4));
	camera().setRotation(glm::vec3(0, 0, +90));
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::createScene() {
	renderer.initialize();
	renderer.enableSampleShading();
	renderer.setEnvironmentGradientColor(0.903, 0.903, 0.903);
	renderer.enableEnvironment();
	renderer.disableShadows();
	renderer.showLights();
	renderer.disableFaceCulling();
	//renderer.enableTransparency();

	//renderer.setRenderMode(RenderMode::UNLIT);

	domain = Primitives::createCube(100);
	domain->enableWireFrameMode();

	fluid = Primitives::createCube(20);
	fluid->enableWireFrameMode();

	scene = Scene::create("scene");

	scene->add(domain);
	scene->add(fluid);
	scene->add(TransformObject::create("origin", 10));
}

void ExampleLayer::createPhysics(){
	//ParticleSamplerPtr sampler = createShared<VoxelSampler>()
	//PhysicsEntity_Ptr fluid_entity = createShared<PhysicsEntity>("fluid", ParticleSampler_Ptr)

	solver.setDomain(domain->getBoundingBox());
	solver.init();
}

void ExampleLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_TRACE);

	createScene();
	createPhysics();

}



void ExampleLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);

	renderer.clear();
	renderer.render(scene, camera());
	renderer.reset();

}

void ExampleLayer::onImGuiRender()
{

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


	// draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene->nodes());
	ImGui::End();
	


}
