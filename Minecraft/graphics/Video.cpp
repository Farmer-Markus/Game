#include "Video.h"
#include <utils/Log.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include <glad/glad.h>


Video::~Video() {
    destroy();
}

void Video::destroy() {
    if(m_glContext != NULL) {
        SDL_GL_DestroyContext(m_glContext);
        m_glContext = NULL;
    }
    if(m_window != NULL) {
        SDL_DestroyWindow(m_window);
        m_window = NULL;
    }
}

bool Video::initialize() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window = SDL_CreateWindow("Game", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    if(m_window == nullptr) {
        LOG.write(utils::LogTarget::FileErr | utils::LogTarget::Stderr, "Video: Failed to create SDL window: %s") % SDL_GetError();
        return false;
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if(m_glContext == nullptr) {
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
    return m_window != NULL && m_glContext != NULL;
}

// int Video::makeCurrent() {
//     return SDL_GL_MakeCurrent(m_window, m_glContext);
// }

void Video::swapBuffers() const {
    SDL_GL_SwapWindow(m_window);
}

bool Video::getWindowSize(int* w, int* h) const {
    return SDL_GetWindowSize(m_window, w, h) ;
}

void Video::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) const {
    glViewport(x, y, width, height);
}

bool Video::setFullscreen(bool mode) const {
    return SDL_SetWindowFullscreen(m_window, mode);
}

bool Video::setMouseCapture(bool mode) const {
    return SDL_SetWindowMouseGrab(m_window, mode) &&
            SDL_SetWindowRelativeMouseMode(m_window, mode);
}