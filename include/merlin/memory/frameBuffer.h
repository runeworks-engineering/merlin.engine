#pragma once
#include "merlin/core/core.h"

#include "merlin/textures/texture.h"
#include "merlin/textures/textureMultisampled.h"
#include "merlin/memory/renderBuffer.h"

namespace Merlin {
    class PixelRenderer;
    class FrameBuffer{
    public:
        FrameBuffer(int width, int height);
        ~FrameBuffer();

        void bind(GLenum target = GL_FRAMEBUFFER);
        void unbind();

        void resize(GLsizei width, GLsizei height);
        void print() const;

        inline GLuint const id() { return _FrameBufferID; }
         
        void setDrawBuffer(std::vector<unsigned int> buffers = std::vector<unsigned int>());

        void attachColorTexture(shared<TextureBase> tex);
        void attachDepthStencilRBO(shared<RenderBuffer> rbo);
        void attachDepthStencilTexture(shared<TextureBase> tex);


        void renderAttachement(GLuint id);

        inline int width() const { return _width; }
        inline int height() const{ return _height; }

        static std::shared_ptr<FrameBuffer> create(int width, int height);
        
    private:

        void checkErrors(std::string prefix) const;
        std::vector<GLuint> m_attatchments;

        static GLint MaxColorAttachement;

        int _width = 0;
        int _height = 0;

        GLuint _FrameBufferID;
    };

    typedef FrameBuffer FBO;
    typedef shared<FBO> FBO_Ptr;
}
