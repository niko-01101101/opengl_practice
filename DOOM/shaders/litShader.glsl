#version 330 core

struct Material{
  float shine;
  sampler2D diffuse;
  int diffuseId;
  sampler2D specular;
  int specularId;
  vec3 color;
};

struct AreaLight{
  vec3 position;
  vec3 diffuse;
  vec3 specular;
  float intensity;
};

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D ourTexture;

uniform vec3 viewPos;
uniform int textureId;

uniform Material material;

vec3 CalcAreaLight(AreaLight light, vec3 normal, vec3 fragPos,  vec3 viewDir);

#define AREA_LIGHTS 3
uniform AreaLight areaLights[AREA_LIGHTS];

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0f);
    for(int i = 0; i < AREA_LIGHTS; i++)
        result += CalcAreaLight(areaLights[i], norm, FragPos, viewDir);

    if(textureId != 0){
      if(material.diffuseId == textureId){
        FragColor =  texture(material.diffuse, TexCoord) * vec4(result * material.color, 1.0f);
      }else{
        FragColor =  texture(ourTexture, TexCoord) * vec4(result * material.color, 1.0f);
      }
    }else{
      FragColor = vec4(result * material.color, 1.0f);
    }
}




//Lights
vec3 CalcAreaLight(AreaLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * material.shine;
    // combine results
    vec3 ambient = vec3(0.1);
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
    return (ambient + diffuse + specular);
}
