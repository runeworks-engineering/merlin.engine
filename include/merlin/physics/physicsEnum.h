#pragma once

namespace Merlin {
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