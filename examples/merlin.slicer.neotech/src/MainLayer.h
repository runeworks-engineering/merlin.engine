#pragma once

#include <Merlin.h>
#include "SampleObject.h"
#include "Slicer.h"

using namespace Merlin;

enum class Machine {
	NEOTECH, 
	TOOLHANGER
};

class MainLayer : public Layer3D {
public:
	MainLayer();
	~MainLayer();

	void createScene();
	void slice();
	void createSample(SampleProperty);

	void onAttach() override;
	void onUpdate(Timestep ts) override;
	void onImGuiRender() override;

	void newProject();
	void saveProject();
	void importProject();

private:

	int current_layer = 0;
	bool showG0 = false;

	std::string current_project_path = "";

	Slicer slicer;

	ParticleSystem_Ptr toolpath;
	TransformObject_Ptr origin;

	Model_Ptr bed;
	Model_Ptr bed_glass;
	Model_Ptr bed_surface;

	Model_Ptr bed_neotech;

	Shader_Ptr toolpath_shader;
	SSBO_Ptr<ToolPath> toolpath_buffer;

	std::vector<SampleObject> samples;
	std::vector<Mesh_Ptr> samples_3D;
	SampleProperty default_props;

	Machine current_machine = Machine::NEOTECH;

	Scene_Ptr scene;
	Renderer renderer;
};