#pragma once
#include <cstddef>
#include <cstdint>

#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glbinding/gl/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>

using namespace gl;
using namespace glm;

// TODO: À ajouter dans votre classe actuelle.

class EdgeEffect;
class CelShading;


class Car
{   
public:
    void draw(glm::mat4& projView, glm::mat4& view); // À besoin de la matrice de vue séparément, pour la partie 3.
    
    void drawWindows(glm::mat4& projView, glm::mat4& view); // Dessin des vitres séparées.
    
private:
    // TODO: Adapter les paramètres des méthodes privée ici au besoin, surtout pour la partie 3.    
    
private:    
    Model windows[6]; // Nouveaux modèles à ajouter.
    
public:
    // TODO: À ajouter dans votre classe actuelle.
    glm::mat4 carModel;
   
    EdgeEffect* edgeEffectShader;
    CelShading* celShadingShader;
    UniformBuffer* material;
};


