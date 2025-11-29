#pragma once

float groundVertices[] =
{
    // Position           // Texture coordinates
    -0.5f, 0.0f, -0.5f,   0.0f, 0.0f,
     0.5f, 0.0f, -0.5f,   10.0f, 0.0f,
     0.5f, 0.0f,  0.5f,   10.0f, 5.0f,
    -0.5f, 0.0f,  0.5f,   0.0f, 5.0f
};

unsigned int groundElements[] =
{
    0, 2, 1,
    0, 3, 2
};