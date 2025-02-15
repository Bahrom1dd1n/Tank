#ifndef __BULLET__
#define __BULLET__

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include <list>
#include <vector>

#include "Object.h"
#include "Texture.h"
class Game;
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
    int life_time = 0;
    short owner_id = 0;  // id of Tank
    short type = 0;
    Bullet(Game* game, const SDL_FPoint& center, const float& angle, const short& owner_id,
           const short& type = 0);

    std::list<Bullet>::const_iterator turn;

   public:
    static std::list<Bullet> bullets;
    static Bullet& Create(Game* game, const SDL_FPoint& center, const float& angle,
                          const short& owner_id, const short& type = 0);

    inline short GetOwner() { return this->owner_id; }
    void Render();
    bool Move();

    inline ~Bullet() { this->points = this->original_points = nullptr; }
};
#endif  // !__BULLET__
