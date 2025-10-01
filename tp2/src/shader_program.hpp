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

#include <unordered_map>

class ShaderProgram
{
public:
    ShaderProgram();
    virtual ~ShaderProgram();
    
    void create();
    void reload();
    
    void use();

protected:
    void loadShaderSource(GLenum type, const char* path);
    void link();
    
    void setUniformBlockBinding(const char* name, GLuint bindingIndex);

    virtual void load() = 0;
    virtual void getAllUniformLocations() = 0;
    virtual void assignAllUniformBlockIndexes() {};

protected:
    GLuint id_;
    const char* name_;
    std::unordered_map<std::string, GLuint> shaderSourcesCompiled_;
};

