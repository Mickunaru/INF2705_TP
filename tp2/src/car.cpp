
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
    // ...
    // À ajouter à la fin
    carModel = glm::mat4(1.0f);
    carModel = glm::translate(carModel, position);
    carModel = glm::rotate(carModel, orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
}

// TODO: Revoir vos méthodes de dessin. Elles seront à modifier pour la partie 2 et 3.
//       Partie 2: Ajouter le calcul de stencil pour le chassi et les roues pour avoir
//                 le contour de la voiture.

void Car::draw(glm::mat4& projView, glm::mat4& view)
{
    // Code de solution partielle, à ignorer si votre voiture est décente.
    glm::mat4 mvp = projView * carModel;
    glm::mat4 mvpFrame = glm::translate(mvp, glm::vec3(0.0f, 0.25f, 0.0f));
    // glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]); // Avec une bonne location
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
    
    // TODO: À ajouter dans votre méthode. À compléter pour la partie 3.
    Material blinkerMat = 
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

