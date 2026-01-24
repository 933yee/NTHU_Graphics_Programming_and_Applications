#pragma once
#include <vector>
#include <Rendering_Framework/src/Scene/LoadModel.h>
#include <glad/glad.h>
#include <Rendering_Framework/src/Rendering/Camera/Camera.h>

namespace INANOA {
	class Grass {
	public:
		Grass();
		Grass(std::string);
		~Grass();
		void init(int idx);
		void render();
		void update(const Camera* camera);
		void setupInstancing(const int num, float** position);
		void bindIBO();

	public:
		std::string modelName;
		std::vector<Shape> shapes;
		std::vector<Material> materials;
		glm::mat4 m_modelMat;
		GLuint m_instanceVBO;
	};
}