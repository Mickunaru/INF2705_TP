
// TODO: À ajouter, et à compléter dans votre projet.

#include <map>

#include "shaders.hpp"

// À ajouter
struct Material
{
    glm::vec4 emission; // vec3, but padded
    glm::vec4 ambient;  // vec3, but padded
    glm::vec4 diffuse;  // vec3, but padded
    glm::vec3 specular;
    GLfloat shininess;
};
<<<<<<< Updated upstream
=======
 
Car::Car()
: position(0.f, 0.f, 0.f), orientation(0.f, 0.f), speed(0.f)
, wheelsRollAngle(0.f), steeringAngle(0.f)
, isHeadlightOn(false), isBraking(false)
, isLeftBlinkerActivated(false), isRightBlinkerActivated(false)
, isBlinkerOn(false), blinkerTimer(0.f)
{}
>>>>>>> Stashed changes

void Car::loadModels()
{
    // ...
    // À ajouter, l'ordre est à considérer
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
<<<<<<< Updated upstream
    // ...
    // À ajouter à la fin
=======
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
    
    glm::vec4 temp = glm::rotate(glm::mat4(1.0f), orientation.y, glm::vec3(0.f, 1.0f, 0.f)) * glm::vec4(-speed, 0.f, 0.f, 1.f);
    glm::vec3 positionMod = glm::vec3(temp);
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

>>>>>>> Stashed changes
    carModel = glm::mat4(1.0f);
    carModel = glm::translate(carModel, position);
    carModel = glm::rotate(carModel, orientation.y, glm::vec3(0.f, 1.0f, 0.f));
}

// TODO: Revoir vos méthodes de dessin. Elles seront à modifier pour la partie 2 et 3.
//       Partie 2: Ajouter le calcul de stencil pour le chassi et les roues pour avoir
//                 le contour de la voiture.

void Car::draw(glm::mat4& projView, glm::mat4& view)
{
<<<<<<< Updated upstream
    // Code de solution partielle, à ignorer si votre voiture est décente.
    glm::mat4 mvp = projView * carModel;
    glm::mat4 mvpFrame = glm::translate(mvp, glm::vec3(0.0f, 0.25f, 0.0f));
    // glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]); // Avec une bonne location
=======
    drawFrame(projView);

	drawWheels(projView);

    celShadingShader->use();
	drawHeadlights(projView);
}
    
void Car::drawFrame(glm::mat4& projView)
{
    glm::mat4 model = translate(carModel, vec3(0.f, 0.25f, 0.f));
    glm::mat4 mvp = projView * model;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    celShadingShader->setMatrices(mvp, carModel, model);
>>>>>>> Stashed changes
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
    
    // TODO: À ajouter et compléter.
    //       Dessiner les vitres de la voiture. Celles-ci ont une texture transparente,
    //       il est donc nécessaire d'activer le mélange des couleurs (GL_BLEND).
    //       De plus, vous devez dessiner les fenêtres du plus loin vers le plus proche
    //       pour éviter les problèmes de mélange.
    //       Utiliser un map avec la distance en clef pour trier les fenêtres (les maps trient
    //       à l'insertion).
    //       Les fenêtres doivent être visibles des deux sens.
    //       Il est important de restaurer l'état du contexte qui a été modifié à la fin de la méthode.
    
    
    // Les fenêtres sont par rapport au chassi, à considérer dans votre matrice
    // model = glm::translate(model, glm::vec3(0.0f, 0.25f, 0.0f));
    
    std::map<float, unsigned int> sorted;
    for (unsigned int i = 0; i < 6; i++)
    {
        // TODO: Calcul de la distance par rapport à l'observateur (utiliser la matrice de vue!)
        //       et faite une insertion dans le map
<<<<<<< Updated upstream
=======
        glm::vec4 worldPos = glm::vec4(WINDOW_POSITION[i], 1.0f);
        glm::vec4 viewSpacePos = view * worldPos;
        float distance = -viewSpacePos.z;

        std::multimap<float, unsigned> sorted;
        sorted.insert({ distance, i });
>>>>>>> Stashed changes
    }
    
    // TODO: Itération à l'inverse (de la plus grande distance jusqu'à la plus petit)
    for (std::map<float, unsigned int>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        // TODO: Dessin des fenêtres
    }
}

void Car::drawBlinker()
{
        
    const glm::vec3 ON_COLOR (1.0f, 0.7f , 0.3f );
    const glm::vec3 OFF_COLOR(0.5f, 0.35f, 0.15f);
<<<<<<< Updated upstream
    
    // TODO: À ajouter dans votre méthode. À compléter pour la partie 3.
    Material blinkerMat = 
=======

    bool isBlinkerActivated = (isLeftHeadlight  && isLeftBlinkerActivated) ||
                              (!isLeftHeadlight && isRightBlinkerActivated);

    // TODO: � ajouter dans votre m�thode. � compl�ter pour la partie 3.
    Material blinkerMat =
>>>>>>> Stashed changes
    {
        {0.0f, 0.0f, 0.0f, 0.0f},
        {OFF_COLOR, 0.0f},
        {OFF_COLOR, 0.0f},
        {OFF_COLOR},
        10.0f
    };
    
    //if (isBlinkerOn && isBlinkerActivated)
    //    TODO: Modifier le matériel pour qu'il ait l'air d'émettre de la lumière.
    //    ... = glm::vec4(ON_COLOR, 0.0f);
   
    // TODO: Envoyer le matériel au shader. Partie 3.
    
}

void Car::drawLight()
{
    
    const glm::vec3 FRONT_ON_COLOR (1.0f, 1.0f, 1.0f);
    const glm::vec3 FRONT_OFF_COLOR(0.5f, 0.5f, 0.5f);
    const glm::vec3 REAR_ON_COLOR  (1.0f, 0.1f, 0.1f);
    const glm::vec3 REAR_OFF_COLOR (0.5f, 0.1f, 0.1f);
    
    // TODO: À ajouter dans votre méthode. À compléter pour la partie 3.
    
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
    
    if (isFront)
    {
        // if (isHeadlightOn)
        //    TODO: Modifier le matériel pour qu'il ait l'air d'émettre de la lumière.
        //    ... = glm::vec4(FRONT_ON_COLOR, 0);
   
        // TODO: Envoyer le matériel au shader. Partie 3.
    }
    else
    {
        // if (isBraking)
        //    TODO: Modifier le matériel pour qu'il ait l'air d'émettre de la lumière.
        //    ... = glm::vec4(REAR_ON_COLOR, 0);
            
        // TODO: Envoyer le matériel au shader. Partie 3.
    }
}

