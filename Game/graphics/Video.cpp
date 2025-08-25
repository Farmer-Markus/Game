#include "Video.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>


bool Video::initialize() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    mainWindow = SDL_CreateWindow("Game", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if(mainWindow == nullptr) {
        //LOG.write(LogTarget::FileAndStderr, "Video: Failed to create SDL window: %s") % SDL_GetError();
        return false;
    }

    glContext = SDL_GL_CreateContext(mainWindow);
    if(glContext == nullptr) {
        // LOG.write(LogTarget::FileAndStderr, "Video: Failed to create OpenGl context: %s") S SDL_GetError();
        return false;
    }

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        // LOG.write(LogTarget::FileAndStderr, "Video: Failed to initialize glad");
        return false;
    }

    return true;
}

void Video::swapBuffers() {
    SDL_GL_SwapWindow(mainWindow);
}

bool Video::getWindowSize(int* w, int* h) {
    return SDL_GetWindowSize(mainWindow, w, h);
}


bool Video::isInitialized() const {
    return mainWindow != NULL && glContext != NULL;
}
