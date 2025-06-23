#pragma once

#include <Merlin.h>
#include "Slicer.h"

using namespace Merlin;

class SampleObject {
public:
	SampleObject(const Sample& props);

	void renderMenu();
	const Sample& getProperties() const { return props; }
	const Mesh_Ptr& getMeshA() const { return mesh_A; }
	const Mesh_Ptr& getMeshB() const { return mesh_B; }

	bool enabled = true;
private:
	Sample props;
	Mesh_Ptr mesh_A;
	Mesh_Ptr mesh_B;

	bool show_toolpath = false;
	bool show_mesh = true;
	bool show_tool = true;
	bool show_purge_tower = false;
};


class MainLayer : public Layer3D {
public:
	MainLayer();
	virtual ~MainLayer();

	void createScene();
	void slice();
	void createSample(Sample);

	virtual void onAttach() override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:

	Slicer slicer;

	ParticleSystem_Ptr toolpath;
	TransformObject_Ptr origin;
	Model_Ptr bed;
	Model_Ptr bed_glass;
	Model_Ptr bed_surface;

	Shader_Ptr toolpath_shader;
	SSBO_Ptr<ToolPath> toolpath_buffer;

	std::vector<SampleObject> samples;
	std::vector<Mesh_Ptr> samples_3D;
	Sample default_props;

	Scene_Ptr scene;
	Renderer renderer;
};