#pragma once

#include <vector>
#include "Shader.h"
#include "GBuffer.h"
#include "SceneManager.h"
#include "DynamicSceneObject.h"
#include "terrain\TerrainSceneObject.h"
#include "airplane\AirplaneSceneObject.h"
#include "magicStone\MagicStoneSceneObject.h"


class SceneRenderer
{
public:
	SceneRenderer();
	virtual ~SceneRenderer();

private:
	ShaderProgram *m_shaderProgram = nullptr;
	glm::mat4 m_projMat;
	glm::mat4 m_viewMat;
	int m_frameWidth;
	int m_frameHeight;	

	std::vector<DynamicSceneObject*> m_dynamicSOs;
	TerrainSceneObject* m_terrainSO = nullptr;
	AirplaneSceneObject* m_airplaneSO = nullptr;
	MagicStoneSceneObject* m_magicStoneSO = nullptr;

	bool m_isNormalMapping;


public:
	void resize(const int w, const int h);
	bool initialize(const int w, const int h, ShaderProgram* shaderProgram);

	void setProjection(const glm::mat4 &proj);
	void setView(const glm::mat4 &view);
	void setViewport(const int x, const int y, const int w, const int h);
	void appendDynamicSceneObject(DynamicSceneObject *obj);
	void appendTerrainSceneObject(TerrainSceneObject* tSO);
	void appendAirplaneSceneObject(AirplaneSceneObject* aSO);
	void appendMagicStoneSceneObject(MagicStoneSceneObject* mSO);

	void setIsNormalMapping(bool isNormalMapping);

// pipeline
public:
	void startNewFrame();
	void renderPass();

private:
	void clear(const glm::vec4 &clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0), const float depth = 1.0);
	bool setUpShader();
};

