#pragma once

#include <vector>
#include <Rendering_Framework/src/Scene/GrassLand/GrassLand.h>

#include <Rendering_Framework/src/Rendering/RendererBase.h>
#include <Rendering_Framework/src/Scene/RViewFrustum.h>
#include <Rendering_Framework/src/Scene/RHorizonGround.h>

namespace INANOA {
	class RenderingOrderExp
	{
	public:
		RenderingOrderExp();
		virtual ~RenderingOrderExp();

	public:
		bool init(const int w, const int h) ;
		void resize(const int w, const int h) ;
		void update() ;
		void render();
		void setViewOrg(const glm::vec3& offset);
		void rotateView(float rad);
	private:
		SCENE::RViewFrustum* m_viewFrustum = nullptr;
		SCENE::EXPERIMENTAL::HorizonGround* m_horizontalGround = nullptr;
		GrassLand* grassLand = nullptr;


		Camera* m_playerCamera = nullptr;
		Camera* m_godCamera = nullptr;

		glm::vec3 m_cameraForwardMagnitude;
		float m_cameraForwardSpeed;

		int m_frameWidth;
		int m_frameHeight;

		OPENGL::RendererBase* m_renderer = nullptr;
	};

}


