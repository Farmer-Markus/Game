#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_timer.h"
#include <GLES2/gl2.h>
#include <SDL3/SDL_events.h>
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <graphics/Video.h>
#include <config/Config.h>
#include <stdexcept>
#include <utils/Log.h>
#include <graphics/Shader.h>

#include <config/Paths.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


#include <SDL3/SDL.h>




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


Video video;


Shader shader;
unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int texture;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

glm::mat4 trans = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);


std::vector<glm::vec3> positions;

bool glTesting() {

    /*float vboData[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.9f, 0.2f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.7f, 1.0f,
        0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.2f, 1.0f
    };*/

    /*float vboData[] = {
        // positions                        // colors                       // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };*/

    float vboData[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    try {
        shader.load(CFG.replacePath("<DATA>/shaders/main.vert"), CFG.replacePath("<DATA>/shaders/main.frag"));
    } catch (std::runtime_error e) {
        return false;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), vboData, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*// color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    // Texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    

    int width, height, nrChannels;
    unsigned char *data = stbi_load(CFG.replacePath("<DATA>/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0); 

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                                    // mipmap lvl
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);


    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    

    int w,h;
    video.getWindowSize(&w, &h);
    projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h , 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);

    positions = {
        {0.0f, 1.0f, 0.0f},
        {0.0f, 2.0f, 0.0f},
        {0.0f, 3.0f, 0.0f},
        {0.0f, 4.0f, 0.0f},
        {0.0f, 5.0f, 0.0f},
    };


    /*

    glm::vec3 testPos = {
        0.0f, 0.0f, 0.0f
    };

    glm::mat4 testMat = {
        0.0f,0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,0.0f
    };

    */

    return true;
}

/*
// glm::translate
glm::mat4 translate(glm::mat4 mt, glm::vec3& vc) {
    mt[3].x += vc.x;
    mt[3].y += vc.y;
    mt[3].z += vc.z;
    return mt;
}
*/


//----------------------------------------------------------------------------
float rotation = 0.5f; // 10 grad
float pitch = 0.0f;
float yaw = -90.0f;

//----------------------------------------------------------------------------

void glUpdate() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    /*
        Das macht alles Sinn!
        Ich dachte erst, dass folgendes richtig sein muss: (https://www.youtube.com/watch?v=w-hXOYZ2gpo)

                cameraFront.x = cos(glm::radians(yaw));
                cameraFront.y = sin(glm::radians(pitch));
                cameraFront.z = sin(glm::radians(yaw));

        Aaaaber, wenn man sich das Video genau anschaut, sieht man wenn man dem Blickwinkel folgt (wenn man von der Mitte aus immer auf P schaut)
        sieht man, dass sich nicht nur die Höhe(sinus) verändert sondern auch unten der cosinus! Der wird kleiner, je höher es geht.
        Das muss beim vec3 beachtet werden, da sonnst (wenn x oder im 3d fall z gleich bleiben würde) das hochgucken immer langsamer werden würde.
        Der Winkel würde nur langsam steigen, da sich nur die höhe verändern würde. Also muss sich wie im Video auch die z(im vid. x) koordinate dynamisch
        anpassen. Und da man im 3d Space auch seitlich zwischen z und x stehen kann, ist die x UND z achse betroffen! Also:

                cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront.y = sin(glm::radians(pitch));
                cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    */
    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraFront);

    // Wenn nur camFront, dann würde immer von 0,0 aus die blickrichtung genommen werden und so wirds von der cam position
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    /*int loc = glGetUniformLocation(shader, "offset");
    glUniform1f(loc, (SDL_GetTicks() / 2000.0f));

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, (float)SDL_GetTicks(), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::rotate(model, (float)SDL_GetTicks() / 10000 * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));


    loc = glGetUniformLocation(shader, "transform");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(trans));*/

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);

    /*
    template<typename T, qualifier Q>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR mat<4, 4, T, Q> translate(mat<4, 4, T, Q> const& m, vec<3, T, Q> const& v)
	{
		mat<4, 4, T, Q> Result(m);
		Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
		return Result;
	}
    */


    for(glm::vec3 pos : positions) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));


        int loc = glGetUniformLocation(shader, "model");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

        loc = glGetUniformLocation(shader, "projection");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

        loc = glGetUniformLocation(shader, "view");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));


        
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }



    

}


int main() {
    video.initialize();
    if(!video.isInitialized())
        return 1;

    if(!glTesting())
        return 1;
    
    bool running = true;
    while(running) {
        SDL_Event ev;
        while(SDL_PollEvent(&ev)) {
            switch(ev.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                
                case SDL_EVENT_WINDOW_RESIZED: {
                    int w,h;
                    video.getWindowSize(&w, &h);
                    video.viewport(0, 0, w, h);
                    break;
                }

                case SDL_EVENT_KEY_DOWN: {
                    switch(ev.key.key) {
                        case SDLK_W:
                            if(SDL_GetModState() & SDL_KMOD_LSHIFT)
                                pitch -= rotation;
                            else
                                cameraPos += cameraFront * 0.05f;
                            break;
                        
                        case SDLK_S:
                            if(SDL_GetModState() & SDL_KMOD_LSHIFT)
                                pitch += rotation;
                            else
                                cameraPos -= cameraFront * 0.05f;
                            break;

                        case SDLK_A:
                            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp) * 0.05f);
                            break;
                        
                        case SDLK_D:
                            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp) * 0.05f);
                            break;
                        
                        case SDLK_Q:
                            yaw -= rotation;
                            break;
                        
                        case SDLK_E:
                            yaw += rotation;
                            break;
                        
                        case SDLK_SPACE:
                            cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * 0.3f;
                            break;
                        
                        case SDLK_LCTRL:
                            cameraPos -= glm::vec3(0.0f, 1.0f, 0.0f) * 0.3f;
                            break;
                        
                    }

                    break;
                }
                
                default:
                    break;
            }
        }
        

        glUpdate();
        video.swapBuffers();
        //SDL_Delay(100);
    }
    
    shader.destroy();
    video.destroy();

    //SDL_Delay(5000);
    return 0;
}

