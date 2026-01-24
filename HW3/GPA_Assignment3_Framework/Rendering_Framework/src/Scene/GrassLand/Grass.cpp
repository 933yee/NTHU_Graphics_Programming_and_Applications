#pragma once
#include <vector>
#include <Rendering_Framework/src/Scene/GrassLand/Grass.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>

namespace INANOA {
	Grass::Grass() {}
	Grass::Grass(std::string modelName) {
		this->modelName = modelName;
	}
	Grass::~Grass() {}

	void Grass::init(int idx) {
		//const aiScene* grass = aiImportFile("./models/foliages/grassB.obj", aiProcessPreset_TargetRealtime_MaxQuality);
		std::string path = "./models/foliages/" + this->modelName + ".obj";
		const aiScene* grass = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		if (grass == nullptr) {
			printf("YOU SUCK, scene is empty");
		}

		// materials
		for (unsigned int i = 0; i < grass->mNumMaterials; ++i)
		{
			aiMaterial* material = grass->mMaterials[i];
			Material mat;
			aiString texturePath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
			{
				char newPath[256];
				snprintf(newPath, sizeof(newPath), "./textures/%s", texturePath.C_Str());
				texture_data img_data = loadImg(newPath);
				// load width, height and data from texturePath.C_Str();
				if (img_data.data)
				{
					if (idx == 3) {
						glGenTextures(1, &mat.diffuse_tex);
						glBindTexture(GL_TEXTURE_2D, mat.diffuse_tex);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img_data.width, img_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data.data);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						glGenerateMipmap(GL_TEXTURE_2D);
					}
					else {
						glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, idx, 1024, 1024, 1, GL_RGBA, GL_UNSIGNED_BYTE, img_data.data);
					}

					free(img_data.data);
				}
				else
				{
					printf("Failed to load texture: %s\n", texturePath.C_Str());
				}
			}
			materials.push_back(mat);
		}

		// vertices
		for (unsigned int i = 0; i < grass->mNumMeshes; ++i) {
			aiMesh* mesh = grass->mMeshes[i];
			Shape shape;
			/*glGenVertexArrays(1, &shape.vao);
			glBindVertexArray(shape.vao);*/

			glGenBuffers(1, &shape.vbo_position);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), &mesh->mVertices[0], GL_STATIC_DRAW);

			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//glEnableVertexAttribArray(0);

			if (mesh->HasNormals()) {
				glGenBuffers(1, &shape.vbo_normal);
				glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
				glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), &mesh->mNormals[0], GL_STATIC_DRAW);
				//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				//glEnableVertexAttribArray(2);
			}

			if (mesh->HasTextureCoords(0)) {
				glGenBuffers(1, &shape.vbo_texcoord);
				glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
				glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D), &mesh->mTextureCoords[0][0], GL_STATIC_DRAW);
				//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
				//glEnableVertexAttribArray(1);
			}

			glGenBuffers(1, &shape.ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
			std::vector<GLuint> indices;
			for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
				aiFace face = mesh->mFaces[j];
				for (unsigned int k = 0; k < face.mNumIndices; k++) {
					indices.push_back(face.mIndices[k]);
				}
			}
			shape.drawCount = indices.size();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.drawCount * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

			shape.materialID = mesh->mMaterialIndex;
			shapes.push_back(shape);
		}
		aiReleaseImport(grass);
	}

	void Grass::update(const Camera* camera) {
		const glm::vec3 viewPos = camera->viewOrig();
		const glm::mat4 viewMat = camera->viewMatrix();
		const float height = 0.0f;
		glm::mat4 tMat = glm::translate(glm::vec3(viewPos.x, height, viewPos.z));
		glm::mat4 viewT = glm::transpose(viewMat);
		glm::vec3 forward = -1.0f * glm::vec3(viewT[2].x, 0.0, viewT[2].z);
		glm::vec3 y(0.0, 1.0, 0.0);
		glm::vec3 x = glm::normalize(glm::cross(y, forward));

		glm::mat4 rMat;
		rMat[0] = glm::vec4(x, 0.0);
		rMat[1] = glm::vec4(y, 0.0);
		rMat[2] = glm::vec4(forward, 0.0);
		rMat[3] = glm::vec4(0.0, 0.0, 0.0, 1.0);

		this->m_modelMat = tMat * rMat;
	}

	void Grass::render() {
		glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, materials[shapes[0].materialID].diffuse_tex);
		//glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
		//glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, 1, 0);
	}
}