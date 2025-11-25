#include "shaders.hpp"

#include <glm/gtc/type_ptr.hpp>


void EdgeEffect::load()
{
    const char* VERTEX_SRC_PATH = "./shaders/edge.vs.glsl";
    const char* FRAGMENT_SRC_PATH = "./shaders/edge.fs.glsl";
    
    name_ = "EdgeEffect";
    loadShaderSource(GL_VERTEX_SHADER, VERTEX_SRC_PATH);
    loadShaderSource(GL_FRAGMENT_SHADER, FRAGMENT_SRC_PATH);
    link();
}

void EdgeEffect::getAllUniformLocations()
{
    mvpULoc = glGetUniformLocation(id_, "mvp");
}


void Sky::load()
{
    const char* VERTEX_SRC_PATH = "./shaders/sky.vs.glsl";
    const char* FRAGMENT_SRC_PATH = "./shaders/sky.fs.glsl";
    
    name_ = "Sky";
    loadShaderSource(GL_VERTEX_SHADER, VERTEX_SRC_PATH);
    loadShaderSource(GL_FRAGMENT_SHADER, FRAGMENT_SRC_PATH);
    link();
}

void Sky::getAllUniformLocations()
{
    mvpULoc = glGetUniformLocation(id_, "mvp");
}


void CelShading::load()
{
    const char* VERTEX_SRC_PATH = "./shaders/phong.vs.glsl";
    const char* FRAGMENT_SRC_PATH = "./shaders/phong.fs.glsl";
    
    name_ = "CelShading";
    loadShaderSource(GL_VERTEX_SHADER, VERTEX_SRC_PATH);
    loadShaderSource(GL_FRAGMENT_SHADER, FRAGMENT_SRC_PATH);
    link();
}

void CelShading::getAllUniformLocations()
{
    mvpULoc = glGetUniformLocation(id_, "mvp");
    viewULoc = glGetUniformLocation(id_, "view");
    modelViewULoc = glGetUniformLocation(id_, "modelView");
    normalULoc = glGetUniformLocation(id_, "normalMatrix");
    
    nSpotLightsULoc = glGetUniformLocation(id_, "nSpotLights");
    
    globalAmbientULoc = glGetUniformLocation(id_, "globalAmbient");
}

void CelShading::assignAllUniformBlockIndexes()
{
    setUniformBlockBinding("MaterialBlock", 0);
    setUniformBlockBinding("LightingBlock", 1);
}


void CelShading::setMatrices(glm::mat4& mvp, glm::mat4& view, glm::mat4& model)
{
    glm::mat4 modelView = view * model;
    
    glUniformMatrix4fv(viewULoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(mvpULoc, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(modelViewULoc, 1, GL_FALSE, &modelView[0][0]);
    glUniformMatrix3fv(normalULoc, 1, GL_TRUE, glm::value_ptr(glm::inverse(glm::mat3(modelView))));
}

void CelShading::printAllProperties()
{
    printf("CelShading Shader Properties:\n");
    printf("MVP Uniform Location: %u\n", mvpULoc);
    printf("View Uniform Location: %u\n", viewULoc);
    printf("ModelView Uniform Location: %u\n", modelViewULoc);
    printf("Normal Matrix Uniform Location: %u\n", normalULoc);
    printf("Number of Spot Lights Uniform Location: %u\n", nSpotLightsULoc);
    printf("Global Ambient Uniform Location: %u\n", globalAmbientULoc);
}

void Grass::load()
{
    const char* VERTEX_SRC_PATH = "./shaders/grass.vs.glsl";
    const char* FRAGMENT_SRC_PATH = "./shaders/grass.fs.glsl";
	const char* GEOMETRY_SRC_PATH = "./shaders/grass.gs.glsl";
	const char* TESS_CONTROL_SRC_PATH = "./shaders/grass.tcs.glsl";
	const char* TESS_EVALUATION_SRC_PATH = "./shaders/grass.tes.glsl";
    
    name_ = "Grass";
    loadShaderSource(GL_VERTEX_SHADER, VERTEX_SRC_PATH);
    loadShaderSource(GL_FRAGMENT_SHADER, FRAGMENT_SRC_PATH); 
	loadShaderSource(GL_GEOMETRY_SHADER, GEOMETRY_SRC_PATH);
	loadShaderSource(GL_TESS_CONTROL_SHADER, TESS_CONTROL_SRC_PATH);
	loadShaderSource(GL_TESS_EVALUATION_SHADER, TESS_EVALUATION_SRC_PATH);
    link();
}

void Grass::getAllUniformLocations()
{
    mvpULoc = glGetUniformLocation(id_, "mvp");
    modelViewULoc = glGetUniformLocation(id_, "modelView");
}

void Grass::setMatrices(glm::mat4& modelView, glm::mat4& mvp)
{
    glUniformMatrix4fv(modelViewULoc, 1, GL_FALSE, glm::value_ptr(modelView));
	glUniformMatrix4fv(mvpULoc, 1, GL_FALSE, glm::value_ptr(mvp));
}

void ParticlesDraw::load()
{
    const char* VERTEX_SRC_PATH = "./shaders/particlesDraw.vs.glsl";
    const char* GEOMETRY_SRC_PATH = "./shaders/particlesDraw.gs.glsl";
    const char* FRAGMENT_SRC_PATH = "./shaders/particlesDraw.fs.glsl";

    name_ = "ParticlesDraw";
    loadShaderSource(GL_VERTEX_SHADER, VERTEX_SRC_PATH);
    loadShaderSource(GL_GEOMETRY_SHADER, GEOMETRY_SRC_PATH);
    loadShaderSource(GL_FRAGMENT_SHADER, FRAGMENT_SRC_PATH);
    link();
}

void ParticlesDraw::getAllUniformLocations()
{
    modelViewULoc = glGetUniformLocation(id_, "modelView");
    projectionULoc = glGetUniformLocation(id_, "projection");
	viewULoc = glGetUniformLocation(id_, "view");
}

void ParticlesDraw::setMatrices(glm::mat4& modelView, glm::mat4& projection, glm::mat4& view)
{
    glUniformMatrix4fv(modelViewULoc, 1, GL_FALSE, glm::value_ptr(modelView));
    glUniformMatrix4fv(projectionULoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewULoc, 1, GL_FALSE, glm::value_ptr(view));
}

void ParticlesUpdate::load()
{
    const char* COMPUTE_SRC_PATH = "./shaders/particlesUpdate.cs.glsl";

    name_ = "ParticlesUpdate";
    loadShaderSource(GL_COMPUTE_SHADER, COMPUTE_SRC_PATH);
    link();
}

void ParticlesUpdate::getAllUniformLocations()
{
    deltaTimeLoc = glGetUniformLocation(id_, "deltaTime");
    timeLoc = glGetUniformLocation(id_, "time");
    emitterPosLoc = glGetUniformLocation(id_, "emitterPosition");
    emitterDirLoc = glGetUniformLocation(id_, "emitterDirection");
}

void ParticlesUpdate::setUniforms(float deltaTime, float totalTime, glm::vec3& emitterPos, glm::vec3& emitterDir)
{
    glUniform1f(deltaTimeLoc, deltaTime);
    glUniform1f(timeLoc, totalTime);
    glUniform3fv(emitterPosLoc, 1, glm::value_ptr(emitterPos));
    glUniform3fv(emitterDirLoc, 1, glm::value_ptr(emitterDir));
}