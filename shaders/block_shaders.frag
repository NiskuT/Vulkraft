#version 450

layout(location = 0) in vec3 fragPosWorld;
layout(location = 1) in vec3 fragNormalWorld;
layout(location = 2) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push
{
    mat4 modelMatrix; // projection * view * model
    mat4 normalMatrix;
} push;

struct PointLight
{
    vec4 position; // ignore w
    vec4 color; // w is the intensity
};

layout (set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;


void main()
{
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.invView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    for (int i = 0; i < ubo.numLights; i++)
    {
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        directionToLight = normalize(directionToLight);

        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);

        attenuation *= light.color.w;
        if (i <= 1)
        {
            attenuation = light.color.w;            
        }
        vec3 intensity = light.color.xyz * attenuation;
        diffuseLight += intensity * cosAngIncidence;

        // specular component lighting
        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0.0, 1.0);
        blinnTerm = pow(blinnTerm, 10.0); // higher values -> sharper specular highlight
        specularLight += intensity * blinnTerm;
    }
    diffuseLight = clamp(diffuseLight, 0.0, 1.0);
    
    vec3 finalColor = texture(texSampler, fragUv).rgb;
    outColor = vec4(diffuseLight* finalColor + specularLight * finalColor, 1.0);
}
