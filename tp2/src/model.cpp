#include "model.hpp"

#include "happly.h"
#include <glm/glm.hpp>

using namespace gl;
using namespace glm;

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};


void Model::load(const char* path)
{
    // Chargement des données du fichier .ply.
    // Ne modifier pas cette partie.
    happly::PLYData plyIn(path);

    happly::Element& vertex = plyIn.getElement("vertex");
    std::vector<float> positionX = vertex.getProperty<float>("x");
    std::vector<float> positionY = vertex.getProperty<float>("y");
    std::vector<float> positionZ = vertex.getProperty<float>("z");
    
    std::vector<unsigned char> colorRed   = vertex.getProperty<unsigned char>("red");
    std::vector<unsigned char> colorGreen = vertex.getProperty<unsigned char>("green");
    std::vector<unsigned char> colorBlue  = vertex.getProperty<unsigned char>("blue");

    // Tableau de faces, une face est un tableau d'indices.
    // Les faces sont toutes des triangles dans nos modèles (donc 3 indices par face).
    std::vector<std::vector<unsigned int>> facesIndices = plyIn.getFaceIndices<unsigned int>();
    
    std::vector<Vertex> vertices;
    vertices.reserve(positionX.size());

    for (int i = 0; i < positionX.size(); i++) {
        Vertex v;
        v.position = { positionX[i], positionY[i], positionZ[i] };
        if (colorRed.empty()) {
            v.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        }
        else {
            v.color = { colorRed[i] / 255.0f, colorGreen[i] / 255.0f, colorBlue[i] / 255.0f, 1.0f };
        }
        vertices.push_back(v);
    }
    
    const unsigned int NUM_INDICES_PER_FACE = 3;
    std::vector<unsigned int> indices;
    indices.reserve(facesIndices.size() * NUM_INDICES_PER_FACE);

    for (int i = 0; i < facesIndices.size(); i++) {
        for (int j = 0; j < NUM_INDICES_PER_FACE; j++) {
            indices.push_back(facesIndices[i][j]);
        }
    }
    
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glBindVertexArray(0);

    count_ = indices.size();
}

Model::~Model()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ebo_);
}

void Model::draw()
{
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, count_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

