#pragma once
#include <Rendering_Framework/src/Scene/GrassLand/GrassLand.h>
#include <Rendering_Framework/src/Scene/SpatialSample.h>
#include <glm/gtc/type_ptr.hpp>

namespace INANOA {
	GrassLand::GrassLand() {
		grass = new Grass("grassB");
		bush01 = new Grass("bush01_lod2");
		bush05 = new Grass("bush05_lod2");
		trajectory = new SCENE::EXPERIMENTAL::Trajectory();
		trajectory->enable(true);
		slime = new Grass("slime");
	}
	GrassLand::~GrassLand() {}

	void GrassLand::init() {
		glGenVertexArrays(1, &vaoHandle);
		glGenVertexArrays(1, &vaoHandle2);
		glGenVertexArrays(1, &vaoHandle3);
		glGenVertexArrays(1, &slimeVAO);

		glGenTextures(1, &textureArrayHandle);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 1024, 1024, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		grass->init(0);
		bush01->init(1);
		bush05->init(2);
		slime->init(3);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		this->totalInstances = 155304 + 1010 + 2797;

		// 155304
		INANOA::SCENE::EXPERIMENTAL::SpatialSample* grassSample = INANOA::SCENE::EXPERIMENTAL::SpatialSample::importBinaryFile("./models/spatialSamples/poissonPoints_155304s.ss2");
		this->numberOfGrass = 155304;

		InstanceProperties* rawInsData = new InstanceProperties[this->totalInstances];
		for (int i = 0; i < this->numberOfGrass; i++) {
			const float* POSITION_BUFFER = grassSample->position(i);
			rawInsData[i].position = glm::vec4(POSITION_BUFFER[0], POSITION_BUFFER[1], POSITION_BUFFER[2], 1.0f);
		}

		// 1010
		grassSample = INANOA::SCENE::EXPERIMENTAL::SpatialSample::importBinaryFile("./models/spatialSamples/poissonPoints_1010s.ss2");
		this->numberOfBush01 = 1010;

		for (int i = 0; i < this->numberOfBush01; i++) {
			const float* POSITION_BUFFER = grassSample->position(i);
			rawInsData[this->numberOfGrass + i].position = glm::vec4(POSITION_BUFFER[0], POSITION_BUFFER[1], POSITION_BUFFER[2], 1.0f);
		}

		// 2797
		grassSample = INANOA::SCENE::EXPERIMENTAL::SpatialSample::importBinaryFile("./models/spatialSamples/poissonPoints_2797s.ss2");
		this->numberOfBush05 = 2797;

		for (int i = 0; i < this->numberOfBush05; i++) {
			const float* POSITION_BUFFER = grassSample->position(i);
			rawInsData[this->numberOfGrass + this->numberOfBush01 + i].position = glm::vec4(POSITION_BUFFER[0], POSITION_BUFFER[1], POSITION_BUFFER[2], 1.0f);
		}

		// prepare a SSBO for storing raw instance data
		GLuint rawInstanceDataBufferHandle;
		glGenBuffers(1, &rawInstanceDataBufferHandle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawInstanceDataBufferHandle);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, this->totalInstances * sizeof(InstanceProperties), rawInsData, GL_MAP_READ_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rawInstanceDataBufferHandle);

		// prepare a SSBO for storing VALID instance data
		GLuint validInstanceDataBufferHandle;
		glGenBuffers(1, &validInstanceDataBufferHandle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, validInstanceDataBufferHandle);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, this->totalInstances * sizeof(InstanceProperties), nullptr, GL_MAP_READ_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, validInstanceDataBufferHandle);

		const int numberOfDrawCommands = 3;
		DrawElementsIndirectCommand drawCommands[numberOfDrawCommands];
		drawCommands[0].count = grass->shapes[0].drawCount;
		drawCommands[0].instanceCount = 0;
		drawCommands[0].firstIndex = 0;
		drawCommands[0].baseVertex = 0;
		drawCommands[0].baseInstance = 0;

		drawCommands[1].count = bush01->shapes[0].drawCount;
		drawCommands[1].instanceCount = 0;
		drawCommands[1].firstIndex = 0;
		drawCommands[1].baseVertex = 0;
		drawCommands[1].baseInstance = numberOfGrass;

		drawCommands[2].count = bush05->shapes[0].drawCount;
		drawCommands[2].instanceCount = 0;
		drawCommands[2].firstIndex = 0;
		drawCommands[2].baseVertex = 0;
		drawCommands[2].baseInstance = numberOfGrass + numberOfBush01;

		glGenBuffers(1, &cmdBufferHandle);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, cmdBufferHandle);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, numberOfDrawCommands * sizeof(DrawElementsIndirectCommand), drawCommands, GL_MAP_READ_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cmdBufferHandle);

		glBindVertexArray(vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, grass->shapes[0].vbo_position);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, grass->shapes[0].vbo_texcoord);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, grass->shapes[0].vbo_normal);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, validInstanceDataBufferHandle);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, nullptr);
		glEnableVertexAttribArray(3);
		glVertexAttribDivisor(3, 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grass->shapes[0].ibo);

		glBindVertexArray(vaoHandle2);
		glBindBuffer(GL_ARRAY_BUFFER, bush01->shapes[0].vbo_position);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, bush01->shapes[0].vbo_texcoord);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, bush01->shapes[0].vbo_normal);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, validInstanceDataBufferHandle);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, nullptr);
		glEnableVertexAttribArray(3);
		glVertexAttribDivisor(3, 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bush01->shapes[0].ibo);

		glBindVertexArray(vaoHandle3);
		glBindBuffer(GL_ARRAY_BUFFER, validInstanceDataBufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER, bush05->shapes[0].vbo_position);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, bush05->shapes[0].vbo_texcoord);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, bush05->shapes[0].vbo_normal);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, validInstanceDataBufferHandle);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, nullptr);
		glEnableVertexAttribArray(3);
		glVertexAttribDivisor(3, 1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bush05->shapes[0].ibo);

		glBindVertexArray(slimeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, validInstanceDataBufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER, slime->shapes[0].vbo_position);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, slime->shapes[0].vbo_texcoord);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, slime->shapes[0].vbo_normal);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, slime->shapes[0].ibo);

		glBindVertexArray(0);
	}

	void GrassLand::render() {
		glBindVertexArray(this->vaoHandle);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		glUniform1f(5, 0.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, grass->materials[grass->shapes[0].materialID].diffuse_tex);

		glMultiDrawElementsIndirect(
			GL_TRIANGLES,
			GL_UNSIGNED_INT,
			nullptr,  
			1,     
			sizeof(DrawElementsIndirectCommand)
		);

		glBindVertexArray(this->vaoHandle2);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		glUniform1f(5, 1.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, bush01->materials[bush01->shapes[0].materialID].diffuse_tex);

		glMultiDrawElementsIndirect(
			GL_TRIANGLES,
			GL_UNSIGNED_INT,
			reinterpret_cast<void*>(sizeof(DrawElementsIndirectCommand)),
			1,  
			sizeof(DrawElementsIndirectCommand)
		);

		glBindVertexArray(this->vaoHandle3);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		glUniform1f(5, 2.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandle);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, bush05->materials[bush05->shapes[0].materialID].diffuse_tex);

		glMultiDrawElementsIndirect(
			GL_TRIANGLES,
			GL_UNSIGNED_INT,
			reinterpret_cast<void*>(sizeof(DrawElementsIndirectCommand) * 2),
			1,
			sizeof(DrawElementsIndirectCommand)
		);

		glBindVertexArray(0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	}

	void GrassLand::renderSlime() {
		glBindVertexArray(this->slimeVAO);
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		glUniform4fv(3, 1, glm::value_ptr(trajectory->positionVec4()));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, slime->materials[slime->shapes[0].materialID].diffuse_tex);
		glDrawElements(GL_TRIANGLES, slime->shapes[0].drawCount, GL_UNSIGNED_INT, 0);
	}

	void GrassLand::update(const Camera* camera) {
		//grass->update(camera);
		//bush01->update(camera);
		//bush05->update(camera);
		trajectory->update();
		//slime->update(camera);
	}
}