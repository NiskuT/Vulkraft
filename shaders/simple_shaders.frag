#version 450

layout(location = 0) in vec3 fragColor; // color of the fragment 
layout(location = 1) in vec3 fragPosWorld; // position of the fragment 
layout(location = 2) in vec3 fragNormalWorld; // normal vector to the fragment 
layout(location = 3) in vec2 fragUv; // fragment's texture coordinates 

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push // to give constants to the shader
{
    mat4 modelMatrix; // projection * view * model ; to transform the model's coordinates 
    mat4 normalMatrix; // to transform the normals to the vertices 
} push;

struct PointLight
{
    vec4 position; // ignore w
    vec4 color; // w is the intensity
};

layout (set = 0, binding = 0) uniform GlobalUbo //to give the global data to the shader
{
    mat4 projection; // to project the coordinates of the world on the screen 
    mat4 view; // to project the coordinates of the world on the camera
    mat4 invView; // inverse : camera's matrix 
    vec4 ambientLightColor; 
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(binding = 1) uniform sampler2D texSampler; // access to the textures 


void main()
{
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.invView[3].xyz; // get the position of the camera from the camera matrix 
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    for (int i = 0; i < ubo.numLights; i++) // for each light in the scene 
    {
        PointLight light = ubo.pointLights[i];  // get the point lights 
        vec3 directionToLight = light.position.xyz - fragPosWorld; // vector between the fragment and the point light source 
        float attenuation = 1.0f / dot(directionToLight, directionToLight); // attenuation = 1 / distance²
        directionToLight = normalize(directionToLight);


        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0.0); 
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        // specular component lighting
        vec3 halfAngle = normalize(directionToLight + viewDirection); // used to determine the intensity 
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0.0f, 1.0f);
        blinnTerm = pow(blinnTerm, 20.0f); // higher values -> sharper specular highlight
        specularLight += intensity * blinnTerm;
    }
    // we chose to keep the four dimensions of the texture(texSampler, fragUv) vector (4) because without the last 
    // component, alpha, determines the transparency of the texture, and without it here, 
    // the color blue is rendered green 
    vec4 finalColor = vec4(fragColor, 1.0f) * texture(texSampler, fragUv);
    outColor = vec4(diffuseLight, 1.0f)* finalColor + vec4(specularLight, 1.0f) * finalColor; 
}
