#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <cstddef>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
    bool running = true;
    SDL_Event event;
    short old_key = 0;
    SDL_Window *win = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500,
                                       500, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Init(SDL_INIT_EVENTS);
    SDL_Color col = {0, 0, 0, 0};
    printf("W key: %d\n", SDL_SCANCODE_W);

    printf("S key: %d\n", SDL_SCANCODE_S);

    printf("A key: %d\n", SDL_SCANCODE_A);

    printf("D key: %d\n", SDL_SCANCODE_D);

    while (running) {
        SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, 255);
        col.r += 1;
        col.g += col.r >> 6;
        col.b += col.g >> 6;
        SDL_RenderClear(ren);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN: {
                    int key = event.key.keysym.scancode;
                    if (key == old_key) break;
                    printf("KEY pressed : %d\n", key);
                    if (key == SDLK_ESCAPE) running = false;
                    old_key = key;
                    break;
                }
            };
        }
        SDL_RenderPresent(ren);
        SDL_Delay(20);
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    return 0;
}
