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
    glBindTexture(GL_TEXTURE_2D, m_id);

    GLenum format = GL_RGB;
    switch (nChannels)
    {
    case 1: format = GL_RED; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,// Mipmap level pour reduire perf. cost
        format, //On peut changer index si innerFormat est different.
        width,
        height,
        0, // Used to express pixels around border.
        format,
        GL_UNSIGNED_BYTE,
        data // Tableau contenant la texture
    );

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D()
{
    // TODO: Libérer les ressources allouées.
    glDeleteTextures(1, &m_id);
    m_id = 0;

}

void Texture2D::setFiltering(GLenum filteringMode1, GLenum filteringMode2)
{
    // TODO: Configurer le filtre min et le mag avec le mode en paramètre.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode2);
}

void Texture2D::setWrap(GLenum wrapMode1, GLenum wrapMode2)
{
    // TODO: Configurer le wrap S et T avec le mode en paramètre.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode2);
}

void Texture2D::enableMipmap()
{
    // TODO: Génère le mipmap et configure les paramètres pour l'utiliser.
    glBindTexture(GL_TEXTURE_2D, m_id);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
}

void Texture2D::use(GLenum mode)
{
    // TODO: Met la texture active pour être utilisée dans les prochaines commandes de dessins.
    //setPixelData();
    glActiveTexture(mode);
    glBindTexture(GL_TEXTURE_2D, m_id);
   
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
        GLenum format = GL_RGB;
        switch (nChannels[i])
        {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default:
            std::cout << "Unsupported channel count for cubemap face " << i << std::endl;
            continue;
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // each face target
            0, // mipmap level
            format, // internal format
            widths[i],
            heights[i],
            0,
            format, // pixel data format
            GL_UNSIGNED_BYTE,
            datas[i]
        );
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < N_TEXTURES; i++)
        stbi_image_free(datas[i]);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

TextureCubeMap::~TextureCubeMap()
{
    // TODO: Libérer les ressources allouées.
    glDeleteTextures(1, &m_id);
    m_id = 0;
}

void TextureCubeMap::use(GLenum mode)
{
    // TODO: Met la texture active pour être utilisée dans les prochaines commandes de dessins.
    glActiveTexture(mode);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    
}

