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
    float frame;
};

layout(std430, binding = 0) readonly restrict buffer ParticlesInputBlock
{
    Particle particles[];
} dataIn;

layout(std430, binding = 1) writeonly restrict buffer ParticlesOutputBlock
{
    Particle particles[];
} dataOut;

uniform float time;
uniform float deltaTime;
uniform vec3 emitterPosition;
uniform vec3 emitterDirection;

float rand01(uint id)
{
    return fract(sin(dot(vec2(time*100, id), vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    Particle pIn = dataIn.particles[id];
    Particle pOut;

    if (pIn.timeToLive <= 0.0)
    {
        float PI = 3.14159265;

        float angle = rand01(id) * 2.0 * PI;
        float radius = 0.1 + rand01(id + 1u) * 0.05;
        float height = rand01(id + 2u) * 0.4;
        float horizontalJitter = (rand01(id + 3u) - 0.5) * 0.05; // small x/z jitter
        float verticalOffset = (rand01(id + 4u) - 0.5) * 0.1;    // small up/down jitter

        vec3 offset = vec3(
            cos(angle) * radius + horizontalJitter,
            height + verticalOffset,
            sin(angle) * radius + horizontalJitter
        );
        pOut.position = emitterPosition + offset;

        pOut.zOrientation = rand01(id + 5u) * 2 * PI;

        float speedFactor = 1.0 - height;
        pOut.velocity = emitterDirection * (rand01(id + 6u) * 0.1 + 0.1) * speedFactor;

        pOut.color = vec4(0.5, 0.5, 0.5, 0.0);
        pOut.size = vec2(0.05, 0.05);

        float life = 1.5 + rand01(id + 7u) * 2.5;
        pOut.timeToLive = life;
        pOut.maxTimeToLive = life;
        pOut.frame = 0.0;
    }
    else
    {
        pOut.timeToLive = pIn.timeToLive - deltaTime;
        pOut.position = pIn.position + pIn.velocity * deltaTime;
        pOut.zOrientation = pIn.zOrientation + 0.5 * deltaTime;
        pOut.velocity = pIn.velocity;
        pOut.maxTimeToLive = pIn.maxTimeToLive;

        float t = clamp(1.0 - pOut.timeToLive / pOut.maxTimeToLive, 0.0, 1.0);
        float waveFactor = t < 0.5 ? t * 2.0 : (1.0 - t) * 2.0;

        vec3 baseColor = vec3(0.5);
        vec3 targetColor = vec3(1.0);
        vec3 finalColor = mix(baseColor, targetColor, waveFactor);
        float alpha = 1.0 - abs(2.0 * t - 1.0);
        pOut.color = vec4(finalColor, alpha);

        vec2 minSize = vec2(0.05);
        vec2 maxSize = vec2(0.2);
        pOut.size = mix(minSize, maxSize, waveFactor);
        
        float numFrames = 9.0;
        pOut.frame = min(floor(t * float(numFrames)), numFrames - 1);
    }

    dataOut.particles[gl_GlobalInvocationID.x] = pOut;
}

