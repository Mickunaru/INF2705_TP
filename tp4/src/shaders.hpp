#include "shader_program.hpp"

#include <glm/glm.hpp>


class Sky : public ShaderProgram
{
public:
    GLuint mvpULoc;

protected:
    virtual void load() override;
    virtual void getAllUniformLocations() override;
};


class CelShading : public ShaderProgram
{
public:
    GLuint mvpULoc;
    GLuint viewULoc;
    GLuint modelViewULoc;
    GLuint normalULoc;
    
    GLuint nSpotLightsULoc;
    GLuint globalAmbientULoc;

public:
    void setMatrices(glm::mat4& mvp, glm::mat4& view, glm::mat4& model);
	void printAllProperties();

protected:
    virtual void load() override;
    virtual void getAllUniformLocations() override;
    virtual void assignAllUniformBlockIndexes() override;
};


class ParticlesDraw : public ShaderProgram
{
public:
    GLuint modelViewULoc;
    GLuint projectionULoc;
	GLuint viewULoc;

    void setMatrices(glm::mat4& modelView, glm::mat4& projection, glm::mat4& view);

protected:
    virtual void load() override;
    virtual void getAllUniformLocations() override;
};


class ParticlesUpdate : public ShaderProgram
{
public:
    GLuint deltaTimeLoc;
    GLuint timeLoc;
    GLuint emitterPosLoc;
    GLuint emitterDirLoc;

    void setUniforms(float deltaTime, float totalTime, glm::vec3& emitterPos, glm::vec3& emitterDir);

protected:
    virtual void load() override;
    virtual void getAllUniformLocations() override;
};