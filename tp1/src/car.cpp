#include "car.hpp"
#include "vertex.hpp"
    
Car::Car()
: position(0.0f, 0.0f, 0.0f), orientation(0.0f, 0.0f), speed(0.f)
, wheelsRollAngle(0.f), steeringAngle(0.f)
, isHeadlightOn(false), isBraking(false)
, isLeftBlinkerActivated(false), isRightBlinkerActivated(false)
, isBlinkerOn(false), blinkerTimer(0.f)
{}

void Car::loadModels()
{
    frame_.load("../models/frame.ply");
    wheel_.load("../models/wheel.ply");
    blinker_.load("../models/blinker.ply");
    light_.load("../models/light.ply");
}

void Car::update(float deltaTime)
{
    if (isBraking)
    {
        const float LOW_SPEED_THRESHOLD = 0.1f;
        const float BRAKE_APPLIED_SPEED_THRESHOLD = 0.01f;
        const float BRAKING_FORCE = 4.f;
    
        if (fabs(speed) < LOW_SPEED_THRESHOLD)
            speed = 0.f;
            
        if (speed > BRAKE_APPLIED_SPEED_THRESHOLD)
            speed -= BRAKING_FORCE * deltaTime;
        else if (speed < -BRAKE_APPLIED_SPEED_THRESHOLD)
            speed += BRAKING_FORCE * deltaTime;
    }
    
    const float WHEELBASE = 2.7f;
    float angularSpeed = speed * sin(-glm::radians(steeringAngle)) / WHEELBASE;
    orientation.y += angularSpeed * deltaTime;
    
    glm::vec3 positionMod = glm::rotate(glm::mat4(1.0f), orientation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(-speed, 0.f, 0.f, 1.f);
    position += positionMod * deltaTime;
    
    const float WHEEL_RADIUS = 0.2f;
    wheelsRollAngle += speed / (2.f * M_PI * WHEEL_RADIUS) * deltaTime;
    
    if (wheelsRollAngle > M_PI)
        wheelsRollAngle -= 2.f * M_PI;
    else if (wheelsRollAngle < -M_PI)
        wheelsRollAngle += 2.f * M_PI;
        
    if (isRightBlinkerActivated || isLeftBlinkerActivated)
    {
        const float BLINKER_PERIOD_SEC = 0.5f;
        blinkerTimer += deltaTime;
        if (blinkerTimer > BLINKER_PERIOD_SEC)
        {
            blinkerTimer = 0.f;
            isBlinkerOn = !isBlinkerOn;
        }
    }
    else
    {
        isBlinkerOn = true;
        blinkerTimer = 0.f;
    }  
}

void Car::draw(glm::mat4& projView,GLuint& transformSP_)
{
    // TODO: Dessin de la totalité de l'automobile.
    //       Elle sera positionnée et orientée selon les propriétés
    //       position et orientation, respectivement.
    //
    //       L'automobile est constituée d'un châssis, de 4 roues
    //       et de 4 phares.
    //
    //       Les modèles sont faits de façon à ce que le devant de la voiture
    //       soit en -x (x négatif, vers la gauche dans l'écran).
    transformSP = transformSP_;
    projV = projView;
    drawFrame();
    drawWheels();
    drawHeadlights();
}
    
void Car::drawFrame()
{
    // TODO: Dessin du châssis de l'automobile.
    //       Le châssis est positionné 0.25 unité au-dessus de l'origine de
    //       l'automobile.
    glUseProgram(transformSP);
    mat4 mvp;

    mat4 carFrame = translate(mat4(1), { 0.0f,0.25f,0.00f });
    mvp = projV * carFrame;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);
    frame_.draw();
}

void Car::drawWheel(glm::vec3 pos)
{
    // TODO: Dessin d'une roue.
    //       La roue doit être positionnée en dessous du châssis (voir Car::drawWheels).
    //
    //       Les roues tournent selon wheelsRollAngle (en radian).
    //       
    //       Les roues avant peuvent tourner sur elles-mêmes pour orienter la trajectoire
    //       de l'automobile avec l'attribut steeringAngle (en degrés).
    //
    //       Malheureusement, l'origine de l'objet n'est pas centrée. Il faudra
    //       changer l'axe de rotation. Celui-ci est 0.10124 unité vers
    //       l'intérieur de la roue (voir le schéma 4 dans l'énoncé).
    glUseProgram(transformSP);
    mat4 mvp;
    vec3 zPos = { 0.0f,0.0f,0.10124 };
    
    mat4 wheelFrame = translate(mat4(1), pos-zPos);
    mvp = projV * wheelFrame;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);

    wheel_.draw();
}

void Car::drawWheels()
{
    // TODO: Dessin des 4 roues.
    //       Utilisez Car::drawWheel et WHEEL_POSITIONS.
    
    const glm::vec3 WHEEL_POSITIONS[] =
    {
        glm::vec3(-1.29f, 0.245f, -0.57f),
        glm::vec3(-1.29f, 0.245f,  0.57f),
        glm::vec3( 1.4f , 0.245f, -0.57f),
        glm::vec3( 1.4f , 0.245f,  0.57f)
    };
    static int nbWheels = 4;
    for (int i = 0; i < nbWheels;i++) {
        drawWheel(WHEEL_POSITIONS[i]);
    }
}

void Car::drawBlinker(glm::vec3 pos)
{
    // TODO: Dessin d'un cligotant d'un phare.
    //       Il est positionné à z=-0.06065.
    bool isLeft = pos.z < 0.0f;

    bool isBlinkerActivated = (isLeft && isLeftBlinkerActivated) ||
                              (!isLeft && isRightBlinkerActivated);

    const glm::vec3 ON_COLOR (1.0f, 0.7f , 0.3f );
    const glm::vec3 OFF_COLOR(0.5f, 0.35f, 0.15f);
    glm::mat4 blinkerFrame;
    glm::mat4 mvp;
    glm::vec3 color;
    glm::vec3 posZ = { 0.0f,0.0f,0.06065f };

    glUseProgram(transformSP);

    blinkerFrame = isLeft ? blinkerFrame = translate(mat4(1), pos + posZ) : blinkerFrame = translate(mat4(1), pos - posZ);

    mvp = projV * blinkerFrame;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);

    color = (isBlinkerOn && isBlinkerActivated) ? ON_COLOR : OFF_COLOR;
    glUniform3fv(colorModUniformLocation, 1, &color[0]);
    blinker_.draw();

    //// TODO: Changement de couleur si le clignotant est activé ou non.   
}

void Car::drawLight(glm::vec3 pos,bool frontLight)
{
    // TODO: Dessin d'une lumière de phare.
    //       Elle est positionnée à z=0.029.
    //       Les lumières avant et arrière ne sont pas de la même couleur
    //       et peuvent être allumées et éteintes.
    //       Les lumières avant s'ouvrent avec isHeadlightOn, alors que celle
    //       de l'arrière avec isBraking.   
    glUseProgram(transformSP);

    const glm::vec3 FRONT_ON_COLOR(1.0f, 1.0f, 1.0f);
    const glm::vec3 FRONT_OFF_COLOR(0.5f, 0.5f, 0.5f);
    const glm::vec3 REAR_ON_COLOR(1.0f, 0.1f, 0.1f);
    const glm::vec3 REAR_OFF_COLOR(0.5f, 0.1f, 0.1f);

    const glm::vec3 zPos(0.0f, 0.0f, 0.029f);

    glm::mat4 mvp;
    glm::vec3 color;
    glm::mat4 lightColorFrame;

    if (frontLight) {
        lightColorFrame = translate(mat4(1), pos + zPos);
        color = isHeadlightOn ? FRONT_ON_COLOR : FRONT_OFF_COLOR;
    }
    else {
        lightColorFrame = translate(mat4(1), pos - zPos);
        color = isBraking ? REAR_ON_COLOR : REAR_OFF_COLOR;
    }
    mvp = projV * lightColorFrame;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(colorModUniformLocation, 1, &color[0]);
    light_.draw();  
}

void Car::drawHeadlight(glm::vec3 pos)
{
    // TODO: Dessin d'un phare de l'automobile.
    //       Un phare est constitué d'une lumière et d'un clignotant.
    //       Le phare doit être positionné devant ou derrière le châssis (voir Car::drawHeadlights)
    glUseProgram(transformSP);
    mat4 mvp;
    mat4 headlightFrame = translate(mat4(1), pos);
    mvp = projV * headlightFrame;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);
}

void Car::drawHeadlights()
{
    // TODO: Dessin des 4 phares.
    //       Utilisez Car::drawHeadlight et HEADLIGHT_POSITIONS.
    
    const glm::vec3 HEADLIGHT_POSITIONS[] =
    {
        glm::vec3(-2.0019f, 0.64f, -0.45f),
        glm::vec3(-2.0019f, 0.64f,  0.45f),
        glm::vec3( 2.0019f, 0.64f, -0.45f),
        glm::vec3( 2.0019f, 0.64f,  0.45f)
    };
    const int nbHeadlights = 4;
    
    for (int i = 0; i < nbHeadlights; i++) {
        vec3 pos = HEADLIGHT_POSITIONS[i];
        bool isFront = pos.x < 0.0f;
        drawHeadlight(pos);
        drawLight(pos, isFront);
        if (isFront) {
            drawBlinker(pos);
        }
    }
}

