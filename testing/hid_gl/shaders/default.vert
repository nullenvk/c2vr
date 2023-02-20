#version 450 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTex;

uniform mat4 transform;

out vec2 TexCoord;

void
main()
{
    gl_Position = transform * vec4(vPos, 1.0f);
    TexCoord = vTex;
}
