#ifndef __Objects__
#define __Objects__

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>

#include "Big_data_array.h"
#include "Shared_Texture.h"

const int window_width = 1200;
const int window_height = 700;
int time_elapsed;

SDL_FPoint viewpoint = {0.0f, 0.0f};
SDL_Renderer* main_ren;

std::list<Bullet> Bullet::bullets;
Texture Bullet::bullet_texture;  // bullet_texture must be initialized in main !!!
float Bullet::bullet_speed = 1.0f;

class Controller {
   private:
    Tank& target;
    short front = 0;
    short back = 0;
    short right = 0;
    short left = 0;

   public:
    inline Controller(Tank& target, const short front, const short back, const short right, const short left)
        : target(target), front(front), back(back), right(right), left(left) {}
    inline bool OnKeyDown(const short key) {
        if (key == front) {
            target.moving = 1;
            return true;
        }
        if (key == back) {
            target.moving = -1;
            return true;
        }
        if (key == right) {
            target.rotating = 1;
            return true;
        }
        if (key == left) {
            target.rotating = -1;
            return true;
        }
        return false;
    }

    inline bool OnKeyUp(const short key) {
        if (key == front || key == back) {
            target.moving = 0;
            return true;
        }

        if (key == right || key == left) {
            target.rotating = 0;
            return true;
        }
        return false;
    }

    inline Tank& GetTareget() { return target; }
};

class Wall : public Object {
   private:
    SDL_Vertex* vertices = nullptr;
    static SDL_Colour color;
    int* vertex_indecies = nullptr;
    const static short data_size;
    void ReadFromFile(std::ifstream& file, int position = -1) {
        if (position > 0)  // if position given -1 then it will writo to posion where "write poiter" loacated of file
            file.seekg(position);

        // file.read((char*)this, Wall::data_size);

        file.read((char*)&this->center, sizeof(SDL_FPoint));
        file.read((char*)&this->most_right, sizeof(float));
        file.read((char*)&this->most_left, sizeof(float));
        file.read((char*)&this->most_top, sizeof(float));
        file.read((char*)&this->most_bottom, sizeof(float));

        file.read((char*)&num_points, sizeof(int));
        this->points = new SDL_FPoint[num_points];
        file.read((char*)points, num_points * sizeof(SDL_FPoint));
        this->InitVertices();
    }

   public:
    static std::list<Wall> walls;
    Wall() = default;

    Wall(Wall&& obj) {}

    void UpdateVertices() {
        float x = this->center.x - viewpoint.x;
        float y = this->center.y - viewpoint.y;

        for (int i = 0; i < num_points; i++) {
            this->vertices[i].position.x = this->points[i].x + x;
            this->vertices[i].position.y = this->points[i].y + y;
        }
        this->vertices[this->num_points].position = {x, y};
    }

    void InitVertices() {
        this->vertices = new SDL_Vertex[this->num_points + 1];
        this->vertex_indecies = new int[3 * this->num_points];

        for (int i = 0, j = 0; i < num_points; i++, j += 3) {
            this->vertices[i] = {this->points[i], this->color, {0, 0}};

            vertex_indecies[j] = num_points;
            vertex_indecies[j + 1] = i;
            vertex_indecies[j + 2] = i + 1;
        }

        this->vertices[this->num_points] = {this->center, {220, 220, 220}, {0, 0}};
        vertex_indecies[3 * num_points - 1] = 0;
    }

    Wall(const SDL_FPoint& center) {
        this->center = center;
        this->fixed = true;
        this->InitVertices();
    }

    void Render() {
        if (!this->InsideScreen()) return;

        this->UpdateVertices();
        SDL_RenderGeometry(main_ren, NULL, vertices, num_points + 1, vertex_indecies, num_points * 3);

        /*float x = center.x - viewpoint.x;
        float y = center.y - viewpoint.y;

        for (size_t i = this->num_points-1; i >0 ; i--)
        {
            SDL_FPoint& p1 = this->points[i];
            SDL_FPoint& p2 = this->points[i-1];
            SDL_RenderDrawLineF(main_ren, p1.x + x, p1.y + y, p2.x + x, p2.y + y);
        }

        SDL_RenderDrawLineF(main_ren, points[0].x +x , points[0].y+y, points[num_points-1].x +x,
            points[num_points-1].y +y);

        SDL_RenderDrawLineF(main_ren, points[0].x + x, points[0].y + y, x, y);*/
    }

    static void LoadWallsFromFile(const char* file_name) {
        std::ifstream file(file_name, std::ios::binary);
        if (!file.is_open()) {
            std::cout << " File not found!" << std::endl;
            return;
        }
        int num_obj;
        file.read((char*)&num_obj, sizeof(int));

        while (num_obj-- > 0) {
            Wall::walls.emplace_back();
            Wall::walls.back().ReadFromFile(file);
        }
        file.close();
    }

    ~Wall() {
        if (this->vertices) delete[] this->vertices;
        if (this->vertex_indecies) delete[] this->vertex_indecies;
    }
};
const short Wall::data_size = sizeof(SDL_FPoint) + 4 * sizeof(float);
SDL_Color Wall::color = {180, 180, 180, 255};
std::list<Wall> Wall::walls;

class Terrain {
   private:
    SDL_Texture* texture;
    SDL_FRect dstrect;

    Tank& target;
    SDL_FPoint target_position = {0.0F, 0.0F};  // positon of main charrracter inside screen

    float x = 0.0f, y = 0.0f;  // starting position of drawing seamles(continious) texture

    int row = 0, col = 0;  // number of col and row to fill screeen with texture
   public:
    bool moving = false;
    float dx = 0.0f, dy = 0.0f;  // speed of movement

    Terrain(const char* texture_path, int screen_width, int screen_height, Tank& target,
            const SDL_FPoint& target_posiotion)
        : target(target), target_position(target_posiotion) {
        SDL_Surface* surf = IMG_Load(texture_path);
        texture = SDL_CreateTextureFromSurface(main_ren, surf);
        dstrect.w = surf->w;
        dstrect.h = surf->h;
        row = screen_height / surf->h + 2;
        col = screen_width / surf->w + 2;
        SDL_FreeSurface(surf);
    }

    void Render() {
        for (int i = 0; i < col; i++) {
            dstrect.x = x + i * dstrect.w;
            for (int j = 0; j < row; j++) {
                dstrect.y = y + j * dstrect.h;
                SDL_RenderCopyF(main_ren, this->texture, NULL, &dstrect);
            }
        }
    }

    void Move() {
        if (target.moving || target.rotating) {
            if (x > 0)
                x -= dstrect.w;
            else if (x < -dstrect.w)
                x += dstrect.w;

            if (y > 0)
                y -= dstrect.h;
            else if (y < -dstrect.h)
                y += dstrect.h;

            float x1 = viewpoint.x;
            float y1 = viewpoint.y;

            float temp = target.moving * time_elapsed * target.speed;

            viewpoint.x = target.center.x + target.sin_a * temp - target_position.x;
            viewpoint.y = target.center.y - target.cos_a * temp - target_position.y;
            x -= viewpoint.x - x1;
            y -= viewpoint.y - y1;
        }

        this->Render();
    }

    ~Terrain() { SDL_DestroyTexture(this->texture); }
};

void UpdateObjects() {
    auto next = Bullet::bullets.begin();
    while (next != Bullet::bullets.end()) {
        bool hit = false;
        auto it = next;
        next++;

        for (auto it2 = Wall::walls.begin(); it2 != Wall::walls.end(); it2++) {
            if (it->Collision(&(*it2))) {
                Bullet::bullets.erase(it);
                hit = true;
                std::cout << " Hit \n";
                break;
            }
        }
        if (hit) continue;

        for (auto it2 = Tank::tanks.begin(); it2 != Tank::tanks.end(); it2++) {
            if (it2->GetID() == it->GetOwner()) continue;
            if (it->Collision(&(*it2))) {
                Bullet::bullets.erase(it);
                hit = true;
                std::cout << " Hit \n";
                break;
            }
        }

        if (hit) continue;

        it->Move();
    }

    for (auto tank = Tank::tanks.begin(); tank != Tank::tanks.end(); tank++) {
        for (auto wall = Wall::walls.begin(); wall != Wall::walls.end(); wall++) tank->StaticCollision(&(*wall));

        tank->Move();
        tank->Render();
    }

    for (auto wall = Wall::walls.begin(); wall != Wall::walls.end(); wall++) wall->Render();
}
#endif  // !__Objects__
