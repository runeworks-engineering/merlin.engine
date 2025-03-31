#include "pch.h"
#include "merlin/memory/bufferObject.h"
#include "merlin/memory/bindingPointManager.h"
#include "merlin/utils/util.h"

namespace Merlin {
 
    void AbstractBufferObject::releaseBindingPoint() {
        if(m_bindingPoint < 16) BindingPointManager::instance().releaseBindingPoint(m_target, m_bindingPoint);
        m_bindingPoint = -1;
        return;
    }

    void AbstractBufferObject::setBindingPoint(GLuint point) {
        m_bindingPoint = point;

        switch (m_target) {
            case BufferTarget::Atomic_Counter_Buffer:
            case BufferTarget::Transform_Feedback_Buffer:
            case BufferTarget::Uniform_Buffer:
            case BufferTarget::Shader_Storage_Buffer:
                glBindBufferBase(static_cast<GLenum>(m_target), m_bindingPoint, id());
            break;
        }
    }

    AbstractBufferObject_Ptr generateBuffer(const std::string& name, BufferTarget target, BufferType type)
    {
        AbstractBufferObject_Ptr buf;
        switch (type) {
        case Merlin::BufferType::Int:
            buf = createShared<BufferObject<GLint>>(target);
            break;
        case Merlin::BufferType::Float:
            buf = createShared<BufferObject<GLfloat>>(target);
            break;
        case Merlin::BufferType::UInt:
            buf = createShared<BufferObject<GLuint>>(target);
            break;
        case Merlin::BufferType::Bool:
            buf = createShared<BufferObject<GLboolean>>(target);
            break;
        case Merlin::BufferType::Double:
            buf = createShared<BufferObject<GLdouble>>(target);
            break;
        case Merlin::BufferType::DVec2:
            buf = createShared<BufferObject<glm::dvec2>>(target);
            break;
        case Merlin::BufferType::DVec3:
            buf = createShared<BufferObject<glm::dvec3>>(target);
            break;
        case Merlin::BufferType::DVec4:
            buf = createShared<BufferObject<glm::dvec4>>(target);
            break;
        case Merlin::BufferType::IVec2:
            buf = createShared<BufferObject<glm::ivec2>>(target);
            break;
        case Merlin::BufferType::IVec3:
            buf = createShared<BufferObject<glm::ivec3>>(target);
            break;
        case Merlin::BufferType::IVec4:
            buf = createShared<BufferObject<glm::ivec4>>(target);
            break;
        case Merlin::BufferType::UVec2:
            buf = createShared<BufferObject<glm::uvec2>>(target);
            break;
        case Merlin::BufferType::UVec3:
            buf = createShared<BufferObject<glm::uvec3>>(target);
            break;
        case Merlin::BufferType::UVec4:
            buf = createShared<BufferObject<glm::uvec4>>(target);
            break;
        case Merlin::BufferType::Vec2:
            buf = createShared<BufferObject<glm::vec2>>(target);
            break;
        case Merlin::BufferType::Vec3:
            buf = createShared<BufferObject<glm::vec3>>(target);
            break;
        case Merlin::BufferType::Vec4:
            buf = createShared<BufferObject<glm::vec4>>(target);
            break;
        case Merlin::BufferType::Mat2:
            buf = createShared<BufferObject<glm::mat2>>(target);
            break;
        case Merlin::BufferType::Mat2x3:
            buf = createShared<BufferObject<glm::mat2x3>>(target);
            break;
        case Merlin::BufferType::Mat2x4:
            buf = createShared<BufferObject<glm::mat2x4>>(target);
            break;
        case Merlin::BufferType::Mat3x2:
            buf = createShared<BufferObject<glm::mat3x2>>(target);
            break;
        case Merlin::BufferType::Mat3:
            buf = createShared<BufferObject<glm::mat3>>(target);
            break;
        case Merlin::BufferType::Mat3x4:
            buf = createShared<BufferObject<glm::mat3x4>>(target);
            break;
        case Merlin::BufferType::Mat4x2:
            buf = createShared<BufferObject<glm::mat4x2>>(target);
            break;
        case Merlin::BufferType::Mat4x3:
            buf = createShared<BufferObject<glm::mat4x3>>(target);
            break;
        case Merlin::BufferType::Mat4:
            buf = createShared<BufferObject<glm::mat4>>(target);
            break;
        default:
            buf = createShared<BufferObject<GLint>>(target);
            break;
        }
        buf->rename(name);
        return buf;
    }

    AbstractBufferObject_Ptr generateBuffer(const std::string& name, BufferTarget target, BufferType type, GLsizeiptr size, void* data)
    {
        AbstractBufferObject_Ptr buf = generateBuffer(name, target, type);
        GLsizeiptr bytes = size * Utils::getBufferTypeSize(type);
        buf->allocateBuffer(bytes, data, BufferUsage::StaticDraw);
    }

    AbstractBufferObject_Ptr generateImmutableBuffer(const std::string& name, BufferTarget, BufferType)
    {
        return AbstractBufferObject_Ptr();
    }

    AbstractBufferObject_Ptr generateImmutableBuffer(const std::string& name, BufferTarget, BufferType, GLsizeiptr size, void* data)
    {
        return AbstractBufferObject_Ptr();
    }
}