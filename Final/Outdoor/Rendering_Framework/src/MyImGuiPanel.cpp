#include "MyImGuiPanel.h"



MyImGuiPanel::MyImGuiPanel()
{
	this->m_avgFPS = 0.0;
	this->m_avgFrameTime = 0.0;
	this->m_teleport = notSpecified;
	this->m_gBuffer = GBuffer::OutputType::DEFAULT;
	this->m_isNormalMapping = true;
}


MyImGuiPanel::~MyImGuiPanel()
{
}

void MyImGuiPanel::update() {
	// performance information
	const std::string FPS_STR = "FPS: " + std::to_string(this->m_avgFPS);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FPS_STR.c_str());
	const std::string FT_STR = "Frame: " + std::to_string(this->m_avgFrameTime);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FT_STR.c_str());

	teleportGUI();
	gBufferGUI();
	normalMappingGUI();
	
}

void MyImGuiPanel::setAvgFPS(const double avgFPS){
	this->m_avgFPS = avgFPS;
}
void MyImGuiPanel::setAvgFrameTime(const double avgFrameTime){
	this->m_avgFrameTime = avgFrameTime;
}

void MyImGuiPanel::teleportGUI() {
	ImGui::Text("Teleport");
	if (ImGui::Button("Teleport0"))
		this->m_teleport = Teleport0;
	ImGui::SameLine();
	if (ImGui::Button("Teleport1"))
		this->m_teleport = Teleport1;;
	ImGui::SameLine();
	if (ImGui::Button("Teleport2"))
		this->m_teleport = Teleport2;;
}

void MyImGuiPanel::gBufferGUI() {
	ImGui::Text("Deferred Shading");
	if (ImGui::Button("World Space Vertex"))
		this->m_gBuffer = GBuffer::OutputType::WORLDSPACEVERTEX;
	ImGui::SameLine();
	if (ImGui::Button("World Space Normal"))
		this->m_gBuffer = GBuffer::OutputType::WORLDSPACENORMAL;
	if (ImGui::Button("Ambient"))
		this->m_gBuffer = GBuffer::OutputType::AMBIENT;
	ImGui::SameLine();
	if (ImGui::Button("Diffuse"))
		this->m_gBuffer = GBuffer::OutputType::DIFFUSE;
	ImGui::SameLine();
	if (ImGui::Button("Specular"))
		this->m_gBuffer = GBuffer::OutputType::SPECULAR;
	ImGui::SameLine();
	if (ImGui::Button("Default"))
		this->m_gBuffer = GBuffer::OutputType::DEFAULT;
}

void MyImGuiPanel::normalMappingGUI()
{
	ImGui::Text("Normal Mapping");
	ImGui::Checkbox("Normal Mapping", &this->m_isNormalMapping);
}
