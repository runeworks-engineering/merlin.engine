#pragma once

#include <Merlin.h>
#include "SampleObject.h"
#include "Slicer.h"
#include "sim.h"

using namespace Merlin;

enum class Machine {
	NEOTECH, 
	TOOLHANGER
};

class MainLayer : public Layer3D {
public:
	MainLayer();
	~MainLayer();

	void onAttach() override;
	void onUpdate(Timestep ts) override;
	void onImGuiRender() override;

	

	void createBuffers();
	void createScene();
	void createShaders();
	void createSamples();

	void slice();
	void createSample(SampleProperty);

	void newProject();
	void saveProject();
	void importProject();

	
	//void attachBuffers();
	void syncUniform();

public:

	void plotIsoSurface();
	void plotCutView();

	void plotXY();
	void plotXZ();
	void plotYZ();

private:

	Slicer slicer;
	Scene_Ptr scene;
	Renderer renderer;

	Sim sim;

	/*********** Scene ***********/

	
	Model_Ptr bed;
	Model_Ptr bed_glass;
	Model_Ptr bed_surface;
	TransformObject_Ptr origin;

	ParticleSystem_Ptr toolpath;
	ParticleSystem_Ptr ps;
	ParticleSystem_Ptr bs;

	Mesh_Ptr nozzle;



	IsoSurface_Ptr isosurface;
	Texture3D_Ptr volume;

	Texture2D_Ptr texture_debugXZ;
	Texture2D_Ptr texture_debugXY;
	Texture2D_Ptr texture_debugYZ;




	/*********** Shaders ***********/
	Shader_Ptr toolpath_shader;
	AbstractBufferObject_Ptr toolpath_buffer;

	Shader_Ptr particle_shader;
	Shader_Ptr isosurface_shader;
	Shader_Ptr bin_shader;

	ComputeShader_Ptr solver;
	ComputeShader_Ptr isoGen;
	ComputeShader_Ptr texPlot;


	/*********** Rendering ***********/

	bool use_isosurface = false;
	bool use_2Dplot = false;

	//Texture debug
	int offsetPlane = 0;

	/*********** Project ***********/
	
	std::vector<SampleObject> samples;
	std::vector<Mesh_Ptr> samples_3D;
	SampleProperty default_props;

	int current_layer = 0;
	bool showG0 = false;

	std::string current_project_path = "";



};