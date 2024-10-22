#version 330 core

struct Material{
  float shine;
  sampler2D diffuse;
  sampler2D texture;
  int diffuseId;
  sampler2D specular;
  int specularId;
  vec3 color;
};

struct Light{
  //Area Light
  int type;
  vec3 position;
  vec3 diffuse;
  vec3 specular;
  float intensity;

  //Spot Light
  vec3 direction;
  float cutOff;
  float outerCutOff;
  float constant;
  float linear;
  float quadratic;

  mat4 lightSpaceMatrix;
};

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D depthMap;
uniform vec3 ambient;

uniform vec3 viewPos;
uniform int textureId;

uniform Material material;

vec3 CalcAreaLight(Light light, vec3 normal, vec3 fragPos,  vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal,  vec3 viewDir);

float CalcPointShadow(int lightId, Light light, vec4 fragPosLightSpace);

#define LIGHTS 3
uniform Light lights[LIGHTS];

uniform sampler2D depthMaps[LIGHTS];
uniform int lightId = 0;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    float shadow = 0.0;

    vec3 result = vec3(0.0);
    for(int i = 0; i < LIGHTS; i++) {
      Light lightOn = lights[i];

      if(lightOn.type == 0)
        result += CalcAreaLight(lightOn, norm, FragPos, viewDir);
      else if(lightOn.type == 1){
        result += CalcSpotLight(lightOn, norm, viewDir) * lightOn.intensity;
        vec4 fragPosLightSpace = lightOn.lightSpaceMatrix * vec4(FragPos, 1.0);
        float shadowPoint = CalcPointShadow(i, lightOn, fragPosLightSpace);
        
        shadow += shadowPoint * lightOn.intensity;
      }
    }

    result *= (1.0 - shadow);
    result += ambient;

    if(textureId != 0){
      if(material.diffuseId == textureId){
        FragColor =  texture(material.diffuse, TexCoord) * vec4(result * material.color, 1.0f);
      }else{
        FragColor =  texture(material.texture, TexCoord) * vec4(result * material.color, 1.0f);
      }
    }else{
      FragColor = vec4(result * material.color, 1.0f);
    }
}

vec4 LightIDToDepthMap(int lightId, vec2 coords){
if (lightId == 0)
return texture(depthMaps[0], coords);
else if (lightId == 1)
return texture(depthMaps[1], coords);
}

float CalcPointShadow(int lightId, Light light, vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = LightIDToDepthMap(lightId, projCoords.xy).r; 

    float currentDepth = projCoords.z;

    vec3 lightDir = normalize(light.position - FragPos);
    float bias = max(0.05 * (1.0 - dot(normalize(Normal), lightDir)), 0.005);

    float shadow = 0.0;
    
    vec2 texelSize = 1.0 / textureSize(depthMaps[0], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = LightIDToDepthMap(lightId, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 12.0;


    // If the fragment is outside the light's far plane, it is not in shadow
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

//Lights
vec3 CalcAreaLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * material.shine;
    // combine results
    vec3 diffuse = vec3(0.0);
    if(material.diffuseId == 0){
        vec3 ambient  = vec3(0.2);
        diffuse  = light.diffuse  * diff ;
    } else{
         diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    }
    vec3 specular = vec3(0.1);
    if(material.specularId == 0){
      specular = light.specular * spec ;
    }else {
      specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    }
    return (diffuse + specular) * light.intensity;
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 viewDir) {

vec3 lightDir = normalize(light.position - FragPos);
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    
    if(theta > light.outerCutOff) 
    {    
        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = vec3(0.0);
        vec3 specular = vec3(0.0);

        if(material.diffuseId == 0){
          diffuse = light.diffuse * diff;
        }else{
          diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));  
        }

        // specular
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * material.shine;
        if(material.specularId == 0){
          specular = light.specular * spec;
        }else{
          specular = light.specular * spec * vec3(texture(material.specular, TexCoord));  
        }
        
        // attenuation
        float distance    = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        diffuse     *= attenuation;
        specular    *= attenuation;
 
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        diffuse   *= intensity;
        specular  *= intensity;
            
        vec3 result = diffuse + specular;
        return result * light.intensity;
    }
    return vec3(0);
}
