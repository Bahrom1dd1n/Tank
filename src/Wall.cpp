#include "Wall.h"

#include <cmath>
#include <utility>
#include <vector>

#include "Game.h"
#include "Object.h"
SDL_Color Wall::color = {180, 180, 180, 255};
std::vector<Wall> Wall::walls;

Wall::Wall(Wall&& obj)
    : Object(std::move(obj)),
      vertices(std::move(obj.vertices)),
      vertex_indecies(std::move(obj.vertex_indecies)) {}

Wall::Wall(Game* game, const SDL_FPoint& center) {
    this->game = game;
    this->center = center;
    this->fixed = true;
};
void Wall::UpdateVertices() {
    auto& frame = this->game->GetFrame();
    float x = this->center.x - frame.x;
    float y = this->center.y - frame.y;

    for (int i = 0; i < num_points; i++) {
        this->vertices[i].position.x = this->points[i].x + x;
        this->vertices[i].position.y = this->points[i].y + y;
    }
    this->vertices[this->num_points].position = {x, y};
}

void Wall::InitVertices() {
    this->vertices.resize(this->num_points + 1);
    this->vertex_indecies.resize(3 * this->num_points);

    for (int i = 0, j = 0; i < num_points; i++, j += 3) {
        this->vertices[i] = {this->points[i], this->color, {0, 0}};

        vertex_indecies[j] = num_points;
        vertex_indecies[j + 1] = i;
        vertex_indecies[j + 2] = i + 1;
    }

    this->vertices[this->num_points] = {this->center, {220, 220, 220}, {0, 0}};
    vertex_indecies[3 * num_points - 1] = 0;
}

void Wall::Render() {
    if (!this->InsideScreen()) return;

    this->UpdateVertices();
    SDL_RenderGeometry(this->game->GetRenderer(), NULL, vertices.data(), num_points + 1,
                       vertex_indecies.data(), num_points * 3);
}

Wall::~Wall() {}
