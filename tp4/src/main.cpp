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
#include "shaders.hpp"
#include "textures.hpp"
#include "uniform_buffer.hpp"
#include "shader_storage_buffer.hpp"
#include "model_data.hpp"

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

struct Particle
{
    glm::vec3 position;
    GLfloat zOrientation;
    glm::vec4 velocity; // vec3, but padded
    glm::vec4 color;
    glm::vec2 size;
    GLfloat timeToLive;
    GLfloat maxTimeToLive;
    GLfloat frame;
    glm::vec3 padding;
};

Material defaultMat =
{
    {0.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 0.0f},
    {0.7f, 0.7f, 0.7f},
    10.0f
};

struct App : public OpenGLApplication
{
    App()
    : cameraPosition_(0.f, 0.f, 0.f)
    , cameraOrientation_(0.f, 0.f)
    , currentScene_(0)
    , isMouseMotionEnabled_(false)
    , totalTime(0.0)
    , timerParticles_(0.0)
    , nParticles_(0)
    {
    }
	
	void init() override
	{
		// Le message expliquant les touches de clavier.
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
        glEnable(GL_PROGRAM_POINT_SIZE);

        celShadingShader_.create();
        skyShader_.create();
        particlesDrawShader_.create();
        particlesUpdateShader_.create();

		crystalTexture_.load("../textures/crystal-uv-unwrap.png");
        crystalTexture_.setWrap(GL_CLAMP_TO_EDGE);
        crystalTexture_.setFiltering(GL_LINEAR);

        particleTexture_.load("../textures/star-spritesheet.png");
        particleTexture_.setWrap(GL_REPEAT);
        particleTexture_.setFiltering(GL_LINEAR);
        particleTexture_.enableMipmap();

        groundTexture_.load("../textures/moon-ground.png");
        groundTexture_.setWrap(GL_REPEAT);
        groundTexture_.setFiltering(GL_LINEAR);
        groundTexture_.enableMipmap();

        const char* pathes[] = {
            "../textures/skybox/px.bmp",
            "../textures/skybox/nx.bmp",
            "../textures/skybox/py.bmp",
            "../textures/skybox/ny.bmp",
            "../textures/skybox/pz.bmp",
            "../textures/skybox/nz.bmp",
        };
        skyboxTexture_.load(pathes);

        loadModels();
        initStaticModelMatrices();

        material_.allocate(&defaultMat, sizeof(Material));
        material_.setBindingIndex(0);

        lightsData_.dirLight =
        {
            {0.2f, 0.2f, 0.2f, 0.0f},
            {1.0f, 1.0f, 1.0f, 0.0f},
            {0.5f, 0.5f, 0.5f, 0.0f},
            {0.5f, -0.75f, 0.5f, 0.0f}
        };

		setLightingUniform();

        lights_.allocate(&lightsData_, sizeof(lightsData_));
        lights_.setBindingIndex(1);

        std::vector<Particle> zeroData(MAX_PARTICLES_, Particle());
        particles_[0].allocate(zeroData.data(), MAX_PARTICLES_ * sizeof(Particle), GL_DYNAMIC_DRAW);
        particles_[1].allocate(nullptr, MAX_PARTICLES_ * sizeof(Particle), GL_DYNAMIC_DRAW);

        particles_[0].setBindingIndex(0);
        particles_[1].setBindingIndex(1);

		initParticlesBuffers();

        CHECK_GL_ERROR;
	}

    void initParticlesBuffers()
    {
        glGenVertexArrays(1, &vaoParticles_);
        glBindVertexArray(vaoParticles_);

        particles_[1].bindAsArray();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, zOrientation));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, frame));

        glBindVertexArray(0);
    }

    void initStaticModelMatrices()
    {
		crystalModel_ = glm::mat4(1.0f);
		crystalModel_ = glm::translate(crystalModel_, glm::vec3(0.0f, 1.0f, -2.0f));
    }

    void setLightingUniform()
    {
        celShadingShader_.use();
        float ambientIntensity = 0.05;
        glUniform3f(celShadingShader_.globalAmbientULoc, ambientIntensity, ambientIntensity, ambientIntensity);
    }

	// Appelée à chaque trame. Le buffer swap est fait juste après.
	void drawFrame() override
	{
        CHECK_GL_ERROR;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        ImGui::Begin("Scene Parameters");
        ImGui::Combo("Scene", &currentScene_, SCENE_NAMES, N_SCENE_NAMES);
        ImGui::End();
        
        switch (currentScene_)
        {
            case 0: sceneMain();  break;
        }
        CHECK_GL_ERROR;
	}

	// Appelée lorsque la fenêtre se ferme.
	void onClose() override
	{

	}

	// Appelée lors d'une touche de clavier.
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
	        case T:
                currentScene_ = ++currentScene_ < N_SCENE_NAMES ? currentScene_ : 0;
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

        // Keyboard input
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
    
    // TODO: Add more models
    void loadModels()
    {
        skybox_.load("../models/skybox.ply");
		crystal_.load("../models/crystal.ply");
        ground_.load(groundVertices, sizeof(groundVertices), groundElements, sizeof(groundElements));
    }

    void drawCrystal(glm::mat4& projView, glm::mat4& view)
    {
        crystalTexture_.use();

        float floatAmplitude = 0.05f;
        float floatSpeed = 2.0f;
        float yOffset = sin(totalTime * floatSpeed) * floatAmplitude;

        float tiltAmplitude = glm::radians(3.0f);
        float tiltSpeed = 1.5f;
        float tiltX = sin(totalTime * tiltSpeed) * tiltAmplitude;
        float tiltZ = cos(totalTime * tiltSpeed * 0.7f) * tiltAmplitude;

        glm::mat4 floatingModel = glm::translate(crystalModel_, glm::vec3(0.0f, yOffset, 0.0f));
        floatingModel = glm::rotate(floatingModel, tiltX, glm::vec3(1.0f, 0.0f, 0.0f));
        floatingModel = glm::rotate(floatingModel, tiltZ, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 mvp = projView * floatingModel;
        celShadingShader_.setMatrices(mvp, view, floatingModel);
        crystal_.draw();
    }

    void drawParticles(glm::mat4& projView, glm::mat4& view)
    {
        glm::vec3 emitterPos = glm::vec3(crystalModel_[3]);
        glm::vec3 emitterDir = glm::normalize(glm::vec3(crystalModel_ * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

        particlesUpdateShader_.use();

        particlesUpdateShader_.setUniforms(deltaTime_, totalTime, emitterPos, emitterDir);

        particles_[0].setBindingIndex(0);
        particles_[1].setBindingIndex(1);

        GLuint workGroups = (MAX_PARTICLES_ + 63) / 64;
        glDispatchCompute(workGroups, 1, 1);

        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        particlesDrawShader_.use();
        particleTexture_.use();

        glm::mat4 particleModelView = view * crystalModel_;
        particlesDrawShader_.setMatrices(particleModelView, projView, view);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        glBindVertexArray(vaoParticles_);
        glDrawArrays(GL_POINTS, 0, nParticles_);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        std::swap(particles_[0], particles_[1]);
    }

    void drawGround(glm::mat4& projView, glm::mat4& view)
    {
		groundTexture_.use();
        glm::mat4 groundModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        groundModel = glm::scale(groundModel, glm::vec3(MAP_LENGTH, 1.0f, MAP_WIDTH));
        glm::mat4 groundMVP = projView * groundModel;
        celShadingShader_.setMatrices(groundMVP, view, groundModel);
        ground_.draw();
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
        const float far = 300.f;
        float aspect = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), aspect, 0.1f, far);

        return projection;
    }

    void setMaterial(Material& mat)
    {
        material_.updateData(&mat, 0, sizeof(Material));
    }
    
    void sceneMain()
    {
        CHECK_GL_ERROR;
        ImGui::Begin("Scene Parameters");

		// TODO: Add some cool parameters to tweak
        ImGui::SliderFloat("Light Rotation (deg)", &lightRotationDeg, 0.0f, 360.0f);
        ImGui::ColorEdit3("Light Color", (float*)&lightColor);

        ImGui::End();

        updateCameraInput();

        bool hasLightRotationChanged = oldLightRotationDeg != lightRotationDeg;
		bool hasLightColorChanged = oldLightColor.x != lightColor.x || oldLightColor.y != lightColor.y || oldLightColor.z != lightColor.z;
		bool hasLightChanged = hasLightRotationChanged || hasLightColorChanged;

        if (hasLightRotationChanged)
        {
            oldLightRotationDeg = lightRotationDeg;

            float rotRad = glm::radians(lightRotationDeg);

            glm::vec3 dir = glm::normalize(glm::vec3(
                sin(rotRad),
                -0.75,
                cos(rotRad)
            ));

            lightsData_.dirLight.direction = glm::vec4(dir, 0.0f);
		}

        if (hasLightColorChanged)
        {
            lightsData_.dirLight.diffuse = glm::vec4(lightColor.x, lightColor.y, lightColor.z, 1.0f);
        }

        if (hasLightChanged)
        {
            setLightingUniform();

            lights_.allocate(&lightsData_, sizeof(lightsData_));
            lights_.setBindingIndex(1);
		}

        glm::mat4 view = getViewMatrix();
        glm::mat4 proj = getPerspectiveProjectionMatrix();
        glm::mat4 projView = proj * view;

        totalTime += deltaTime_;
        timerParticles_ += deltaTime_;
        const float particlesSpawnInterval = 0.5f;

        unsigned int particlesToAdd = timerParticles_ / particlesSpawnInterval;
        timerParticles_ -= particlesToAdd * particlesSpawnInterval;

        nParticles_ += particlesToAdd;
        if (nParticles_ > MAX_PARTICLES_)
            nParticles_ = MAX_PARTICLES_;

        skyShader_.use();
        skyboxTexture_.use();

        glDepthFunc(GL_LEQUAL);
        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
        glm::mat4 mvp = proj * viewNoTranslation;
        glUniformMatrix4fv(skyShader_.mvpULoc, 1, GL_FALSE, glm::value_ptr(mvp));
        skybox_.draw();
        glDepthFunc(GL_LESS);

        celShadingShader_.use();
		setMaterial(defaultMat);

		drawGround(projView, view);
        drawCrystal(projView, view);
        drawParticles(projView, view);
        
		CHECK_GL_ERROR;
    }
    
private:
    CelShading celShadingShader_;
    Sky skyShader_;
    ParticlesDraw particlesDrawShader_;
    ParticlesUpdate particlesUpdateShader_;

	Texture2D groundTexture_;
    Texture2D crystalTexture_;
	Texture2D particleTexture_;
    TextureCubeMap skyboxTexture_;

    Model street_;
    Model skybox_;
	Model crystal_;
	Model ground_;

    UniformBuffer material_;
    UniformBuffer lights_;

    glm::mat4 crystalModel_;

    struct {
        DirectionalLight dirLight;
        SpotLight spotLights[16];
    } lightsData_;

    glm::vec3 cameraPosition_;
    glm::vec2 cameraOrientation_;

    // Imgui var
    const char* const SCENE_NAMES[1] = {
        "Main Scene"
    };
    const int N_SCENE_NAMES = sizeof(SCENE_NAMES) / sizeof(SCENE_NAMES[0]);
    int currentScene_;
    
    bool isMouseMotionEnabled_;

    GLuint vaoParticles_;

    float totalTime;
    float timerParticles_;

    static const unsigned int MAX_PARTICLES_ = 32;
    unsigned int nParticles_;

    ShaderStorageBuffer particles_[2];

    const float MAP_LENGTH = 50.0f;
    const float MAP_WIDTH = 50.0f;

    // Params
	float oldLightRotationDeg = 0.0f;
    float lightRotationDeg = 0.0f;

	ImVec4 oldLightColor = ImVec4(1, 1, 1, 1);
    ImVec4 lightColor = ImVec4(1, 1, 1, 1);
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
    settings.videoMode.size.x = 1920;
	settings.videoMode.size.y = 1024;

	App app;
	app.run(argc, argv, "Tp4", settings);
}
