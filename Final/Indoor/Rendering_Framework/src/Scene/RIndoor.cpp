#include "RIndoor.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			Indoor::Indoor(const Camera* camera, const int w, const int h) {
				this->init(camera, w, h);
			}

			Indoor::~Indoor() {
			}

			void Indoor::init(const Camera* camera, const int w, const int h) {
				INANOA::OPENGL::RendererBase* renderer = new INANOA::OPENGL::RendererBase();
				const std::string vsFile = "src\\shader\\vertexShader_indoor.glsl";
				const std::string fsFile = "src\\shader\\fragmentShader_indoor.glsl";

				// create vertex shader and fragment shader
				if (!renderer->init(vsFile, fsFile, w, h)) {
					printf("Failed to initialize Plant shader program.\n");
					return;
				}
				m_renderer = renderer;
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				m_textureHandle = m_renderer->getUniformLocation("tex");
				m_modelMat = glm::mat4(1.0);
				loadScene();
			}

			void Indoor::loadScene() {
				const std::string scenePath = "models\\Grey_White_Room.obj";
				const aiScene* scene = aiImportFile(scenePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
				if (!scene) {
					printf("Error importing model: %s\n", aiGetErrorString());
					return;
				}
				for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
					aiMaterial* aimat = scene->mMaterials[i];
					aiString texture_path;
					Material material;

					// load texture
					if (aimat->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path) == aiReturn_SUCCESS) {
						std::string tmpstr(texture_path.C_Str());
						strncpy_s(texture_path.data, tmpstr.c_str(), 1024);
						texture_path.length = static_cast<unsigned int>(tmpstr.length());
						texture_data tdata = loadImg(texture_path.C_Str());
						glGenTextures(1, &material.diffuse_tex);
						glBindTexture(GL_TEXTURE_2D, material.diffuse_tex);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glGenerateMipmap(GL_TEXTURE_2D);
						delete[] tdata.data;
						material.texUsed = 1;
					}
					else {
						// printf("No texture data\n");
						material.texUsed = 0;
						material.diffuse_tex = 0;
					}
					// load Kd
					aiColor3D diffuseColor(0.5f, 0.5f, 0.5f);
					aimat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
					material.Kd = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
					// load Ka
					aiColor3D ambientColor(0.3f, 0.3f, 0.3f);
					aimat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
					material.Ka = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
					// load Ks
					aiColor3D specularColor(0.5f, 0.5f, 0.5f);
					aimat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
					material.Ks = glm::vec3(specularColor.r, specularColor.g, specularColor.b);
					// load Ns
					ai_real shininess;
					aimat->Get(AI_MATKEY_SHININESS, shininess);
					material.Ns = shininess;

					materials.push_back(material);
				}

				// load geometry
				std::vector<float> vertices, texcoords, normals;
				std::vector<unsigned int> indices;
				for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
					Shape shape;
					aiMesh* mesh = scene->mMeshes[i];
					vertices.clear();
					texcoords.clear();
					normals.clear();
					indices.clear();
					for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
						vertices.push_back(mesh->mVertices[j][0]);
						vertices.push_back(mesh->mVertices[j][1]);
						vertices.push_back(mesh->mVertices[j][2]);
						normals.push_back(mesh->mNormals[j][0]);
						normals.push_back(mesh->mNormals[j][1]);
						normals.push_back(mesh->mNormals[j][2]);

						if (mesh->HasTextureCoords(0)) {
							texcoords.push_back(mesh->mTextureCoords[0][j][0]);
							texcoords.push_back(mesh->mTextureCoords[0][j][1]);
						}
						else {
							texcoords.push_back(0.0f);
							texcoords.push_back(0.0f);
						}
					}
					for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
						const aiFace& face = mesh->mFaces[j];
						if (face.mNumIndices != 3) {
							// printf("Non-triangle face detected. Face[%u] has %u indices.\n", j, face.mNumIndices);
							continue; 
						}
						indices.push_back(face.mIndices[0]);
						indices.push_back(face.mIndices[1]);
						indices.push_back(face.mIndices[2]);
					}

					shape.materialID = mesh->mMaterialIndex;
					shape.drawCount = mesh->mNumFaces * 3;

					glGenVertexArrays(1, &shape.vao);
					glBindVertexArray(shape.vao);

					glEnableVertexAttribArray(3);
					glEnableVertexAttribArray(4);
					glEnableVertexAttribArray(5);

					glGenBuffers(1, &shape.vbo_position);
					glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
					glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
					glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

					glGenBuffers(1, &shape.vbo_texcoord);
					glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
					glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
					glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, 0);

					glGenBuffers(1, &shape.vbo_normal);
					glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
					glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
					glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);

					glGenBuffers(1, &shape.ibo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

					shapes.push_back(shape);
				}
				glBindVertexArray(0);
				aiReleaseImport(scene);
			}

			void Indoor::render(const Camera* camera, bool useBloom) {
				m_renderer->useProgram();
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
				glUniform1i(SHADER_PARAMETER_BINDING::USE_BLOOM_LOCATION, useBloom);
				glUniform1i(m_textureHandle, 0);
				glActiveTexture(GL_TEXTURE0);
				for (int i = 0; i < shapes.size(); i++) {
					glBindVertexArray(shapes[i].vao);
					int mid = shapes[i].materialID;
					glBindTexture(GL_TEXTURE_2D, materials[mid].diffuse_tex);
					glUniform1i(SHADER_PARAMETER_BINDING::TEX_USED_LOCATION, materials[mid].texUsed);
					glUniform3fv(SHADER_PARAMETER_BINDING::Kd_LOCATION, 1, &materials[mid].Kd[0]);
					glUniform3fv(SHADER_PARAMETER_BINDING::Ka_LOCATION, 1, &materials[mid].Ka[0]);
					glUniform3fv(SHADER_PARAMETER_BINDING::Ks_LOCATION, 1, &materials[mid].Ks[0]);
					glUniform1f(SHADER_PARAMETER_BINDING::Ns_LOCATION, materials[mid].Ns);
					glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				}
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			void Indoor::renderMVP(const Camera* camera) {
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
				for (int i = 0; i < shapes.size(); i++) {
					glBindVertexArray(shapes[i].vao);
					glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				}
				glBindVertexArray(0);
			}

			void Indoor::renderGBuffer(const Camera* camera) {
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
				glActiveTexture(GL_TEXTURE0);
				glUniform1i(SHADER_PARAMETER_BINDING::IS_SPHERE_LOCATION, false);
				for (int i = 0; i < shapes.size(); i++) {
					glBindVertexArray(shapes[i].vao);
					int mid = shapes[i].materialID;
					glBindTexture(GL_TEXTURE_2D, materials[mid].diffuse_tex);
					glUniform1i(SHADER_PARAMETER_BINDING::TEX_USED_LOCATION, materials[mid].texUsed);
					glUniform3fv(SHADER_PARAMETER_BINDING::Kd_LOCATION, 1, &materials[mid].Kd[0]);
					glUniform3fv(SHADER_PARAMETER_BINDING::Ks_LOCATION, 1, &materials[mid].Ks[0]);
					glUniform3fv(SHADER_PARAMETER_BINDING::Ka_LOCATION, 1, &materials[mid].Ka[0]);
					glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				}
				glBindVertexArray(0);
			}

			void Indoor::update(const Camera* camera) {
				this->m_viewMat = camera->viewMatrix();
				this->m_projMat = camera->projMatrix();

				/*
				const glm::vec3 viewPos = camera->viewOrig();
				const glm::mat4 viewMat = camera->viewMatrix();

				glm::mat4 tMat = glm::translate(glm::vec3(viewPos.x, 0.0, viewPos.z));
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
				*/
			}

			void Indoor::resize(const int w, const int h) {
			}
		}
	}
}
