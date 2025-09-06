#include <SDL3/SDL_video.h>

class Video {
public:

    ~Video();
    void destroy();

    // Create window & ogl context
    bool initialize();
    // const hinter funkton: Funktion verändert keinerlei variablen außer selbst erstellte
    bool isInitialized() const;

    // Draw gl buffer to window
    void swapBuffers() const;

    bool getWindowSize(int* w, int* h) const;
    bool setFullscreen(bool mode) const;
    

private:

    SDL_Window* mainWindow = NULL;
    SDL_GLContext glContext = NULL;
};