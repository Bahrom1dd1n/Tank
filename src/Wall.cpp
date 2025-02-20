#include "Wall.h"

#include <fstream>
#include <iostream>

#include "Game.h"
const short Wall::data_size = sizeof(SDL_FPoint) + 4 * sizeof(float);
SDL_Color Wall::color = {180, 180, 180, 255};
std::list<Wall> Wall::walls;

void Wall::ReadFromFile(std::ifstream& file, int position) {
    if (position > 0) file.seekg(position);

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

Wall::Wall(Wall&& obj) {
    // Implement move constructor if needed
}

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

Wall::Wall(const SDL_FPoint& center) {
    this->center = center;
    this->fixed = true;
    this->InitVertices();
}

void Wall::Render() {
    if (!this->InsideScreen()) return;

    this->UpdateVertices();
    SDL_RenderGeometry(this->game->GetRenderer(), NULL, vertices, num_points + 1, vertex_indecies,
                       num_points * 3);
}

void Wall::LoadWallsFromFile(const char* file_name) {
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

Wall::~Wall() {
    if (this->vertices) delete[] this->vertices;
    if (this->vertex_indecies) delete[] this->vertex_indecies;
}
