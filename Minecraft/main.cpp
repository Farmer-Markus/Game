



#include "SDL3/SDL_video.h"
#include <functional>
#include <unordered_map>
#include <utility>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/*
Drawcalls in openGl sind sehr langsam! Es kann aber pro drawcall immer nur ein shader program benutzt werden.
Also sollte man einen shader und drawcall für jede kathegorie haben. Am besten 100-500 drawcalls MAXIMUM

->Terrain rendering
->Tree rendering
->User building rendering
->Fluid effects rendering
->General screen effect rendering(fog, vignette etc.)
->Animated rendering: 
    GLDrawElementsInstanced
    Big vbo buffer with, [mat4 transformation, int frameOffsetInAtlas,] // Just give shader the matrix, the atlas and the offset to the current frame for the current object to render. Now the shader only needs to go to the offset and render the image/frame on that offset
    big texture atlas with every element and all of its frames
    (asked chatgpt (private): https://chatgpt.com/c/68c53712-476c-8321-882d-64c5c312f45c)
*/

#include <SDL3/SDL_events.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



/*
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
*/


#include "SDL3/SDL_keycode.h"
#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sys/types.h>
#include <vector>


#include "graphics/Video.h"
#include "graphics/Shader.h"
#include "config/Config.h"
#include "utils/Log.h"

using namespace utils;




// Just flat plate
float vertexData[] = {
    // x, y,   u, v                 // textur umgedreht
    -0.5f,  0.5f,   1.0f, 0.0f, // unten rechts
     0.5f,  0.5f,   0.0f, 0.0f, // unten links
    -0.5f, -0.5f,   1.0f, 1.0f, // oben rechts
     0.5f, -0.5f,   0.0f, 1.0f  // oben links
};


uint indizes[] = {
    0, 1, 2,
    1, 3, 2
};
//

Video video;
GLuint VBO, VAO, EBO, instancedVBO;
Shader shader;
glm::mat4 view, proj, model;
unsigned int texture;


glm::vec3 camPos(0.0f, 0.0f, -5.0f);
glm::vec3 cameTop(0.0f, 1.0f, 0.0f);
glm::vec3 camFront(0.0f, 0.0f, -1.0f);

float camPitch = 0.0f;
float camYaw = 90.0f;

float mouseSensitivity = 0.1f;
float walkSpeed = 0.1f;

struct blockFace {
    glm::vec3 coords;           // World coords
    uint8_t face;               // oben, unten, vorne, hinten, links, rechts
                                // R, G, B, A
    glm::vec4 colorMultiplier = glm::vec4(1.0f);  // To change color of texture
    glm::vec2 atlasCoords = glm::vec2(0.0f, 0.0f);
};

struct grassBlock {
    glm::vec3 worldCoords;
    std::vector<blockFace> faces = {
        {worldCoords, 0, glm::vec4(0.243f, 0.64f, 0.196f, 1.0f), {0.0f, 0.0f}},
        {worldCoords, 1, glm::vec4(0.65f), {1.0f, 0.0f}},
        {worldCoords, 2, glm::vec4(0.65f), {2.0f, 0.0f}},
        {worldCoords, 3, glm::vec4(0.65f), {2.0f, 0.0f}},
        {worldCoords, 4, glm::vec4(0.65f), {2.0f, 0.0f}},
        {worldCoords, 5, glm::vec4(0.65f), {2.0f, 0.0f}}
    };
};

struct worldPos {
    float x,y,z;

    bool operator==(const worldPos& o) const {
        return (x == o.x && y == o.y && z == o.z);
    }
};

namespace std {
    template<>
    struct hash<worldPos> {
        size_t operator()(const worldPos& wp) const {
            return hash<float>()(wp.x) ^ hash<float>()(wp.y) ^ hash<float>()(wp.z);
        }
    };
}


std::unordered_map<worldPos, grassBlock> world;
std::vector<blockFace> renderFaces;


bool glInit() {
    if(!video.initialize()) return false;


    if(!shader.load(CFG.replacePath("<DATA>/shaders/main.vert"), CFG.replacePath("<DATA>/shaders/main.frag"))) {
        LOG.write(LogTarget::Stderr, "Failed to load shaders! %s") % shader.getErrorMessage();
        return false;
    }

    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(CFG.replacePath("<DATA>/textures/blockAtlas.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        int format;
        if(nrChannels == 1) {
            format = GL_RED;
            // Use same channel value for every value(RGB) and 1 for A
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

        } else if(nrChannels == 2)
            format = GL_RG;
        else if(nrChannels == 3)
            format = GL_RGB;
        else if(nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);




    glm::vec2 worldSize = glm::vec2(100, 100);

    world.reserve(worldSize.x * worldSize.y);
    for(size_t x = 0; x < worldSize.x; x++) {
        for(size_t y = 0; y < worldSize.y; y++) {
            world[{(float)x, 0.0f, (float)y}] = {{x, 0.0f, y}};
        }
    }


    // Some sort of face culling. Don't draw faces that are next to another face
    renderFaces.reserve(worldSize.x * worldSize.y);
    for(const auto& entry : world) {
        const grassBlock& b = entry.second;
        if(world.find({b.worldCoords.x, b.worldCoords.y + 1.0f, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[0]);

        }
        if(world.find({b.worldCoords.x, b.worldCoords.y - 1.0f, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[1]);

        }
        if(world.find({b.worldCoords.x, b.worldCoords.y, b.worldCoords.z - 1.0f}) == world.end()) {
            renderFaces.push_back(b.faces[2]);

        }
        if(world.find({b.worldCoords.x, b.worldCoords.y, b.worldCoords.z + 1.0f}) == world.end()) {
            renderFaces.push_back(b.faces[3]);

        }
        if(world.find({b.worldCoords.x + 1.0f, b.worldCoords.y, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[4]);

        }
        if(world.find({b.worldCoords.x - 1.0f, b.worldCoords.y, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[5]);

        }
    }

    // for(uint8_t i = 0; i < 6; i++)
    //     blockPositions.push_back({glm::vec3(2, 2, 3), i});
    
    // for(uint8_t i = 0; i < 6; i++)
    //     blockPositions.push_back({glm::vec3(i + 15, -4, 7), i});

    // for(uint8_t i = 0; i < 6; i++)
    //     blockPositions.push_back({glm::vec3(17, -6, 7), i});


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indizes), indizes, GL_STATIC_DRAW);

    // Vertex Data
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex texture coordinates
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(3);



    glGenBuffers(1, &instancedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instancedVBO);
    glBufferData(GL_ARRAY_BUFFER, renderFaces.size() * sizeof(blockFace), renderFaces.data(), GL_STATIC_DRAW);
    
    // BlockFace coords
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(blockFace), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // BlockFace facing direction
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(blockFace), (void*)(offsetof(blockFace, face)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(blockFace), (void*)(offsetof(blockFace, colorMultiplier)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // Texture atlas start coords
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(blockFace), (void*)(offsetof(blockFace, atlasCoords)));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);


    // BlockFace texture Atlas coords
    // ...

    /*
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(blockFace), (void*)(sizeof(glm::vec3)));
glEnableVertexAttribArray(2);
glVertexAttribDivisor(2, 1);*/


    //glVertexAttribDivisor()


    glEnable(GL_DEPTH_TEST);

    int w,h;
    video.getWindowSize(&w, &h);
    proj = glm::mat4(1.0);
    proj = glm::perspective(glm::radians(45.0f), (float)w / (float)h , 0.1f, 10000.0f);

    view = glm::mat4(1.0f);
    //model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));


    return true;
}


void glRender() {
    renderFaces.clear();
    for(const auto& entry : world) {
        const grassBlock& b = entry.second;

    
        if(world.find({b.worldCoords.x, b.worldCoords.y + 1.0f, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[0]);

        }
        if(world.find({b.worldCoords.x, b.worldCoords.y - 1.0f, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[1]);

        }
        if(world.find({b.worldCoords.x, b.worldCoords.y, b.worldCoords.z - 1.0f}) == world.end()) {
            renderFaces.push_back(b.faces[2]);

        }
        if(world.find({b.worldCoords.x, b.worldCoords.y, b.worldCoords.z + 1.0f}) == world.end()) {
            renderFaces.push_back(b.faces[3]);

        }
        if(world.find({b.worldCoords.x + 1.0f, b.worldCoords.y, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[4]);

        }
        if(world.find({b.worldCoords.x - 1.0f, b.worldCoords.y, b.worldCoords.z}) == world.end()) {
            renderFaces.push_back(b.faces[5]);

        }
    }


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader);
    
    camFront.x = cos(glm::radians(camYaw)) * cos(glm::radians(camPitch));
    camFront.y = sin(glm::radians(camPitch));
    camFront.z = sin(glm::radians(camYaw)) * cos(glm::radians(camPitch));

    camFront = glm::normalize(camFront);

    view = glm::lookAt(camPos, camPos + camFront, cameTop);
    int loc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

    loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj));

    // loc = glGetUniformLocation(shader, "model");
    // glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));


    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderFaces.size());
    glBindVertexArray(0);
}


int main() {
    if(!glInit())
        return 1;

    bool captMouse = true;
    bool outlines = false;
    video.setMouseCapture(captMouse);

    bool running = true;
    while(running) {
        SDL_Event ev;
        while(SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                
                case SDL_EVENT_WINDOW_RESIZED:
                    video.setViewport(0, 0, ev.window.data1, ev.window.data2);
                    proj = glm::perspective(glm::radians(45.0f), (float)ev.window.data1 / (float)ev.window.data2 , 0.1f, 10000.0f);
                    break;
                
                case SDL_EVENT_MOUSE_MOTION:
                    if(!captMouse)
                        break;
                    camYaw += ev.motion.xrel * mouseSensitivity;
                    camPitch += -ev.motion.yrel * mouseSensitivity;

                    if(camPitch >= 90.0f)
                        camPitch = 89.0f;
                    else if(camPitch <= -90.0f)
                        camPitch = -89.0f;

                    break;
                
                case SDL_EVENT_KEY_DOWN: {
                    switch(ev.key.key) {
                        case SDLK_W:
                            camPos += glm::normalize(glm::vec3(camFront.x, 0.0f, camFront.z)) * walkSpeed;
                            break;

                        case SDLK_S:
                            camPos -= glm::normalize(glm::vec3(camFront.x, 0.0f, camFront.z)) * walkSpeed;
                            break;

                        case SDLK_A:
                            camPos += glm::cross(cameTop, glm::normalize(glm::vec3(camFront.x, 0.0f, camFront.z)) * walkSpeed);
                            break;

                        case SDLK_D:
                            camPos -= glm::cross(cameTop, glm::normalize(glm::vec3(camFront.x, 0.0f, camFront.z)) * walkSpeed);
                            break;

                        case SDLK_SPACE:
                            camPos += glm::vec3(0.0f, 1.0f, 0.0f) * walkSpeed;
                            break;
                        
                        case SDLK_LSHIFT:
                            camPos -= glm::vec3(0.0f, 1.0f, 0.0f) * walkSpeed;
                            break;
                        
                        case SDLK_ESCAPE:
                            if(captMouse) {
                                video.setMouseCapture(false);
                                captMouse = false;
                            } else {
                                video.setMouseCapture(true);
                                captMouse = true;
                            }
                            break;
                        
                        case SDLK_O:
                            if(outlines) {
                                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                                outlines = false;
                            } else {
                                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                                outlines = true;
                            }
                            break;
                        
                        case SDLK_PLUS:
                            walkSpeed += 0.1f;
                            break;
                        
                        case SDLK_MINUS:
                            walkSpeed -= 0.1f;
                            break;
                    }
                    break;
                }

            
            }
        }

        glRender();
        video.swapBuffers();
    }

    return 0;
}

