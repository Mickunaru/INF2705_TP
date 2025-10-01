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

// TODO:  À ajouter dans votre classe actuelle.
class Model
{
public:
    void load(float* vertices, size_t verticesSize, unsigned int* elements, size_t elementsSize);
};

