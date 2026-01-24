#include "RTrice.h"

namespace INANOA {
	namespace SCENE {
		namespace EXPERIMENTAL {
			Trice::Trice(const Camera* camera, const int w, const int h) {
				this->init(camera, w, h);
			}

			Trice::~Trice() {
			}

			void Trice::init(const Camera* camera, const int w, const int h) {
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
				m_modelMat = glm::translate(glm::mat4(1.0), glm::vec3(2.05, 0.628725, -1.9));
				m_modelMat = glm::scale(m_modelMat, glm::vec3(0.001));

				loadModel();
			}

			void Trice::loadModel() {
				const std::string scenePath = "models\\Trice.obj";
				const aiScene* scene = aiImportFile(scenePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_CalcTangentSpace);
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

					// Normal map
					if (aimat->GetTexture(aiTextureType_HEIGHT, 0, &texture_path) == aiReturn_SUCCESS) {
						std::string tmpstr(texture_path.C_Str());
						texture_data tdata = loadImg(texture_path.C_Str());
						glGenTextures(1, &material.normal_tex);
						glBindTexture(GL_TEXTURE_2D, material.normal_tex);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glGenerateMipmap(GL_TEXTURE_2D);
						delete[] tdata.data;
						material.normal_tex_used = 1;
					}
					else {
						material.normal_tex_used = 0;
						material.normal_tex = 0;
					}

					// load Kd
					aiColor3D diffuseColor(0.5f, 0.5f, 0.5f);
					aimat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
					material.Kd = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);

					// Load Specular Color
					aiColor3D specularColor(1.0f, 1.0f, 1.0f);
					aimat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
					material.Ks = glm::vec3(specularColor.r, specularColor.g, specularColor.b);

					// Load Ambient Color
					aiColor3D ambientColor(0.1f, 0.1f, 0.1f);
					aimat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
					material.Ka = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
					materials.push_back(material);
				}

				// load geometry
				std::vector<float> vertices, texcoords, normals;
				std::vector<unsigned int> indices;
				std::vector<float> tangents, bitangents;
				for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
					Shape shape;
					aiMesh* mesh = scene->mMeshes[i];
					vertices.clear();
					texcoords.clear();
					normals.clear();
					indices.clear();
					tangents.clear();
					bitangents.clear();
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

						if (mesh->mTangents) {
							tangents.push_back(mesh->mTangents[j][0]);
							tangents.push_back(mesh->mTangents[j][1]);
							tangents.push_back(mesh->mTangents[j][2]);

							bitangents.push_back(mesh->mBitangents[j][0]);
							bitangents.push_back(mesh->mBitangents[j][1]);
							bitangents.push_back(mesh->mBitangents[j][2]);
						}
						else {
							tangents.push_back(0.0f);
							tangents.push_back(0.0f);
							tangents.push_back(0.0f);

							bitangents.push_back(0.0f);
							bitangents.push_back(0.0f);
							bitangents.push_back(0.0f);
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
					glEnableVertexAttribArray(6); 
					glEnableVertexAttribArray(7); 

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

					glGenBuffers(1, &shape.vbo_tangent);
					glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_tangent);
					glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(float), tangents.data(), GL_STATIC_DRAW);
					glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, 0);

					glGenBuffers(1, &shape.vbo_bitangent);
					glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_bitangent);
					glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(float), bitangents.data(), GL_STATIC_DRAW);
					glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, 0);

					glGenBuffers(1, &shape.ibo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

					shapes.push_back(shape);
				}
				glBindVertexArray(0);
				aiReleaseImport(scene);
			}

			void Trice::render(const Camera* camera) {
				m_renderer->useProgram();
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
				glUniform1i(m_textureHandle, 0);
				glActiveTexture(GL_TEXTURE0);
				for (int i = 0; i < shapes.size(); i++) {
					glBindVertexArray(shapes[i].vao);
					int mid = shapes[i].materialID;
					glBindTexture(GL_TEXTURE_2D, materials[mid].diffuse_tex);
					glUniform1i(SHADER_PARAMETER_BINDING::TEX_USED_LOCATION, materials[mid].texUsed);
					glUniform3fv(SHADER_PARAMETER_BINDING::Kd_LOCATION, 1, &materials[mid].Kd[0]);
					glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				}
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			void Trice::renderGBuffer(const Camera* camera, bool toogleNormalMapping) {
				m_renderer->setCamera(camera->projMatrix(), camera->viewMatrix(), camera->viewOrig());
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
				glUniform1i(SHADER_PARAMETER_BINDING::IS_SPHERE_LOCATION, false);
				glActiveTexture(GL_TEXTURE0);
				for (int i = 0; i < shapes.size(); i++) {
					glBindVertexArray(shapes[i].vao);
					int mid = shapes[i].materialID;
					glBindTexture(GL_TEXTURE_2D, materials[mid].diffuse_tex);
					glUniform1i(SHADER_PARAMETER_BINDING::TEX_USED_LOCATION, materials[mid].texUsed);
					glUniform3fv(SHADER_PARAMETER_BINDING::Kd_LOCATION, 1, &materials[mid].Kd[0]);
					glUniform3fv(SHADER_PARAMETER_BINDING::Ks_LOCATION, 1, &materials[mid].Ks[0]);
					glUniform3fv(SHADER_PARAMETER_BINDING::Ka_LOCATION, 1, &materials[mid].Ka[0]);
					if (toogleNormalMapping && materials[mid].normal_tex_used) {
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, materials[mid].normal_tex);
						glUniform1i(SHADER_PARAMETER_BINDING::NORMAL_MAP_TEXTURE, 1);
						glUniform1i(SHADER_PARAMETER_BINDING::NORMAL_MAP_USED_LOCATION, 1);
					}
					else {
						glUniform1i(SHADER_PARAMETER_BINDING::NORMAL_MAP_USED_LOCATION, 0);
					}
					glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				}
				glBindVertexArray(0);
				glUniform1i(SHADER_PARAMETER_BINDING::NORMAL_MAP_USED_LOCATION, 0);
			}

			void Trice::renderMVP() {
				glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
				for (int i = 0; i < shapes.size(); i++) {
					glBindVertexArray(shapes[i].vao);
					glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				}
				glBindVertexArray(0);
			}

			void Trice::update(const Camera* camera) {
				this->m_viewMat = camera->viewMatrix();
				this->m_projMat = camera->projMatrix();
			}

			void Trice::resize(const int w, const int h) {
			}
		}
	}
}
