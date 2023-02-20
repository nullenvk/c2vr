#version 450 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, TexCoord); 
    //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
