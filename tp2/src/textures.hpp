#ifndef TEXTURES_H
#define TEXTURES_H

#include <glbinding/gl/gl.h>

using namespace gl;

class Texture2D
{
public:
	Texture2D();
	~Texture2D();
	
	void load(const char* path);
	
	void setFiltering(GLenum filteringMode1);
	void setWrap(GLenum wrapMode1);

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

	void use(GLenum mode);

private:
	GLuint m_id;
};



#endif // TEXTURES
