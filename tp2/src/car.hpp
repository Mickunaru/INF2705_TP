#pragma once

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include "model.hpp"
#include <stack>

#include "uniform_buffer.hpp"

struct CarDrawResult {
    glm::mat4 frameMvp;
    glm::mat4 wheelMvps[4];
};

class EdgeEffect;
class CelShading;

class Car
{   
public:
    Car();
    
    void loadModels();
    
    void update(float deltaTime);
    
    CarDrawResult draw(glm::mat4& projView); // À besoin de la matrice de vue séparément, pour la partie 3.

    void drawBorder(CarDrawResult& carDrawResult);

    void drawWindows(glm::mat4& projView, glm::mat4& view); // Dessin des vitres séparées.
    
private:
    // TODO: Adapter les paramètres des méthodes privée ici au besoin, surtout pour la partie 3.  
    glm::mat4 drawFrame(glm::mat4& projView);
    glm::mat4 drawWheel(glm::mat4& projView, const glm::vec3& wheelPos, bool isFront, bool isLeft);
    void drawWheels(glm::mat4& projView, glm::mat4 outWheelMvps[4]);
    
    void drawBlinker(glm::mat4& projView, glm::mat4& headlightMatrix, bool isLeftHeadlight);
    void drawLight(glm::mat4& projView, glm::mat4& headlightMatrix,  bool isFront, bool isLeft);
    void drawHeadlight(glm::mat4& projView, const glm::vec3& position, bool isFront, bool isLeft);
    void drawHeadlights(glm::mat4& projView);

	void drawFrameBorder(glm::mat4& frameMvp);
	void drawWheelBorder(glm::mat4& wheelMvp);
    
private:    
    Model frame_;
    Model wheel_;
    Model blinker_;
    Model light_;
    Model windows[6];
    
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

    EdgeEffect* edgeEffectShader;
    CelShading* celShadingShader;
    UniformBuffer* material;
};


