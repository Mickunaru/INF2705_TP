#include "model.hpp"
// TODO: Il est fortement recommandé de définir quelques structs
//       pour représenter les attributs.
//       Faire de même pour représenter une vertex, qui est constitué d'attributs.
//       Cela facilitera l'utilisation et rendra votre code plus clair.
//       Un format entrelacé est recommandé (ordonné par vertex au lieu par attribut).
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
    
    // TODO: Rassemblez les propriétés du fichier .ply pour correspondre au
    //       format de donnée souhaité (celui que vous avez défini dans la struct).
    vertices.clear(); // Removes all elements in vertices
    for (size_t i = 0; i < positionX.size(); i++) {
        glm::vec3 pos(
            positionX[i],
            positionY[i],
            positionZ[i]
        );
        glm::vec4 col(
            colorRed[i]/ 255.0f,
            colorGreen[i] / 255.0f,
            colorBlue[i] / 255.0f,
            1.0f
        );
        vertices.push_back(VertexData{ pos,col });
    }    
    // TODO: Rassemblez les indices dans un seul tableau contigu.;
    elements.clear(); // Removes all elements
    for (const auto&f: facesIndices) {
        if (f.size() == 3) {
            elements.insert(elements.end(),f.begin(),f.end());
        }
    }
    count_ = static_cast<GLsizei>(elements.size());

    // TODO: Allocation des ressources sur la carte graphique et envoyer les
    //       données traitées dans le vbo_ et ebo_ sur la carte graphique.
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    //vbo_
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

    //ebo_
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()* sizeof(GLuint), elements.data(), GL_STATIC_DRAW);

    // Attribut 1 = couleur (vec4)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));

     
    // Attribut 1= couleur (vec4)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, color));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);  // Unlik vao
    
    // TODO: Créez un vao_ et spécifiez le format des données dans celui-ci.
    //       N'oubliez pas de lier le ebo_ avec le vao_ et de délier le vao_
    //       du contexte pour empêcher des modifications sur celui-ci.z
    
    // TODO: Initialisez count_, qui correspond au nombre d'indices à dessiner.
}

Model::~Model()
{
    // TODO: Libérez les ressources allouées.
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
}

void Model::draw()
{
    // TODO: Dessin du modèle.
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, count_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

