#include <SDL3/SDL_video.h>

class Video {
public:
    // Create window & ogl context
    bool initialize();
    // const hinter funkton: Funktion verändert keinerlei variablen außer selbst erstellte
    bool isInitialized() const;

    // Draw gl buffer to window
    void swapBuffers();

    bool getWindowSize(int* w, int* h);
    

private:

    SDL_Window* mainWindow = NULL;
    SDL_GLContext glContext = NULL;
};