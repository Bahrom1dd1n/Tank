#ifndef __WALL__
#define __WALL__

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include <fstream>
#include <vector>

#include "Object.h"
#include "Texture.h"

class Game;

class Wall : public Object {
   private:
    std::vector<SDL_Vertex> vertices;
    std::vector<int> vertex_indecies;

    static SDL_Colour color;

    void InitVertices();

   public:
    static std::vector<Wall> walls;
    /*Wall() = default;*/
    Wall(Wall&& obj);
    Wall(Game* game, const SDL_FPoint& center);
    void UpdateVertices();
    template <typename V>
    void SetPoints(int num_points, V&& boundary_points);

    void Render();
    static void LoadWallsFromFile(const char* file_name);
    ~Wall();
    friend class Game;
};
template <typename V>
void Wall::SetPoints(int num_points, V&& boundary_points) {
    // using universal forwarding , so that , move semantics
    // will be aplied if boundary points is r-value
    static_assert(std::is_same_v<std::decay_t<V>, std::vector<SDL_FPoint>>,
                  "boundary_points is not SDL_FPoint type");

    Object::SetPoints(num_points, std::forward<V>(boundary_points));
    this->InitVertices();
}

#endif  // !__WALL__
