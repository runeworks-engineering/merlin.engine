#pragma once
#include "bufferObject.h"

namespace Merlin {
    class AtomicCounterBuffer : public AbstractBufferObject {
    public:
        AtomicCounterBuffer()
            : AbstractBufferObject(BufferTarget::Atomic_Counter_Buffer) {
        }

        void allocate(GLsizeiptr size, BufferUsage usage) {
            allocateBuffer(size, nullptr, usage);
        }

        void allocate(GLsizeiptr size, const void* data, BufferUsage usage) {
            allocateBuffer(size, data, usage);
        }

        void allocateImmutable(GLsizeiptr size, BufferStorageFlags flags) {
            allocateImmutableBuffer(size, nullptr, flags);
        }

        void allocateImmutable(GLsizeiptr size, const void* data, BufferStorageFlags flags) {
            allocateImmutableBuffer(size, data, flags);
        }

        void bindBase(GLuint index) const {
            glBindBufferBase(static_cast<GLenum>(BufferTarget::Atomic_Counter_Buffer), index, id());
        }

        void bindRange(GLuint index, GLintptr offset, GLsizeiptr size) const {
            glBindBufferRange(static_cast<GLenum>(BufferTarget::Atomic_Counter_Buffer), index, id(), offset, size);
        }
    };
}