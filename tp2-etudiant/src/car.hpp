#pragma once

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


