#version 330 core
layout(location = 0) in vec3 inLoc;
layout(location = 1) in vec3 inOffset;
layout(location = 2) in uint blockFace; //


uniform mat4 view;
uniform mat4 projection;


vec3 rotate(vec3 pos, uint face) {    
    if(face == 0u) return vec3(pos.x, -pos.z + 0.5f, pos.y);    // Top
    if(face == 1u) return vec3(pos.x, pos.z - 0.5f, pos.y);     // Bottom
    if(face == 2u) return vec3(pos.x, pos.y, pos.z - 0.5f);     // Front
    if(face == 3u) return vec3(-pos.x, pos.y, pos.z + 0.5f);    // Back
    if(face == 4u) return vec3(0.5f, pos.y, pos.x);             // Left
    if(face == 5u) return vec3(-0.5f, pos.y, -pos.x);           // Right

    // if(face == 0u) { // Top
    //     pos = vec3(pos.x, -pos.z + 0.5, pos.y);
    //     //pos.y += 0.5f;

    // } else if(face == 1u) { // Bottom
    //     pos = vec3(pos.x, pos.z - 0.5, -pos.y);
    //     // pos.y -= 0.5f;

    // } else if(face == 2u) { // Front
    //     pos.z -= 0.5f;

    // } else if(face == 3u) { // Back
    //     pos.x = -pos.x;
    //     pos.z += 0.5f;

    // } else if(face == 4u) { // Left
    //     pos = vec3(0.5f, pos.y, pos.x);
    //     // pos.x += 0.5f;

    // } else if(face == 5u) { // Right
    //     pos = vec3(-0.5f, pos.y, -pos.x);
    //     // pos.x -= 0.5f;
    // }

    // return pos;
}


void main() {
    vec3 rotPosition;
    rotPosition = rotate(inLoc, blockFace);

    gl_Position = projection * view * vec4(rotPosition + inOffset, 1.0f);
}