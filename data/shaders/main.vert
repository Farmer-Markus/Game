#version 330 core

layout (location = 0) in vec3 aPos;      // Vertex-Position
layout (location = 1) in vec2 aOffset;   // Instanz-Offset

void main()
{
    gl_Position = vec4(aPos.xy + aOffset, aPos.z, 1.0);
}
