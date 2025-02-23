#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>

const float radius = 3;

ExampleLayer::ExampleLayer(){
	camera().setNearPlane(0.1f);
	camera().setFarPlane(100.0f);
	camera().setFOV(55); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0.7, -7, 2.4));
	camera().setRotation(glm::vec3(0, 0, +90));
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_TRACE);

	renderer.initialize();
	renderer.enableSampleShading();
	renderer.setEnvironmentGradientColor(0.903, 0.803, 0.703);
	renderer.showLights();
	
	shared<Model> model = ModelLoader::loadModel("./assets/models/model.obj");
	//model->translate(glm::vec3(-0.5, 0, 0));

	shared<Model> floor = Model::create("floor", Primitives::createRectangle(10, 10));
	shared<PhongMaterial> floorMat = createShared<PhongMaterial>("floormat");
	floorMat->loadTexture("./assets/textures/planks.png", TextureType::DIFFUSE);
	floorMat->loadTexture("./assets/textures/planks_specular.png", TextureType::SPECULAR);
	floorMat->setAmbient(glm::vec3(0.25, 0.20725, 0.20725));
	floorMat->setDiffuse(glm::vec3(1, 0.829, 0.829));
	floorMat->setSpecular(glm::vec3(0.296648, 0.296648, 0.296648));
	floorMat->setShininess(0.125);
	floor->setMaterial(floorMat);
	

	/**/
	light = createShared<PointLight>("light0");
	light->translate(glm::vec3(radius, radius, 3));
	light->setAttenuation(glm::vec3(0.6, 0.08, 0.008));
	light->setAmbient(0.05, 0.05, 0.05);
	light->setDiffuse(1, 1, 1);
	scene.add(light);
	/**/

	shared<DirectionalLight>  dirlight;

	/**/
	dirlight = createShared<DirectionalLight>("light1", glm::vec3(-0.5f, 0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light2", glm::vec3(0.5f, 0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(1));
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light3", glm::vec3(0.0f, -0.5f, -0.8f));
	dirlight->translate(dirlight->direction() * glm::vec3(-10));
	dirlight->setDiffuse(glm::vec3(1));
	scene.add(dirlight);
	/**/

	/**/
	shared<AmbientLight> amLight = createShared<AmbientLight>("light4");
	amLight->setAmbient(glm::vec3(0.1));
	scene.add(amLight);
	/**/

	Model_Ptr cube = Model::create("cube", Primitives::createCube(0.5));
	cube->setMaterial("jade");
	cube->translate(glm::vec3(-1,-1,0.25));
	renderer.disableFaceCulling();
	scene.add(cube);
	scene.add(model);
	scene.add(floor);

	//scene.setCamera(camera);

	/*
	shared<Environment> env;
	env = createShared<Environment>("env", 2048);
	//env->setCubeMap(light->getShadowMap());

	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/right.jpg",
		"./assets/textures/skybox/left.jpg",
		"./assets/textures/skybox/top.jpg",
		"./assets/textures/skybox/bottom.jpg",
		"./assets/textures/skybox/front.jpg",
		"./assets/textures/skybox/back.jpg"
	};

	shared<CubeMap> sky = CubeMap::create(skyBoxPath);
	env->setCubeMap(sky);

	scene.setEnvironment(env);
	*/

	scene.add(TransformObject::create("origin", 2));

	//noise = ComputeShader::create("noise", "assets/noise.comp");
	//glm::ivec3 pWkgSize = glm::ivec3(8, 8, 8); //Number of thread per workgroup
	//glm::ivec3 pWkgCount = (volume_size + pWkgSize - glm::ivec3(1)) / pWkgSize; //Total number of workgroup needed
	//noise->SetWorkgroupLayout(pWkgCount);

	//volume = Texture3D::create(volume_size.x, volume_size.y, volume_size.z, 4, 16);

	//isosurface = IsoSurface::create("isosurface", volume);
	//isosurface->mesh()->setMaterial("gold");
	//scene.add(isosurface);
}



float t = 0.0;

void ExampleLayer::onUpdate(Timestep ts){
	Layer3D::onUpdate(ts);

	const float hpi = 3.14159265358;
	t += ts;

	float x = light->position().x;
	float y = light->position().y;
	light->translate(glm::vec3(cos(t)* radius - x, sin(t)* radius - y, 0.0));

	/*
	volume->bindImage(0);
	noise->use();
	noise->setFloat("u_time", glfwGetTime());
	noise->setInt("u_mode_index", 4); //"FBM Noise", "SDF Sphere", "SDF Box", "SDF Torus", "SDF Metaballs"
	noise->setFloat("u_twist", 0.0);
	noise->setFloat("u_bend", 0.0);

	noise->dispatch();
	noise->barrier();

	isosurface->setIsoLevel(0.1);
	isosurface->compute();
	*/
	renderer.clear();
	renderer.renderScene(scene, camera());
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
	traverseNodes(scene.nodes());
	ImGui::End();
	


}
