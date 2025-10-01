#ifndef TEXTURES_H
#define TEXTURES_H

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

class Texture2D
{
public:
	Texture2D();
	~Texture2D();
	
	void load(const char* path);
	
	void setFiltering(GLenum filteringMode);
	void setWrap(GLenum wrapMode);

	void enableMipmap();

	void use();

private:
	GLuint m_id;
};


class TextureCubeMap
{
public:
	TextureCubeMap();
	~TextureCubeMap();
	
	void load(const char** path);

	void use();

private:
	GLuint m_id;
};



#endif // TEXTURES
