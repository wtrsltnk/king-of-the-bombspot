
CPMAddPackage(
    NAME glm
    GIT_TAG 0.9.9.7
    GITHUB_REPOSITORY g-truc/glm
)

CPMAddPackage(
    NAME SDL2
    VERSION 2.0.12
    URL https://www.libsdl.org/release/SDL2-2.0.12.zip
    OPTIONS
        "SDL_SHARED Off"
)
