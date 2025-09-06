#include "Video.h"
#include <utils/Log.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <glad/glad.h>


Video::~Video() {
    destroy();
}

void Video::destroy() {
    if(glContext != NULL) {
        SDL_GL_DestroyContext(glContext);
        glContext = NULL;
    }
    if(mainWindow != NULL) {
        SDL_DestroyWindow(mainWindow);
        mainWindow = NULL;
    }
}

bool Video::initialize() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    mainWindow = SDL_CreateWindow("Game", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if(mainWindow == nullptr) {
        LOG.write(utils::LogTarget::FileErr | utils::LogTarget::Stderr, "Video: Failed to create SDL window: %s") % SDL_GetError();
        return false;
    }

    glContext = SDL_GL_CreateContext(mainWindow);
    if(glContext == nullptr) {
        LOG.write(utils::LogTarget::FileErr | utils::LogTarget::Stderr, "Video: Failed to create OpenGl context: %s") % SDL_GetError();
        return false;
    }

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        LOG.write(utils::LogTarget::FileErr | utils::LogTarget::Stderr, "Video: Failed to initialize glad");
        return false;
    }

    return true;
}

bool Video::isInitialized() const {
    return mainWindow != NULL && glContext != NULL;
}

void Video::swapBuffers() const {
    SDL_GL_SwapWindow(mainWindow);
}

bool Video::getWindowSize(int* w, int* h) const {
    return SDL_GetWindowSize(mainWindow, w, h) ;
}

void Video::viewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    glViewport(x, y, width, height);
}

bool Video::setFullscreen(bool mode) const {
    return SDL_SetWindowFullscreen(mainWindow, mode);
}