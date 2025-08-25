#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("game", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
}

