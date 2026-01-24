#pragma once
#include <Rendering_Framework/src/Scene/GrassLand/Grass.h>
#include <Rendering_Framework/src/Scene/GrassLand/Bush01.h>
#include <Rendering_Framework/src/Rendering/Camera/Camera.h>
#include <Rendering_Framework/src/Scene/Trajectory.h>
#include <unordered_map>
struct DrawElementsIndirectCommand {
	unsigned int count;
	unsigned int instanceCount;
	unsigned int firstIndex;
	unsigned int baseVertex;
	unsigned int baseInstance;
};

struct InstanceProperties {
	glm::vec4 position;
	//float textureLayer;
};


namespace INANOA {
	class GrassLand {
	public:
		GrassLand();
		~GrassLand();

		void init();

		void render();
		void renderSlime();
		void setupModelInstances(
			const std::string& modelName,
			Grass* model,
			const char* samplePath,
			int instanceCount,
			int commandIndex
		);
		void appendMeshData(
			const Shape& shape,
			std::vector<float>& positions,
			std::vector<float>& normals,
			std::vector<float>& texCoords,
			std::vector<GLuint>& indices,
			GLuint baseVertex
		);
		void update(const Camera* camera);

	public:
		struct ModelInfo {
			GLuint rawInstanceDataBuffer;
			GLuint validInstanceDataBuffer;
			int numberOfInstances;
			int commandIndex; 
		};
		//Trajectory trajectory;
		GLuint vaoHandle;
		GLuint vaoHandle2;
		GLuint vaoHandle3;
		GLuint slimeVAO;
		GLuint cmdBufferHandle;
		GLuint textureArrayHandle;

		SCENE::EXPERIMENTAL::Trajectory* trajectory;
		int totalInstances;

		Grass* grass = nullptr;
		float** grassPosition;
		int numberOfGrass;

		Grass* bush01 = nullptr;
		float** bush01Position;
		int numberOfBush01;

		Grass* bush05 = nullptr;
		float** bush05Position;
		int numberOfBush05;
		std::unordered_map<std::string, ModelInfo> modelInfos;

		Grass* slime = nullptr;
	};
}