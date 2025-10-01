#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

#include <cstddef>
#include <cstdint>

#include <array>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glbinding/gl/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>

using namespace gl;
using namespace glm;

class UniformBuffer
{
public:
    UniformBuffer();
    ~UniformBuffer();
    
    void allocate(const void* data, GLsizeiptr byteSize);
    
    void setBindingIndex(GLuint index);

    void updateData(const void* data, GLintptr offset, GLsizeiptr byteSize);
    
private:
    GLuint id_;
};

#endif // UNIFORM_BUFFER_H
