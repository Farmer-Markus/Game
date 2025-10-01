



// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

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



/*
float verexData[] = {
    // Vorderseite
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,

    // Hinterseite
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
};

unsigned int indizes[] = {
    0, 1, 2,
    1, 2, 3
};
*/

/*
float verexData[] = {
    // 8 eindeutige Eckpunkte
    -0.5f, -0.5f, -0.5f, // 0
     0.5f, -0.5f, -0.5f, // 1
     0.5f,  0.5f, -0.5f, // 2
    -0.5f,  0.5f, -0.5f, // 3
    -0.5f, -0.5f,  0.5f, // 4
     0.5f, -0.5f,  0.5f, // 5
     0.5f,  0.5f,  0.5f, // 6
    -0.5f,  0.5f,  0.5f  // 7
};

unsigned int indizes[] = {
    // Vorderseite
    4, 5, 6,
    4, 6, 7,
    // Rückseite
    0, 1, 2,
    0, 2, 3,
    // Links
    0, 4, 7,
    0, 7, 3,
    // Rechts
    1, 5, 6,
    1, 6, 2,
    // Oben
    3, 2, 6,
    3, 6, 7,
    // Unten
    0, 1, 5,
    0, 5, 4
};*/


// Just flat plate
float vertexData[] = {
    -0.5f, 0.5f,
    0.5f, 0.5f,
    -0.5f, -0.5f,
    0.5f, -0.5f
};

uint indizes[] = {
    0, 1, 2,
    1, 2, 3
};
//

Video video;
GLuint VBO, VAO, EBO, instancedVBO;
Shader shader;
glm::mat4 view, proj, model;

glm::vec3 camPos(0.0f, 0.0f, -5.0f);
glm::vec3 cameTop(0.0f, 1.0f, 0.0f);
glm::vec3 camFront(0.0f, 0.0f, -1.0f);

float camPitch = 0.0f;
float camYaw = 90.0f;

float mouseSensitivity = 0.1f;
float walkSpeed = 0.1f;

struct blockFace {
    glm::vec3 coords;   // World coords
    uint8_t face;       // oben, unten, vorne, hinten, links, rechts
};


std::vector<blockFace> blockPositions;


bool glInit() {
    if(!video.initialize()) return false;

    shader.load(CFG.replacePath("<DATA>/shaders/main.vert"), CFG.replacePath("<DATA>/shaders/main.frag"));



    glm::vec2 worldSize = glm::vec2(100, 100);

    blockPositions.reserve(worldSize.x * worldSize.y);
    for(size_t x = 0; x < worldSize.x; x++) {
        for(size_t y = 0; y < worldSize.y; y++) {
            for(uint8_t i = 0; i < 6; i++)
                blockPositions.push_back({glm::vec3(x, 0, y), i});
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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glGenBuffers(1, &instancedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instancedVBO);
    glBufferData(GL_ARRAY_BUFFER, blockPositions.size() * sizeof(blockFace), blockPositions.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(blockFace), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(blockFace), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
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


    glBindVertexArray(VAO);
    
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, blockPositions.size());
    glBindVertexArray(0);
}


int main() {
    if(!glInit())
        return 1;

    bool captMouse = true;
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

