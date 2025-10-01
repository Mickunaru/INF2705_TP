
// TODO: À ajouter et compléter dans votre projet.
#include "model_data.hpp"
#include "shaders.hpp"
#include "textures.hpp"
#include "uniform_buffer.hpp"


// Définition des structures pour la communication avec le shader. NE PAS MODIFIER.

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
    : isDay_(true) // TODO: À ajouter.
    , cameraPosition_(0.f, 0.f, 0.f)
    , cameraOrientation_(0.f, 0.f)
    , currentScene_(0)
    , isMouseMotionEnabled_(false)
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
        
        // Partie 1
        
        // TODO:
        // Création des shaders program.
        // Fait appel à la méthode "create()".
        
        
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
	    
        
        // TODO: Chargement des deux skyboxes.
        
        const char* pathes[] = {
            "../textures/skybox/Daylight Box_Right.bmp",
            "../textures/skybox/Daylight Box_Left.bmp",
            "../textures/skybox/Daylight Box_Top.bmp",
            "../textures/skybox/Daylight Box_Bottom.bmp",
            "../textures/skybox/Daylight Box_Front.bmp",
            "../textures/skybox/Daylight Box_Back.bmp",
        };
        
        const char* nightPathes[] = {
            "../textures/skyboxNight/right.png",
            "../textures/skyboxNight/left.png",
            "../textures/skyboxNight/top.png",
            "../textures/skyboxNight/bottom.png",
            "../textures/skyboxNight/front.png",
            "../textures/skyboxNight/back.png",
        };
        
        loadModels();        
        initStaticModelMatrices();
        
        // Partie 3
        
        // TODO: À ajouter. Aucune modification nécessaire.
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
            lightsData_.spotLights[i].position = glm::vec4(streetlightLightPositions[i], 0.0f);
            lightsData_.spotLights[i].direction = glm::vec3(0, -1, 0);
            lightsData_.spotLights[i].exponent = 6.0f;
            lightsData_.spotLights[i].openingAngle = 60.f;        
        }
        
        // Initialisation des paramètres de lumière des phares
        
        lightsData_.spotLights[N_STREETLIGHTS].position = glm::vec4(-1.6, 0.64, -0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS].direction = glm::vec3(-10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS].openingAngle = 30.f;
            
        lightsData_.spotLights[N_STREETLIGHTS+1].position = glm::vec4(-1.6, 0.64, 0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS+1].direction = glm::vec3(-10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS+1].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS+1].openingAngle = 30.f;
            
        lightsData_.spotLights[N_STREETLIGHTS+2].position = glm::vec4(1.6, 0.64, -0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS+2].direction = glm::vec3(10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS+2].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS+2].openingAngle = 60.f;
                   
        lightsData_.spotLights[N_STREETLIGHTS+3].position = glm::vec4(1.6, 0.64, 0.45, 0.0f);
        lightsData_.spotLights[N_STREETLIGHTS+3].direction = glm::vec3(10, -1, 0);
        lightsData_.spotLights[N_STREETLIGHTS+3].exponent = 4.0f;
        lightsData_.spotLights[N_STREETLIGHTS+3].openingAngle = 60.f;
        
        
        toggleStreetlight();
        updateCarLight();
        
        setLightingUniform();
        
        lights_.allocate(&lightsData_, sizeof(lightsData_));
        lights_.setBindingIndex(1);
        
        CHECK_GL_ERROR;
	}
	

	// Appelée à chaque trame. Le buffer swap est fait juste après.
	void drawFrame() override
	{
	    CHECK_GL_ERROR;
	    // TODO: Partie 2: Ajouter le nettoyage du tampon de stencil
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        
        ImGui::Begin("Scene Parameters");
        ImGui::Combo("Scene", &currentScene_, SCENE_NAMES, N_SCENE_NAMES);
        
        // TODO: À ajouter.
        // Et oui, il est désormais possible de recharger les shaders en gardant l'application ouvert.
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


	// TODO: À supprimer, tout ce qui gère le chargement des shaders.
	//       Le chargement est fait dans la classe ShaderProgram.
    
    
    void loadModels()
    {
        car_.loadModels();
        tree_.load("../models/tree.ply");
        streetlight_.load("../models/streetlight.ply");
        streetlightLight_.load("../models/streetlight_light.ply");
        skybox_.load("../models/skybox.ply");
        
        // TODO: Ajouter le chargement du sol et de la route avec la nouvelle méthode load
        //       des modèles. Voir "model_data.hpp".
    }
    
    // Méthode pour le calcul des matrices initiales des arbres et des lampadaires.
    void initStaticModelMatrices()
    {
        // ...
        for (unsigned int i = 0; i < N_STREETLIGHTS; i++)
        {
            // ...
            
            // TODO: À ajouter. C'est pour avoir la position de la lumière du lampadaire pour la partie 3.
            streetlightLightPositions[i] = glm::vec3(streetlightModelMatrices_[i] * glm::vec4(-2.77, 5.2, 0.0, 1.0));
        }
    }
    
    // TODO: À modifier, ajouter les textures, et l'effet de contour.
    //       De plus, le modèle a été séparé en deux (pour la partie 3), adapter
    //       votre code pour faire le dessin des deux parties.
    void drawStreetlights(glm::mat4& projView, glm::mat4& view)
    {
        // ...
        
        for (unsigned int i = 0; i < N_STREETLIGHTS; i++)
        {
            // ...

            if (!isDay_)
                setMaterial(streetlightLightMat);
            else
                setMaterial(streetlightMat);
            // TODO: Dessin du mesh de la lumière.
            
            setMaterial(streetlightMat);
            // TODO: Dessin du mesh du lampadaire.
        }
    }
    
    // TODO: À modifier, ajouter les textures, et l'effet de contour.
    void drawTrees(glm::mat4& projView, glm::mat4& view)
    {
        // ...
        
        for (unsigned int i = 0; i < N_TREES; i++)
        {
            // ...
        }
    }
    
    // TODO: À modifier, ajouter les textures
    void drawGround(glm::mat4& projView, glm::mat4& view)
    {
        // ...
        setMaterial(streetMat);
        // TODO: Dessin de la route.
        
        // ...
        setMaterial(grassMat);
        // TODO: Dessin du sol.        
    }
    
    // Solution
    glm::mat4 getViewMatrix()
    {
        glm::mat4 view = glm::mat4(1.0);                
        view = glm::rotate(view, -cameraOrientation_.x, glm::vec3(1.0, 0.0, 0.0));
        view = glm::rotate(view, -cameraOrientation_.y, glm::vec3(0.0, 1.0, 0.0));
        view = glm::translate(view, -cameraPosition_);
        return view;
    }
    
    // TODO: À ajouter. Pas de modification.
    void setLightingUniform()
    {
        celShadingShader_.use();
        glUniform1i(celShadingShader_.nSpotLightsULoc, N_STREETLIGHTS+4);
        
        float ambientIntensity = 0.05;
        glUniform3f(celShadingShader_.globalAmbientULoc, ambientIntensity, ambientIntensity, ambientIntensity);
    }
    
    // TODO: À ajouter. Pas de modification.
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
    
    // TODO: À ajouter. Pas de modification.
    void toggleStreetlight()
    {
        if (isDay_)
        {
            for (unsigned int i = 0; i < N_STREETLIGHTS; i++)
            {
                lightsData_.spotLights[i].ambient = glm::vec4(glm::vec3(0.0f), 0.0f);
                lightsData_.spotLights[i].diffuse = glm::vec4(glm::vec3(0.0f), 0.0f);
                lightsData_.spotLights[i].specular = glm::vec4(glm::vec3(0.0f), 0.0f);
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
    
    // TODO: À ajouter.
    void updateCarLight()
    {
        if (car_.isHeadlightOn)
        {
            lightsData_.spotLights[N_STREETLIGHTS].ambient = glm::vec4(glm::vec3(0.01), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].diffuse = glm::vec4(glm::vec3(1.0), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].specular = glm::vec4(glm::vec3(0.4), 0.0f);
            
            lightsData_.spotLights[N_STREETLIGHTS+1].ambient = glm::vec4(glm::vec3(0.01), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+1].diffuse = glm::vec4(glm::vec3(1.0), 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+1].specular = glm::vec4(glm::vec3(0.4), 0.0f);
            
            // TODO: Partie 3.
            //       Utiliser car_.carModel pour calculer la nouvelle position et orientation de la lumière.
            //       La lumière devrait suivre le véhicule qui se déplace.
            
            lightsData_.spotLights[N_STREETLIGHTS].position = glm::vec4(-1.6, 0.64, -0.45, 1.0f);
            lightsData_.spotLights[N_STREETLIGHTS].direction = glm::vec3(-10, -1, 0);
            
            lightsData_.spotLights[N_STREETLIGHTS+1].position = glm::vec4(-1.6, 0.64, 0.45, 1.0f);
            lightsData_.spotLights[N_STREETLIGHTS+1].direction = glm::vec3(-10, -1, 0);
        }
        else
        {
            lightsData_.spotLights[N_STREETLIGHTS].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS].specular = glm::vec4(0.0f);
            
            lightsData_.spotLights[N_STREETLIGHTS+1].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+1].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+1].specular = glm::vec4(0.0f);
        }
        
        if (car_.isBraking)
        {
            lightsData_.spotLights[N_STREETLIGHTS+2].ambient = glm::vec4(0.01, 0.0, 0.0, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+2].diffuse = glm::vec4(0.9, 0.1, 0.1, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+2].specular = glm::vec4(0.35, 0.05, 0.05, 0.0f);
            
            lightsData_.spotLights[N_STREETLIGHTS+3].ambient = glm::vec4(0.01, 0.0, 0.0, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+3].diffuse = glm::vec4(0.9, 0.1, 0.1, 0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+3].specular = glm::vec4(0.35, 0.05, 0.05, 0.0f);
            
            // TODO: Partie 3.
            //       Utiliser car_.carModel pour calculer la nouvelle position et orientation de la lumière.
            //       La lumière devrait suivre le véhicule qui se déplace.
            
            lightsData_.spotLights[N_STREETLIGHTS+2].position = glm::vec4(1.6, 0.64, -0.45, 1.0f);        
            lightsData_.spotLights[N_STREETLIGHTS+2].direction = glm::vec3(10, -1, 0);
            
            lightsData_.spotLights[N_STREETLIGHTS+3].position = glm::vec4(1.6, 0.64, 0.45, 1.0f);
            lightsData_.spotLights[N_STREETLIGHTS+3].direction = glm::vec3(10, -1, 0);
        }
        else
        {
            lightsData_.spotLights[N_STREETLIGHTS+2].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+2].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+2].specular = glm::vec4(0.0f);
            
            lightsData_.spotLights[N_STREETLIGHTS+3].ambient = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+3].diffuse = glm::vec4(0.0f);
            lightsData_.spotLights[N_STREETLIGHTS+3].specular = glm::vec4(0.0f);
        }
    }
    
    // TODO: À ajouter. Pas de modification.
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
        
        setMaterial(windowMat);
        // TODO: Dessin des fenêtres
        
        setMaterial(defaultMat);
        // TODO: Dessin de l'automobile
        
        // TODO: Dessin du skybox
        
        setMaterial(grassMat);
        // TODO: Dessin des arbres. Oui, ils utilisent le même matériel que le sol.
        
        setMaterial(streetlightMat);
        // TODO: Dessin des lampadaires.
    }
    
private:
    // TODO: À ajouter.

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
    
    // Imgui var
    const char* const SCENE_NAMES[1] = {
        "Main scene"
    };
    const int N_SCENE_NAMES = sizeof(SCENE_NAMES) / sizeof(SCENE_NAMES[0]);
    int currentScene_;
    
    bool isMouseMotionEnabled_;
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
	app.run(argc, argv, "Tp2", settings); // TODO: Modifier le nom, c'est le tp2 maintenant ;)
}
