#include <SDL3/SDL_events.h>
#include <graphics/Video.h>
#include <config/Config.h>
#include <stdexcept>
#include <utils/Log.h>
#include <graphics/Shader.h>

#include <config/Paths.h>
#include <glad/glad.h>

#include <SDL3/SDL.h>


Shader shader;
unsigned int VBO;
unsigned int VAO;


bool glTesting() {

    float vboData[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    try {
        shader.create(CFG.replacePath("<DATA>/shaders/main.vert"), CFG.replacePath("<DATA>/shaders/main.frag"));
        //shader.create(vertShaderSource, fragShaderSource);
    } catch (std::runtime_error e) {
        return false;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), vboData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);




    return true;
}

void glUpdate() {
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

}

int main() {
    Video video;
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

