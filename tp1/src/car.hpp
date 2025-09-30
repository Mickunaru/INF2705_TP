#pragma once

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include "model.hpp"
#include <stack>

class Car
{   
public:
    Car();
    
    void loadModels();
    
    void update(float deltaTime);
    
    void draw(glm::mat4& projView);
    
private:
    // TODO: Une division en méthodes a déjà été réalisée pour vous.
    //       Libre à vous de modifier (ou ajouter) les définitions (et surtout
    //       d'ajouter des paramètres d'entrées). Procédez comme bon vous semble.
    //       Il sera important de penser à comment réutiliser le plus possible les matrices.
    //       Utilisez le passage par référence ou par copie pour vous aider.
    
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
    
public:
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
};


