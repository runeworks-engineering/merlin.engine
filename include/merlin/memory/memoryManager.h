#pragma once
#include "merlin/core/core.h"

#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include "merlin/memory/bufferObject.h"


namespace Merlin {
    class MemoryManager {
        SINGLETON(MemoryManager)
            MemoryManager() { initializeAvailableBindingPoints(); }


    public:
        void registerBuffer(AbstractBufferObject_Ptr buf);

        GLuint allocateBindingPoint(BufferTarget bufferTarget, GLuint bufferID);
        void releaseBindingPoint(BufferTarget bufferTarget, GLuint bindingPoint);
        const std::vector<GLuint>& getUsedBindingPoints(BufferTarget bufferTarget) const;
        void resetBindings();



        template<typename T>
        AbstractBufferObject_Ptr createBuffer(const std::string& name, GLsizeiptr size);

        template<typename T>
        void writeBuffer(const std::string& name, std::vector<T> data);


        void clearBuffer(const std::string& name);
        bool hasBuffer(const std::string& name) const;
        AbstractBufferObject_Ptr getBuffer(const std::string& key);

        void printLimits();

    private:
        void initializeAvailableBindingPoints();

        std::unordered_map<std::string, AbstractBufferObject_Ptr> m_buffers;

        std::unordered_map<BufferTarget, std::queue<GLuint>> availableBindingPoints;
        std::unordered_map<BufferTarget, std::vector<GLuint>> usedBindingPoints;
        std::unordered_map<GLuint, GLuint> bufferToBindingPoint; // Maps buffer ID to binding point

    };



}


#include "merlin/memory/memoryManager.inl"