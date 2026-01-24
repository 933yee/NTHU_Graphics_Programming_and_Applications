#pragma once
#include <vector>
#include <Rendering_Framework/src/Scene/LoadModel.h>
#include <glad/glad.h>
#include <Rendering_Framework/src/Rendering/Camera/Camera.h>

namespace INANOA {
	class Bush01 {
	public:
		Bush01();
		~Bush01();
		void init();
		void render(const int num);
		void update(const Camera* camera);
		void setupInstancing(const int num, float** position);

	private:
		std::vector<Shape> shapes;
		std::vector<Material> materials;
		glm::mat4 m_modelMat;
		GLuint m_instanceVBO;
	};
}