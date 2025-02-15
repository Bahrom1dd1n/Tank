#include "Game.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include "Object.h"
#include "Texture.h"

Game::Game(int width, int height, Object& target, const SDL_FPoint& target_posiotion) : target(target) {
    this->window = SDL_CreateWindow("Tank", 0, 20, frame.w, frame.h, SDL_WINDOW_SHOWN);
    this->main_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_InitSubSystem(SDL_INIT_VIDEO);
}
void Game::SetBackgroundTexture(const char* texture_path) {
    if (!this->main_renderer) return;
    SDL_Surface* surf = IMG_Load(texture_path);
    this->texture = {SDL_CreateTextureFromSurface(main_renderer, surf), surf->w, surf->h};
    row = frame.h / surf->h + 2;
    col = frame.h / surf->w + 2;
    SDL_FreeSurface(surf);
}
void Game::Render() {
    SDL_FRect dstrect;
    for (int i = 0; i < col; i++) {
        dstrect.x = start_point.x + i * dstrect.w;
        for (int j = 0; j < row; j++) {
            dstrect.y = start_point.y + j * dstrect.h;
            SDL_RenderCopyF(this->main_renderer, this->texture.texture, NULL, &dstrect);
        }
    }
}

void Game::Move() {
    float dx = target.center.x - this->previous_point.x;
    float dy = target.center.y - this->previous_point.y;
    this->previous_point = target.center;
    if (dx || dy) {
        float x1 = frame.x;
        float y1 = frame.y;

        frame.x = target.center.x - target_position.x;
        frame.y = target.center.y - target_position.y;
        start_point.x -= frame.x - x1;
        start_point.y -= frame.y - y1;

        if (start_point.x > 0)
            start_point.x -= texture.width;
        else if (start_point.x < -texture.height)
            start_point.x += texture.width;

        if (start_point.y > 0)
            start_point.y -= texture.height;
        else if (start_point.y < -texture.height)
            start_point.y += texture.height;
    }
}
void Game::Start() {
    this->running = true;
    while (running) {
        auto start = SDL_GetTicks();
        /*if (this->ondraw_frame) ondraw_frame(this->current_time,
         * time_elapsed);*/
        this->time_elapsed = this->current_time - start;
        if (this->time_elapsed < this->frame_delay) {
            SDL_Delay(this->frame_delay - this->time_elapsed);
            this->time_elapsed = frame_delay;
        }
    }
};
