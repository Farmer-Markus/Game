#pragma once
#include <SDL3/SDL_video.h>

class Video {
public:

    ~Video();
    void destroy();

    // Create window & ogl context
    bool initialize();
    // const hinter funkton: Funktion verändert keinerlei variablen außer selbst erstellte
    bool isInitialized() const;

    // // Binds local window and gl Context to draw to
    // int makeCurrent();

    // Draw gl buffer to window
    void swapBuffers() const;

    bool getWindowSize(int* w, int* h) const;
    void setViewport(unsigned int x, unsigned int y,
        unsigned int width, unsigned int height) const;
 
    bool setFullscreen(bool mode) const;
    bool setMouseCapture(bool mode) const;
    

private:

    SDL_Window* m_window = NULL;
    SDL_GLContext m_glContext = NULL;
};