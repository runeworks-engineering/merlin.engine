#pragma once
#include "merlin/core/core.h"

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <map>
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
        void resetBindings();

        std::vector<GLuint> getUsedBindingPoints(BufferTarget bufferType) const;

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

        std::map<BufferTarget, std::set<GLuint>> availableBindingPoints;
        std::map<BufferTarget, std::unordered_set<GLuint>> usedBindingPoints;
        std::unordered_map<GLuint, GLuint> bufferToBindingPoint;

    };



}


#include "merlin/memory/memoryManager.inl"