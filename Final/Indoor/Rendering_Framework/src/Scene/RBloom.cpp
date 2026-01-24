#include "RBloom.h"
#include "Rendering_Framework/src/Rendering/Shader.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			Bloom::Bloom(const Camera* camera, const int w, const int h) {
				this->init(camera, w, h);
			}

			Bloom::~Bloom() {
			}

			void Bloom::init(const Camera* camera, const int w, const int h) {
				INANOA::OPENGL::RendererBase* renderer = new INANOA::OPENGL::RendererBase();
				const std::string vsFile = "src\\shader\\vertexShader_indoor.glsl";
				const std::string fsFile = "src\\shader\\fragmentShader_indoor.glsl";
				this->bloom_program = OPENGL::ShaderProgram::createShaderProgram("src\\shader\\framebuffer.vs.glsl", "src\\shader\\bloom.fs.glsl");
				this->blur_program = OPENGL::ShaderProgram::createShaderProgram("src\\shader\\framebuffer.vs.glsl", "src\\shader\\blur.fs.glsl");
				this->extract_program = OPENGL::ShaderProgram::createShaderProgram("src\\shader\\framebuffer.vs.glsl", "src\\shader\\extract.fs.glsl");

				// create vertex shader and fragment shader
				if (!renderer->init(vsFile, fsFile, w, h)) {
					printf("Failed to initialize Bloom shader program.\n");
					return;
				}

				this->horizontalLoc = glGetUniformLocation(blur_program->programId(), "horizontal");

				GLuint sceneTexLoc = glGetUniformLocation(bloom_program->programId(), "scene");
				GLuint blurTexLoc = glGetUniformLocation(bloom_program->programId(), "bloomBlur");

				bloom_program->useProgram();
				glUniform1i(sceneTexLoc, 0);
				glUniform1i(blurTexLoc, 1);

				extract_program->useProgram();
				sceneTexLoc = glGetUniformLocation(extract_program->programId(), "scene");
				blurTexLoc = glGetUniformLocation(extract_program->programId(), "isSphere_tex");
				glUniform1i(sceneTexLoc, 0);
				glUniform1i(blurTexLoc, 1);

				blur_program->useProgram();
				sceneTexLoc = glGetUniformLocation(blur_program->programId(), "tex");
				glUniform1i(sceneTexLoc, 0);

				m_renderer = renderer;
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				m_textureHandle = m_renderer->getUniformLocation("tex");
				loadScene();
			}

			void Bloom::loadScene() {
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

			void Bloom::renderGBuffer(const Camera* camera, glm::vec3 pos) {
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				
				m_modelMat = glm::translate(glm::mat4(1.0), pos);
				m_modelMat = glm::scale(m_modelMat, glm::vec3(0.22));
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
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
				glBindVertexArray(0);
			}

			void Bloom::renderExtractedBall(OPENGL::FrameBuffer* scene_fb, GBuffer* gBuffer) {
				extract_program->useProgram();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glDisable(GL_DEPTH_TEST);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, scene_fb->getFBOtex());
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(GBufferTexture::ShadingNormal));

				glBindVertexArray(scene_fb->getVAO());
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glBindVertexArray(0);
				glEnable(GL_DEPTH_TEST);
			}

			void Bloom::renderBlur(OPENGL::FrameBuffer* scene_fb, bool horizontal) {
				this->blur_program->useProgram();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glDisable(GL_DEPTH_TEST);

				glUniform1i(this->horizontalLoc, horizontal);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, scene_fb->getFBOtex());
				glBindVertexArray(scene_fb->getVAO());
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glBindVertexArray(0);
				glEnable(GL_DEPTH_TEST);
			}

			void Bloom::renderBloom(OPENGL::FrameBuffer* scene_fb, OPENGL::FrameBuffer* effect_fb) {
				this->bloom_program->useProgram();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glDisable(GL_DEPTH_TEST);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, scene_fb->getFBOtex());
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, effect_fb->getFBOtex());

				glBindVertexArray(scene_fb->getVAO());
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glBindVertexArray(0);
				glEnable(GL_DEPTH_TEST);
			}

			void Bloom::update(const Camera* camera) {
				this->m_viewMat = camera->viewMatrix();
				this->m_projMat = camera->projMatrix();
			}

		}
	}
}
