#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D ourTexture;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform int textureId;


void main()
{
    if(textureId != 0){
      FragColor = texture(ourTexture, TexCoord) * vec4(objectColor, 1.0);
    }else{
      FragColor = vec4(objectColor, 1.0);
    }
}
