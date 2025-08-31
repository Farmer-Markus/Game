add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

set(SDL_SHARED FALSE)
set(SDL_STATIC TRUE)
add_subdirectory(SDL3)