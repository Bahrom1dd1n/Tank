#ifndef __TANK__
#define __TANK__
#include <SDL2/SDL_render.h>

#include <cstddef>
#include <list>
#include <vector>

#include "Bullet.h"
#include "Object.h"
#include "Texture.h"
class Game;
class Tank : public Object {
   public:
    struct TankType {
        Texture body;
        Texture head;
        float max_speed = 0;
        float ang_speed = 0;
    };
    std::list<Tank>::iterator turn;
    static std::vector<TankType> tank_types;
    SDL_Renderer* renderer = nullptr;
    SDL_FPoint
        p_array[4];  // boundary points of polygon when it is rotated (it will created dynamically)
    SDL_FPoint op_array[4];  // boundary points of polygon when it is "not" rotated (it will created
                             // dynamically)
    SDL_FRect body_rect;
    SDL_FRect head_rect;
    char moving = 0;    // if moving > 0 : moving forward, < 0: backwards, 0: not moving
    char rotating = 0;  // if rotating > 0: rotating right , < 0 : rotating left, 0: not ratating
    short id = 0;
    static short last_id;

    float speed = 0.0F;
    float ang_speed =
        0.0F;  // is body of the tank rotating(0: not rotating; >0: roating right ;<0 rotatong left)

    float head_angle;  // angle of turret

    SDL_FPoint body_to_head;       // distance from body_rect to head_rect
    SDL_FPoint head_rotate_point;  // the turret will rotate relative to this point
    // control keys:

    short health = 100;          // hp of player
    unsigned int fire_time = 0;  // time since last fire
    short reload_time = 80;      // time to reload ammo
    short type = 0;
    Tank(Game* game, const SDL_FPoint& center, float ang_speed, short type = 0);

   public:
    static std::list<Tank> tanks;

    static Tank& Create(Game* game, const SDL_FPoint& center, float speed, float ang_speed,
                        const char* body_path, const char* head_path);
    void RotateTurretToPoint(float x, float y);

    void Render();

    void Fire();

    void Move();
    inline short GetID() { return id; }

    ~Tank() { this->points = this->original_points = nullptr; }
    friend class Game;
    friend class Bullet;
};
#endif  //!__TANK__
