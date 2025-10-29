#pragma once

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include "model.hpp"
#include <stack>

#include "uniform_buffer.hpp"

class EdgeEffect;
class CelShading;

class Car
{   
public:
    Car();
    
    void loadModels();
    
    void update(float deltaTime);
    
    void draw(glm::mat4& projView, glm::mat4& view); // À besoin de la matrice de vue séparément, pour la partie 3.

    void drawWindows(glm::mat4& projView, glm::mat4& view); // Dessin des vitres séparées.
    
private:
    // TODO: Adapter les paramètres des méthodes privée ici au besoin, surtout pour la partie 3.  
    void drawFrame(const glm::mat4& projView, const glm::mat4& carMatrix);
    void drawWheel(const glm::mat4& projView, const glm::mat4& carMatrix, const glm::vec3& wheelPos, bool isFront, bool isLeft);
    void drawWheels(const glm::mat4& projView, const glm::mat4& carMatrix);
    
    void drawBlinker(const glm::mat4& projView, const glm::mat4& carMatrix, bool isLeftHeadlight);
    void drawLight(const glm::mat4& projView, const glm::mat4& carMatrix, bool isFront, bool isLeft);
    void drawHeadlight(const glm::mat4& projView, const glm::mat4& carMatrix, const glm::vec3& position, bool isFront, bool isLeft);
    void drawHeadlights(const glm::mat4& projView, const glm::mat4& carMatrix);
    
private:    
    Model frame_;
    Model wheel_;
    Model blinker_;
    Model light_;
    Model windows[6]; // Nouveaux modèles à ajouter.
    
public:
    glm::mat4 carModel;
    glm::vec3 position;
    glm::vec2 orientation;    
    
    float speed;
    float wheelsRollAngle;
    float steeringAngle;
    bool isHeadlightOn;
    bool isBraking;
    bool isLeftBlinkerActivated;
    bool isRightBlinkerActivated;
    
    bool isBlinkerOn;
    float blinkerTimer;
    
    GLuint colorModUniformLocation;
    GLuint mvpUniformLocation;

    EdgeEffect* edgeEffectShader;
    CelShading* celShadingShader;
    UniformBuffer* material;
};


