#include "Tank.h"

#include "Game.h"
Tank::Tank(Game* game, const SDL_FPoint& center, float ang_speed, short type)
    : ang_speed(ang_speed) {
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

    SDL_FPoint edge_points[4] = {{-body_rect.w * 0.5f, -body_rect.h * 0.5f},
                                 {body_rect.w * 0.5f, -body_rect.h * 0.5f},
                                 {body_rect.w * 0.5f, body_rect.h * 0.5f},
                                 {-body_rect.w * 0.5f, body_rect.h * 0.5f}};

    this->points = p_array;
    this->original_points = op_array;
    this->num_points = 4;  // if boundary points created with object , its num_points must be
                           // initialized before calling SetPoints()

    this->SetPoints(4, edge_points);
}

Tank& Tank::Create(Game* game, const SDL_FPoint& center, float speed, float ang_speed,
                   const char* body_path, const char* head_path) {
    tanks.emplace_front(game, center, speed, ang_speed, body_path, head_path);
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
    SDL_RenderCopyExF(this->renderer, tank_types[type].head.texture, NULL, &head_rect, angle,
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
    if (moving) MoveForward(moving * speed * time_elapsed);

    if (rotating) RotateBy(rotating * ang_speed * time_elapsed);

    if (fire_time <= reload_time) fire_time += time_elapsed;
}
