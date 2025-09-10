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

// TODO: Il est fortement recommandé de définir quelques structs
//       pour représenter les attributs.
//       Faire de même pour représenter une vertex, qui est constitué d'attributs.
//       Cela facilitera l'utilisation et rendra votre code plus clair.
//       Un format entrelacé est recommandé (ordonné par vertex au lieu par attribut).
// struct ... { ... };

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
};

struct App : public OpenGLApplication
{
    App()
        : nSide_(5)
        , oldNSide_(0)
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

        // TODO: Initialisez la couleur de fond.
        glClearColor(0.5f, 0.5f, 0.5f, 1);

        // TODO: Partie 2: Activez le test de profondeur (GL_DEPTH_TEST) et
        //       l'élimination des faces arrières (GL_CULL_FACE).

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        loadShaderPrograms();

        // Partie 1
        initShapeData();

        // Partie 2
        loadModels();

        // TODO: Insérez les initialisations supplémentaires ici au besoin.
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
        // TODO: Nettoyage de la surface de dessin.
        // TODO: Partie 2: Ajoutez le nettoyage du tampon de profondeur.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Begin("Scene Parameters");
        ImGui::Combo("Scene", &currentScene_, SCENE_NAMES, N_SCENE_NAMES);
        ImGui::End();

        switch (currentScene_)
        {
        case 0: sceneShape();  break;
        case 1: sceneModels(); break;
        }
    }

    // Appelée lorsque la fenêtre se ferme.
    void onClose() override
    {
        // TODO: Libérez les ressources allouées (buffers, shaders, etc.).
        glDeleteProgram(basicSP_);
        glDeleteProgram(transformSP_);
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ebo_);
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
        // TODO: Chargement d'un shader object.
        //       Utilisez readFile pour lire le fichier.
        //       N'oubliez pas de vérifier les erreurs suite à la compilation
        //       avec la méthode App::checkShaderCompilingError.
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
        // TODO: Chargement des shader programs.
        //       N'oubliez pas de vérifier les erreurs suite à la liaison (linking)
        //       avec la méthode App::checkProgramLinkingError. Vous pouvez
        //       donner un nom unique pour plus facilement lire les erreurs 
        //       dans la console.
        //       Il est recommandé de détacher et de supprimer les shaders objects
        //       directement après la liaison.

        // Partie 1
        const char* COLOR_VERTEX_SRC_PATH = "./shaders/basic.vs.glsl";
        const char* COLOR_FRAGMENT_SRC_PATH = "./shaders/basic.fs.glsl";

        basicSP_ = glCreateProgram();
        GLuint colorVertexShader = loadShaderObject(GL_VERTEX_SHADER, COLOR_VERTEX_SRC_PATH);
        GLuint colorFragmentShader = loadShaderObject(GL_FRAGMENT_SHADER, COLOR_FRAGMENT_SRC_PATH);
        glAttachShader(basicSP_, colorVertexShader);
        glAttachShader(basicSP_, colorFragmentShader);
        glLinkProgram(basicSP_);

        App::checkProgramLinkingError("basic shader program", basicSP_);

        glDetachShader(basicSP_, colorVertexShader);
        glDetachShader(basicSP_, colorFragmentShader);
        glDeleteShader(colorVertexShader);
        glDeleteShader(colorFragmentShader);

        // Partie 2
        const char* TRANSFORM_VERTEX_SRC_PATH = "./shaders/transform.vs.glsl";
        const char* TRANSFORM_FRAGMENT_SRC_PATH = "./shaders/transform.fs.glsl";

        transformSP_ = glCreateProgram();
        GLuint transformVertexShader = loadShaderObject(GL_VERTEX_SHADER, TRANSFORM_VERTEX_SRC_PATH);
        GLuint transformFragmentShader = loadShaderObject(GL_FRAGMENT_SHADER, TRANSFORM_FRAGMENT_SRC_PATH);
        glAttachShader(transformSP_, transformVertexShader);
        glAttachShader(transformSP_, transformFragmentShader);
        glLinkProgram(transformSP_);

        App::checkProgramLinkingError("transform shader program", transformSP_);

        glDetachShader(transformSP_, transformVertexShader);
        glDetachShader(transformSP_, transformFragmentShader);
        glDeleteShader(transformVertexShader);
        glDeleteShader(transformFragmentShader);

        // TODO: Allez chercher les locations de vos variables uniform dans le shader
        //       pour initialiser mvpUniformLocation_ et car_.mvpUniformLocation,
        //       puis colorModUniformLocation_ et car_.colorModUniformLocation.

        mvpUniformLocation_ = glGetUniformLocation(basicSP_, "mvp");
        colorModUniformLocation_ = glGetUniformLocation(basicSP_, "colorMod");
        car_.mvpUniformLocation = glGetUniformLocation(transformSP_, "mvp");
        car_.colorModUniformLocation = glGetUniformLocation(transformSP_, "colorMod");
    }

    // TODO: Modifiez les types de vertices et elements pour votre besoin.
    void generateNgon(std::vector<Vertex>* vertices, std::vector<GLuint>* elements, unsigned int side)
    {
        // TODO: Générez un polygone à N côtés (couramment appelé N-gon).
        //       Vous devez gérer les cas entre 5 et 12 côtés (pentagone, hexagone
        //       , etc.). Ceux-ci ont un rayon constant de 0.7.
        //       Chaque point possède une couleur (libre au choix).
        //       Vous devez minimiser le nombre de points et définir des indices
        //       pour permettre la réutilisation.
        const float RADIUS = 0.7f;
        const vec4 RED = { 1.f, 0.f, 0.f, 1.f };
        const vec4 GREEN = { 0.f, 1.f, 0.f, 1.f };
        const vec4 BLUE = { 0.f, 0.f, 1.f, 1.f };
        const vec4 WHITE = { 1.f, 1.f, 1.f, 1.f };
        const float angleInc = 2.f * M_PI / side;
        *vertices = {};
        *elements = {};

        vertices->push_back({
            { 0.f, 0.f, 0.f }, WHITE
        });

        for (int i = 0; i < side; i++) {
            const float angle = i * angleInc;
            const float x = RADIUS * cos(angle);
            const float y = RADIUS * sin(angle);
            vec4 color;
            switch (i % 3) {
            case 0:
                color = RED;
                break;
            case 1:
                color = GREEN;
                break;
            case 2:
                color = BLUE;
                break;
            }

            vertices->push_back({
                {x, y, 0.f}, color
            });

            elements->push_back(0);
            elements->push_back(i + 1);

            if (i + 2 > side) {
                elements->push_back(1);
            }
            else {
                elements->push_back(i + 2);
            }
        }
    }

    void initShapeData()
    {
        // TODO: Initialisez les objets graphiques pour le dessin du polygone.
        //       Ne passez aucune donnée pour le moment (déjà géré dans App::sceneShape),
        //       on demande seulement de faire l'allocation de buffers suffisamment gros
        //       pour contenir le polygone durant toute l'exécution du programme.
        //       Réfléchissez bien à l'usage des buffers (paramètre de glBufferData).
        glGenBuffers(1, &vbo_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

        // TODO: Créez un vao et spécifiez le format des données dans celui-ci.
        //       N'oubliez pas de lier le ebo avec le vao et de délier le vao
        //       du contexte pour empêcher des modifications sur celui-ci.
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &ebo_);

        glBindVertexArray(vao_);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_.size() * sizeof(GLuint), elements_.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    void sceneShape()
    {
        ImGui::Begin("Scene Parameters");
        ImGui::SliderInt("Sides", &nSide_, MIN_N_SIDES, MAX_N_SIDES);
        ImGui::End();

        bool hasNumberOfSidesChanged = nSide_ != oldNSide_;
        if (hasNumberOfSidesChanged)
        {
            oldNSide_ = nSide_;
            generateNgon(&vertices_, &elements_, nSide_);

            // TODO: Le nombre de côtés a changé, la méthode App::generateNgon
            //       (que vous avez implémentée) a modifié les données sur le CPU.
            //       Ici, il faut envoyer les données à jour au GPU.
            //       Attention, il ne faut pas faire d'allocation/réallocation, on veut
            //       seulement mettre à jour les buffers actuels.
            initShapeData();
        }

        // TODO: Dessin du polygone.
        glUseProgram(basicSP_);
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLE_FAN, elements_.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void drawStreetlights(glm::mat4& projView)
    {
        // TODO: Dessin des lampadaires. Ceux-ci doivent être immobiles.
        //
        //       Ceux-ci ont des positions aléatoires.
        //       Le long de la route, ils sont distancés les uns des autres de [10, 20].
        //       La distance par rapport au bord de la route est de 0.5.
        //       La hauteur est de -0.15 (un peu renfoncé dans le sol).
        //
        //       Ils sont toujours orientés de façon perpendiculaire à la route
        //       pour "l'éclairer".
        //
        //       Les nombres n'ont pas vraiment d'importance, libre à vous de les
        //       modifier, l'important est d'avoir les deux transformations
        //       indépendantes les unes des autres.
    }

    void drawTrees(glm::mat4& projView)
    {
        // TODO: Dessin des arbres. Ceux-ci doivent être immobiles.
        //
        //       Ceux-ci ont des positions aléatoires.
        //       Le long de la route, ils sont distancés les uns des autres de [5, 11].
        //       La distance par rapport au bord de la route est de [1.5, 3.5].
        //       La hauteur est toujours de -0.15 (un peu renfoncé dans le sol).
        //
        //       Ils ont aussi une orientation aléatoire entre [0, 2pi].
        //
        //       Pour finir, ils ont une mise à l'échelle sur tous leurs axes
        //       d'un facteur variant entre [0.6, 1.2].
        //
        //       Les nombres n'ont pas vraiment d'importance, libre à vous de les
        //       modifier, l'important est d'avoir les trois transformations
        //       indépendantes les unes des autres.
    }

    void drawGround(glm::mat4& projView)
    {
        // TODO: Dessin du sol.
        //       
        //       La route a seulement une mise à l'échelle pour être longue de
        //       100 unités et large de 5 unités. Le modèle original est un
        //       carré de 1 unité. La ligne jaune devrait traverser le long de
        //       la route.
        //
        //       Le gazon a aussi une mise à l'échelle pour être long de 100
        //       unités et large de 50. Celui-ci doit aussi être légèrement en
        //       dessous de la route de 0.1.
        //       Boni: Que se passe-t-il s'il n'est pas déplacé? Comment expliquer
        //       ce qui est visible?
    }

    glm::mat4 getViewMatrix()
    {
        // TODO: Calculer la matrice de vue.
        //
        //       Vous n'avez pas le droit d'utiliser de fonction lookAt ou 
        //       d'inversion de matrice. À la place, procéder en inversant
        //       les opérations. N'oubliez pas que cette matrice est appliquée
        //       aux éléments de la scène. Au lieu de déplacer la caméra 10
        //       unités vers la gauche, on déplace le monde 10 unités vers la
        //       droite.
        //
        //       La caméra est placée à la position cameraPosition et orientée
        //       par les angles cameraOrientation (en radian).

        return glm::mat4(1.0);
    }

    glm::mat4 getPerspectiveProjectionMatrix()
    {
        // TODO: Calculer la matrice de projection.
        //
        //       Celle-ci aura un fov de 70 degrés, un near à 0.1 et un far à 100.
        //       Vous pouvez calculer le aspect ratio en utilisant la dimension de
        //       la fenêtre. Attention à la division entière.

        sf::Vector2u windowSize = window_.getSize();

        return glm::mat4(1.0);
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

        // TODO: Dessin de la totalité de la scène graphique.
        //       On devrait voir la route, le gazon, les arbres, les lampadaires
        //       et la voiture. La voiture est contrôlable avec l'interface graphique.
    }

private:
    // Shaders
    GLuint basicSP_;
    GLuint transformSP_;
    GLuint colorModUniformLocation_;
    GLuint mvpUniformLocation_;

    // Partie 1
    GLuint vbo_, ebo_, vao_;

    static constexpr unsigned int MIN_N_SIDES = 5;
    static constexpr unsigned int MAX_N_SIDES = 12;

    // TODO: Modifiez les types de vertices_ et elements_ pour votre besoin.
    std::vector<Vertex> vertices_;
    std::vector<GLuint> elements_;

    int nSide_, oldNSide_;

    // Partie 2
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

    // Imgui var
    const char* const SCENE_NAMES[2] = {
        "Introduction",
        "3D Model & transformation",
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
    app.run(argc, argv, "Tp1", settings);
}
