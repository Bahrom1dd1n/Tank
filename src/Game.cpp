#include "Game.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

#include "Object.h"
#include "Tank.h"
#include "Texture.h"

Game::Game(int width, int height) {
    this->window = SDL_CreateWindow("Tank", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width,
                                    height, SDL_WINDOW_SHOWN);
    this->frame = {0.0F, 0.0F, float(width), float(height)};
    this->main_renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_EVENTS);
    Tank::Init(this);
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
    float dx = target->center.x - this->previous_point.x;
    float dy = target->center.y - this->previous_point.y;
    this->previous_point = target->center;
    if (dx || dy) {
        float x1 = frame.x;
        float y1 = frame.y;

        frame.x = target->center.x - target_position.x;
        frame.y = target->center.y - target_position.y;
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
void Game::OnMouseDown(float x, float y, short mb) {
    if (this->target && mb == 1) target->Fire();
};
void Game::OnMouseMove(float x, float y, short mb) {
    if (this->target) target->RotateTurretToPoint(x, y);
};
void Game::OnKeyDown(const short key) {
    if (key == old_key) return;
    printf("Pressed key: %d\n", key);

    old_key = key;
    if (!target) return;
    if (key == SDL_SCANCODE_W) {
        target->moving = 1;
        return;
    }
    if (key == SDL_SCANCODE_S) {
        target->moving = -1;
        return;
    }
    if (key == SDL_SCANCODE_D) {
        target->rotating = 1;
        return;
    }
    if (key == SDL_SCANCODE_A) {
        target->rotating = -1;
        return;
    }
}
void Game::OnKeyUp(short key) {
    old_key = 0;
    if (!target) return;
    if (key == SDL_SCANCODE_W || key == SDL_SCANCODE_S) {
        target->moving = 0;
        return;
    }

    if (key == SDL_SCANCODE_D || key == SDL_SCANCODE_A) {
        target->rotating = 0;
        return;
    }
};

void Game::Start() {
    this->running = true;

    SDL_Event event;
    while (running) {
        /*int start = SDL_GetTicks();*/
        // event handling
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    OnKeyDown(event.key.keysym.scancode);
                    break;
                case SDL_KEYUP:
                    OnKeyUp(event.key.keysym.scancode);
                case SDL_MOUSEBUTTONDOWN:
                    OnMouseDown(event.button.x + frame.x, event.button.y + frame.y,
                                event.button.button);
                    break;
                case SDL_MOUSEMOTION:
                    OnMouseMove(event.button.x + frame.x, event.button.y + frame.y,
                                event.button.button);
                    break;
            };
        };
        SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
        SDL_RenderClear(this->main_renderer);
        SDL_RenderPresent(this->main_renderer);
        target->Render();
        auto now = SDL_GetTicks();
        this->time_elapsed = now - this->current_time;
        this->current_time = now;
        if (this->time_elapsed < this->frame_delay) {
            SDL_Delay(this->frame_delay - this->time_elapsed);
            this->time_elapsed = frame_delay;
        }
    }
};
void Game::SetTarget(Tank* target, const SDL_FPoint& target_position) {
    this->target = target;
    this->target_position = target_position;
    this->frame.x = target->center.x - target_position.x;
    this->frame.y = target->center.y - target_position.y;
}
Game::~Game() {
    if (this->texture.texture) SDL_DestroyTexture(this->texture.texture);
    Tank::Quit();
}
