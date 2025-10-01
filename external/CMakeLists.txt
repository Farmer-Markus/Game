add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)


if(${LINK_STATIC})
  set(SDL_SHARED FALSE)
  set(SDL_STATIC TRUE)
else()
  set(SDL_SHARED TRUE)
  set(SDL_STATIC FALSE)
endif()
add_subdirectory(SDL3)
