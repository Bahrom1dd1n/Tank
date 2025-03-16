#ifndef __OBJECT__
#define __OBJECT__

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include <vector>
class Game;
class Object {
   public:
    SDL_FPoint center = {0, 0};  // coordinates of object in the play zone

    float most_right = 0.0F;   // distance on x axis to farest point on the right
    float most_left = 0.0F;    // distance on x axis to farest point in the left , always negative
    float most_top = 0.0F;     // distance on y axis to farest point on the top , always negative
    float most_bottom = 0.0F;  // distance on y axis to farest point on the bottom

    /* boundary points of polygon
     if Object is not fixed , it's original points are stored at the end of `points` array ,
     thats starting at points[num_points]
     original points are boundary points of polygon when it is not rotated*/
    int num_points = 0;
    std::vector<SDL_FPoint> points;
    double angle = 0;  // rotated angle
    float cos_a = 1;
    float sin_a = 0;
    // float radious = 0;// distance to farest points2 from center
    bool fixed = true;  // if fixed then original_points = points : object can not be rotated
    Game* game;
    Object() {};

   public:
    Object(Object&& obj);

    template <typename V>
    Object(Game* game, SDL_FPoint center, int num_points, V&& boundary_points, bool fixed = false);
    template <typename V>
    void SetPoints(int num_points, V&& boundary_points);

    bool Collision(Object* object);
    bool StaticCollision(Object* object);
    bool InsideScreen();
    void RotateBy(float da);

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
    inline ~Object() {}
};
template <typename V>
inline Object::Object(Game* game, SDL_FPoint center, int num_points, V&& boundary_points,
                      bool fixed)
    : game(game) {
    // using universal forwarding , so that , move semantics
    // will be aplied if boundary points is r-value
    static_assert(std::is_same_v<std::decay_t<V>, std::vector<SDL_FPoint>>,
                  "boundary_points is not SDL_FPoint type");

    this->center = center;
    this->SetPoints(num_points, std::forward<V>(boundary_points));
}

template <typename V>
inline void Object::SetPoints(int num_points, V&& boundary_points) {
    // using universal forwarding , so that , move semantics
    // will be aplied if boundary points is r-value
    static_assert(std::is_same_v<std::decay_t<V>, std::vector<SDL_FPoint>>,
                  "boundary_points is not SDL_FPoint type");
    this->num_points = num_points;
    this->points = std::forward<V>(boundary_points);
    // if Object is not fixed origina  points will be stored at the end of the points array
    if (!fixed) {
        this->points.resize(num_points * 2);
        for (int i = 0; i < num_points; i++) points[num_points + i] = points[i];
    }

    float m_r = -FLT_MAX, m_l = FLT_MAX;
    float m_b = -FLT_MAX, m_t = FLT_MAX;
    for (int i = 0; i < num_points; i++) {
        SDL_FPoint& p = points[i];
        // identifying edges of object
        if (m_r < p.x) m_r = p.x;
        if (m_l > p.x) m_l = p.x;
        if (m_b < p.y) m_b = p.y;
        if (m_t > p.y) m_t = p.y;
    }

    this->most_right = m_r;
    this->most_left = m_l;
    this->most_top = m_t;
    this->most_bottom = m_b;
}
#endif  // !__OBJECT__
