# Infographie - Applications créatives

---

## Aperçu général

Ce projet présente une petite scène 3D réalisée avec OpenGL, incluant:

-   Un cristal flottant animé
-   Un système de particules GPU via compute shader avec des animations de lutin
-   Un éclairage de Phong
-   Des paramètres en temps réel grâce à ImGui

L'objectif principal était de développer une scène personalisée avec comme thème "cristal scintillant" en appliquant différentes techniques de rendu.

---

## Concepts utilisés et leur impact sur le rendu final

### 1. Cel Shading

Le cel shading a été repris du TP3. Il permet de simuler la lumière sur les surfaces des modèles en utilisant le modèle de réflexion spéculaire de Phong.
Il permet aussi de mettre en place une lumière directionnelle pour donner un effet intéressant sur les modèles.

L'utilisateur peut modifier la rotation et la couleur de la lumière directionnelle en temps réel pour obtenir des effets intéressants.

### 2. Animation du cristal

Le cristal flotte légèrement grâce à une animation basée sur le temps.

Les transformations appliquées:

-   Oscillation verticale (sinus)
-   Très légère inclinaison aléatoire

Ces détails donnent vie à l'objet principal de la scène.

### 3. Système de particules GPU

Les particules sont générées entièrement sur le GPU dans un compute shader.

Caractéristiques :

-   Position de spawn aléatoire
-   Hauteur influençant la vitesse
-   Cycle de vie ajustable
-   Transition de couleur (gris → blanc → gris)
-   Variation de taille (petit → grand → petit)
-   Animation de spritesheet avec décalage par particule

Cet effet crée une aura magique autour du cristal et ajoute du dynamisme à la scène.

### 4. Sol texturé

Un simple quad est utilisé pour le sol. Nous avons utilisé un texture de sol de la lune.

### 5. Animation de caméra sur une spline

Une animation de caméra sur une spline est utilisé pour apporter l'utilisateur vers une surprise.

---

## Librairies utilisées

### glm

Pour la gestion des matrices, vecteurs et transformations.

### ImGui

Pour offrir une interface utilisateur simple permettant d’ajuster des paramètres de la scène.

### stb_image

Pour charger les textures (sol, cristal, sprites de particules, etc).

### Autres librairies repris des anciens TP

-   uniform_buffer: pour simplifier la gestion des matériaux/lumières
-   textures: pour simplifer la gestion des textures
-   shader_program: pour simplifier la gestion des nuanceurs
-   shader_storage_buffer: pour simplifier l'utilisation des SSBO

---

## Contrôles

Via l'interface ImGui:

-   Light Rotation (deg): Fait tourner la lumière directionnelle autour de la scène
-   Light Color: Change la couleur diffuse de la lumière directionnelle
-   À AJOUTER

---

## Assets utilisés

-   Modèle de cristal: Venait avec le projet de base
-   Texture du sol: https://www.reddit.com/r/Unity3D/comments/6sdq7s/i_just_made_a_seamless_lunar_surface_texture_you/
-   Skybox: https://svs.gsfc.nasa.gov/4851
-   Spritesheet de particules: https://kenney.nl/assets/particle-pack
-   Autres modèles: NOUS-MÊME

---

## Auteurs

-   2221795, Michael Le
-   , Arani Balasingam
