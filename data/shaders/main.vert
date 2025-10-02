#version 330 core
layout(location = 0) in vec2 inLoc;
layout(location = 1) in vec3 inOffset;
layout(location = 2) in uint inBlockFace;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec4 inTexColor;
layout(location = 5) in vec2 inAtlasCoords;

out vec2 texCoords;
out vec4 texColor;
out vec2 atlasCoords;

uniform mat4 view;
uniform mat4 projection;


vec3 rotate(vec3 pos, uint face) {
    if(face == 0u) return vec3(pos.x, -pos.z + 0.5f, pos.y);    // Top
    if(face == 1u) return vec3(pos.x, pos.z - 0.5f, pos.y);     // Bottom
    if(face == 2u) return vec3(pos.x, pos.y, pos.z - 0.5f);     // Front
    if(face == 3u) return vec3(-pos.x, pos.y, pos.z + 0.5f);    // Back
    if(face == 4u) return vec3(0.5f, pos.y, pos.x);             // Left
    if(face == 5u) return vec3(-0.5f, pos.y, -pos.x);           // Right
}


void main() {
    vec3 rotPosition;
    rotPosition = rotate(vec3(inLoc, 0.0f), inBlockFace);

    gl_Position = projection * view * vec4(rotPosition + inOffset, 1.0f);

    const float tileSize = 1.0 / 7.0;
    texCoords = inTexCoords * tileSize + inAtlasCoords * tileSize;
    texColor = inTexColor;
}