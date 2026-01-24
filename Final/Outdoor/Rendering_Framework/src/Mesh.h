#pragma once
#include <string>
#include <vector>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <glad/glad.h>
#include <glm/vec3.hpp>

/* --------------------------------------------------------------------
    Since all model(.obj) in this project have only one object(mesh),
    we don't process the node recurrsively.
---------------------------------------------------------------------- */ 

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec3 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

class Mesh
{
public:
    Mesh(std::string objPath);

private:
    void loadMesh(std::string objPath);
    void processMesh(aiMesh* mesh, const aiScene* scene);
    void setUpVAO();

public:
    GLuint getVAO() { return VAO; };
    int getNumIndex() { return vertices.size(); };
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
private:
    GLuint VAO, VBO, EBO;
};