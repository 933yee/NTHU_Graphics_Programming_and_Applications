#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <Rendering_Framework/src/Scene/GBuffer.h>

struct Interface {
	// toggle effect
	bool toggleNPR;
	bool toggleNPRObjOnly;
	bool toggleFXAA;
	bool toggleNormalMapping;
	bool toggleVolumetricLight;
	// bool toggleAreaLight;
	bool togglePointLight;
	bool toggleSSAO;
	bool toggleOnlySSAO;

	// deferred shading
	GBufferTexture deferredShadingMode;

	// volumetric light
	glm::vec3 volumetricLightPosition;
	bool toggleDirectionalShadow;

	// area light
	// glm::vec3 areaLightColor;
	// float areaLightDegree;

	// point light
	glm::vec3 pointLightPosition;
	bool togglePointShadow;
	Interface() {
		toggleNPR = false;
		toggleNPRObjOnly = false;
		toggleFXAA = false;
		toggleNormalMapping = false;
		toggleVolumetricLight = false;
		toggleSSAO = false;
		toggleOnlySSAO = false;
		togglePointLight = false;

		deferredShadingMode = GBufferTexture::Diffuse;

		volumetricLightPosition = glm::vec3(-2.845, 2.028, -1.293);
		pointLightPosition = glm::vec3(1.87659, 0.4625, 0.103928);
		toggleDirectionalShadow = false;
	}
};