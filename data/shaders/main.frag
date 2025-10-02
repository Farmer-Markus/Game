#version 330 core
in vec2 texCoords;
in vec4 texColor;

out vec4 FragColor;

uniform sampler2D ourTexture;


void main() {
    FragColor = texColor * texture(ourTexture, texCoords);
}