#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <cstddef>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
    bool running = true;
    SDL_Event event;
    short old_key = 0;
    SDL_Window *win = SDL_CreateWindow("Test", 0, 0, 100, 200, SDL_WINDOW_SHOWN);
    SDL_Init(SDL_INIT_EVENTS);
    while (running) {
        SDL_PollEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN: {
                int key = event.key.keysym.scancode;
                if (key == old_key) break;
                printf("KEY pressed : %d\n", key);
                if (key == SDLK_ESCAPE) running = false;
                old_key = key;
                break;
            }
        };
        SDL_Delay(20);
    }
    SDL_DestroyWindow(win);
    return 0;
}
