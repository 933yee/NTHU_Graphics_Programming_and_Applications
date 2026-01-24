#pragma once

#include <vector>

#include <Rendering_Framework/src/Rendering/RendererBase.h>
#include <Rendering_Framework/src/Rendering/FrameBuffer.h>
#include <Rendering_Framework/src/Rendering/DepthCubeMap.h>
#include <Rendering_Framework/src/Scene/RIndoor.h>
#include <Rendering_Framework/src/Scene/RTrice.h>
#include <Rendering_Framework/src/Scene/RBloom.h>
#include <Rendering_Framework/src/Scene/SSAO.h>
#include <Rendering_Framework/src/Scene/FXAA.h>
#include <Rendering_Framework/src/Scene/NPR.h>
#include <Rendering_Framework/src/Scene/GBuffer.h>
#include <Rendering_Framework/src/Scene/ShadowMapping.h>
#include <Rendering_Framework/src/Scene/VolumetricLight.h>
#include <Rendering_Framework/src/Scene/DeferredShading.h>
#include <Rendering_Framework/src/RenderWidgets/Interface.h>
#include <glm/gtc/matrix_transform.hpp>

namespace INANOA
{
	class RenderingOrderExp
	{
	public:
		RenderingOrderExp();
		virtual ~RenderingOrderExp();

	public:
		bool init(const int w, const int h);
		void resize(const int w, const int h);
		void update();
		void render();
		bool renderBloomEffect(OPENGL::FrameBuffer* scene_fb);
		void move(int front, int right, int up);
		void setMousePos(double x, double y);
		void setDrag(int drag);
		void mouseDrag();
		void initializeMouseAngles();
		void updateInterface(Interface& new_interface);

	private:
		SCENE::EXPERIMENTAL::Indoor *m_indoor = nullptr;
		SCENE::EXPERIMENTAL::Trice *m_trice = nullptr;
		SCENE::EXPERIMENTAL::GBuffer *m_GBuffer = nullptr;
		SCENE::EXPERIMENTAL::Bloom *m_bloom = nullptr;
		SCENE::EXPERIMENTAL::ShadowMapping *m_ShadowMapping = nullptr;
		SCENE::EXPERIMENTAL::DeferredShading* m_DeferredShading = nullptr;
		SCENE::EXPERIMENTAL::SSAO* m_SSAO = nullptr;
		SCENE::EXPERIMENTAL::VolumetricLight* m_volumetricLight = nullptr;
		SCENE::EXPERIMENTAL::FXAA* m_FXAA = nullptr;
		SCENE::EXPERIMENTAL::NPR* m_NPR = nullptr;
		OPENGL::DepthCubeMap* m_PointShadowMapping = nullptr;

		Camera *m_playerCamera = nullptr;

		glm::vec3 m_cameraForwardMagnitude;
		float m_cameraForwardSpeed;
		float m_cameraRotateSpeed;
		float m_cameraRotateMagnitude;

		int m_frameWidth;
		int m_frameHeight;

		double m_mousex;
		double m_prevMousex;
		double m_mousey;
		double m_prevMousey;
		float m_mouseHoriz = 0.0f;
		float m_mouseVerti = 0.0f;
		int isDragging;

		OPENGL::RendererBase *m_renderer = nullptr;
		Interface gui_interface;

		OPENGL::FrameBuffer* m_deferredFB = nullptr;
		OPENGL::FrameBuffer* m_nprFB = nullptr;
		OPENGL::FrameBuffer* m_bloomFB[2] = {nullptr, nullptr};
	};

}
