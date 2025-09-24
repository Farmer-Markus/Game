#version 330 core
layout(location = 0) out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D container;


void main() {
    FragColor = texture(container, texCoords);
}