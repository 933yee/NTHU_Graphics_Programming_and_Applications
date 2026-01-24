#include "RenderingOrderExp.h"

namespace INANOA
{

	// ===========================================================
	RenderingOrderExp::RenderingOrderExp()
	{
		this->m_cameraForwardSpeed = 0.0025f;
		this->m_cameraForwardMagnitude = glm::vec3(0.0f, 0.0f, 0.0f);
		this->m_cameraRotateSpeed = 0.005f;
		this->m_cameraRotateMagnitude = 0.0;
		this->m_frameWidth = 64;
		this->m_frameHeight = 64;
	}
	RenderingOrderExp::~RenderingOrderExp() {}

	bool RenderingOrderExp::init(const int w, const int h)
	{
		INANOA::OPENGL::RendererBase *renderer = new INANOA::OPENGL::RendererBase();
		const std::string vsFile = "src\\shader\\vertexShader_ogl_450.glsl";
		const std::string fsFile = "src\\shader\\fragmentShader_ogl_450.glsl";
		if (renderer->init(vsFile, fsFile, w, h) == false)
		{
			return false;
		}
		this->m_renderer = renderer;

		this->m_deferredFB = new OPENGL::FrameBuffer(w, h);
		this->m_nprFB = new OPENGL::FrameBuffer(w, h);
		this->m_bloomFB[0] = new OPENGL::FrameBuffer(w, h);
		this->m_bloomFB[1] = new OPENGL::FrameBuffer(w, h);

		this->m_playerCamera = new Camera(glm::vec3(4.0f, 1.0f, -1.5f), glm::vec3(3.0f, 1.0f, -1.5f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0, 60.0f, 0.01f, 150.0f);
		this->m_playerCamera->resize(w, h);
		this->m_playerCamera->update();

		this->m_bloom = new SCENE::EXPERIMENTAL::Bloom(m_playerCamera, w, h);
		this->m_indoor = new SCENE::EXPERIMENTAL::Indoor(m_playerCamera, w, h);
		this->m_trice = new SCENE::EXPERIMENTAL::Trice(m_playerCamera, w, h);
		this->m_GBuffer = new SCENE::EXPERIMENTAL::GBuffer(m_playerCamera, w, h);
		this->m_ShadowMapping = new SCENE::EXPERIMENTAL::ShadowMapping(m_playerCamera, w, h);
		this->m_DeferredShading = new SCENE::EXPERIMENTAL::DeferredShading(m_playerCamera, w, h);
		this->m_SSAO = new SCENE::EXPERIMENTAL::SSAO(w, h);
		this->m_FXAA = new SCENE::EXPERIMENTAL::FXAA(w, h);
		this->m_NPR = new SCENE::EXPERIMENTAL::NPR(w, h);
		this->m_volumetricLight = new SCENE::EXPERIMENTAL::VolumetricLight(w, h);
		this->m_PointShadowMapping = new OPENGL::DepthCubeMap();

		this->resize(w, h);

		initializeMouseAngles();

		return true;
	}

	void RenderingOrderExp::initializeMouseAngles()
	{
		glm::vec3 lookCenter = m_playerCamera->lookCenter();
		glm::vec3 viewOrg = m_playerCamera->viewOrig();
		glm::vec3 direction = glm::normalize(lookCenter - viewOrg);

		// Compute pitch (vertical angle)
		m_mouseVerti = glm::degrees(asin(direction.y));

		// Compute yaw (horizontal angle)
		m_mouseHoriz = glm::degrees(atan2(direction.z, direction.x));
	}

	void RenderingOrderExp::resize(const int w, const int h)
	{
		this->m_playerCamera->resize(w, h);
		m_renderer->resize(w, h);
		this->m_frameWidth = w;
		this->m_frameHeight = h;

		// framebuffer resize
		this->m_deferredFB->resize(w, h);
		this->m_nprFB->resize(w, h);
		this->m_bloomFB[0]->resize(w, h);
		this->m_bloomFB[1]->resize(w, h);

		// class resize
		this->m_FXAA->resize(w, h);
		this->m_GBuffer->resize(w, h);
		this->m_DeferredShading->resize(w, h);
		this->m_SSAO->resize(w, h);
		this->m_NPR->resize(w, h);
	}
	void RenderingOrderExp::update()
	{
		mouseDrag();
		this->m_playerCamera->forward(this->m_cameraForwardMagnitude, false);
		this->m_playerCamera->rotateLookCenterAccordingToViewOrg(m_cameraRotateMagnitude);
		this->m_playerCamera->update();

		this->m_indoor->update(this->m_playerCamera);
	}
	void RenderingOrderExp::render()
	{
		// Render to G-Buffer
		m_GBuffer->beginRender();
		m_ShadowMapping->renderGBuffer();
		m_indoor->renderGBuffer(m_playerCamera);
		m_trice->renderGBuffer(m_playerCamera, gui_interface.toggleNormalMapping);
		if (gui_interface.togglePointLight) {
			m_bloom->renderGBuffer(m_playerCamera, gui_interface.pointLightPosition);
		}
		m_GBuffer->endRender();

		// Depth texture (camera space)
		m_SSAO->beginRenderDepth();
		m_indoor->renderMVP(m_playerCamera);
		m_trice->renderMVP();
		m_SSAO->endRenderDepth();

		// Depth texutre (light space)
		m_ShadowMapping->beginRender(gui_interface.volumetricLightPosition);
		m_indoor->renderMVP(m_playerCamera);
		m_trice->renderMVP();
		m_ShadowMapping->endRender();

		// Light Scattering Pass
		m_volumetricLight->beginRenderObstacle(m_playerCamera);
		m_indoor->renderMVP(m_playerCamera);
		m_trice->renderMVP();
		m_volumetricLight->endRenderObstacle();

		// Depth texture (point light space)
		m_PointShadowMapping->start_render(gui_interface.pointLightPosition);
		m_indoor->renderMVP(m_playerCamera);
		m_trice->renderMVP();
		m_PointShadowMapping->end_render();

		// Deferred Rendering
		m_deferredFB->bind();
		m_DeferredShading->render(*m_GBuffer, *m_ShadowMapping, m_PointShadowMapping->getFBOtex(), gui_interface, m_playerCamera);
		m_SSAO->render(gui_interface);
		m_volumetricLight->render(m_playerCamera, m_frameWidth, m_frameHeight, gui_interface);
		m_deferredFB->unbind();

		OPENGL::FrameBuffer* m_currentSceneFB = m_deferredFB;

		// NPR
		if (gui_interface.toggleNPR || gui_interface.toggleNPRObjOnly) {
			OPENGL::FrameBuffer* m_edgeFB = m_bloomFB[0];

			m_edgeFB->bind();
			m_NPR->renderEdgeDetection(m_currentSceneFB, m_GBuffer, !gui_interface.toggleNPR);
			m_edgeFB->unbind();

			this->m_nprFB->bind();
			m_NPR->renderEdgeCombine(m_currentSceneFB, m_edgeFB);
			this->m_nprFB->unbind();

			m_currentSceneFB = m_nprFB;
		}

		// Bloom effect
		if (gui_interface.togglePointLight) {
			bool bloomFBIndex = renderBloomEffect(m_currentSceneFB);
			m_currentSceneFB = m_bloomFB[bloomFBIndex];
		}

		// FXAA apply
		m_FXAA->render(m_currentSceneFB, gui_interface.toggleFXAA);
	}

	bool RenderingOrderExp::renderBloomEffect(OPENGL::FrameBuffer* scene_fb) {
		const int count = 4;
		bool horizontal = true;

		// extract the ball
		this->m_bloomFB[!horizontal]->bind();
		m_bloom->renderExtractedBall(scene_fb, m_GBuffer);
		this->m_bloomFB[!horizontal]->unbind();

		// blur effect on light source
		for (unsigned int i = 0; i < count; i++){
			this->m_bloomFB[horizontal]->bind();
			m_bloom->renderBlur(m_bloomFB[!horizontal], horizontal);
			this->m_bloomFB[horizontal]->unbind();
			horizontal = !horizontal;
		}

		// apply bloom effect on the scene
		this->m_bloomFB[horizontal]->bind();
		m_bloom->renderBloom(scene_fb, m_bloomFB[!horizontal]);
		this->m_bloomFB[horizontal]->unbind();

		return horizontal;
	}


	void RenderingOrderExp::move(int front, int right, int up)
	{
		m_cameraForwardMagnitude.x -= right * m_cameraForwardSpeed;
		m_cameraForwardMagnitude.y += up * m_cameraForwardSpeed;
		m_cameraForwardMagnitude.z -= front * m_cameraForwardSpeed;
	}
	void RenderingOrderExp::setMousePos(double x, double y)
	{
		m_mousex = x;
		m_mousey = y;
	}
	void RenderingOrderExp::setDrag(int drag)
	{
		isDragging = drag;
		if (drag)
		{
			m_prevMousex = m_mousex;
			m_prevMousey = m_mousey;
		}
	}

	void RenderingOrderExp::mouseDrag()
	{
		if (isDragging)
		{
			// Calculate the offset since the last mouse position
			float xoffset = m_mousex - m_prevMousex;
			float yoffset = m_prevMousey - m_mousey; // Inverted because y-coordinates are reversed
			m_prevMousex = m_mousex;
			m_prevMousey = m_mousey;

			// Scale the offsets to adjust sensitivity
			xoffset *= m_cameraRotateSpeed * 10.0f;
			yoffset *= m_cameraRotateSpeed * 10.0f;

			// Update horizontal and vertical angles
			m_mouseHoriz += xoffset;
			m_mouseVerti += yoffset;

			// Clamp the vertical angle to prevent flipping
			if (m_mouseVerti > 89.0f)
				m_mouseVerti = 89.0f;
			if (m_mouseVerti < -89.0f)
				m_mouseVerti = -89.0f;

			glm::vec3 front;
			front.x = cos(glm::radians(m_mouseHoriz)) * cos(glm::radians(m_mouseVerti));
			front.y = sin(glm::radians(m_mouseVerti));
			front.z = sin(glm::radians(m_mouseHoriz)) * cos(glm::radians(m_mouseVerti));
			front = glm::normalize(front);

			// Update the camera's look center
			glm::vec3 viewOrg = m_playerCamera->viewOrig();
			glm::vec3 lookCenter = viewOrg + front * m_playerCamera->distance();
			m_playerCamera->setLookCenter(lookCenter);
		}
	}

	void RenderingOrderExp::updateInterface(Interface &new_interface)
	{
		gui_interface = new_interface;
	}

}
