#include "graphics/Video.h"
#include <SDL3/SDL.h>

int main() {
    Video video;
    video.initialize();
    if(!video)
        return 1;

    video.swapBuffers();
    SDL_Delay(5000);
    return 0;
}

