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

#define CHECK_GL_ERROR printGLError(__FILE__, __LINE__)

using namespace gl;
using namespace glm;

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
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

struct App : public OpenGLApplication
{
    App()
        : cameraPosition_(0.0f, 0.0f, 0.0f)
        , cameraOrientation_(0.0f, 0.0f)
        , isMouseMotionEnabled_(false)
        , currentScene_(0)
        , isDay_(true)
    {
    }

    void init() override
    {
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

        // Partie 1

        // TODO:
        // Création des shaders program.
        // Fait appel à la méthode "create()".
        edgeEffectShader_.create();
        celShadingShader_.create();
        skyShader_.create();

        // TODO: À ajouter.
        car_.edgeEffectShader = &edgeEffectShader_;
        car_.celShadingShader = &celShadingShader_;
        car_.material = &material_;

        // TODO: Chargement des textures, ainsi que la configuration de leurs paramètres.
        //
        //       Les textures ne se répètent pas, sauf le sol, la route, les arbres et les lampadaires.
        //
        //       Les textures ont un fini lisse, à l’exception des arbres, des lumières de lampadaire et
        //       des fenêtres de la voiture.
        //       
        //       Le mipmap __ne doit pas__ être activé pour toutes les textures, seulement le sol et la route.
        //

        // Simplement pour réduire l'effet "négatif" du mipmap qui rend la
        // texture flou trop près.
        // streetTexture_.use();
        // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.0f);
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

        streetTexture_.use();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1.0f);
        glBindTexture(GL_TEXTURE_2D, 0);

        // TODO: Chargement des deux skyboxes.

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

        // Partie 3
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
            lightsData_.spotLights[i].position = glm::vec4(streetlightLightPositions[i], 1.0f); // FIXED: w=1
            lightsData_.spotLights[i].direction = glm::vec3(0, -1, 0);
            lightsData_.spotLights[i].exponent = 6.0f;
            lightsData_.spotLights[i].openingAngle = glm::radians(60.0f); // Also convert to radians!
        }

        // Initialisation des paramètres de lumière des phares
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

    // Appelée à chaque trame. Le buffer swap est fait juste après.
    void drawFrame() override
    {
        CHECK_GL_ERROR;
        // TODO: Partie 2: Ajouter le nettoyage du tampon de stencil
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        ImGui::Begin("Scene Parameters");
        ImGui::Combo("Scene", &currentScene_, SCENE_NAMES, N_SCENE_NAMES);

        if (ImGui::Button("Reload Shaders"))
        {
            CHECK_GL_ERROR;
            edgeEffectShader_.reload();
            celShadingShader_.reload();
            skyShader_.reload();

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

        // TODO: Ajouter le chargement du sol et de la route avec la nouvelle méthode load
        //       des modèles. Voir "model_data.hpp".
        grass_.load(ground, sizeof(ground), planeElements, sizeof(planeElements));
        street_.load(street, sizeof(street), planeElements, sizeof(planeElements));
    }

    // Méthode pour le calcul des matrices initiales des arbres et des lampadaires.
    void initStaticModelMatrices()
    {
        // Initialize streetlights first
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

            // Calculate light position in world space
            streetlightLightPositions[i] = glm::vec3(streetlightModelMatrices_[i] * glm::vec4(-2.77, 5.2, 0.0, 1.0));
        }

        // Initialize trees
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

    // TODO: À modifier, ajouter les textures, et l'effet de contour.
    //       De plus, le modèle a été séparé en deux (pour la partie 3), adapter
    //       votre code pour faire le dessin des deux parties.
    void drawStreetlights(glm::mat4& projView, glm::mat4& view)
    {
        streetlightTexture_.use();
        setMaterial(streetlightMat);
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
        setMaterial(grassMat); 
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

    // TODO: À modifier, ajouter les textures
    void drawGround(glm::mat4& projView, glm::mat4& view)
    {
        streetTexture_.use();
        setMaterial(streetMat);
        glm::mat4 streetModel = glm::scale(glm::mat4(1), glm::vec3(MAP_SIZE, 1.0f, STREET_WIDTH));
        glm::mat4 streetMVP = projView * streetModel;
        celShadingShader_.setMatrices(streetMVP, view, streetModel);
        street_.draw();

        grassTexture_.use();
        setMaterial(grassMat);
        glm::mat4 grassModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f));
        grassModel = glm::scale(grassModel, glm::vec3(MAP_SIZE, 1.0f, 50.0f));
        glm::mat4 grassMVP = projView * grassModel;
        celShadingShader_.setMatrices(grassMVP, view, grassModel);
        grass_.draw();
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

        float aspect = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 100.0f);

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
        // Ça vous donne une idée de comment utiliser les ubo dans car.cpp.
        material_.updateData(&mat, 0, sizeof(Material));
    }

    // TODO: À ajouter et modifier.
    //       Ajouter les textures, les skyboxes, les fenêtres de la voiture,
    //       les effets de contour, etc.
    void sceneMain()
    {
        ImGui::Begin("Scene Parameters");
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

        updateCameraInput();
        car_.update(deltaTime_);

        updateCarLight();
        lights_.updateData(&lightsData_.spotLights[N_STREETLIGHTS], sizeof(DirectionalLight) + N_STREETLIGHTS * sizeof(SpotLight), 4 * sizeof(SpotLight));

        glm::mat4 view = getViewMatrix();
        glm::mat4 proj = getPerspectiveProjectionMatrix();
        glm::mat4 projView = proj * view;

        // TODO: Dessin des éléments
        // ...
        // Penser à votre ordre de dessin, les todos sont volontairement mélangé ici.

        // TODO: Dessin des fenêtres 
        // TODO: Dessin de l'automobile
        // TODO: Dessin du skybox
        // TODO: Dessin des arbres. Oui, ils utilisent le même matériel que le sol.
        // TODO: Dessin des lampadaires.

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

        celShadingShader_.use();
        drawGround(projView, view);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        drawTrees(projView, view);
        drawStreetlights(projView, view);

        carTexture_.use();
        CarDrawResult carDrawResult = car_.draw(projView);

        carWindowTexture_.use();
        car_.drawWindows(projView, view);

        carTexture_.use();
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
    }

private:
    // Shaders
    EdgeEffect edgeEffectShader_;
    CelShading celShadingShader_;
    Sky skyShader_;

    // Textures
    Texture2D grassTexture_;
    Texture2D streetTexture_;
    Texture2D carTexture_;
    Texture2D carWindowTexture_;
    Texture2D treeTexture_;
    Texture2D streetlightTexture_;
    Texture2D streetlightLightTexture_;
    TextureCubeMap skyboxTexture_;
    TextureCubeMap skyboxNightTexture_;

    // Uniform buffers
    UniformBuffer material_;
    UniformBuffer lights_;

    struct {
        DirectionalLight dirLight;
        SpotLight spotLights[16];
        //PointLight pointLights[4];
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

    // Imgui var
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
