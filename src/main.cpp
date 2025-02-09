#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>

#include <cmath>
#include <iostream>

#include "Bullet.h"
#include "Field.h"
#include "Object.h"
#include "Tank.h"
#include "Window.h"

const int delay = 20;

void Run() {
    bool running = true;

    Controller player(
        Tank::Create({window_width * 0.5f, window_height * 0.5f}, 0.1F, 0.1F, "Sources/hull.png", "Sources/turret.png"),
        26, 22, 7, 4);
    Bullet::bullet_texture.Load("Sources/bullet.png");
    Terrain ground("Sources/grass.png", window_width, window_height, player.GetTareget(),
                   {window_width * 0.5F, window_height * 0.5F});
    Wall::LoadWallsFromFile("map");
    int old_key = 0;
    const auto keydown = [&](int key) {
        if (key == old_key) return;

        player.OnKeyDown(key);

        old_key = key;
        std::cout << " key value: " << key << std::endl;
    };
    const auto keyup = [&](int key) {
        old_key = 0;
        player.OnKeyUp(key);
    };
    const auto mousemove = [&](int x, int y) {
        player.GetTareget().RotateTurretToPoint(float(x) + viewpoint.x, float(y) + viewpoint.y);
    };
    const auto mousedown = [&](int x, int y) {
        player.GetTareget().Fire();
        std::cout << " bullets: " << Bullet::bullets.size() << '\n';
    };

    SDL_Event event;
    while (running) {
        int start = SDL_GetTicks();
        // event handling
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    keydown(event.key.keysym.scancode);
                    break;

                case SDL_KEYUP:
                    keyup(event.key.keysym.scancode);
                    break;

                case SDL_MOUSEMOTION:
                    mousemove(event.button.x, event.button.y);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mousedown(event.button.x, event.button.y);
                    break;
            }
        }
        SDL_SetRenderDrawColor(main_ren, 0, 0, 0, 0);
        SDL_RenderClear(main_ren);
        SDL_SetRenderDrawColor(main_ren, 255, 255, 255, 0);
        ground.Move();
        UpdateObjects();
        SDL_RenderPresent(main_ren);
        // stricting frame rate to 40 fps
        {
            time_elapsed = SDL_GetTicks() - start;
            if (time_elapsed < ::delay) {
                SDL_Delay(::delay - time_elapsed);
                time_elapsed = ::delay;
            }
        }
    }
}

int main(int argc, char* args[]) {
    return 0;
}
