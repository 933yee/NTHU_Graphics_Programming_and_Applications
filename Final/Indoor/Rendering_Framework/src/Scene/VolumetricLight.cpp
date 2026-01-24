#include "VolumetricLight.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {

			VolumetricLight::VolumetricLight(const int w, const int h){
				this->init(w, h);
				this->loadModel();
			}

			VolumetricLight::~VolumetricLight() {
				delete m_renderer;
				m_renderer = nullptr;
            }

			void VolumetricLight::loadModel() {
				const std::string scenePath = "models\\Sphere.obj";
				const aiScene* scene = aiImportFile(scenePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
				if (!scene) {
					printf("Error importing model: %s\n", aiGetErrorString());
					return;
				}
				for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
					aiMaterial* aimat = scene->mMaterials[i];
					Material material;

					// printf("No texture data\n");
					material.texUsed = 0;
					material.diffuse_tex = 0;

					// load Kd
					aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);
					material.Kd = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
					// load Ka
					aiColor3D ambientColor(1.0f, 1.0f, 1.0f);
					material.Ka = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
					// load Ks
					aiColor3D specularColor(1.0f, 1.0f, 1.0f);
					material.Ks = glm::vec3(specularColor.r, specularColor.g, specularColor.b);
					// load Ns
					material.Ns = 0;

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

			void VolumetricLight::init(const int w, const int h) {
				m_lightPosition = glm::vec3(-2.845 * 5, 2.028 * 2.5, -1.293 * 5);

				m_renderer = new OPENGL::RendererBase();
				const std::string vsFile = "src\\shader\\vertexShader_obstacle.glsl";
				const std::string fsFile = "src\\shader\\fragmentShader_obstacle.glsl";

				if (!m_renderer->init(vsFile, fsFile, w, h)) {
					printf("Failed to initialize obstacle mapping shader program.\n");
					return;
				}

				glGenFramebuffers(1, &m_FBO);
				glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
				// Obstacle
				glGenTextures(1, &m_obstacleMap);
				glBindTexture(GL_TEXTURE_2D, m_obstacleMap);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_obstacleMap, 0);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					printf("Failed to initialize Obstacle Map Framebuffer.\n");

				GLenum attachments[1] = {
					GL_COLOR_ATTACHMENT0,
				};
				glDrawBuffers(1, attachments);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
          
			void VolumetricLight::beginRenderObstacle(const Camera* camera) {
				glClearColor(0.19f, 0.19f, 0.19f, 1.0f);
				glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_renderer->useProgram();

				// render model
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				m_modelMat = glm::translate(glm::mat4(1.0), m_lightPosition);
				m_modelMat = glm::scale(m_modelMat, glm::vec3(1.0));
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(m_modelMat));
				glUniform1i(SHADER_PARAMETER_BINDING::IS_SPHERE_LOCATION, true);
				glActiveTexture(GL_TEXTURE0);
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
				glUniform1i(SHADER_PARAMETER_BINDING::IS_SPHERE_LOCATION, false);
				glBindVertexArray(0);
			}

			void VolumetricLight::endRenderObstacle() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			void VolumetricLight::render(const Camera* camera, const int w, const int h, Interface& gui) {
				glActiveTexture(GL_TEXTURE12);
				glBindTexture(GL_TEXTURE_2D, m_obstacleMap);
				glUniform1i(SHADER_PARAMETER_BINDING::OBSTACLE_TEXTURE, 12);

				// Light Source on the Screen
				this->m_lightPosition = gui.volumetricLightPosition;
				m_lightPosition *= glm::vec3(5.0f, 2.5f, 5.0f);
				GLdouble screenX, screenY, screenZ;
				int viewport[4] = { 0, 0, w, h};

				calculateScreenCoordinates(
					m_lightPosition,
					camera,
					viewport,
					screenX, screenY, screenZ
				);
				glUniform2fv(SHADER_PARAMETER_BINDING::LIGHT_POS_ON_SCREEN, 1, glm::value_ptr(glm::vec2(screenX / w, screenY / h)));
			}

			void VolumetricLight::calculateScreenCoordinates(
				const glm::vec3& worldPosition,
				const Camera* camera,
				const int viewport[4],
				double& screenX, double& screenY, double& screenZ) {
				glm::vec4 clipSpace = camera->projMatrix() * camera->viewMatrix() *glm::vec4(worldPosition, 1.0f);

				if (clipSpace.w != 0.0f) {
					clipSpace /= clipSpace.w;
				}
				screenX = viewport[0] + (viewport[2] * (clipSpace.x * 0.5f + 0.5f));
				screenY = viewport[1] + (viewport[3] * (clipSpace.y * 0.5f + 0.5f));
				screenZ = clipSpace.z * 0.5f + 0.5f; 
			}
		} 
	}
} 
