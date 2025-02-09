#ifndef __BULLET__
#define __BULLET__

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include <cstddef>
#include <ctime>
#include <list>
#include <vector>

#include "Object.h"
#include "Texture.h"
#include "game.h"
extern SDL_Renderer* main_renderer;
extern SDL_FRect viewpoint;
extern int time_elapsed;
extern int current_time;
class Bullet : public Object {
   public:
    struct BulletType : Texture {
        float speed = 0;
    };
    static std::vector<BulletType> bullet_types;
    SDL_Renderer* renderer = nullptr;
    SDL_FPoint p_array[2];
    SDL_FPoint op_array[2];
    SDL_FRect rect;
    float speed = 0;
    game& game;
    int life_time = 0;
    short owner_id = 0;  // id of Tank
    short type = 0;
    Bullet(game& game, const SDL_FPoint& center, const float& angle, const short& owner_id, const short& type = 0)
        : owner_id(owner_id), game(game) {
        this->center = center;
        this->renderer = game.GetRenderer();
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
    std::list<Bullet>::const_iterator turn;

   public:
    static std::list<Bullet> bullets;
    inline static Bullet& Create(game& game, const SDL_FPoint& center, const float& angle, const short& owner_id,
                                 const short& type = 0) {
        bullets.emplace_front(game, center, angle, owner_id, type);
        bullets.front().turn = bullets.begin();
        return bullets.front();
    }

    inline void Render() {
        if (!this->InsideScreen()) return;
        const SDL_FRect& frame = game.GetFrame();
        this->rect.x = this->center.x - frame.x - this->rect.w * 0.5f;
        this->rect.y = this->center.y - frame.y - this->rect.h * 0.5f;
        SDL_RenderCopyExF(renderer, bullet_types[type].texture, NULL, &rect, this->angle, NULL, SDL_FLIP_NONE);
        // SDL_RenderCopyExF(main_ren, this->bullet_texture, NULL, &(this->rect), this->angle, NULL, SDL_FLIP_NONE);
    }

    inline bool Move() {
        auto& time_elapsed = this->game.GetTimeElapsed();
        life_time += time_elapsed;
        if (life_time > 2000) {
            Bullet::bullets.erase(turn);
            return false;
        }

        this->MoveForward(bullet_types[type].speed * time_elapsed);

        return true;
    }

    inline short GetOwner() { return this->owner_id; }

    inline ~Bullet() { this->points = this->original_points = nullptr; }
};
inline std::list<Bullet> Bullet::bullets;
#endif  // !__BULLET__
