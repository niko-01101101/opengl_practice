#version 330 core

struct Material{
  float shine;
  sampler2D diffuse;
  sampler2D specular;
  vec3 color;
};

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D ourTexture;

uniform Material material;
uniform vec3 viewPos;
uniform int textureId;


void main()
{
    if(textureId != 0){
      FragColor = texture(ourTexture, TexCoord) * vec4(material.color, 1.0);
    }else{
      FragColor = vec4(material.color, 1.0);
    }
}
