#include "pch.h"
#include "merlin/memory/bindingPointManager.h"

namespace Merlin {

    GLuint BindingPointManager::allocateBindingPoint(BufferTarget bufferType, GLuint bufferID) {
        // Check if the buffer is already assigned a binding point
        if (bufferToBindingPoint.find(bufferID) != bufferToBindingPoint.end()) {
            return bufferToBindingPoint[bufferID];
        }

        if (availableBindingPoints[bufferType].empty()) {
            Console::error("BindingPointManager") << "No available binding points for the given buffer type" << Console::endl;
            throw std::runtime_error("No available binding points for the given buffer type");
        }

        GLuint bindingPoint = availableBindingPoints[bufferType].front();
        availableBindingPoints[bufferType].pop();
        usedBindingPoints[bufferType].push_back(bindingPoint);
        bufferToBindingPoint[bufferID] = bindingPoint;

        Console::trace("BindingPointManager") << bindingPoint << " allocated for buffer " << bufferID << Console::endl;
        return bindingPoint;
    }

    void BindingPointManager::releaseBindingPoint(BufferTarget bufferType, GLuint bufferID) {
        if (bufferToBindingPoint.find(bufferID) == bufferToBindingPoint.end()) {
            Console::trace("BindingPointManager") << "Attempt to release a binding point for a buffer that was not allocated" << Console::endl;
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

        Console::trace("BindingPointManager") << bindingPoint << " freed from buffer " << bufferID << Console::endl;
    }

    const std::vector<GLuint>& BindingPointManager::getUsedBindingPoints(BufferTarget bufferType) const {
        return usedBindingPoints.at(bufferType);
    }

    void BindingPointManager::resetBindings(){
        availableBindingPoints.clear();
        initializeAvailableBindingPoints();
        usedBindingPoints.clear();
        bufferToBindingPoint.clear();
    }

    void BindingPointManager::printLimits(){
        GLint maxSSBOBindings;
        GLint maxUBOBindings;
        GLint maxACBBindings;

        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxSSBOBindings);
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUBOBindings);
        glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &maxACBBindings);

        Console::info("BindingPointManager") << "OpenGL Binding Point Limits: " << Console::endl;
        Console::info("BindingPointManager") << "Max SSBO bindings: " << maxSSBOBindings << Console::endl;
        Console::info("BindingPointManager") << "Max UBO bindings: " << maxUBOBindings << Console::endl;
        Console::info("BindingPointManager") << "Max Atomic Counter Buffer bindings: " << maxACBBindings << Console::endl;
        
    }

    void BindingPointManager::initializeAvailableBindingPoints() {
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


}