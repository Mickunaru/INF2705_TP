#include "car.hpp"

#include <cmath>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shaders.hpp"
#include <inf2705/OpenGLApplication.hpp>


using namespace gl;
using namespace glm;

#define CHECK_GL_ERROR printGLError(__FILE__, __LINE__)

struct Material
{
    glm::vec4 emission; // vec3, but padded
    glm::vec4 ambient;  // vec3, but padded
    glm::vec4 diffuse;  // vec3, but padded
    glm::vec3 specular;
    GLfloat shininess;
};

Car::Car()
    : position(0.0f, 0.0f, 0.0f), orientation(0.0f, 0.0f), speed(0.f)
    , wheelsRollAngle(0.f), steeringAngle(0.f)
    , isHeadlightOn(false), isBraking(false)
    , isLeftBlinkerActivated(false), isRightBlinkerActivated(false)
    , isBlinkerOn(false), blinkerTimer(0.f)
{
}

void Car::loadModels()
{
    frame_.load("../models/frame.ply");
    wheel_.load("../models/wheel.ply");
    blinker_.load("../models/blinker.ply");
    light_.load("../models/light.ply");

    const char* WINDOW_MODEL_PATHES[] =
    {
        "../models/window.f.ply",
        "../models/window.r.ply",
        "../models/window.fl.ply",
        "../models/window.fr.ply",
        "../models/window.rl.ply",
        "../models/window.rr.ply"
    };
    for (unsigned int i = 0; i < 6; ++i)
    {
        windows[i].load(WINDOW_MODEL_PATHES[i]);
    }
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
        isBlinkerOn = false;
        blinkerTimer = 0.f;
    }

    carModel = glm::mat4(1.0f);
    carModel = glm::translate(carModel, position);
    carModel = glm::rotate(carModel, orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
}

CarDrawResult Car::draw(glm::mat4& projView)
{
    CarDrawResult result;

    result.frameMvp = drawFrame(projView);

    drawWheels(projView, result.wheelMvps);

    return result;
}

void Car::drawBorder(CarDrawResult& carDrawResult)
{
    drawFrameBorder(carDrawResult.frameMvp);
    for (int i = 0; i < 4; i++)
    {
        drawWheelBorder(carDrawResult.wheelMvps[i]);
    }
}

glm::mat4 Car::drawFrame(glm::mat4& projView)
{
    glm::mat4 model = translate(carModel, vec3(0.0f, 0.25f, 0.0f));
    glm::mat4 mvp = projView * model;

    celShadingShader->setMatrices(mvp, carModel, model);
    frame_.draw();

    return mvp;
}

void Car::drawFrameBorder(glm::mat4& frameMvp)
{
    glUniformMatrix4fv(edgeEffectShader->mvpULoc, 1, GL_FALSE, glm::value_ptr(frameMvp));
    frame_.draw();
}

void Car::drawWindows(glm::mat4& projView, glm::mat4& view)
{
    const glm::vec3 WINDOW_POSITION[] =
    {
        glm::vec3(-0.813, 0.755, 0.0),
        glm::vec3(1.092, 0.761, 0.0),
        glm::vec3(-0.3412, 0.757, 0.51),
        glm::vec3(-0.3412, 0.757, -0.51),
        glm::vec3(0.643, 0.756, 0.508),
        glm::vec3(0.643, 0.756, -0.508)
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

   glm::mat4 model = glm::translate(carModel, glm::vec3(0.0f, 0.25f, 0.0f));

    std::map<float, unsigned int> sorted;
    for (unsigned int i = 0; i < 6; i++)
    {
        glm::vec4 worldPos = glm::vec4(WINDOW_POSITION[i], 1.0f);
        glm::vec4 viewSpacePos = view * worldPos;
        float distance = -viewSpacePos.z;

        sorted[distance] = i;
    }

    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        unsigned int i = it->second;

        glm::mat4 mvp = projView * model;
        celShadingShader->setMatrices(mvp, view, model);
        windows[i].draw();
    }

    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

glm::mat4 Car::drawWheel(glm::mat4& projView, const glm::vec3& wheelPos, bool isFront, bool isLeft)
{
    glm::mat4 model = glm::translate(carModel, wheelPos);

    float axisOffset = isLeft ? 0.10124f : -0.10124f;
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, axisOffset));

    if (isLeft)
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));

    if (isFront)
        model = glm::rotate(model, -glm::radians(steeringAngle), glm::vec3(0, 1, 0));

    model = glm::rotate(model, isLeft ? -wheelsRollAngle : wheelsRollAngle, glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -axisOffset));
    glm::mat4 mvp = projView * model;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    celShadingShader->setMatrices(mvp, carModel, model);
    wheel_.draw();

    return mvp;
}

void Car::drawWheelBorder(glm::mat4& wheelMvp)
{
    glUniformMatrix4fv(edgeEffectShader->mvpULoc, 1, GL_FALSE, glm::value_ptr(wheelMvp));
    wheel_.draw();
}

void Car::drawWheels(glm::mat4& projView, glm::mat4 outWheelMvps[4])
{
    glm::vec3 WHEEL_POSITIONS[] =
    {
        glm::vec3(-1.29f, 0.245f, -0.57f), //Front right
        glm::vec3(-1.29f, 0.245f,  0.38f), //Front left
        glm::vec3(1.4f , 0.245f, -0.57f), //Rear right
        glm::vec3(1.4f , 0.245f,  0.38f)  //Rear left
    };

    outWheelMvps[0] = drawWheel(projView, WHEEL_POSITIONS[0], true, false);
    outWheelMvps[1] = drawWheel(projView, WHEEL_POSITIONS[1], true, true);
    outWheelMvps[2] = drawWheel(projView, WHEEL_POSITIONS[2], false, false);
    outWheelMvps[3] = drawWheel(projView, WHEEL_POSITIONS[3], false, true);
}

void Car::drawBlinker(glm::mat4& projView, glm::mat4& headlightMatrix, bool isLeftHeadlight)
{
    const glm::vec3 ON_COLOR(1.0f, 0.7f, 0.3f);
    const glm::vec3 OFF_COLOR(0.5f, 0.35f, 0.15f);

    bool isBlinkerActivated = (isLeftHeadlight && isLeftBlinkerActivated) ||
        (!isLeftHeadlight && isRightBlinkerActivated);

    Material blinkerMat =
    {
        glm::vec4(0.0f),                    
        glm::vec4(OFF_COLOR, 1.0f),         
        glm::vec4(OFF_COLOR, 1.0f),        
        glm::vec4(OFF_COLOR, 1.0f),        
        10.0f                                
    };

    if (isBlinkerOn && isBlinkerActivated)
    {
        blinkerMat.emission = glm::vec4(ON_COLOR, 1.0f);
    }
    else
    {
        blinkerMat.emission = glm::vec4(OFF_COLOR, 1.0f);
    }

    material->updateData(&blinkerMat, 0, sizeof(Material));

    glm::mat4 model = glm::translate(headlightMatrix, glm::vec3(0.0f, 0.0f, isLeftHeadlight ? 0.06065f : -0.06065f));
    glm::mat4 mvp = projView * model;

    celShadingShader->setMatrices(mvp, headlightMatrix, model);

    blinker_.draw();
}

void Car::drawLight(glm::mat4& projView, glm::mat4& headlightMatrix, bool isFront, bool isLeft)
{
    const glm::vec3 FRONT_ON_COLOR(1.0f, 1.0f, 1.0f);
    const glm::vec3 FRONT_OFF_COLOR(0.5f, 0.5f, 0.5f);
    const glm::vec3 REAR_ON_COLOR(1.0f, 0.1f, 0.1f);
    const glm::vec3 REAR_OFF_COLOR(0.5f, 0.1f, 0.1f);

    Material lightFrontMat =
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {FRONT_OFF_COLOR, 0.0f},
        {FRONT_OFF_COLOR, 0.0f},
        {FRONT_OFF_COLOR},
        10.0f
    };

    Material lightRearMat =
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {REAR_OFF_COLOR, 0.0f},
        {REAR_OFF_COLOR, 0.0f},
        {REAR_OFF_COLOR},
        10.0f
    };

    glm::mat4 model = glm::translate(headlightMatrix, glm::vec3(0.0f, 0.0f, isLeft ? -0.029f : 0.029f));

    if (isFront) {
        lightFrontMat.emission = glm::vec4(isHeadlightOn ? FRONT_ON_COLOR : FRONT_OFF_COLOR, 0);

        material->updateData(&lightFrontMat, 0, sizeof(Material));
    }
    else {
        lightRearMat.emission = glm::vec4(isBraking ? REAR_ON_COLOR : REAR_OFF_COLOR, 0);

        material->updateData(&lightRearMat, 0, sizeof(Material));
    }

    glm::mat4 mvp = projView * model;
    celShadingShader->setMatrices(mvp, headlightMatrix, model);

    light_.draw();
}

void Car::drawHeadlight(glm::mat4& projView, const glm::vec3& position, bool isFront, bool isLeft)
{
    glm::mat4 headlightMatrix = glm::translate(carModel, position);
    drawLight(projView, headlightMatrix, isFront, isLeft);
    drawBlinker(projView, headlightMatrix, isLeft);
}

void Car::drawHeadlights(glm::mat4& projView)
{
    const glm::vec3 HEADLIGHT_POSITIONS[] =
    {
        glm::vec3(-2.0019f, 0.64f, -0.45f), //Front right
        glm::vec3(-2.0019f, 0.64f,  0.45f), //Front left, 
        glm::vec3(2.0019f, 0.64f, -0.45f), //Rear right, 
        glm::vec3(2.0019f, 0.64f,  0.45f)  //Rear left, 
    };

    drawHeadlight(projView, HEADLIGHT_POSITIONS[0], true, false);
    drawHeadlight(projView, HEADLIGHT_POSITIONS[1], true, true);
    drawHeadlight(projView, HEADLIGHT_POSITIONS[2], false, false);
    drawHeadlight(projView, HEADLIGHT_POSITIONS[3], false, true);
}

