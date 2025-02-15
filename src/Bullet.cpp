#include "Bullet.h"

#include <cstddef>
#include <ctime>
#include <list>
#include <vector>

#include "Game.h"
#include "Object.h"

inline std::list<Bullet> Bullet::bullets;
Bullet::Bullet(Game* game, const SDL_FPoint& center, const float& angle, const short& owner_id,
               const short& type)
    : owner_id(owner_id) {
    this->game = game;
    this->center = center;
    this->renderer = game->GetRenderer();
    this->rect = {0.0F, 0.0F, float(bullet_types[type].width), float(bullet_types[type].height)};
    this->num_points = 2;
    this->points = p_array;
    this->original_points = op_array;
    this->fixed = false;
    SDL_FPoint boundary_points[2] = {{0.0F, -this->rect.h * 0.5f}, {0.0F, this->rect.h * 0.5f}};
    this->SetPoints(2, boundary_points);
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
