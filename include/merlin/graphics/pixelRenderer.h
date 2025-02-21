#pragma once
#include "merlin/core/core.h"

#include "merlin/graphics/rendererBase.h"
#include "merlin/textures/texture.h"
#include "merlin/memory/vao.h"

namespace Merlin {
	class PixelRenderer : public RendererBase {
	public:
		// Constructor
		PixelRenderer();

		// Render the screen quad
		void render();
		void render(const shared<TextureBase>& tex);

		inline VAO& getVAO() { return m_vao; }
		inline Shader& getShader() { return *m_shader; }
		inline void setShader(shared<Shader> shader) { m_shader = shader;  m_shader->use(); m_shader->setInt("screen", 0); }

	private:
		//Empty vertex array object for the binding quad
		VAO m_vao;
		// Shader program for rendering the screen quad
		shared<Shader> m_shader;
	};

	typedef PixelRenderer SQRenderer;
	typedef shared<PixelRenderer> ScreenQuadRenderer_Ptr;
}


