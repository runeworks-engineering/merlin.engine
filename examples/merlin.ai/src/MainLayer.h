#pragma once

#include <Merlin.h>
#include "slicer/sampleObject.h"
#include "slicer/slicer.h"
#include "sim/sim.h"
#include "gym/gym.h"
#include <random>

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
	void onDetach() override;
	void onUpdate(Timestep ts) override;
	void onImGuiRender() override;

	void createBuffers();
	void createScene();
	void createShaders();
	void createCamera();
	void createGym();

	void createRandomSample();

	std::vector<uint8_t> captureGoalImage();
	std::vector<uint8_t> captureGoalDepthImage();
	std::vector<uint8_t> captureCurrentImage();

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
	float min_size_, max_size_;
	std::mt19937 rng_{ std::random_device{}() };

	bool use_zmq_control = true;
	int colorMode = 4;

	Slicer slicer;
	Scene_Ptr scene;
	Renderer renderer;

	Sim sim;
	GymServer gym;

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

	const int img_res = 128;
	Camera camera_output;
	FBO_Ptr camera_fbo;
	RBO_Ptr camera_rbo;
	Texture2D_Ptr camera_texture;
	Texture2D_Ptr camera_goal_texture;





	/*********** Shaders ***********/
	Shader_Ptr toolpath_shader;
	AbstractBufferObject_Ptr toolpath_buffer;

	Shader_Ptr particle_shader;
	Shader_Ptr isosurface_shader;
	Shader_Ptr bin_shader;

	ComputeShader_Ptr solver;
	ComputeShader_Ptr isoGen;
	ComputeShader_Ptr texPlot;


	GLsizeiptr last_numParticle = 0;
	bool need_sync = true;


	/*********** Rendering ***********/

	bool use_isosurface = false;
	bool use_2Dplot = false;

	//Texture debug
	float offsetPlane = 0;

	/*********** Project ***********/
	
	SampleObject sample_obj;
	Mesh_Ptr sample_mesh;

	SampleProperty default_props;

	int current_layer = 0;
	bool showG0 = false;

	std::string current_project_path = "";



};