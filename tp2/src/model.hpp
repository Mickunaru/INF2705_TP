#pragma once

#include <glbinding/gl/gl.h>

using namespace gl;

class Model
{
public:
    void load(float* vertices, size_t verticesSize, unsigned int* elements, size_t elementsSize);

    void load(const char* path);
    
    ~Model();
    
    void draw();

private:
    GLuint vao_, vbo_, ebo_;
    GLsizei count_;
};

