#include <cstddef>
#include <cstdint>

#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "happly.h"
#include <imgui/imgui.h>

#include <inf2705/OpenGLApplication.hpp>

#include "model.hpp"
#include "car.hpp"
#include "model_data.hpp"
#include "shaders.hpp"
#include "textures.hpp"
#include "uniform_buffer.hpp"
#include "shader_storage_buffer.hpp"

#define CHECK_GL_ERROR printGLError(__FILE__, __LINE__)

using namespace gl;
using namespace glm;

struct Vertex {
    glm::vec3 position;
    glm::vec4 color; // We might need to remove this as its not used
};

struct Material
{
    glm::vec4 emission; // vec3, but padded
    glm::vec4 ambient;  // vec3, but padded
    glm::vec4 diffuse;  // vec3, but padded
    glm::vec3 specular;
    GLfloat shininess;
};

struct DirectionalLight
{
    glm::vec4 ambient;   // vec3, but padded
    glm::vec4 diffuse;   // vec3, but padded
    glm::vec4 specular;  // vec3, but padded    
    glm::vec4 direction; // vec3, but padded
};

struct SpotLight
{
    glm::vec4 ambient;   // vec3, but padded
    glm::vec4 diffuse;   // vec3, but padded
    glm::vec4 specular;  // vec3, but padded

    glm::vec4 position;  // vec3, but padded
    glm::vec3 direction;
    GLfloat exponent;
    GLfloat openingAngle;

    GLfloat padding[3];
};

struct BezierCurve
{
    glm::vec3 p0;
    glm::vec3 c0;
    glm::vec3 c1;
    glm::vec3 p1;
};

// Ne pas modifier
struct Particle
{
    glm::vec3 position;
    GLfloat zOrientation;
    glm::vec4 velocity; // vec3, but padded
    glm::vec4 color;
    glm::vec2 size; 
    GLfloat timeToLive;
    GLfloat maxTimeToLive;
};

// Matériels

Material defaultMat =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {0.7f, 0.7f, 0.7f},
    10.0f
};

Material grassMat =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.8f, 0.8f, 0.8f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {0.05f, 0.05f, 0.05f},
    100.0f
};

Material streetMat =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.7f, 0.7f, 0.7f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {0.025f, 0.025f, 0.025f},
    300.0f
};

Material streetlightMat =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.8f, 0.8f, 0.8f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {0.7f, 0.7f, 0.7f},
    10.0f
};

Material streetlightLightMat =
{
    {0.8f, 0.7f, 0.5f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {0.7f, 0.7f, 0.7f},
    10.0f
};

Material windowMat =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 1.0f},
    2.0f
};

Material bezierMat =
{
    {1.0f, 1.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    0.0f
};

BezierCurve curves[5] =
{
    {
        glm::vec3(-28.7912, 1.4484, -1.7349),
        glm::vec3(-28.0654, 1.4484, 6.1932),
        glm::vec3(-10.3562, 8.8346, 6.5997),
        glm::vec3(-7.6701, 8.8346, 8.9952)
    },
    {
        glm::vec3(-7.6701, 8.8346, 8.9952),
        glm::vec3(-3.9578, 8.8346, 12.3057),
        glm::vec3(-2.5652, 2.4770, 13.6914),
        glm::vec3(2.5079, 1.4484, 11.6581)
    },
    {
        glm::vec3(2.5079, 1.4484, 11.6581),
        glm::vec3(7.5810, 0.4199, 9.6248),
        glm::vec3(16.9333, 3.3014, 5.7702),
        glm::vec3(28.4665, 6.6072, 3.9096)
    },
    {
        glm::vec3(28.4665, 6.6072, 3.9096),
        glm::vec3(39.9998, 9.9131, 2.0491),
        glm::vec3(30.8239, 5.7052, -15.2108),
        glm::vec3(21.3852, 5.7052, -9.0729)
    },
    {
        glm::vec3(21.3852, 5.7052, -9.0729),
        glm::vec3(11.9464, 5.7052, -2.9349),
        glm::vec3(-1.0452, 1.4484, -12.4989),
        glm::vec3(-12.2770, 1.4484, -13.2807)
    }
};

struct App : public OpenGLApplication
{
    App()
        : cameraPosition_(0.0f, 0.0f, 0.0f)
        , cameraOrientation_(0.0f, 0.0f)
        , isMouseMotionEnabled_(false)
        , currentScene_(0)
        , isDay_(true)
        , totalTime(0.0)
        , timerParticles_(0.0)
        , nParticles_(0)
    {
    }

    void init() override
    {
        // TODO: Allocation des SSBO.
        //       Allouer suffisament d'espace pour le nombre maximal de particules.
        //       Seulement le buffer en entrée à besoin d'être initialisé à 0.
        //       Réfléchisser au type d'usage.
        
        // TODO: Créer un vao pour le dessin des particules et activer les attributs nécessaires.


        // TODO: Création des nouveaux shaders.
        
        
        // TODO: Initialisation de la nouvelle texture pour les particules.
        // "../textures/smoke.png"

        setKeybindMessage(
            "ESC : quitter l'application." "\n"
            "T : changer de scène." "\n"
            "W : déplacer la caméra vers l'avant." "\n"
            "S : déplacer la caméra vers l'arrière." "\n"
            "A : déplacer la caméra vers la gauche." "\n"
            "D : déplacer la caméra vers la droite." "\n"
            "Q : déplacer la caméra vers le bas." "\n"
            "E : déplacer la caméra vers le haut." "\n"
            "Flèches : tourner la caméra." "\n"
            "Souris : tourner la caméra" "\n"
            "Espace : activer/désactiver la souris." "\n"
        );

        // Config de base.
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_PROGRAM_POINT_SIZE); // pour être en mesure de modifier gl_PointSize dans les shaders

        // TODO: Création des nouveaux shaders

        // TODO: Initialisation des meshes (béziers, patches)
        // TP1 polygone; au lieu de faire le point tu fait la forme pour faire un plan
        //calculateCurveVertices(bezierNPoints);
        //calculatePatchesVertices(patchesNPoints);


        edgeEffectShader_.create();
        celShadingShader_.create();
        skyShader_.create();
		grassShader_.create();
        particleShadingShader_.create();

        car_.edgeEffectShader = &edgeEffectShader_;
        car_.celShadingShader = &celShadingShader_;
        car_.material = &material_;

        grassTexture_.load("../textures/grass.jpg");
        grassTexture_.setWrap(GL_REPEAT);
        grassTexture_.setFiltering(GL_LINEAR);
        grassTexture_.enableMipmap();

        streetTexture_.load("../textures/street.jpg");
        streetTexture_.setWrap(GL_REPEAT);
        streetTexture_.setFiltering(GL_LINEAR);
        streetTexture_.enableMipmap();

        carTexture_.load("../textures/car.png");
        carTexture_.setWrap(GL_CLAMP_TO_EDGE);
        carTexture_.setFiltering(GL_LINEAR);

        carWindowTexture_.load("../textures/window.png");
        carWindowTexture_.setWrap(GL_CLAMP_TO_EDGE);
        carWindowTexture_.setFiltering(GL_NEAREST);

        treeTexture_.load("../textures/tree.jpg");
        treeTexture_.setWrap(GL_REPEAT);
        treeTexture_.setFiltering(GL_NEAREST);

        streetlightTexture_.load("../textures/streetlight.jpg");
        streetlightTexture_.setWrap(GL_REPEAT);
        streetlightTexture_.setFiltering(GL_LINEAR);

        streetlightLightTexture_.load("../textures/streetlight_light.png");
        streetlightLightTexture_.setWrap(GL_REPEAT);
        streetlightLightTexture_.setFiltering(GL_NEAREST);

        smokeTexture_.load("../textures/smoke.png");
        smokeTexture_.setWrap(GL_REPEAT);
        smokeTexture_.setFiltering(GL_LINEAR);
        smokeTexture_.enableMipmap();

        streetTexture_.use();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.0f);
        glBindTexture(GL_TEXTURE_2D, 0);

        const char* pathes[] = {
            "../textures/skybox/Daylight Box_Right.bmp",
            "../textures/skybox/Daylight Box_Left.bmp",
            "../textures/skybox/Daylight Box_Top.bmp",
            "../textures/skybox/Daylight Box_Bottom.bmp",
            "../textures/skybox/Daylight Box_Front.bmp",
            "../textures/skybox/Daylight Box_Back.bmp",
        };
        skyboxTexture_.load(pathes);

        const char* nightPathes[] = {
            "../textures/skyboxNight/right.png",
            "../textures/skyboxNight/left.png",
            "../textures/skyboxNight/top.png",
            "../textures/skyboxNight/bottom.png",
            "../textures/skyboxNight/front.png",
            "../textures/skyboxNight/back.png",
        };
        skyboxNightTexture_.load(nightPathes);

        loadModels();
        initStaticModelMatrices();

        material_.allocate(&defaultMat, sizeof(Material));
        material_.setBindingIndex(0);

        lightsData_.dirLight =
        {
            {0.2f, 0.2f, 0.2f, 0.0f},
            {1.0f, 1.0f, 1.0f, 0.0f},
            {0.5f, 0.5f, 0.5f, 0.0f},
            {0.5f, -1.0f, 0.5f, 0.0f}
        };

        for (unsigned int i = 0; i < N_STREETLIGHTS; i++)
        {
            lightsData_.spotLights[i].position = glm::vec4(streetlightLightPositions[i], 1.0f);
            lightsData_.spotLights[i].direction = glm::vec3(0, -1, 0);
            lightsData_.spotLights[i].exponent = 6.0f;
            lightsData_.spotLights[i].openingAngle = glm::radians(60.0f);
        }

        lightsData_.spotLights[N_STREETLIGHTS].position = glm::vec4(-1.6, 0.64, -0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS].direction = glm::vec3(-10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS].openingAngle = radians(30.f);

        lightsData_.spotLights[N_STREETLIGHTS + 1].position = glm::vec4(-1.6, 0.64, 0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS + 1].direction = glm::vec3(-10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS + 1].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS + 1].openingAngle = radians(30.f);

        lightsData_.spotLights[N_STREETLIGHTS + 2].position = glm::vec4(1.6, 0.64, -0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS + 2].direction = glm::vec3(10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS + 2].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS + 2].openingAngle = radians(60.f);

        lightsData_.spotLights[N_STREETLIGHTS + 3].position = glm::vec4(1.6, 0.64, 0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS + 3].direction = glm::vec3(10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS + 3].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS + 3].openingAngle = radians(60.f);

        toggleStreetlight();
        updateCarLight();

        setLightingUniform();

        lights_.allocate(&lightsData_, sizeof(lightsData_));
        lights_.setBindingIndex(1);

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        CHECK_GL_ERROR;
    }

    void checkShaderCompilingError(const char* name, GLuint id)
    {
        GLint success;
        GLchar infoLog[1024];

        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(id, 1024, NULL, infoLog);
            glDeleteShader(id);
            std::cout << "Shader \"" << name << "\" compile error: " << infoLog << std::endl;
        }
    }


    void checkProgramLinkingError(const char* name, GLuint id)
    {
        GLint success;
        GLchar infoLog[1024];

        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 1024, NULL, infoLog);
            glDeleteProgram(id);
            std::cout << "Program \"" << name << "\" linking error: " << infoLog << std::endl;
        }
    }

    void drawFrame() override
    {
        CHECK_GL_ERROR;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        ImGui::Begin("Scene Parameters");
        ImGui::Combo("Scene", &currentScene_, SCENE_NAMES, N_SCENE_NAMES);

        // TODO: Au besoin, ajouter la recharge de vos nouveaux shaders
        //       après if (ImGui::Button("Reload Shaders")).
        if (ImGui::Button("Reload Shaders"))
        {
            CHECK_GL_ERROR;
            edgeEffectShader_.reload();
            celShadingShader_.reload();
            skyShader_.reload();
			grassShader_.reload();

            setLightingUniform();
            CHECK_GL_ERROR;
        }
        ImGui::End();

        switch (currentScene_)
        {
        case 0: sceneMain(); break;
        }
        CHECK_GL_ERROR;
    }

    void onKeyPress(const sf::Event::KeyPressed& key) override
    {
        using enum sf::Keyboard::Key;
        switch (key.code)
        {
        case Escape:
            window_.close();
            break;
        case Space:
            isMouseMotionEnabled_ = !isMouseMotionEnabled_;
            if (isMouseMotionEnabled_)
            {
                window_.setMouseCursorGrabbed(true);
                window_.setMouseCursorVisible(false);
            }
            else
            {
                window_.setMouseCursorGrabbed(false);
                window_.setMouseCursorVisible(true);
            }
            break;
        default: break;
        }
    }

    void onResize(const sf::Event::Resized& event) override
    {
    }

    void onMouseMove(const sf::Event::MouseMoved& mouseDelta) override
    {
        if (!isMouseMotionEnabled_)
            return;

        const float MOUSE_SENSITIVITY = 0.1;
        float cameraMouvementX = mouseDelta.position.y * MOUSE_SENSITIVITY;
        float cameraMouvementY = mouseDelta.position.x * MOUSE_SENSITIVITY;
        cameraOrientation_.y -= cameraMouvementY * deltaTime_;
        cameraOrientation_.x -= cameraMouvementX * deltaTime_;
    }

    void updateCameraInput()
    {
        if (!window_.hasFocus())
            return;

        if (isMouseMotionEnabled_)
        {
            sf::Vector2u windowSize = window_.getSize();
            sf::Vector2i windowHalfSize(windowSize.x / 2.0f, windowSize.y / 2.0f);
            sf::Mouse::setPosition(windowHalfSize, window_);
        }

        float cameraMouvementX = 0;
        float cameraMouvementY = 0;

        const float KEYBOARD_MOUSE_SENSITIVITY = 1.5f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            cameraMouvementX -= KEYBOARD_MOUSE_SENSITIVITY;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            cameraMouvementX += KEYBOARD_MOUSE_SENSITIVITY;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            cameraMouvementY -= KEYBOARD_MOUSE_SENSITIVITY;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            cameraMouvementY += KEYBOARD_MOUSE_SENSITIVITY;

        cameraOrientation_.y -= cameraMouvementY * deltaTime_;
        cameraOrientation_.x -= cameraMouvementX * deltaTime_;

        glm::vec3 positionOffset = glm::vec3(0.0);
        const float SPEED = 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            positionOffset.z -= SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            positionOffset.z += SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            positionOffset.x -= SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            positionOffset.x += SPEED;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
            positionOffset.y -= SPEED;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
            positionOffset.y += SPEED;

        positionOffset = glm::rotate(glm::mat4(1.0f), cameraOrientation_.y, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(positionOffset, 1);
        cameraPosition_ += positionOffset * glm::vec3(deltaTime_);
    }

    void loadModels()
    {
        car_.loadModels();
        tree_.load("../models/tree.ply");
        streetlight_.load("../models/streetlight.ply");
        streetlightLight_.load("../models/streetlight_light.ply");
        skybox_.load("../models/skybox.ply");
        grass_.load(ground, sizeof(ground), planeElements, sizeof(planeElements));
        street_.load(street, sizeof(street), planeElements, sizeof(planeElements));
    }

    void initStaticModelMatrices()
    {
        const unsigned int SEED = 123;
        std::mt19937 rng(SEED);
        const float STREET_OFFSET = STREET_WIDTH / 2;
        std::uniform_real_distribution<float> distMargin(10.0f, 20.0f);

        float x = -MAP_SIZE / 2;
        for (unsigned int i = 0; i < N_STREETLIGHTS; ++i)
        {
            x += distMargin(rng);
            float y = -0.15f;
            float z = 0.5f + STREET_OFFSET;
            float angle = M_PI * 3 / 2;
            glm::mat4 model(1);
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            streetlightModelMatrices_[i] = model;
            streetlightLightPositions[i] = glm::vec3(streetlightModelMatrices_[i] * glm::vec4(-2.77, 5.2, 0.0, 1.0));
        }

        std::uniform_real_distribution<float> distMarginTrees(5.0f, 11.0f);
        std::uniform_real_distribution<float> distEdgePadding(1.5f + STREET_OFFSET, 3.5f + STREET_OFFSET);
        std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * M_PI);
        std::uniform_real_distribution<float> distScale(0.6f, 1.2f);

        x = -MAP_SIZE / 2;
        for (unsigned int i = 0; i < N_TREES; ++i)
        {
            x += distMarginTrees(rng);
            float y = -0.15f;
            float z = -distEdgePadding(rng);
            float angle = distAngle(rng);
            float scale = distScale(rng);
            glm::mat4 model(1);
            model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(scale));
            treeModelMatrices_[i] = model;
        }

        areStreetlightsInitialized_ = true;
        areTreesInitialized_ = true;
    }

    void drawStreetlights(glm::mat4& projView, glm::mat4& view)
    {
        streetlightTexture_.use();
        for (unsigned int i = 0; i < N_STREETLIGHTS; ++i)
        {
            streetlightMvps[i] = projView * streetlightModelMatrices_[i];
            celShadingShader_.setMatrices(streetlightMvps[i], view, streetlightModelMatrices_[i]);
            streetlight_.draw();
        }

        streetlightLightTexture_.use();
        for (unsigned int i = 0; i < N_STREETLIGHTS; i++)
        {
            if (!isDay_)
                setMaterial(streetlightLightMat);
            else
                setMaterial(streetlightMat);

            celShadingShader_.setMatrices(streetlightMvps[i], view, streetlightModelMatrices_[i]);
            streetlightLight_.draw();
        }
    }

    void drawStreetlightBorders()
    {
        for (unsigned int i = 0; i < N_STREETLIGHTS; ++i)
        {
            glUniformMatrix4fv(edgeEffectShader_.mvpULoc, 1, GL_FALSE, glm::value_ptr(streetlightMvps[i]));
            streetlight_.draw();
            streetlightLight_.draw();
        }
    }

    void drawTrees(glm::mat4& projView, glm::mat4& view)
    {
        treeTexture_.use();

        for (unsigned int i = 0; i < N_TREES; ++i)
        {
            treeMvps[i] = projView * treeModelMatrices_[i];
            celShadingShader_.setMatrices(treeMvps[i], view, treeModelMatrices_[i]);
            tree_.draw();
        }
    }

    void drawTreesBorder()
    {
        for (unsigned int i = 0; i < N_TREES; ++i)
        {
            glUniformMatrix4fv(edgeEffectShader_.mvpULoc, 1, GL_FALSE, glm::value_ptr(treeMvps[i]));
            tree_.draw();
        }
    }

    void drawStreet(glm::mat4& projView, glm::mat4& view)
    {
        glm::mat4 streetModel = glm::scale(glm::mat4(1), glm::vec3(MAP_SIZE, 1.0f, STREET_WIDTH));
        glm::mat4 streetMVP = projView * streetModel;
        celShadingShader_.setMatrices(streetMVP, view, streetModel);
        street_.draw();
    }

    void drawGrass(glm::mat4& projView, glm::mat4& view)
    {
        glm::mat4 grassModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f));
        grassModel = glm::scale(grassModel, glm::vec3(MAP_SIZE, 1.0f, 50.0f));
        glm::mat4 grassMVP = projView * grassModel;
        celShadingShader_.setMatrices(grassMVP, view, grassModel);
        grass_.draw();
    }

    void calculateCurveVertices(unsigned int nPoints)
    {
        curveVertices.clear();
        indices.clear();

        unsigned int currentIndex = 0;

        for (unsigned int j = 0; j < 5; ++j)
        {
            BezierCurve curve = curves[j];
            unsigned int start = (j == 0) ? 0 : 1;
            for (unsigned int i = start; i <= nPoints + 1; ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(nPoints + 1);
                float u = 1.0f - t;
                glm::vec3 position =
                    u * u * u * curve.p0 +
                    3 * t * u * u * curve.c0 +
                    3 * t * t * u * curve.c1 +
                    t * t * t * curve.p1;
                curveVertices.push_back({ position, glm::vec4(1.0f) });
                indices.push_back(currentIndex++);
            }
        }
    }

    void calculatePatchesVertices(unsigned int nPoints) {
        patchesVertices.clear();
        indices2.clear();

        unsigned int currentIndex = 0;

        for (unsigned int j = 0; j < 5; ++j)
        {
            //Vertex patch = patches[j]; // Puisquon a seulement besoin de pos? 
            // Ajouter un boucle for pour les coords
            // On peut commencer par ajouter un grand caree un bord et ensuite dans lautre bord
            unsigned int start = (j == 0) ? 0 : 1;
            for (unsigned int i = start; i <= nPoints + 1; ++i)
            {
                //float t = static_cast<float>(i) / static_cast<float>(nPoints + 1);
                //float u = 1.0f - t;
                //glm::vec3 position = patch; 
                //patchesVertices.push_back({ position, glm::vec4(1.0f) }); // Enlever color? 
                //indices2.push_back(currentIndex++);
            }
        }
    }

    void drawCurve(glm::mat4& projView, glm::mat4& view)
    {
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(Vertex), curveVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = projView * model;
        celShadingShader_.setMatrices(mvp, view, model);

        glDrawElements(GL_LINE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void drawPatch() {

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo); 

        glBufferData(GL_ARRAY_BUFFER, patchesVertices.size() * sizeof(Vertex), &patchesVertices[0], GL_STATIC_DRAW); 

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(unsigned int), &indices2[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); 

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); // Can remove l8tr

        glDrawElements(GL_TRIANGLES, indices2.size(), GL_UNSIGNED_INT, 0); // GL_PATCH pour la primitive APRES AVOIR AJOUTER LES SHADERS

        glBindVertexArray(0);
    }

    glm::mat4 getViewMatrix()
    {
        glm::mat4 view = glm::mat4(1.0);
        view = glm::rotate(view, -cameraOrientation_.x, glm::vec3(1.0, 0.0, 0.0));
        view = glm::rotate(view, -cameraOrientation_.y, glm::vec3(0.0, 1.0, 0.0));
        view = glm::translate(view, -cameraPosition_);
        return view;
    }

    glm::mat4 getPerspectiveProjectionMatrix()
    {
        sf::Vector2u windowSize = window_.getSize();

        if (windowSize.y == 0) {
            std::cerr << "Error: Division by zero is not allowed." << std::endl;
            return mat4(1);
        }

        // TODO: Pertinent de modifier la distance ici.
        const float far = 300.f;
        float aspect = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, far);

        return projection;
    }

    void setLightingUniform()
    {
        celShadingShader_.use();
        glUniform1i(celShadingShader_.nSpotLightsULoc, N_STREETLIGHTS + 4);

        float ambientIntensity = 0.05;
        glUniform3f(celShadingShader_.globalAmbientULoc, ambientIntensity, ambientIntensity, ambientIntensity);
    }

    void toggleSun()
    {
        if (isDay_)
        {
            lightsData_.dirLight.ambient = glm::vec4(0.2f, 0.2f, 0.2f, 0.0f);
            lightsData_.dirLight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
            lightsData_.dirLight.specular = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);
        }
        else
        {
            lightsData_.dirLight.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            lightsData_.dirLight.diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            lightsData_.dirLight.specular = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    void toggleStreetlight()
    {
        if (isDay_)
        {
            for (unsigned int i = 0; i < N_STREETLIGHTS; i++)
            {
                lightsData_.spotLights[i].ambient = glm::vec4(0.0f);
                lightsData_.spotLights[i].diffuse = glm::vec4(0.0f);
                lightsData_.spotLights[i].specular = glm::vec4(0.0f);
            }
        }
        else
        {
            for (unsigned int i = 0; i < N_STREETLIGHTS; i++)
            {
                lightsData_.spotLights[i].ambient = glm::vec4(glm::vec3(0.02f), 0.0f);
                lightsData_.spotLights[i].diffuse = glm::vec4(glm::vec3(0.8f), 0.0f);
                lightsData_.spotLights[i].specular = glm::vec4(glm::vec3(0.4f), 0.0f);
            }
        }
    }

    void updateCarLight()
    {
        if (car_.isHeadlightOn)
        {
            lightsData_.spotLights[N_STREETLIGHTS].ambient = glm::vec4(glm::vec3(0.01), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].diffuse = glm::vec4(glm::vec3(1.0), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].specular = glm::vec4(glm::vec3(0.4), 0.0f);

            lightsData_.spotLights[N_STREETLIGHTS + 1].ambient = glm::vec4(glm::vec3(0.01), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 1].diffuse = glm::vec4(glm::vec3(1.0), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 1].specular = glm::vec4(glm::vec3(0.4), 0.0f);


            glm::vec3 headlightPosLeft = glm::vec3(-1.6, 0.64, -0.45);
            glm::vec3 headlightPosRight = glm::vec3(-1.6, 0.64, 0.45);
            glm::vec3 headlightDir = glm::normalize(glm::vec3(-10, -1, 0));

            lightsData_.spotLights[N_STREETLIGHTS].position = car_.carModel * glm::vec4(headlightPosLeft, 1.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 1].position = car_.carModel * glm::vec4(headlightPosRight, 1.0f);

            glm::vec3 transformedDir = glm::mat3(car_.carModel) * headlightDir;
            lightsData_.spotLights[N_STREETLIGHTS].direction = transformedDir;
            lightsData_.spotLights[N_STREETLIGHTS + 1].direction = transformedDir;
        }
        else
        {
            lightsData_.spotLights[N_STREETLIGHTS].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].specular = glm::vec4(0.0f);

            lightsData_.spotLights[N_STREETLIGHTS + 1].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 1].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 1].specular = glm::vec4(0.0f);
        }

        if (car_.isBraking)
        {
            lightsData_.spotLights[N_STREETLIGHTS + 2].ambient = glm::vec4(0.01, 0.0, 0.0, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 2].diffuse = glm::vec4(0.9, 0.1, 0.1, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 2].specular = glm::vec4(0.35, 0.05, 0.05, 0.0f);

            lightsData_.spotLights[N_STREETLIGHTS + 3].ambient = glm::vec4(0.01, 0.0, 0.0, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 3].diffuse = glm::vec4(0.9, 0.1, 0.1, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 3].specular = glm::vec4(0.35, 0.05, 0.05, 0.0f);

            glm::vec3 brakeLightPosLeft = glm::vec3(1.6, 0.64, -0.45);
            glm::vec3 brakeLightPosRight = glm::vec3(1.6, 0.64, 0.45);
            glm::vec3 brakeLightDir = glm::normalize(glm::vec3(10, -1, 0));

            lightsData_.spotLights[N_STREETLIGHTS + 2].position = car_.carModel * glm::vec4(brakeLightPosLeft, 1.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 3].position = car_.carModel * glm::vec4(brakeLightPosRight, 1.0f);

            glm::vec3 transformedDir = glm::mat3(car_.carModel) * brakeLightDir;
            lightsData_.spotLights[N_STREETLIGHTS + 2].direction = transformedDir;
            lightsData_.spotLights[N_STREETLIGHTS + 3].direction = transformedDir;
        }
        else
        {
            lightsData_.spotLights[N_STREETLIGHTS + 2].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 2].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 2].specular = glm::vec4(0.0f);

            lightsData_.spotLights[N_STREETLIGHTS + 3].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 3].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS + 3].specular = glm::vec4(0.0f);
        }

    }

    void setMaterial(Material& mat)
    {
        material_.updateData(&mat, 0, sizeof(Material));
    }

    void sceneMain()
    {
        ImGui::Begin("Scene Parameters");
        // TODO: À ajouter
        ImGui::SliderInt("Bezier Number Of Points", (int*)&bezierNPoints, 0, 16);
        if (ImGui::Button("Animate Camera"))
        {
            isAnimatingCamera = true;
            cameraMode = 1;
        }
        //
        if (ImGui::Button("Toggle Day/Night"))
        {
            isDay_ = !isDay_;
            toggleSun();
            toggleStreetlight();
            lights_.updateData(&lightsData_, 0, sizeof(DirectionalLight) + N_STREETLIGHTS * sizeof(SpotLight));
        }
        ImGui::SliderFloat("Car Speed", &car_.speed, -10.0f, 10.0f, "%.2f m/s");
        ImGui::SliderFloat("Steering Angle", &car_.steeringAngle, -30.0f, 30.0f, "%.2f°");
        if (ImGui::Button("Reset Steering"))
            car_.steeringAngle = 0.f;
        ImGui::Checkbox("Headlight", &car_.isHeadlightOn);
        ImGui::Checkbox("Left Blinker", &car_.isLeftBlinkerActivated);
        ImGui::Checkbox("Right Blinker", &car_.isRightBlinkerActivated);
        ImGui::Checkbox("Brake", &car_.isBraking);
        ImGui::End();

        if (isAnimatingCamera)
        {
            if (cameraAnimation < 5)
            {
                // TODO: Animation de la caméra
                // cameraPosition_ = ...

                cameraAnimation += deltaTime_ / 3.0;

                float progress = cameraAnimation / 5.0f;
                unsigned int totalNPoints = (bezierNPoints + 1) * 5;
                float tGlobal = progress * (totalNPoints - 1);
                unsigned int idx = static_cast<unsigned int>(tGlobal);
                idx = std::min(idx, totalNPoints - 2);
                float t = tGlobal - idx;

                glm::vec3 start = curveVertices[idx].position;
                glm::vec3 end = curveVertices[idx + 1].position;

                cameraPosition_ = glm::mix(start, end, t);

                glm::vec3 diff = cameraPosition_ - car_.position;
                cameraOrientation_.y = atan2(diff.x, diff.z);
                cameraOrientation_.x = atan2(-diff.y, glm::length(glm::vec2(diff.x, diff.z)));
            }
            else
            {
                // Remise à 0 de l'orientation
                glm::vec3 diff = car_.position - cameraPosition_;
                cameraOrientation_.y = M_PI + atan2(diff.z, diff.x);

                cameraAnimation = 0.f;
                isAnimatingCamera = false;
                cameraMode = 0;
            }
        }
        else {
            updateCameraInput();
        }

        bool hasNumberOfSidesChanged = bezierNPoints != oldBezierNPoints;
        if (hasNumberOfSidesChanged)
        {
            oldBezierNPoints = bezierNPoints;

            // TODO: Calcul et mise à jour de la courbe
            calculateCurveVertices(bezierNPoints);
            //calculatePatchesVertices(patchesNPoints); 
        }

        car_.update(deltaTime_);

        updateCarLight();
        lights_.updateData(&lightsData_.spotLights[N_STREETLIGHTS], sizeof(DirectionalLight) + N_STREETLIGHTS * sizeof(SpotLight), 4 * sizeof(SpotLight));

        // TODO: Attention à l'endroit où vous faites votre dessin, la texture des particules est transparente.
        
        // Particles    
        totalTime += deltaTime_;
        timerParticles_ += deltaTime_;        
        const float particlesSpawnInterval = 0.2f;
        
        unsigned int particlesToAdd = timerParticles_ / particlesSpawnInterval;
        timerParticles_ -= particlesToAdd * particlesSpawnInterval;
        
        nParticles_ += particlesToAdd;
        if (nParticles_ > MAX_PARTICLES_)
            nParticles_ = MAX_PARTICLES_;

        glm::mat4 view = getViewMatrix();
        glm::mat4 proj = getPerspectiveProjectionMatrix();
        glm::mat4 projView = proj * view;

        skyShader_.use();
        if (isDay_)
        {
            skyboxTexture_.use();
        }
        else
        {
            skyboxNightTexture_.use();
        }
        glDepthFunc(GL_LEQUAL);
        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
        glm::mat4 mvp = proj * viewNoTranslation;
        glUniformMatrix4fv(skyShader_.mvpULoc, 1, GL_FALSE, glm::value_ptr(mvp));
        skybox_.draw();
        glDepthFunc(GL_LESS);

        // TODO: Dessin du gazon
        // glDraw...

        celShadingShader_.use();

        // TODO: Dessin de la courbe
        // glDraw...

        setMaterial(bezierMat);
        drawCurve(projView, view);

        streetTexture_.use();
        setMaterial(streetMat);
        drawStreet(projView, view);

        grassTexture_.use();
        setMaterial(grassMat);
        drawGrass(projView, view);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        drawTrees(projView, view);

        setMaterial(streetlightMat);
        drawStreetlights(projView, view);

        carTexture_.use();
        setMaterial(defaultMat);
        CarDrawResult carDrawResult = car_.draw(projView);

        carWindowTexture_.use();
        setMaterial(windowMat);
        car_.drawWindows(projView, view);

        carTexture_.use();
        setMaterial(defaultMat);
        car_.drawHeadlights(projView);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDepthMask(GL_FALSE);

        edgeEffectShader_.use();
        drawTreesBorder();
        drawStreetlightBorders();
        car_.drawBorder(carDrawResult);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glDepthMask(GL_TRUE);

        // Particles update
    
        // TODO: Mise à jour des données à l'aide du compute shader
        //       Envoyer vos uniforms.
        
        // Utiliser car_.carModel pour calculer la nouvelle position et direction d'émission de particule.
        // glm::vec3 exhaustPos = vec3(2.0, 0.24, -0.43);
        // glm::vec3 exhaustDir = vec3(1, 0, 0);
        
        // TODO: Configurer les buffers d'entrée et de sortie.
        
        // TODO: Envois de la commande de calcul.
        //       Pas besoin d'optimiser le nombre de work group vs la taille local (dans le shader).
        
        
        // Particles draw
        
        // TODO: Dessin des particules. Utiliser le nombre de particules actuellement utilisées.
        //       Utiliser la texture et envoyer vos uniforms.
        //       Il sera nécessaire de spécifier les entrée en spécifiant le buffer d'entrée.
        //       Activer le blending et restaurer l'état du contexte modifié.
        
        // TODO: Interchanger les deux buffers, celui en entrée devient la sortie, et vice versa.
    }

private:
    EdgeEffect edgeEffectShader_;
    CelShading celShadingShader_;
    Sky skyShader_;
	Grass grassShader_;
    ParticleShading particleShadingShader_;

    Texture2D grassTexture_;
    Texture2D streetTexture_;
    Texture2D carTexture_;
    Texture2D carWindowTexture_;
    Texture2D treeTexture_;
    Texture2D streetlightTexture_;
    Texture2D streetlightLightTexture_;
    Texture2D smokeTexture_;
    TextureCubeMap skyboxTexture_;
    TextureCubeMap skyboxNightTexture_;

    UniformBuffer material_;
    UniformBuffer lights_;

    struct {
        DirectionalLight dirLight;
        SpotLight spotLights[16];
    } lightsData_;

    bool isDay_;

    Model tree_;
    Model streetlight_;
    Model streetlightLight_;
    Model grass_;
    Model street_;
    Model skybox_;

    Car car_;

    glm::vec3 cameraPosition_;
    glm::vec2 cameraOrientation_;

    static constexpr unsigned int N_TREES = 12;
    static constexpr unsigned int N_STREETLIGHTS = 5;

    glm::mat4 treeModelMatrices_[N_TREES];
    glm::mat4 streetlightModelMatrices_[N_STREETLIGHTS];
    glm::vec3 streetlightLightPositions[N_STREETLIGHTS];

    glm::mat4 treeMvps[N_TREES];
    glm::mat4 streetlightMvps[N_STREETLIGHTS];

    const char* const SCENE_NAMES[1] = {
        "Main scene"
    };
    const int N_SCENE_NAMES = sizeof(SCENE_NAMES) / sizeof(SCENE_NAMES[0]);
    int currentScene_;

    bool isMouseMotionEnabled_;
    bool areTreesInitialized_ = false;
    bool areStreetlightsInitialized_ = false;

    const float MAP_SIZE = 100.0f;
    const float STREET_WIDTH = 5.0f;

    // TODO: Ajouter ces attributs
    unsigned int bezierNPoints = 3;
    unsigned int oldBezierNPoints = 0;
    unsigned int patchesNPoints = 3;
    unsigned int patchesLength = 4;
    unsigned int patchesHeight = 13;

    int cameraMode = 0;
    float cameraAnimation = 0.f;
    bool isAnimatingCamera = false;

    // TODO: Ajouter les attributs de vbo, ebo, vao nécessaire
    GLuint vao, vbo, ebo;
    std::vector<Vertex> curveVertices;
    std::vector<unsigned int> indices;

	std::vector<Vertex> patchesVertices;
	std::vector<unsigned int> indices2;

    GLuint vaoParticles_;

    float totalTime;
    float timerParticles_;

    static const unsigned int MAX_PARTICLES_ = 64;
    unsigned int nParticles_;    

    // Ssbo
    ShaderStorageBuffer particles_[2];
};


int main(int argc, char* argv[])
{
    WindowSettings settings = {};
    settings.fps = 100;
    settings.context.depthBits = 24;
    settings.context.stencilBits = 8;
    settings.context.antiAliasingLevel = 4;
    settings.context.majorVersion = 3;
    settings.context.minorVersion = 3;
    settings.context.attributeFlags = sf::ContextSettings::Attribute::Core;

    App app;
    app.run(argc, argv, "Tp2", settings);
}
