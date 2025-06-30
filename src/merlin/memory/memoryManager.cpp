#include "pch.h"
#include "merlin/memory/memoryManager.h"

namespace Merlin {
    void MemoryManager::registerBuffer(AbstractBufferObject_Ptr buf){
        if (buf)
            m_buffers[buf->name()] = buf;
        else Console::error("MemoryManager") << "Null buffer cannot be registered, instantiate buffer before trying to register it, or create it through the memory manager" << Console::endl;
    }

    GLuint MemoryManager::allocateBindingPoint(BufferTarget bufferType, GLuint bufferID) {
        // Check if the buffer is already assigned a binding point
        if (bufferToBindingPoint.find(bufferID) != bufferToBindingPoint.end()) {
            return bufferToBindingPoint[bufferID];
        }

        if (availableBindingPoints[bufferType].empty()) {
            Console::error("MemoryManager") << "No available binding points for the given buffer type" << Console::endl;
            throw std::runtime_error("No available binding points for the given buffer type");
        }

        GLuint bindingPoint = availableBindingPoints[bufferType].front();
        availableBindingPoints[bufferType].pop();
        usedBindingPoints[bufferType].push_back(bindingPoint);
        bufferToBindingPoint[bufferID] = bindingPoint;

        Console::trace("MemoryManager") << bindingPoint << " allocated for buffer " << bufferID << Console::endl;
        return bindingPoint;
    }

    void MemoryManager::releaseBindingPoint(BufferTarget bufferType, GLuint bufferID) {
        if (bufferToBindingPoint.find(bufferID) == bufferToBindingPoint.end()) {
            Console::trace("MemoryManager") << "Attempt to release a binding point for a buffer that was not allocated" << Console::endl;
            return;
        }

        GLuint bindingPoint = bufferToBindingPoint[bufferID];
        bufferToBindingPoint.erase(bufferID);

        auto& points = usedBindingPoints[bufferType];
        points.erase(std::remove(points.begin(), points.end(), bindingPoint), points.end());

        std::queue<GLuint> tempQueue;
        tempQueue.push(bindingPoint);
        while (!availableBindingPoints[bufferType].empty()) {
            tempQueue.push(availableBindingPoints[bufferType].front());
            availableBindingPoints[bufferType].pop();
        }
        availableBindingPoints[bufferType] = tempQueue;

        Console::trace("MemoryManager") << bindingPoint << " freed from buffer " << bufferID << Console::endl;
    }

    const std::vector<GLuint>& MemoryManager::getUsedBindingPoints(BufferTarget bufferType) const {
        return usedBindingPoints.at(bufferType);
    }

    void MemoryManager::resetBindings(){
        usedBindingPoints.clear();
        bufferToBindingPoint.clear();
        availableBindingPoints.clear();
        initializeAvailableBindingPoints();

    }

    void MemoryManager::printLimits(){
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
        GLint maxSSBOBindings;
        GLint maxUBOBindings;
        GLint maxVBOBindings = 16; // Limite arbitraire pour les VBOs
        GLint maxEBOBindings = 16; // Limite arbitraire pour les EBOs
        GLint maxACBBindings;

        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings);
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUBOBindings);
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &maxACBBindings);

        for (int i = 0; i < maxSSBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Shader_Storage_Buffer].push(i);
        }
        for (int i = 0; i < maxUBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Uniform_Buffer].push(i);
        }
        for (int i = 0; i < maxVBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Array_Buffer].push(i);
        }
        for (int i = 0; i < maxEBOBindings; ++i) {
            availableBindingPoints[BufferTarget::Element_Array_Buffer].push(i);
        }
        for (int i = 0; i < maxACBBindings; ++i) {
            availableBindingPoints[BufferTarget::Atomic_Counter_Buffer].push(i);
        }
        // Add other buffer types if needed
    }


    AbstractBufferObject_Ptr MemoryManager::getBuffer(const std::string& key) {
        if (!hasBuffer(key)) Console::error("Sim") << "has no buffer named " << key << Console::endl;
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