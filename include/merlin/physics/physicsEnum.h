#pragma once

namespace Merlin {

	enum class MaterialPhase : uint32_t {
		MATERIAL_UNUSED = 0,
		MATERIAL_FLUID = 1 << 0,
		MATERIAL_GRANULAR = 1 << 1,
		MATERIAL_RIGID = 1 << 2,
		MATERIAL_SOFT = 1 << 3,
		MATERIAL_BOUNDARY = 1 << 5
	};

	inline MaterialPhase operator|(MaterialPhase a, MaterialPhase b) {
		return static_cast<MaterialPhase>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline bool hasTag(uint32_t flags, MaterialPhase tag) {
		return (flags & static_cast<uint32_t>(tag)) != 0;
	}


	enum class PhysicsField {
		FIELD_POSITION = 0,
		FIELD_VELOCITY = 1,
		FIELD_DENSITY = 2,
		FIELD_PRESSURE = 3,
		FIELD_TEMPERATURE = 4,
		FIELD_STRESS = 5,
		FIELD_PLASTICITY = 6
	};

	enum class ColorMap {
		COLORMAP_INFERNO = 0,
		COLORMAP_BLACKBODY = 1,
		COLORMAP_PLASMA = 2,
		COLORMAP_VIRIDIS = 3,
		COLORMAP_WARMCOOL = 4,
		COLORMAP_PARULA = 5,
		COLORMAP_JET = 6,
		COLORMAP_BLUE = 7
	};


	enum NNS_Stages {
		NNS_STEP_0 = 0, //local prefix sum
		NNS_STEP_1 = 1, //parallel redux
		NNS_STEP_2 = 2, //parallel redux
		NNS_STEP_3 = 3, //final pass
		CLEAR = 4, //final pass
	};

	enum SolverStages {
		INIT = 0,
		COPY = 1,
		BIN = 2,
		SORT = 3,
		SPAWN = 4,
		SOLVER_STEP_1 = 5,
		SOLVER_STEP_2 = 6,
		SOLVER_STEP_3 = 7,
		SOLVER_STEP_4 = 8,
		SOLVER_STEP_5 = 9,
		SOLVER_STEP_6 = 10,
	};

	enum class PressureSolver {
		WCSPH, //Weakly compressible SPH (force based)
		PBF	   //Position based fluids
	};

	enum class ViscositySolver {
		XSPH //artificial viscosity
	};

	enum class SoftBodySolver {
		MMC_SPH, //Continuum mechanics SPH (force based & co-rotational formulation)
		PBD_WDC, //PBD WDC(Position based weighted distance constraint)
		MMC_PBD, //MMC based PBD constraint & co-rotational formulation
	};
	enum class RigidBodySolver {
		SHAPE_MATCHING //PBD shape matching constraint
	};
	enum class GranularBodySolver {
		DEM,//DEM force based
		PBD_DC //PBD positive distance constraints
	};

}