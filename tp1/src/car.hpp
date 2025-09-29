#pragma once

#include "model.hpp"

class Car
{   
public:
    Car();
    
    void loadModels();
    
    void update(float deltaTime);
    
    void draw(glm::mat4& projView,GLuint& transformSP_);
    
private:
    // TODO: Une division en méthodes a déjà été réalisée pour vous.
    //       Libre à vous de modifier (ou ajouter) les définitions (et surtout
    //       d'ajouter des paramètres d'entrées). Procédez comme bon vous semble.
    //       Il sera important de penser à comment réutiliser le plus possible les matrices.
    //       Utilisez le passage par référence ou par copie pour vous aider.
    
    void drawFrame();
    
    void drawWheel(glm::vec3 pos);
    void drawWheels();
    
    void drawBlinker(glm::vec3 pos);
    void drawLight(glm::vec3 pos, bool backLight);
    void drawHeadlight(glm::vec3 pos);
    void drawHeadlights();
    
private:    
    Model frame_;
    Model wheel_;
    Model blinker_;
    Model light_;
    
public:
    glm::vec3 position;
    glm::vec2 orientation;   
    glm::mat4 projV;
    
    float speed;
    float wheelsRollAngle;
    float steeringAngle;
    bool isHeadlightOn;
    bool isBraking;
    bool isLeftBlinkerActivated;
    bool isRightBlinkerActivated;
    
    bool isBlinkerOn;
    float blinkerTimer;
    
    GLuint transformSP;
    GLuint colorModUniformLocation;
    GLuint mvpUniformLocation;
};


