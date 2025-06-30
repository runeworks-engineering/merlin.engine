#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>

const float radius = 3;

ExampleLayer::ExampleLayer() {
	camera().setPosition(glm::vec3(0.7, -10, 2.4));
	camera().setView(CameraView::Iso);
}


void ExampleLayer::onAttach(){
	Layer3D::onAttach();

	renderer.initialize();
	renderer.setEnvironmentGradientColor(0.903, 0.803, 0.703);
	renderer.showLights();
	renderer.disableFaceCulling();


	shared<Model> bunny = ModelLoader::loadModel("./assets/common/models/bunny.stl");
	bunny->setMaterial("pearl");
	bunny->scale(0.2);
	bunny->translate(glm::vec3(0,0,-0.5));
	//bunny->setShader("debug.normals");
	scene.add(bunny);

	/**/
	light = createShared<PointLight>("light0");
	light->translate(glm::vec3(radius, radius, 1));
	light->setAttenuation(glm::vec3(0.7, 0.08, 0.008));
	light->setAmbient(0.09, 0.05, 0.05);
	light->setDiffuse(0.7, 0.7, 0.7);
	light->setShadowResolution(1024);
	scene.add(light);
	/**/

	shared<DirectionalLight>  dirlight;

	/**/
	dirlight = createShared<DirectionalLight>("light1", glm::vec3(-0.5f, 0.5f, -0.4f));
	dirlight->translate(glm::vec3(-10,0,0));
	dirlight->setDiffuse(glm::vec3(0.3, 0, 0));
	//dirlight->setShadowResolution(512);
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light2", glm::vec3(0.5f, 0.5f, -0.4f));
	dirlight->translate(glm::vec3(-10,0,0));
	dirlight->setDiffuse(glm::vec3(0.0, 0.3, 0));
	//dirlight->setShadowResolution(1024);
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light3", glm::vec3(0.0f, -0.5f, -0.4f));
	dirlight->translate(glm::vec3(-10,0,0));
	dirlight->setDiffuse(glm::vec3(0.0,0,0.3));
	//dirlight->setShadowResolution(1024);
	scene.add(dirlight);
	/**/

	/**/
	shared<AmbientLight> amLight = createShared<AmbientLight>("light4");
	amLight->setAmbient(glm::vec3(0.2));
	scene.add(amLight);
	/**/

	scene.add(Primitives::createFloor(50, 0.5));
	//scene.add(Primitives::createRectangle(10, 10));
}

void ExampleLayer::onDetach(){}

void ExampleLayer::onEvent(Event& event){
	Layer3D::onEvent(event);
}

float t = 0.0;

void ExampleLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);
	const float hpi = 3.14159265358;
	t += ts;

	float x = light->position().x;
	float y = light->position().y;
	light->translate(glm::vec3(cos(t) * radius - x, sin(t) * radius - y, 0.0));

	renderer.clear();
	renderer.renderScene(scene, camera());
	renderer.reset();
}

void ExampleLayer::onImGuiRender()
{
	ImGui::Begin("Camera");

	model_matrix_translation = camera().getPosition();
	if (ImGui::DragFloat3("Camera position", &model_matrix_translation.x, -100.0f, 100.0f)) {
		camera().setPosition(model_matrix_translation);

	}
	ImGui::End();

	// Define a recursive lambda function to traverse the scene graph
	std::function<void(const std::list<shared<RenderableObject>>&)> traverseNodes = [&](const std::list<shared<RenderableObject>>& nodes){
		for (auto& node : nodes){
			bool node_open = ImGui::TreeNode(node->name().c_str());
			if (node_open){
				
				if (node != nullptr){
					ImGui::Text(node->name().c_str());
				}

				traverseNodes(node->children());
				ImGui::TreePop();
			}
		}
	};

	// draw the scene graph starting from the root node
	ImGui::Begin("Scene Graph");
	traverseNodes(scene.nodes());
	ImGui::End();
	


}
