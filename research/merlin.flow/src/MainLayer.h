#pragma once

#include <Merlin.h>
#include "sim/sim.h"
#include "gcode/gcode_sim.h"

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

	void createGCode();
	void createBuffers();
	void createScene();
	void createShaders();
	
	void syncUniform();

public:

	void plotIsoSurface();
	void plotCutView();

	void plotXY();
	void plotXZ();
	void plotYZ();

private:

	Scene_Ptr scene;
	Renderer renderer;

	Sim sim;
	GCodeSimulator controller;

	/*********** Scene ***********/

	Model_Ptr bed;
	Model_Ptr bed_glass;
	Model_Ptr bed_surface;
	TransformObject_Ptr origin;

	ParticleSystem_Ptr ps;
	ParticleSystem_Ptr bs;

	Mesh_Ptr nozzle;



	/*********** Visualization ***********/
	float offsetPlane = 0;
	IsoSurface_Ptr isosurface;
	Texture3D_Ptr volume;

	Texture2D_Ptr texture_debugXZ;
	Texture2D_Ptr texture_debugXY;
	Texture2D_Ptr texture_debugYZ;


	/*********** Shaders ***********/

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

};