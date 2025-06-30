#include "memoryManager.h"
#include "merlin/memory/ssbo.h"

namespace Merlin {

    template<typename T>
    AbstractBufferObject_Ptr MemoryManager::createBuffer(const std::string& name, GLsizeiptr size) {
        m_buffers[name] = Merlin::ShaderStorageBuffer<T>::create(name, size);
        return m_buffers[name];
    }


    template<typename T>
    void MemoryManager::writeBuffer(const std::string& name, std::vector<T> data) {
        if (hasBuffer(name)) {
            if (m_buffers[name]->elements() < data.size()) {
                //Console::error("ParticleSystem") << "Field hasn't been allocated" << Console::endl;
                m_buffers[name]->resizeBuffer(data.size() * sizeof(T));
            }
            m_buffers[name]->writeBuffer(data.size() * sizeof(T), data.data());
        }
        else Console::error("Sim") << name << " is not registered in the particle system." << Console::endl;
    }



}