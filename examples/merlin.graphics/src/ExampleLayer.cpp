#include "Examplelayer.h"

using namespace Merlin;

#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>

const float radius = 3;

ExampleLayer::ExampleLayer(){
	camera().setNearPlane(0.1f);
	camera().setFarPlane(300.0f);
	camera().setFOV(45); //Use 90.0f as we are using cubemaps
	camera().setPosition(glm::vec3(0.7, -7, 2.4));
	camera().setView(CameraView::Iso, 50);
}

ExampleLayer::~ExampleLayer(){}

void ExampleLayer::createScene() {
	renderer.initialize();
	renderer.enableSampleShading();
	renderer.setEnvironmentGradientColor(0.903, 0.803, 0.703);
	renderer.enableEnvironment();
	renderer.enableShadows();
	renderer.showLights();

	shared<Model> model = ModelLoader::loadModel("./assets/models/model.obj");
	//model->translate(glm::vec3(-0.5, 0, 0));

	shared<Model> floor = Model::create("floor", Primitives::createRectangle(15, 15));
	//shared<Model> floor = Model::create("floor", Primitives::createRectangle(1000, 1000));
	shared<PhongMaterial> floorMat = createShared<PhongMaterial>("floormat");
	floorMat->loadTexture("./assets/textures/planks.png", TextureType::DIFFUSE);
	floorMat->loadTexture("./assets/textures/planks_specular.png", TextureType::SPECULAR);
	floorMat->setAmbient(glm::vec3(0.25, 0.20725, 0.20725));
	floorMat->setDiffuse(glm::vec3(1, 0.829, 0.829));
	floorMat->setSpecular(glm::vec3(0.296648, 0.296648, 0.296648));
	floorMat->setShininess(0.125);
	//floorMat->setTextureRepeat(glm::vec2(100,100));
	floor->setMaterial(floorMat);


	/**/
	light = createShared<PointLight>("light0");
	light->translate(glm::vec3(radius, radius, 4));
	light->setAttenuation(glm::vec3(0.6, 0.08, 0.008));
	light->setAmbient(0.05, 0.05, 0.05);
	light->setDiffuse(1, 1, 1);
	scene.add(light);
	/**/

	shared<DirectionalLight>  dirlight;

	/**/
	dirlight = createShared<DirectionalLight>("light1", glm::vec3(-0.5f, 0.5f, -0.8f));
	dirlight->translate(glm::vec3(-10, 0, 0));
	dirlight->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light2", glm::vec3(0.5f, 0.5f, -0.8f));
	dirlight->translate(glm::vec3(-10, 0, 0));
	dirlight->setDiffuse(glm::vec3(1));
	scene.add(dirlight);
	/**/

	/**/
	dirlight = createShared<DirectionalLight>("light3", glm::vec3(0.0f, -0.5f, -0.8f));
	dirlight->translate(glm::vec3(-10, 0, 0));
	dirlight->setDiffuse(glm::vec3(1));
	scene.add(dirlight);
	/**/

	/**/
	shared<AmbientLight> amLight = createShared<AmbientLight>("light4");
	amLight->setAmbient(glm::vec3(0.1));
	scene.add(amLight);
	/**/

	//Model_Ptr cube = Model::create("cube", Primitives::createCube(0.5));
	Mesh_Ptr custom_mesh = ModelLoader::loadMesh("./assets/models/dragon.obj");
	
	custom_mesh->rotate(glm::vec3(glm::pi<float>()*0.5,0,0));
	custom_mesh->rotate(glm::vec3(0, 0.8, 0));
	custom_mesh->smoothNormals();
	custom_mesh->scale(0.1);
	custom_mesh->applyMeshTransform();

	custom_mesh->translate(glm::vec3(-2, -2, 2));

	Model_Ptr dragon = createShared<Model>("dragon", custom_mesh);
	dragon->setMaterial("gold");
	dragon->useNormalMap(false);

	renderer.disableFaceCulling();

	scene.add(dragon);
	scene.add(model);
	scene.add(floor);

	Model_Ptr cube = Model::create("cube", Primitives::createCube(2));
	cube->translate(glm::vec3(-2, -2, 1));
	cube->setMaterial("chrome");
	scene.add(cube);

	shared<Environment> env;
	env = createShared<Environment>("env", 2048);

	std::vector<std::string> skyBoxPath = {
		"./assets/textures/skybox/front.png",
		"./assets/textures/skybox/back.png",
		"./assets/textures/skybox/top.png",
		"./assets/textures/skybox/bottom.png",
		"./assets/textures/skybox/left.png",
		"./assets/textures/skybox/right.png"
	};

	shared<CubeMap> sky = CubeMap::create(skyBoxPath);
	env->setCubeMap(sky);
	scene.setEnvironment(env);


	scene.add(TransformObject::create("origin", 2));
}

void ExampleLayer::onAttach(){
	Layer3D::onAttach();

	Console::setLevel(ConsoleLevel::_TRACE);

	createScene();
	//createPhysics();

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

	ImGui::DockSpaceOverViewport((ImGuiViewport*)0, ImGuiDockNodeFlags_PassthruCentralNode);

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
	traverseNodes(scene.nodes());
	ImGui::End();

	// Properties panel (shows either physics or graphics properties)
	ImGui::Begin("Properties");
	if (selected_renderable) {
		selected_renderable->onRenderMenu();  // Assume `drawProperties()` exists for graphics properties
	}
	ImGui::End();

}
