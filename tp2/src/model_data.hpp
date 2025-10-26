#pragma once

// TODO: Compléter les coordonnées de texture.
//       On veut que les texture se répètent 10 fois sur le long du sol.
//       Pour le sol, on aura aussi une répétition de 5 sur le large du sol.
// 
// GL_REPEAT in glTexParameteri
float ground[] =
{
    // Position           // Texture coordinates (u, v)
    -0.5f, 0.0f, -0.5f,   0.0f, 0.0f, // bottom-left
     0.5f, 0.0f, -0.5f,   5.0f, 0.0f, // bottom-right (width repeat x5)
     0.5f, 0.0f,  0.5f,   5.0f,10.0f, // top-right (height repeat x10)
     -0.5f, 0.0f,  0.5f,   0.0f, 10.0f //top-left
};

unsigned int planeElements[] =
{
    0, 2, 1,
    0, 3, 2
};

float street[] =
{
    // Position           // Texture coordinates (u, v)
     -0.5f, 0.0f, -0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, 0.0f, -0.5f,   1.0f, 0.0f, // bottom-right (width repeat x1)
     0.5f, 0.0f,  0.5f,   1.0f, 1.0f, // top-right (length repeat x10)
     -0.5f, 0.0f,  0.5f,   0.0f, 1.0f // top-left
};

