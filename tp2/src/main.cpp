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

#define CHECK_GL_ERROR printGLError(__FILE__, __LINE__)

using namespace gl;
using namespace glm;

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

struct App : public OpenGLApplication
{
    App()
        : cameraPosition_(-9.0f, 1.5f, 1.5f)
        , cameraOrientation_(-10.f, -60.0f)
        , isMouseMotionEnabled_(false)
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

        glClearColor(0.7f, 0.7f, 0.7f, 1);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        loadShaderPrograms();
        loadModels();
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneModels();
    }

    void onClose() override
    {
        glDeleteProgram(shaderProgram_);
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
        grass_.load("../models/grass.ply");
        street_.load("../models/street.ply");
    }

    GLuint loadShaderObject(GLenum type, const char* path)
    {
        GLuint shader = glCreateShader(type);
        std::string source = readFile(path);
        const char* source_cstr = source.c_str();

        glShaderSource(shader, 1, &source_cstr, nullptr);
        glCompileShader(shader);

        App::checkShaderCompilingError(path, shader);

        return shader;
    }

    void loadShaderPrograms()
    {
        const char* TRANSFORM_VERTEX_SRC_PATH = "./shaders/transform.vs.glsl";
        const char* TRANSFORM_FRAGMENT_SRC_PATH = "./shaders/transform.fs.glsl";

        shaderProgram_ = glCreateProgram();
        GLuint transformVertexShader = loadShaderObject(GL_VERTEX_SHADER, TRANSFORM_VERTEX_SRC_PATH);
        GLuint transformFragmentShader = loadShaderObject(GL_FRAGMENT_SHADER, TRANSFORM_FRAGMENT_SRC_PATH);
        glAttachShader(shaderProgram_, transformVertexShader);
        glAttachShader(shaderProgram_, transformFragmentShader);
        glLinkProgram(shaderProgram_);

        App::checkProgramLinkingError("transform shader program", shaderProgram_);

        glDetachShader(shaderProgram_, transformVertexShader);
        glDetachShader(shaderProgram_, transformFragmentShader);
        glDeleteShader(transformVertexShader);
        glDeleteShader(transformFragmentShader);

        mvpUniformLocation_ = glGetUniformLocation(shaderProgram_, "mvp");
        colorModUniformLocation_ = glGetUniformLocation(shaderProgram_, "colorMod");
        car_.mvpUniformLocation = glGetUniformLocation(shaderProgram_, "mvp");
        car_.colorModUniformLocation = glGetUniformLocation(shaderProgram_, "colorMod");
    }

    void drawStreetlights(glm::mat4& projView)
    {
        if (!areStreetlightsInitialized_) {
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
            }
            areStreetlightsInitialized_ = true;
        }

        for (unsigned int i = 0; i < N_STREETLIGHTS; ++i)
        {
            glm::mat4 streetlightMVP = projView * streetlightModelMatrices_[i];
            glUniformMatrix4fv(mvpUniformLocation_, 1, GL_FALSE, glm::value_ptr(streetlightMVP));
            glUniform3f(colorModUniformLocation_, 1.0f, 1.0f, 1.0f);
            streetlight_.draw();
        }
    }

    void drawTrees(glm::mat4& projView)
    {
        if (!areTreesInitialized_) {
			const unsigned int SEED = 123;
            std::mt19937 rng(SEED);

            const float STREET_OFFSET = STREET_WIDTH / 2;

            std::uniform_real_distribution<float> distMargin(5.0f, 11.0f);
            std::uniform_real_distribution<float> distEdgePadding(1.5f + STREET_OFFSET, 3.5f + STREET_OFFSET);
            std::uniform_real_distribution<float> distAngle(0.0f, 2.0f * M_PI);
            std::uniform_real_distribution<float> distScale(0.6f, 1.2f);

            float x = -MAP_SIZE / 2;
            for (unsigned int i = 0; i < N_TREES; ++i)
            {
                x += distMargin(rng);
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
            areTreesInitialized_ = true;
        }

        for (unsigned int i = 0; i < N_TREES; ++i)
        {
            glm::mat4 treeMVP = projView * treeModelMatrices_[i];
            glUniformMatrix4fv(mvpUniformLocation_, 1, GL_FALSE, glm::value_ptr(treeMVP));
            glUniform3f(colorModUniformLocation_, 1.0f, 1.0f, 1.0f);
            tree_.draw();
		}
    }

    void drawGround(glm::mat4& projView)
    {
        glm::mat4 streetModel = glm::scale(glm::mat4(1), glm::vec3(MAP_SIZE, 0.0f, STREET_WIDTH));
        glm::mat4 streetMVP = projView * streetModel;
        glUniformMatrix4fv(mvpUniformLocation_, 1, GL_FALSE, glm::value_ptr(streetMVP));
        glUniform3f(colorModUniformLocation_, 1.f, 1.0f, 1.0f);
        street_.draw();

        glm::mat4 grassModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f));
        grassModel = glm::scale(grassModel, glm::vec3(MAP_SIZE, 1.0f, 50.0f));
        glm::mat4 grassMVP = projView * grassModel;
        glUniformMatrix4fv(mvpUniformLocation_, 1, GL_FALSE, glm::value_ptr(grassMVP));
        glUniform3f(colorModUniformLocation_, 1.0f, 1.0f, 1.0f);
        grass_.draw();
    }

    glm::mat4 getViewMatrix()
    {
        glm::mat4 view = glm::rotate(glm::mat4(1), -glm::radians(cameraOrientation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, -glm::radians(cameraOrientation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
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

    void sceneModels()
    {
        ImGui::Begin("Scene Parameters");
        ImGui::SliderFloat("Car speed", &car_.speed, -10.0f, 10.0f, "%.2f m/s");
        ImGui::SliderFloat("Steering Angle", &car_.steeringAngle, -30.0f, 30.0f, "%.2f°");
        if (ImGui::Button("Reset steering"))
            car_.steeringAngle = 0.f;
        ImGui::Checkbox("Headlight", &car_.isHeadlightOn);
        ImGui::Checkbox("Left Blinker", &car_.isLeftBlinkerActivated);
        ImGui::Checkbox("Right Blinker", &car_.isRightBlinkerActivated);
        ImGui::Checkbox("Brake", &car_.isBraking);
        ImGui::End();

        updateCameraInput();
        car_.update(deltaTime_);

        glUseProgram(shaderProgram_);

        glm::mat4 projView = getPerspectiveProjectionMatrix() * getViewMatrix();
        drawGround(projView);
        drawTrees(projView);

		drawStreetlights(projView);
        car_.draw(projView);
    }

private:
    GLuint shaderProgram_;
    GLuint colorModUniformLocation_;
    GLuint mvpUniformLocation_;

    Model tree_;
    Model streetlight_;
    Model grass_;
    Model street_;

    Car car_;

    glm::vec3 cameraPosition_;
    glm::vec2 cameraOrientation_;

    static constexpr unsigned int N_TREES = 12;
    static constexpr unsigned int N_STREETLIGHTS = 5;
    glm::mat4 treeModelMatrices_[N_TREES];
    glm::mat4 streetlightModelMatrices_[N_STREETLIGHTS];

    bool isMouseMotionEnabled_;
	bool areTreesInitialized_ = false;
    bool areStreetlightsInitialized_ = false;

	const float MAP_SIZE = 100.0f;
	const float STREET_WIDTH = 5.0f;
};


int main(int argc, char* argv[])
{
    WindowSettings settings = {};
    settings.fps = 60;
    settings.context.depthBits = 24;
    settings.context.stencilBits = 8;
    settings.context.antiAliasingLevel = 4;
    settings.context.majorVersion = 3;
    settings.context.minorVersion = 3;
    settings.context.attributeFlags = sf::ContextSettings::Attribute::Core;

    App app;
    app.run(argc, argv, "TP2", settings);
}
