#include "Bullet.h"

#include <cstddef>
#include <ctime>
#include <list>
#include <utility>
#include <vector>

#include "Game.h"
std::list<Bullet> Bullet::bullets;
std::vector<Bullet::BulletType> Bullet::bullet_types;
Bullet::Bullet(Game* game, const SDL_FPoint& center, const float& angle, const short& owner_id,
               const short& type)
    : owner_id(owner_id) {
    this->game = game;
    this->center = center;
    this->renderer = game->GetRenderer();
    this->rect = {0.0F, 0.0F, float(bullet_types[type].width), float(bullet_types[type].height)};
    this->num_points = 2;
    this->fixed = false;
    std::vector<SDL_FPoint> boundary_points = {{0.0F, -this->rect.h * 0.5f},
                                               {0.0F, this->rect.h * 0.5f}};
    this->SetPoints(2, std::move(boundary_points));
    // boundary points should be rotated after setting originla points!!!
    this->angle = 0;        // angle of bullet will not change
    this->RotateBy(angle);  // so once it is created its angle rotated once
}
Bullet& Bullet::Create(Game* game, const SDL_FPoint& center, const float& angle,
                       const short& owner_id, const short& type) {
    bullets.emplace_front(game, center, angle, owner_id, type);
    bullets.front().turn = bullets.begin();
    return bullets.front();
}

void Bullet::Render() {
    if (!this->InsideScreen()) return;
    const SDL_FRect& frame = this->game->GetFrame();
    this->rect.x = this->center.x - frame.x - this->rect.w * 0.5f;
    this->rect.y = this->center.y - frame.y - this->rect.h * 0.5f;
    SDL_RenderCopyExF(renderer, bullet_types[type].texture, NULL, &rect, this->angle, NULL,
                      SDL_FLIP_NONE);
}

bool Bullet::Move() {
    auto& time_elapsed = this->game->GetTimeElapsed();
    life_time += time_elapsed;
    if (life_time > 2000) {
        Bullet::bullets.erase(turn);
        return false;
    }

    this->MoveForward(bullet_types[type].speed * time_elapsed);

    return true;
}
void Bullet::Init(Game* game) {
    char path[30];
    SDL_Renderer* renderer = game->GetRenderer();
    for (int i = 0; i < 2; i++) {
        snprintf(path, 30, "assets/bullet%d.png", i);
        SDL_Texture* texture = IMG_LoadTexture(renderer, path);
        Texture hull{texture};
        SDL_QueryTexture(texture, NULL, NULL, &hull.width, &hull.height);
        Bullet::bullet_types.push_back(Bullet::BulletType{hull, 1.5F});
    }
}
void Bullet::Quit() {
    for (int i = 0; i < 2; i++) {
        BulletType& temp = Bullet::bullet_types[i];
        if (temp.texture) SDL_DestroyTexture(temp.texture);
    }
}
