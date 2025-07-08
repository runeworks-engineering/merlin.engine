#include "pch.h"
#include "merlin/memory/memoryManager.h"

namespace Merlin {
    void MemoryManager::registerBuffer(AbstractBufferObject_Ptr buf){
        if (buf)
            m_buffers[buf->name()] = buf;
        else Console::error("MemoryManager") << "Null buffer cannot be registered, instantiate buffer before trying to register it, or create it through the memory manager" << Console::endl;
    }

    GLuint MemoryManager::allocateBindingPoint(BufferTarget bufferType, GLuint bufferID) {
        // If already assigned, return it
        if (bufferToBindingPoint.find(bufferID) != bufferToBindingPoint.end()) {
            return bufferToBindingPoint[bufferID];
        }

        auto& available = availableBindingPoints[bufferType];
        auto& used = usedBindingPoints[bufferType];

        if (available.empty()) {
            Console::error("MemoryManager") << "No available binding points for the given buffer type" << Console::endl;
            throw std::runtime_error("No available binding points for the given buffer type");
        }

        // Always grab the lowest available binding point
        GLuint bindingPoint = *available.begin();
        available.erase(available.begin());
        used.insert(bindingPoint);
        bufferToBindingPoint[bufferID] = bindingPoint;

        Console::trace("MemoryManager") << bindingPoint << " allocated for buffer " << bufferID << Console::endl;
        return bindingPoint;
    }

    void MemoryManager::releaseBindingPoint(BufferTarget bufferType, GLuint bufferID) {
        auto it = bufferToBindingPoint.find(bufferID);
        if (it == bufferToBindingPoint.end()) {
            Console::trace("MemoryManager") << "Attempt to release a binding point for a buffer that was not allocated" << Console::endl;
            return;
        }

        GLuint bindingPoint = it->second;
        bufferToBindingPoint.erase(it);

        auto& available = availableBindingPoints[bufferType];
        auto& used = usedBindingPoints[bufferType];

        used.erase(bindingPoint);
        available.insert(bindingPoint); // O(log n), minimal

        Console::trace("MemoryManager") << bindingPoint << " freed from buffer " << bufferID << Console::endl;
    }

    std::vector<GLuint> MemoryManager::getUsedBindingPoints(BufferTarget bufferType) const {
        // Return as vector for compatibility
        std::vector<GLuint> result;
        const auto& used = usedBindingPoints.at(bufferType);
        result.insert(result.end(), used.begin(), used.end());
        return result;
    }

    void MemoryManager::resetBindings() {
        usedBindingPoints.clear();
        bufferToBindingPoint.clear();
        availableBindingPoints.clear();
        initializeAvailableBindingPoints();
    }

    void MemoryManager::printLimits() {
        GLint maxSSBOBindings;
        GLint maxUBOBindings;
        GLint maxACBBindings;

        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings);
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUBOBindings);
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &maxACBBindings);

        Console::info("MemoryManager") << "OpenGL Binding Point Limits: " << Console::endl;
        Console::info("MemoryManager") << "Max SSBO bindings: " << maxSSBOBindings << Console::endl;
        Console::info("MemoryManager") << "Max UBO bindings: " << maxUBOBindings << Console::endl;
        Console::info("MemoryManager") << "Max Atomic Counter Buffer bindings: " << maxACBBindings << Console::endl;
    }

    void MemoryManager::initializeAvailableBindingPoints() {
        static bool init = false;

        static GLint maxSSBOBindings = 16;
        static GLint maxUBOBindings = 16;
        static GLint maxVBOBindings = 16; // Arbitrary for VBO
        static GLint maxEBOBindings = 16; // Arbitrary for EBO
        static GLint maxACBBindings = 0;

        if (!init) {
            glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings);
            glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUBOBindings);
            glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &maxACBBindings);
            init = true;
        }

        for (int i = 0; i < maxSSBOBindings; ++i)
            availableBindingPoints[BufferTarget::Shader_Storage_Buffer].insert(i);
        for (int i = 0; i < maxUBOBindings; ++i)
            availableBindingPoints[BufferTarget::Uniform_Buffer].insert(i);
        for (int i = 0; i < maxVBOBindings; ++i)
            availableBindingPoints[BufferTarget::Array_Buffer].insert(i);
        for (int i = 0; i < maxEBOBindings; ++i)
            availableBindingPoints[BufferTarget::Element_Array_Buffer].insert(i);
        for (int i = 0; i < maxACBBindings; ++i)
            availableBindingPoints[BufferTarget::Atomic_Counter_Buffer].insert(i);
        // Extend for other types if needed
    }

    AbstractBufferObject_Ptr MemoryManager::getBuffer(const std::string& key) {
        if (!hasBuffer(key)) 
            Console::error("Sim") << "has no buffer named " << key << Console::endl;
        return m_buffers[key];
    }


    bool MemoryManager::hasBuffer(const std::string& name) const {
        return m_buffers.find(name) != m_buffers.end();
    }

    void MemoryManager::clearBuffer(const std::string& name) {
        if (hasBuffer(name)) {
            m_buffers[name]->clearBuffer();
        }
        else Console::error("ParticleSystem") << name << " is not registered in the particle system." << Console::endl;
    }

}