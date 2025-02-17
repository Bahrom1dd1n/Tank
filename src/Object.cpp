#include "Object.h"

#include <SDL2/SDL_rect.h>

#include "Game.h"

Object::Object(Game* game, SDL_FPoint center, int num_points, SDL_FPoint* boundary_points)
    : game(game) {
    this->center = center;
    this->num_points = num_points;
    this->points = new SDL_FPoint[num_points];

    float m_r = -FLT_MAX, m_l = FLT_MAX;
    float m_b = -FLT_MAX, m_t = FLT_MAX;

    for (int i = 0; i < num_points; i++) {
        SDL_FPoint& p = boundary_points[i];
        this->points[i] = p;  // copiing coordinates of all points

        m_r = std::fmaxf(p.x, m_r);
        m_l = std::fminf(p.x, m_l);
        m_b = std::fmaxf(p.y, m_b);
        m_t = std::fminf(p.y, m_t);
    }

    this->most_right = m_r;
    this->most_left = m_l;
    this->most_top = m_t;
    this->most_bottom = m_b;
}

bool Object::Collision(Object* object) {
    {
        /*
         * Checking for rectangular collision , if their rectangles not collide , they are indeed
         * not intersecting
         */

        if (this->most_left + this->center.x > object->center.x + object->most_right) return false;
        if (this->most_right + this->center.x < object->center.x + object->most_left) return false;

        if (this->most_bottom + this->center.y < object->most_top + object->center.y) return false;
        if (this->most_top + this->center.y > object->most_bottom + object->center.y) return false;
    }

    auto overlap = [](Object* obj_1, Object* obj_2) {
        int& num1 = obj_1->num_points;
        int& num2 = obj_2->num_points;

        SDL_FPoint* points1 = obj_1->points;
        SDL_FPoint* points2 = obj_2->points;

        float& x1 = obj_1->center.x;
        float& y1 = obj_1->center.y;

        float& x2 = obj_2->center.x;
        float& y2 = obj_2->center.y;

        for (int i = 0; i < num1; i++) {
            int j = (i + 1) % num1;
            SDL_FPoint axis = {points1[i].y - points1[j].y,
                               points1[j].x - points1[i].x};  //////////////////////

            float left1 = INFINITY, right1 = -INFINITY;

            float d = 0.0f;
            for (j = 0; j < num1; j++) {
                SDL_FPoint p = {points1[j].x + x1, points1[j].y + y1};

                d = p.x * axis.x + p.y * axis.y;

                if (d < left1) left1 = d;

                if (d > right1) right1 = d;
            }

            d = (points2[0].x + x2) * axis.x + (points2[0].y + y2) * axis.y;
            if (left1 <= d && d <= right1) continue;

            bool overlap = false;
            bool sign = (d >= left1);

            for (j = 1; j < num2; j++) {
                d = (points2[j].x + x2) * axis.x + (points2[j].y + y2) * axis.y;
                if ((d >= left1) != sign) {
                    overlap = true;  // there is overlapping
                    break;
                }
                if (left1 <= d && d <= right1) {
                    overlap = true;  // there is overlapping
                    break;
                }
            }

            if (!overlap) return false;  // found seperating points2
        }

        return true;  // could not find seperating line for obj_1
    };

    if (!overlap(this, object)) return false;

    return overlap(object, this);
}

bool Object::StaticCollision(Object* object) {
    {
        /*
         * Checking for rectangular collision , if their rectangles not collide , they are indeed
         * not intersecting
         */

        if (this->most_left + this->center.x > object->center.x + object->most_right) return false;
        if (this->most_right + this->center.x < object->center.x + object->most_left) return false;

        if (this->most_bottom + this->center.y < object->most_top + object->center.y) return false;
        if (this->most_top + this->center.y > object->most_bottom + object->center.y) return false;
    }

    auto push = [](Object* poly1, Object* poly2) {
        SDL_FPoint& p1 = poly1->center;
        SDL_FPoint& c2 = poly2->center;
        int& num1 = poly1->num_points;
        int& num2 = poly2->num_points;
        auto& points1 = poly1->points;
        auto& points2 = poly2->points;
        for (int p = 0; p < num1; p++) {
            SDL_FPoint p2 = points1[p];
            p2.x += p1.x;
            p2.y += p1.y;

            SDL_FPoint displacement = {0, 0};

            float dx1 = p2.x - p1.x;
            float dy1 = p2.y - p1.y;

            // ...against edges of this polygon
            for (int q = 0; q < num2; q++) {
                SDL_FPoint p3 = points2[q];
                p3.x += c2.x;
                p3.y += c2.y;

                SDL_FPoint p4 = points2[(q + 1) % num2];
                p4.x += c2.x;
                p4.y += c2.y;

                // Standard "off the shelf" line segment intersection

                float dx2 = p4.x - p3.x;
                float dy2 = p4.y - p3.y;

                float h = dx1 * dy2 - dx2 * dy1;

                float t1 = (dy2 * (p3.x - p1.x) - dx2 * (p3.y - p1.y)) / h;
                float t2 = (dy1 * (p3.x - p1.x) - dx1 * (p3.y - p1.y)) / h;

                if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f) {
                    displacement.x = (1.0f - t1) * dx1;
                    displacement.y = (1.0f - t1) * dy1;

                    if (poly1->fixed) {
                        poly2->center.x += displacement.x;
                        poly2->center.y += displacement.y;
                        return true;
                    }

                    if (poly2->fixed) {
                        poly1->center.x -= displacement.x;
                        poly1->center.y -= displacement.y;
                        return true;
                    }

                    displacement.x *= 0.5f;
                    displacement.y *= 0.5f;

                    poly1->center.x -= displacement.x;
                    poly1->center.y -= displacement.y;
                    poly2->center.x += displacement.x;
                    poly2->center.y += displacement.y;
                    return true;
                }
            }
        }

        return false;
    };

    if (push(this, object)) return true;

    return push(object, this);
}
bool Object::InsideScreen() {
    SDL_FRect frame = game->GetFrame();
    if (this->most_right + this->center.x < frame.x ||
        this->most_left + this->center.x > frame.x + frame.w)
        return false;

    if (this->most_bottom + this->center.y < frame.y ||
        this->most_top + this->center.y > frame.y + frame.h)
        return false;

    return true;
}

void Object::SetPoints(int num_points, SDL_FPoint boundary_points[]) {
    if (this->points) delete[] this->points;
    int num = num_points;
    if (!this->fixed) num *= 2;
    this->points = new SDL_FPoint[num];
    float m_r = -FLT_MAX, m_l = FLT_MAX;
    float m_b = -FLT_MAX, m_t = FLT_MAX;

    for (int i = 0; i < num_points; i++) {
        SDL_FPoint& p = boundary_points[i];
        this->points[i] = p;  // copiing coordinates of all points
        // if Object is not fixed origina  points will be stored at the end of the points array
        if (!fixed) points[num_points + i] = p;
        // identifying edges of object
        m_r = std::fmaxf(p.x, m_r);
        m_l = std::fminf(p.x, m_l);
        m_b = std::fmaxf(p.y, m_b);
        m_t = std::fminf(p.y, m_t);
    }

    this->most_right = m_r;
    this->most_left = m_l;
    this->most_top = m_t;
    this->most_bottom = m_b;
}
void Object::RotateBy(float da) {
    if (!da || !fixed) return;

    this->angle += da;
    float rad = angle * M_PI / 180;
    cos_a = cosf(rad);
    sin_a = sinf(rad);

    float m_r = -FLT_MAX, m_l = FLT_MAX;
    float m_b = -FLT_MAX, m_t = FLT_MAX;

    for (int i = 0; i < num_points; i++) {
        SDL_FPoint& original = points[i + num_points];
        SDL_FPoint& point = points[i];

        point = {original.x * cos_a - original.y * sin_a, original.x * sin_a + original.y * cos_a};

        if (point.x > m_r) m_r = point.x;
        if (point.x < m_l) m_l = point.x;

        if (point.y > m_b) m_b = point.y;
        if (point.y < m_t) m_t = point.y;
    }

    this->most_right = m_r;
    this->most_left = m_l;
    this->most_top = m_t;
    this->most_bottom = m_b;
}
