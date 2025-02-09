#ifndef __TEXTURE__
#define __TEXTURE__

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
struct Texture {
    SDL_Texture* texture = nullptr;
    int width = 0;
    int height = 0;
};
#endif  //!__TEXTURE__
