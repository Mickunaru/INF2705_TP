#include "textures.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

Texture2D::Texture2D()
: m_id(0)
{

}

void Texture2D::load(const char* path)
{
    int width, height, nChannels;
    stbi_set_flip_vertically_on_load(true);
    // flips vertically 2 match OpenGLs coord. sys.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // makes sure Pixels r aligned 4 GPU
    unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);
    // loads data file of file and returns pointer of RAW data in CPU mem (RAM)
    if (data == NULL)
        std::cout << "Error loading texture \"" << path << "\": " << stbi_failure_reason() << std::endl;

    // TODO: Chargement de la texture en mémoire graphique.
    //       Attention au format des pixels de l'image!
    //       Toutes les variables devraient être utilisées (width, height, nChannels, data).

    glGenTextures(1, &m_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);

    gl::GLenum formatArray[] = { GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER,  GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER,  GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT,  GL_DEPTH_STENCIL };    

    glTexImage2D(
        GL_TEXTURE_2D,
        0,// Mipmap level pour reduire perf. cost
        formatArray[nChannels], //On peut changer index si innerFormat est different.
        width,
        height,
        0, // Used to express pixels around border.
        formatArray[nChannels],
        GL_UNSIGNED_BYTE,
        data // Tableau contenant la texture
    );

    stbi_image_free(data);
}

Texture2D::~Texture2D()
{
    // TODO: Libérer les ressources allouées.
    glDeleteTextures(1, &m_id);

}

void Texture2D::setFiltering(GLenum filteringMode)
{
    // TODO: Configurer le filtre min et le mag avec le mode en paramètre.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode);
}

void Texture2D::setWrap(GLenum wrapMode)
{
    // TODO: Configurer le wrap S et T avec le mode en paramètre.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
}

void Texture2D::enableMipmap()
{
    // TODO: Génère le mipmap et configure les paramètres pour l'utiliser.
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::use()
{
    // TODO: Met la texture active pour être utilisée dans les prochaines commandes de dessins.
    setPixelData()

    //getUseText
}

//
// Cubemap
//

TextureCubeMap::TextureCubeMap()
: m_id(0)
{

}

void TextureCubeMap::load(const char** pathes)
{
    const size_t N_TEXTURES = 6;
    unsigned char* datas[N_TEXTURES];
    int widths[N_TEXTURES];
    int heights[N_TEXTURES];
    int nChannels[N_TEXTURES];
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < 6; i++)
    {
        datas[i] = stbi_load(pathes[i], &widths[i], &heights[i], &nChannels[i], 0);
        if (datas[i] == NULL)
            std::cout << "Error loading texture \"" << pathes[i] << "\": " << stbi_failure_reason() << std::endl;
    }

    // TODO: Chargement de la texture en mémoire graphique.
    //       Faites la configuration des min et mag filtering et du wrap S, T, R directement, ils
    //       ne seront pas modifiés ailleurs.
    
    for (unsigned int i = 0; i < 6; i++)
    {
        // TODO
    }

    for (unsigned int i = 0; i < 6; i++)
    {
        stbi_image_free(datas[i]);
    }
}

TextureCubeMap::~TextureCubeMap()
{
    // TODO: Libérer les ressources allouées.
}

void TextureCubeMap::use()
{
    // TODO: Met la texture active pour être utilisée dans les prochaines commandes de dessins.
}

