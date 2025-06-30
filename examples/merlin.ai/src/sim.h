#pragma once
#include "GcodeReader.h"
#include "merlin.h"

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define FLUID_EMITTER 3
#define GRANULAR 4
#define BOUNDARY 5

using namespace Merlin;
class Sim {
public:
    Sim();
    ~Sim();

	void init();
    void reset();
    void start();
    void stop();
    bool isRunning() const;

	void spawn();
	void nns();

	void step(Timestep ts);
	void syncUniform();

	const glm::vec3& getNozzlePosition() const;
	ParticleSystem_Ptr getParticles() const;
	ParticleSystem_Ptr getBins() const;
	StagedComputeShader_Ptr getSolver();

private:

	bool running = false;

    GcodeSimulator simulator;
  
    StagedComputeShader_Ptr solver;
    StagedComputeShader_Ptr prefixSum;

	ParticleSystem_Ptr ps;
	ParticleSystem_Ptr bs;


	// --- Simulation--- 

	Mesh_Ptr static_emitter;
	Mesh_Ptr nozzle_emitter;

	glm::vec3 nozzle_position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };


	//Emitter
	float lastSpawTime = 0;

	//Time
	float elapsedTime = 0;
	bool paused = true;
	bool integrate = true;


	//Settings
	bool use_real_time = false;

};