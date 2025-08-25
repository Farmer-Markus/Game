#include <SDL3/SDL_video.h>

class Video {
public:
    // Create window & ogl context
    bool initialize();

    // Draw gl buffer to window
    void swapBuffers();

    bool getWindowSize(int* w, int* h);

    // if(videoInstance) can be used to check if initialized
    explicit operator bool() const {
        return isInitialized();
    }
    

private:

    // const hinter funkton: Funktion verändert keinerlei variablen außer selbst erstellte
    bool isInitialized() const;

    SDL_Window* mainWindow = NULL;
    SDL_GLContext glContext = NULL;
};