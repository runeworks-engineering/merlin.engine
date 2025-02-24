#include "pch.h"
#include "merlin/physics/nns.h"
#include "merlin/utils/primitives.h"
#include "merlin/utils/ressourceManager.h"

namespace Merlin {
    NNS::NNS() {}

	void NNS::init(BoundingBox domain, float cellsize) {
        m_domain = domain;
        m_cell_size = cellsize;

        computeThreadLayout();

        m_maxBound = glm::uvec3(m_domain.size / m_cell_size);
        m_prefixSum = ComputeShaderLibrary::instance().getStagedComputeShader("prefix.sum");
        setConstants();
        m_prefixSum->compile();

        shared<Mesh> binInstance = Primitives::createQuadCube(m_cell_size, false);
        binInstance->rename("bin");
        m_bins = ParticleSystem::create("bin_system", m_thread);
        m_bins->setDisplayMode(ParticleSystemDisplayMode::MESH);
        m_bins->setMesh(binInstance);
        m_bins->enableWireFrameMode();

        m_bin_buffer = SSBO<Bin>::create("bin_buffer", m_thread);
        Console::info() << "Bin struct size :" << sizeof(Bin) << Console::endl;
        Console::info() << "Bin buffer size :" << int(m_bin_buffer->size()) << Console::endl;
        m_bins->addBuffer<Bin>("bin_buffer", m_thread);

        m_prefixSum->SetWorkgroupLayout(m_bWkgCount);
        m_prefixSum->use();
        m_prefixSum->setUInt("dataSize", m_thread); //data size
        m_prefixSum->setUInt("blockSize", m_blockSize); //block size

		m_ready = true;
		m_active = true;
	}

    SSBO_Ptr<Bin> NNS::getBuffer() {
        return m_bin_buffer;
    }

	void NNS::sort(StagedComputeShader_Ptr solver){
        if (!m_active) return;
        errorNotReady();

        m_prefixSum->use();
        m_prefixSum->setUInt("dataSize", m_thread); //data size
        m_prefixSum->setUInt("blockSize", m_blockSize); //block size

        m_prefixSum->use();
        m_prefixSum->execute(NNS_Stages::CLEAR);// clear bins

        solver->use();
        solver->execute(SolverStages::BIN); //Place particles in bins

        m_prefixSum->use();
        m_prefixSum->execute(NNS_Stages::NNS_STEP_0);// local prefix sum

        //Binary tree on rightmost element of blocks
        GLuint steps = m_blockSize;
        Uniform<GLuint> space("space");
        space.value() = 1;

        for (GLuint step = 0; step < steps; step++) {
            // Calls the parallel operation

            space.sync(*m_prefixSum);
            m_prefixSum->execute(NNS_Stages::NNS_STEP_1);
            m_prefixSum->execute(NNS_Stages::NNS_STEP_2);

            space.value() *= 2;
        }
        m_prefixSum->execute(NNS_Stages::NNS_STEP_3);

        solver->use();
        solver->execute(SolverStages::SORT); //Sort
	}

	void NNS::clean() {
        m_ready = false;
	}

	void NNS::setWkgSize(GLuint size) {
        bWkgSize = size;
        warmUnstagedChanges();
	}

    float NNS::getCellSize() {
        return m_cell_size;
    }

    glm::uvec3 NNS::getBound() {
        return m_maxBound;
    }

    GLuint NNS::getBinCount() {
        return m_thread;
    }


    ParticleSystem_Ptr NNS::getParticleSystem() {
        return m_bins;
    }

	void NNS::computeThreadLayout() {
		m_thread =  int(m_domain.size.x / (m_cell_size)) *
                    int(m_domain.size.y / (m_cell_size)) *
                    int(m_domain.size.z / (m_cell_size)); //Total number of bin (thread)

		m_blockSize = floor(log2f(m_thread));
		m_blocks = (m_thread + m_blockSize - 1) / m_blockSize;
		m_bWkgCount = (m_blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	}

    void NNS::warmUnstagedChanges() {
        if (m_ready) {
            Console::warn("NNS") << "You've commited changes after NNS initilization. Please restart the engine to stage the changes and update the NNS grid" << Console::endl;
            return;
        }
    }

    void NNS::errorNotReady() {
        if (!m_ready) {
            m_active = false;
            Console::error("NNS") <<
                "NNS is not ready." <<
                "Please init before sorting." << Console::endl <<
                "NNS has been disabled" << Console::endl;
            return;
        }
    }

    void NNS::setConstants() {
        if (!m_prefixSum) return;
        m_prefixSum->define("BTHREAD", std::to_string(bWkgSize));
    }
}