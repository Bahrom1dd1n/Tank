#include "Tank.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include <cstddef>
#include <cstdio>
#include <utility>
#include <vector>

#include "Game.h"
#include "Texture.h"

std::list<Tank> Tank::tanks;
std::vector<Tank::TankType> Tank::tank_types;
Tank::Tank(Game* game, const SDL_FPoint& center, short type) {
    this->game = game;
    this->center = center;
    this->renderer = game->GetRenderer();
    this->fixed = false;
    this->type = type;
    body_rect = {0, 0, float(tank_types[type].body.width), float(tank_types[type].body.height)};
    head_rect = {0, 0, float(tank_types[type].head.width), float(tank_types[type].head.height)};

    body_to_head.x = (body_rect.w - head_rect.w) *
                     0.5f;  // calculatiing disctance from body_rect.x to head_rect.x
    body_to_head.y =
        body_rect.h * 0.5f - head_rect.h * 0.7;  // in order draw turret in the center of body

    head_rotate_point = {head_rect.w * 0.5f, head_rect.h * 0.7f};

    std::vector<SDL_FPoint> edge_points = {{-body_rect.w * 0.5f, -body_rect.h * 0.5f},
                                           {body_rect.w * 0.5f, -body_rect.h * 0.5f},
                                           {body_rect.w * 0.5f, body_rect.h * 0.5f},
                                           {-body_rect.w * 0.5f, body_rect.h * 0.5f}};

    this->num_points = 4;  // if boundary points created with object , its num_points must be
                           // initialized before calling SetPoints()

    Object::SetPoints(4, std::move(edge_points));
}

Tank& Tank::Create(Game* game, const SDL_FPoint& center, short type) {
    tanks.emplace_front(game, center, type);
    tanks.front().turn = tanks.begin();
    return tanks.front();
}
void Tank::RotateTurretToPoint(float x, float y) {
    float px = x - this->center.x;
    float py = this->center.y - y;
    if (py == 0) {
        if (px > 0)
            this->head_angle = 90;
        else
            this->head_angle = -90;
        return;
    }
    const float rad = 180 / M_PI;
    head_angle = atanf(px / py) * rad;
    if (py < 0) head_angle += 180;
}

void Tank::Render() {
    if (!this->InsideScreen()) return;
    auto& frame = game->GetFrame();
    this->body_rect.x = this->center.x - frame.x - body_rect.w * 0.5f;
    this->body_rect.y = this->center.y - frame.y - body_rect.h * 0.5f;

    this->head_rect.x = this->body_rect.x + this->body_to_head.x;
    this->head_rect.y = this->body_rect.y + this->body_to_head.y;
    SDL_RenderCopyExF(this->renderer, tank_types[type].body.texture, NULL, &body_rect, angle, NULL,
                      SDL_FLIP_NONE);
    SDL_RenderCopyExF(this->renderer, tank_types[type].head.texture, NULL, &head_rect, head_angle,
                      &head_rotate_point, SDL_FLIP_NONE);
}

void Tank::Fire() {
    if (fire_time < reload_time) return;

    fire_time = 0;
    const float to_rad = (M_PI / 180);
    float rad = this->head_angle * to_rad;

    SDL_FPoint p = {this->center.x + head_rotate_point.y * sinf(rad),
                    this->center.y - this->head_rotate_point.y * cosf(rad)};
    Bullet::Create(game, p, this->head_angle, id);
}

void Tank::Move() {
    auto& time_elapsed = game->GetTimeElapsed();
    auto& acceleration = tank_types[type].acceleration;
    float drag = acceleration;
    if (accelerating * speed < 0) {
        drag *= 4;
    } else if (accelerating) {
        auto& max_speed = tank_types[type].max_speed;
        if (speed > -max_speed && speed < max_speed) speed += acceleration * accelerating;
        drag *= 0.1;
    }
    char direction = 1;
    if (speed < 0) {
        drag = -drag;
        direction = -1;
    }
    if (rotating) {
        drag *= 2;
        RotateBy(rotating * tank_types[type].ang_speed * time_elapsed);
    }
    speed -= drag;
    if (direction * speed < 0) speed = 0;

    if (speed) MoveForward(speed * time_elapsed);

    if (fire_time <= reload_time) fire_time += time_elapsed;
}
void Tank::Init(Game* game) {
    char path[30];
    SDL_Renderer* renderer = game->GetRenderer();
    for (int i = 0; i < 2; i++) {
        snprintf(path, 30, "assets/hull%d.png", i);
        SDL_Texture* texture = IMG_LoadTexture(renderer, path);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
        Texture hull{texture};
        SDL_QueryTexture(texture, NULL, NULL, &hull.width, &hull.height);
        snprintf(path, 30, "assets/turret%d.png", i);
        texture = IMG_LoadTexture(renderer, path);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
        Texture turret{texture};
        SDL_QueryTexture(texture, NULL, NULL, &turret.width, &turret.height);
        Tank::tank_types.push_back(Tank::TankType{hull, turret, 0.4, 0.0005, 0.05});
    }
}
void Tank::Quit() {
    for (int i = 0; i < 2; i++) {
        TankType& temp = Tank::tank_types[i];
        if (temp.body.texture) SDL_DestroyTexture(temp.body.texture);
        if (temp.head.texture) SDL_DestroyTexture(temp.head.texture);
    }
}
