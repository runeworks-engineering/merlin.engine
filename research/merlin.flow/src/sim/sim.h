#pragma once
#include "merlin.h"

#include <mutex>
#include <vector>


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
    void step(Timestep ts);
    bool isRunning() const;

	void spawn();
	void nns();

	void control(float vx, float vy, float ve);

	void syncUniform();

	void setNozzlePosition(const glm::vec3&);
	void setFlowrate(float);
	const glm::vec3& getNozzlePosition() const;
	ParticleSystem_Ptr getParticles() const;
	ParticleSystem_Ptr getBins() const;
	StagedComputeShader_Ptr getSolver();

private:
	float m_current_flowrate = 0;
	bool running = false;
  
    StagedComputeShader_Ptr solver;
    StagedComputeShader_Ptr prefixSum;

	ParticleSystem_Ptr ps;
	ParticleSystem_Ptr bs;


	// --- Simulation--- 

	Mesh_Ptr static_emitter;
	Mesh_Ptr nozzle_emitter;

	glm::vec3 nozzle_position = { 150.0f, 100.0f, 0.0f };
	glm::vec3 model_matrix_translation = { 0.0f, 0.0f, 0.0f };


	//Emitter
	float lastSpawTime = 0;
	float emitterVolume = 0;

	//Time
	float elapsedTime = 0;
	bool paused = true;
	bool integrate = true;


	std::mutex sim_mutex;
	bool locked = false;

	//Settings
	bool use_real_time = false;

	std::vector<glm::vec4> cpu_position;
	std::vector<glm::vec4> cpu_sdf;
	std::vector<glm::vec2> cpu_temp;
	std::vector<glm::uvec4> cpu_meta;
	std::vector<glm::vec4> cpu_emitterPosition;
};