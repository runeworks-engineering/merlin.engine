#include "sim.h"
#include "settings.h"

Sim::Sim(){}
Sim::~Sim() {}


void Sim::init() {

	simulator.reset();

	//Compute Shaders
	solver = StagedComputeShader::create("solver", "assets/shaders/solver/solver.comp", 9, false);
	settings.setConstants(*solver);
	solver->compile();

	prefixSum = StagedComputeShader::create("prefixSum", "assets/shaders/solver/prefix.sum.comp", 4, false);
	settings.setConstants(*prefixSum);
	prefixSum->compile();


	//create particle system
	ps = ParticleSystem::create("ParticleSystem", settings.pThread);
	ps->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);

	shared<Mesh> binInstance = Primitives::createQuadCube(settings.bWidth, false);
	binInstance->rename("bin");
	bs = ParticleSystem::create("BinSystem", settings.bThread);
	bs->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
	bs->setMesh(binInstance);
	bs->enableWireFrameMode();

	solver->setWorkgroupLayout(settings.pWkgCount);
	prefixSum->setWorkgroupLayout(settings.bWkgCount);


	static_emitter = Primitives::createCylinder(8, 2, 10);
	static_emitter->translate(glm::vec3(0, 0, 1.25 * 10));

	nozzle_emitter = ModelLoader::loadMesh("./assets/models/nozzle_emitter.stl");
	nozzle_emitter->setMaterial("gold");
	nozzle_emitter->scale(10);
	nozzle_emitter->applyMeshTransform();


}


void Sim::reset(){
	elapsedTime = 0;
	lastSpawTime = 0;
	simulator.reset();


	auto& memory = MemoryManager::instance();
	memory.resetBindings();

	Console::info() << "Generating particles..." << Console::endl;

	float spacing = settings.particleRadius * 2.0;
	auto cpu_position = std::vector<glm::vec4>();
	auto cpu_sdf = std::vector<glm::vec4>();
	//auto cpu_velocity = std::vector<glm::vec4>();
	auto cpu_temp = std::vector<glm::vec2>();
	auto cpu_meta = std::vector<glm::uvec4>();
	auto cpu_emitterPosition = std::vector<glm::vec4>();

	settings.numEmitter = 0;
	settings.numParticles = 0;
	std::vector<glm::vec3> positions;



	/**/
	static_emitter->computeBoundingBox();
	static_emitter->voxelize(spacing);
	positions = Voxelizer::getVoxelposition(static_emitter->getVoxels(), static_emitter->getBoundingBox(), spacing);

	/**/
	for (int i = 0; i < positions.size(); i++) {
		//cpu_position.push_back(glm::vec4(positions[i], 0));
		//cpu_temp.push_back(298.15); //ambient
		//cpu_meta.push_back(glm::uvec4(FLUID, settings.numParticles(), settings.numParticles(), 0.0));

		cpu_emitterPosition.push_back(glm::vec4(positions[i], 0.0));
		settings.numEmitter()++;
	}
	/**/

	/*
	floor->computeBoundingBox();
	floor->voxelizeSurface(spacing, 0.5);
	positions = Voxelizer::getVoxelposition(floor->getVoxels(), floor->getBoundingBox(), spacing);

	for (int i = 0; i < positions.size(); i++) {
		cpu_position.push_back(glm::vec4(positions[i] - glm::vec3(0,0,settings.smoothingRadius*5.0), 0));
		cpu_temp.push_back(glm::vec2(275.15 + 20, 0)); //ambient
		cpu_meta.push_back(glm::uvec4(BOUNDARY, settings.numParticles(), settings.numParticles(), 0.0));
		settings.numParticles()++;
	}/**/




	/**/
	//nozzle_emitter->scale(1.1);
	nozzle_emitter->setPosition(glm::vec3(0, 0, 0));
	nozzle_emitter->computeBoundingBox();
	nozzle_emitter->voxelize(spacing * 0.8, true);
	positions = Voxelizer::getVoxelposition(nozzle_emitter->getVoxels(), nozzle_emitter->getBoundingBox(), spacing * 0.8);
	std::vector<glm::vec4> sdf = nozzle_emitter->getVoxelsSDF();

	for (int i = 0; i < positions.size(); i++) {
		cpu_position.push_back(glm::vec4(positions[i], 0));
		cpu_sdf.push_back(sdf[i]);
		//cpu_velocity.push_back(nozzle_emitter->computeSDF(positions[i]));
		cpu_temp.push_back(glm::vec2(275.15 + 220, 0));
		cpu_meta.push_back(glm::uvec4(BOUNDARY + 1, settings.numParticles(), settings.numParticles(), 0.0));
		settings.numParticles()++;

	}
	/**/


	if (use_emitter_init) {
		Mesh_Ptr init_fluid = Primitives::createCylinder(4.5, 18, 10);
		init_fluid->setPosition(glm::vec3(0, 0, 23));
		init_fluid->rotate(glm::vec3(0, 90.0f * DEG_TO_RAD, 0));
		init_fluid->computeBoundingBox();
		init_fluid->voxelize(spacing * 0.8);
		positions = Voxelizer::getVoxelposition(init_fluid->getVoxels(), init_fluid->getBoundingBox(), spacing * 0.8);

		/**/
		for (int i = 0; i < positions.size(); i++) {
			cpu_position.push_back(glm::vec4(positions[i], 0));
			cpu_sdf.push_back(glm::vec4(0));
			//cpu_velocity.push_back(glm::vec4(0));
			cpu_temp.push_back(glm::vec2(275.15 + 200, 0)); //ambient
			cpu_meta.push_back(glm::uvec4(FLUID, settings.numParticles(), settings.numParticles(), 0.0));
			settings.numParticles()++; 
		}

		init_fluid = Primitives::createCone(13, 5, 10);
		init_fluid->setPosition(glm::vec3(0, 0, 23));
		init_fluid->rotate(glm::vec3(0.0f, 90.0f * DEG_TO_RAD, 0.0f));
		init_fluid->computeBoundingBox();
		init_fluid->voxelize(spacing * 0.8);
		positions = Voxelizer::getVoxelposition(init_fluid->getVoxels(), init_fluid->getBoundingBox(), spacing * 0.8);


		/**/
		for (int i = 0; i < positions.size(); i++) {
			cpu_position.push_back(glm::vec4(positions[i], 0));
			cpu_sdf.push_back(glm::vec4(0));
			//cpu_velocity.push_back(glm::vec4(0));
			cpu_temp.push_back(glm::vec2(275.15 + 200, 0)); //ambient
			cpu_meta.push_back(glm::uvec4(FLUID, settings.numParticles(), settings.numParticles(), 0.0));
			settings.numParticles()++;
		}
	}

	while (cpu_position.size() < settings.max_pThread) {
		cpu_position.push_back(glm::vec4(0.0, 0.0, 0.0, 0.0));
		cpu_sdf.push_back(glm::vec4(0));
		//cpu_velocity.push_back(glm::vec4(0));
		cpu_temp.push_back(glm::vec2(298.15, 0));
		cpu_meta.push_back(glm::uvec4(FLUID_EMITTER, settings.numParticles(), settings.numParticles(), settings.numParticles()));
	}


	Console::info() << "Uploading buffer on device..." << Console::endl;

	settings.pThread = settings.numParticles();
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	settings.blockSize = std::floor(log2f(settings.bThread));
	settings.blocks = (settings.bThread + settings.blockSize - 1) / settings.blockSize;
	settings.bWkgCount = (settings.blocks + settings.bWkgSize - 1) / settings.bWkgSize; //Total number of workgroup needed
	settings.emitter_transform = glm::mat4(1);
	//settings.emitter_transform.sync(*solver);


	solver->setWorkgroupLayout(settings.pWkgCount);
	prefixSum->setWorkgroupLayout(settings.bWkgCount);

	ps->setInstancesCount(settings.max_pThread);
	ps->setActiveInstancesCount(settings.pThread);
	bs->setInstancesCount(settings.bThread);


	//SyncUniforms();
	Console::info() << "Uploading buffer on device..." << Console::endl;
	memory.writeBuffer("position_buffer", cpu_position);
	memory.writeBuffer("predicted_position_buffer", cpu_position);
	memory.writeBuffer("last_position_buffer", cpu_position);
	memory.writeBuffer("correction_buffer", cpu_sdf);
	//ps->writeField("velocity_buffer", cpu_velocity);
	memory.writeBuffer("temperature_buffer", cpu_temp);
	memory.writeBuffer("meta_buffer", cpu_meta);
	memory.writeBuffer("emitter_position_buffer", cpu_emitterPosition);

	memory.clearBuffer("velocity_buffer");
	memory.clearBuffer("density_buffer");
	memory.clearBuffer("lambda_buffer");
	//memory.clearBuffer("stress_buffer");

}

void Sim::start(){
	running = true;
}

void Sim::stop(){
	running = false;
}

bool Sim::isRunning() const {
	return running;
}

void Sim::spawn(){
	settings.pThread = settings.numParticles() + settings.numEmitter();
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	solver->setWorkgroupLayout(settings.pWkgCount);
	ps->setActiveInstancesCount(settings.pThread);

	solver->use();
	settings.numEmitter.sync(*solver);
	//settings.numParticles.sync(*solver);

	solver->execute(8);

	settings.numParticles() += settings.numEmitter();
	settings.numParticles.sync(*solver);

	solver->use();
	solver->setUInt("u_numEmitter", 0);
	settings.numParticles.sync(*solver);

	settings.pThread = settings.numParticles();
	settings.pWkgCount = (settings.pThread + settings.pWkgSize - 1) / settings.pWkgSize; //Total number of workgroup needed
	solver->setWorkgroupLayout(settings.pWkgCount);
	ps->setActiveInstancesCount(settings.pThread);
}

void Sim::nns(){
	prefixSum->use();
	prefixSum->setUInt("dataSize", settings.bThread); //data size
	prefixSum->setUInt("blockSize", settings.blockSize); //block size
	prefixSum->execute(4);// clear bins

	solver->use();
	solver->execute(0); //Place particles in bins

	prefixSum->use();
	prefixSum->execute(0);// local prefix sum

	//Binary tree on rightmost element of blocks
	GLuint steps = settings.blockSize;
	UniformObject<GLuint> space("space");
	space.value() = 1;

	for (GLuint step = 0; step < steps; step++) {
		// Calls the parallel operation

		space.sync(*prefixSum);
		prefixSum->execute(1);
		prefixSum->execute(2);

		space.value() *= 2;
	}
	prefixSum->execute(3);

	solver->use();
	solver->execute(1); //Sort
}

void Sim::step(Timestep ts){
	if (running) {
		elapsedTime += ts.getSeconds();
		settings.setTimestep(ts.getSeconds());

		//ps->clearField("correction_buffer");

		syncUniform();

		solver->use();
		settings.dt.sync(*solver);

		for (int i = 0; i < settings.solver_substep; i++) {


			for (int i = 0; i < 10; i++)
				simulator.update(ts.getSeconds() / (settings.solver_substep * 10));

			nozzle_position = simulator.getNozzlePosition();
			

			settings.emitter_transform = glm::mat4(1);
			settings.emitter_transform = glm::translate(settings.emitter_transform(), glm::vec3(nozzle_position));
			settings.emitter_transform.sync(*solver);

			float e_speed = simulator.getExtruderDistance();
			float emitterDelay = 1000.0 / (settings.particleVolume * 1.0) / e_speed;
			if (use_emitter && simulator.getExtruderDistance() > 0.01)
				if (elapsedTime - lastSpawTime > (emitterDelay / 1000.0)) {
					spawn();
					lastSpawTime = elapsedTime;
				}

			solver->execute(2);

			nns();


			if (integrate) {
				for (int j = 0; j < settings.solver_iteration; j++) {
					solver->execute(3);
					solver->execute(4);
					solver->execute(5);
				}


				solver->execute(6);
				solver->execute(7);
			}
		}
	}
	if (simulator.lastCommandReached()) paused = true;
}



void Sim::syncUniform(){

	solver->use();

	//settings.timestep.sync(*solver);
	settings.particleMass.sync(*solver);
	settings.restDensity.sync(*solver);
	settings.numParticles.sync(*solver);
	settings.numEmitter.sync(*solver);
	settings.dt.sync(*solver);
	//settings.viscosity.sync(*solver);
	settings.artificialViscosityMultiplier.sync(*solver);
	settings.artificialPressureMultiplier.sync(*solver);

	settings.emitter_transform = glm::mat4(1);
	//settings.emitter_transform = glm::translate(settings.emitter_transform(), glm::vec3(-20, 0, 50));
	settings.emitter_transform.sync(*solver);

	prefixSum->use();
	prefixSum->setUInt("dataSize", settings.bThread); //data size
	prefixSum->setUInt("blockSize", settings.blockSize); //block size
}

const glm::vec3& Sim::getNozzlePosition() const{
	return nozzle_position;
}

ParticleSystem_Ptr Sim::getParticles() const
{
	return ps;
}

ParticleSystem_Ptr Sim::getBins() const
{
	return bs;
}

StagedComputeShader_Ptr Sim::getSolver()
{
	return solver;
}
