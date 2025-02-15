#ifndef __OBJECT__
#define __OBJECT__

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
class Game;
class Object {
   public:
    SDL_FPoint center = {0, 0};  // coordinates of object in the play zone

    float most_right = 0.0F;   // distance on x axis to farest point on the right
    float most_left = 0.0F;    // distance on x axis to farest point in the left , always negative
    float most_top = 0.0F;     // distance on y axis to farest point on the top , always negative
    float most_bottom = 0.0F;  // distance on y axis to farest point on the bottom
    int num_points = 0;        // number of boundary points

    //---------------------------------------- order should not be chabged

    SDL_FPoint* original_points = nullptr;  // boundary points of polygon when it is not rotated
    SDL_FPoint* points = nullptr;           // boundary points of polygon
    double angle = 0;                       // rotated angle
    float cos_a = 1;
    float sin_a = 0;
    // float radious = 0;// distance to farest points2 from center
    bool fixed = true;  // if fixed then original_points = points : object can not be rotated
    Game* game;
    Object() {};

   public:
    Object(Game* game, SDL_FPoint center, int num_points, SDL_FPoint* boundary_points);

    bool Collision(Object* object);
    bool StaticCollision(Object* object);
    bool InsideScreen();
    void RotateBy(float da);
    void SetPoints(int num_points, SDL_FPoint boundary_points[]);

    inline double GetAngle() const { return this->angle; }
    inline double GetSineAngle() const { return this->sin_a; }
    inline double GetCosineAngle() const { return this->cos_a; }
    inline void MoveBy(float dx, float dy) {
        this->center.x += dx;
        this->center.y += dy;
    }
    inline void MoveForward(float ds) {
        if (!ds) return;
        this->center.x += ds * this->sin_a;
        this->center.y -= ds * this->cos_a;
    }
    inline ~Object() {
        // if boundary points created dynamicaly its points dhold be delted , else if boundary
        // points created with object its points points should be assigned to nullptr!!!!
        if (this->points) delete[] this->points;
        this->points = nullptr;
        if (!this->fixed && original_points) delete[] original_points;
        this->original_points = nullptr;
    }
};
#endif  // !__OBJECT__
