#pragma once

#include "vertex.hpp"

class Model
{
public:
    void load(const char* path);
    
    ~Model();
    
    void draw();

private:
    GLuint vao_, vbo_, ebo_;
    GLsizei count_;

    std::vector<VertexData> vertices;
    std::vector<GLuint> elements;
};

