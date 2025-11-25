#version 430 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct Particle
{
    vec3 position;
    float zOrientation;
    vec3 velocity;
    vec4 color;
    vec2 size;
    float timeToLive;
    float maxTimeToLive;
};

layout(std140, binding = 0) readonly restrict buffer ParticlesInputBlock
{
    Particle particles[];
} dataIn;

layout(std140, binding = 1) writeonly restrict buffer ParticlesOutputBlock
{
    Particle particles[];
} dataOut;

uniform float time;
uniform float deltaTime;
uniform vec3 emitterPosition;
uniform vec3 emitterDirection;

float rand01()
{
    return fract(sin(dot(vec2(time*100, gl_GlobalInvocationID.x), vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    Particle pIn = dataIn.particles[gl_GlobalInvocationID.x];
    Particle pOut;

    if (pIn.timeToLive <= 0.0)
    {
        float PI = 3.14159265;
        pOut.position = emitterPosition;
        pOut.zOrientation = rand01() * 2 * PI;
        pOut.velocity = emitterDirection * 0.3 + vec3(0.0, 0.2, 0.0);
        pOut.color = vec4(0.5, 0.5, 0.5, 1.0);
        pOut.size = vec2(0.2, 0.2);

        float life = 1.5 + rand01() * 0.5;
        pOut.timeToLive = life;
        pOut.maxTimeToLive = life;
    }
    else
    {
        pOut.timeToLive = pIn.timeToLive - deltaTime;
        pOut.position = pIn.position + pIn.velocity * deltaTime;
        pOut.zOrientation = pIn.zOrientation + 0.5 * deltaTime;
        pOut.velocity = pIn.velocity;
        pOut.maxTimeToLive = pIn.maxTimeToLive;

        float t = clamp(1.0 - pOut.timeToLive / pOut.maxTimeToLive, 0.0, 1.0);

        vec3 baseColor = vec3(0.5);
        vec3 targetColor = vec3(1.0);
        vec3 finalColor = mix(baseColor, targetColor, t);

        float alpha = 0.2;
        if (t < 0.2)
            alpha *= (t / 0.2);
        else if (t > 0.8)
            alpha *= ((1.0 - t) / 0.2);

        pOut.color = vec4(finalColor, alpha);
        pOut.size = mix(vec2(0.2), vec2(0.5), t);
    }

    dataOut.particles[gl_GlobalInvocationID.x] = pOut;
}

