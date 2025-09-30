#include "car.hpp"

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace gl;
using namespace glm;

    
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

void Car::draw(glm::mat4& projView)
{
    glm::mat4 carMatrix = translate(glm::mat4(1), position);
    carMatrix = rotate(carMatrix, orientation.y, vec3(0, 1, 0));
    carMatrix = rotate(carMatrix, orientation.x, vec3(1, 0, 0));

    drawFrame(projView, carMatrix);
	drawWheels(projView, carMatrix);
	drawHeadlights(projView, carMatrix);
}
    
void Car::drawFrame(const glm::mat4& projView, const glm::mat4& carMatrix)
{
    glm::mat4 model = translate(carMatrix, vec3(0.0f, 0.25f, 0.0f));
    glm::mat4 mvp = projView * model;

    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, value_ptr(mvp));
    glUniform3f(colorModUniformLocation, 1.0f, 1.0f, 1.0f);

    frame_.draw();
}

void Car::drawWheel(const glm::mat4& projView, const glm::mat4& carMatrix, const glm::vec3& wheelPos, bool isFront, bool isLeft)
{
    glm::mat4 model = glm::translate(carMatrix, wheelPos);

    float axisOffset = isLeft ? 0.10124f : -0.10124f;
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, axisOffset));

    if (isFront)
        model = glm::rotate(model, -glm::radians(steeringAngle), glm::vec3(0, 1, 0));

    model = glm::rotate(model, wheelsRollAngle, glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -axisOffset));

    glm::mat4 mvp = projView * model;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(colorModUniformLocation, 1.0f, 1.0f, 1.0f);

    wheel_.draw();
}

void Car::drawWheels(const glm::mat4& projView, const glm::mat4& carMatrix)
{
    const glm::vec3 WHEEL_POSITIONS[] =
    {
		glm::vec3(-1.29f, 0.245f, -0.57f), //Front right
		glm::vec3(-1.29f, 0.245f,  0.38f), //Front left, 
        glm::vec3( 1.4f , 0.245f, -0.57f), //Rear right
        glm::vec3( 1.4f , 0.245f,  0.38f)  //Rear left
    };

    drawWheel(projView, carMatrix, WHEEL_POSITIONS[0], true, false);
    drawWheel(projView, carMatrix, WHEEL_POSITIONS[1], true, true);
    drawWheel(projView, carMatrix, WHEEL_POSITIONS[2], false, false);
    drawWheel(projView, carMatrix, WHEEL_POSITIONS[3], false, true);
}

void Car::drawBlinker(const glm::mat4& projView, const glm::mat4& carMatrix, bool isLeftHeadlight)
{
    bool isBlinkerActivated = (isLeftHeadlight  && isLeftBlinkerActivated) ||
                              (!isLeftHeadlight && isRightBlinkerActivated);

    const glm::vec3 ON_COLOR (1.0f, 0.7f , 0.3f );
    const glm::vec3 OFF_COLOR(0.5f, 0.35f, 0.15f);

    glm::mat4 model = glm::translate(carMatrix, glm::vec3(0.0f, 0.0f, isLeftHeadlight ? 0.06065f : -0.06065f));
    
    glm::vec3 color = (isBlinkerOn && isBlinkerActivated) ? ON_COLOR : OFF_COLOR;

    glm::mat4 mvp = projView * model;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(colorModUniformLocation, color.r, color.g, color.b);

    blinker_.draw();
}

void Car::drawLight(const glm::mat4& projView, const glm::mat4& carMatrix, bool isFront, bool isLeft)
{
    const glm::vec3 FRONT_ON_COLOR (1.0f, 1.0f, 1.0f);
    const glm::vec3 FRONT_OFF_COLOR(0.5f, 0.5f, 0.5f);
    const glm::vec3 REAR_ON_COLOR  (1.0f, 0.1f, 0.1f);
    const glm::vec3 REAR_OFF_COLOR (0.5f, 0.1f, 0.1f);

    glm::mat4 model = glm::translate(carMatrix, glm::vec3(0.0f, 0.0f, isLeft ? -0.029f : 0.029f));

    glm::vec3 color;
    if (isFront)
        color = isHeadlightOn ? FRONT_ON_COLOR : FRONT_OFF_COLOR;
    else
        color = isBraking ? REAR_ON_COLOR : REAR_OFF_COLOR;

    glm::mat4 mvp = projView * model;
    glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(colorModUniformLocation, color.r, color.g, color.b);

    light_.draw();
}

void Car::drawHeadlight(const glm::mat4& projView, const glm::mat4& carMatrix, const glm::vec3& position, bool isFront, bool isLeft)
{
    glm::mat4 headlightMatrix = glm::translate(carMatrix, position);
    drawLight(projView, headlightMatrix, isFront, isLeft);
    drawBlinker(projView, headlightMatrix, isLeft);
}

void Car::drawHeadlights(const glm::mat4& projView, const glm::mat4& carMatrix)
{
    const glm::vec3 HEADLIGHT_POSITIONS[] =
    {
        glm::vec3(-2.0019f, 0.64f, -0.45f), //Front right
        glm::vec3(-2.0019f, 0.64f,  0.45f), //Front left, 
        glm::vec3( 2.0019f, 0.64f, -0.45f), //Rear right, 
        glm::vec3( 2.0019f, 0.64f,  0.45f)  //Rear left, 
    };

    drawHeadlight(projView, carMatrix, HEADLIGHT_POSITIONS[0], true, false);
    drawHeadlight(projView, carMatrix, HEADLIGHT_POSITIONS[1], true, true);
    drawHeadlight(projView, carMatrix, HEADLIGHT_POSITIONS[2], false, false);
    drawHeadlight(projView, carMatrix, HEADLIGHT_POSITIONS[3], false, true);
}

