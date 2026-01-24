#pragma once

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <string>
#include "GBuffer.h"

enum Teleport {
	Teleport0,
	Teleport1,
	Teleport2,
	notSpecified
};


class MyImGuiPanel
{
public:
	MyImGuiPanel();
	virtual ~MyImGuiPanel();

public:
	void update();
	void setAvgFPS(const double avgFPS);
	void setAvgFrameTime(const double avgFrameTime);

	Teleport getTeleport() { return m_teleport; };
	void resetTeleport() { m_teleport = notSpecified; };
	GBuffer::OutputType getGBUFFER() { return m_gBuffer; };
	bool getIsNormalMapping() { return m_isNormalMapping; };

private:
	void teleportGUI();
	void gBufferGUI();
	void normalMappingGUI();

private:
	double m_avgFPS;
	double m_avgFrameTime;

	Teleport m_teleport;
	GBuffer::OutputType m_gBuffer;
	bool m_isNormalMapping;
};

