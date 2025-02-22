#include "Game.h"

#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include "Bullet.h"
#include "Object.h"
#include "Tank.h"
#include "Texture.h"
#include "Wall.h"

Game::Game(int width, int height, SDL_WindowFlags flags) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_EVENTS);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    int x = SDL_WINDOWPOS_CENTERED, y = SDL_WINDOWPOS_CENTERED;
    if ((flags & SDL_WINDOW_FULLSCREEN) && !(flags & 0x00001000)) {
        SDL_Rect bounds;
        SDL_GetDisplayUsableBounds(0, &bounds);
        width = bounds.w;
        height = bounds.h;
        printf("x = %d\ny = %d\nw = %d\nh = %d\n", bounds.x, bounds.y, bounds.w, bounds.h);
        flags = SDL_WindowFlags((flags ^ SDL_WINDOW_FULLSCREEN) | SDL_WINDOW_BORDERLESS);
        x = 0, y = 0;
    }
    this->window = SDL_CreateWindow("Tank", x, y, width, height, SDL_WINDOW_SHOWN | flags);
    this->main_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    frame.w = width;
    frame.h = height;
    Tank::Init(this);
    Bullet::Init(this);
    this->frame_delay = 16;
    int doubleBuffering = 0;
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &doubleBuffering);
    printf("Double Buffering ON: %d\n", doubleBuffering);
}
void Game::SetBackgroundTexture(const char* texture_path) {
    if (!this->main_renderer) return;
    SDL_Surface* surf = IMG_Load(texture_path);
    /*SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));*/
    this->texture = {SDL_CreateTextureFromSurface(main_renderer, surf), surf->w, surf->h};
    SDL_SetTextureBlendMode(texture.texture, SDL_BLENDMODE_NONE);
    /*SDL_SetTextureColorMod(texture.texture, 70, 70, 70);*/
    SDL_SetTextureAlphaMod(texture.texture, 60);
    row = frame.h / surf->h + 2;
    col = frame.w / surf->w + 2;
    start_point.x = -surf->w;
    start_point.y = -surf->w;
    SDL_FreeSurface(surf);
}
void Game::Render() {
    SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
    SDL_RenderClear(this->main_renderer);
    if (!texture.texture) return;
    SDL_FRect dstrect = {start_point.x, start_point.y, float(texture.width), float(texture.height)};
    for (int i = 0; i < col; i++) {
        for (int j = 0; j < row; j++) {
            SDL_RenderCopyF(this->main_renderer, this->texture.texture, NULL, &dstrect);
            dstrect.y += dstrect.h;
        }
        dstrect.y = start_point.y;
        dstrect.x += dstrect.w;
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
        if (start_point.x >= 0)
            start_point.x -= texture.width;
        else if (start_point.x <= -texture.width)
            start_point.x += texture.width;

        if (start_point.y >= 0)
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
        target->accelerating = 1;
        return;
    }
    if (key == SDL_SCANCODE_S) {
        target->accelerating = -1;
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
        target->accelerating = 0;
        return;
    }

    if (key == SDL_SCANCODE_D || key == SDL_SCANCODE_A) {
        target->rotating = 0;
        return;
    }
};

void Game::HandleEvents() {
    SDL_Event event;
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
}
void Game::Update() {
    auto next = Bullet::bullets.begin();
    while (next != Bullet::bullets.end()) {
        bool hit = false;
        auto it = next;
        next++;

        for (auto it2 = Wall::walls.begin(); it2 != Wall::walls.end(); it2++) {
            if (it->Collision(&(*it2))) {
                Bullet::bullets.erase(it);
                hit = true;
                break;
            }
        }
        if (hit) continue;

        for (auto it2 = Tank::tanks.begin(); it2 != Tank::tanks.end(); it2++) {
            if (it2->GetID() == it->GetOwner()) continue;
            if (it->Collision(&(*it2))) {
                Bullet::bullets.erase(it);
                hit = true;
                break;
            }
        }

        if (hit) continue;

        it->Move();
    }
    for (auto tank = Tank::tanks.begin(); tank != Tank::tanks.end(); tank++) {
        for (auto wall = Wall::walls.begin(); wall != Wall::walls.end(); wall++)
            tank->StaticCollision(&(*wall));

        tank->Move();
    }
    this->Move();

    this->Render();
    for (auto wall = Wall::walls.begin(); wall != Wall::walls.end(); wall++) wall->Render();
    for (auto& tk : Tank::tanks) tk.Render();
    for (auto& bt : Bullet::bullets) bt.Render();
    SDL_RenderPresent(this->main_renderer);
}
void Game::Start() {
    this->running = true;

    while (running) {
        /*auto start = SDL_GetTicks();*/
        // event handling
        HandleEvents();
        Update();
        // controlling frame rate
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
    Bullet::Quit();
}
