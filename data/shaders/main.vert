#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 inTexCoords;

out vec2 texCoords;
//uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main() {
    texCoords = inTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0f); //transform * vec4(aPos, 1.0f); //vec4(aPos.x, aPos.y, aPos.z, 1.0);
}