#ifndef __WALL__
#define __WALL__

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include <fstream>
#include <list>
#include <vector>

#include "Object.h"
#include "Texture.h"

class Game;

class Wall : public Object {
   private:
    SDL_Vertex* vertices = nullptr;
    static SDL_Colour color;
    int* vertex_indecies = nullptr;
    const static short data_size;
    void ReadFromFile(std::ifstream& file, int position = -1);
    Game* game = nullptr;

   public:
    static std::list<Wall> walls;
    Wall() = default;
    Wall(Wall&& obj);
    void UpdateVertices();
    void InitVertices();
    Wall(const SDL_FPoint& center);
    void Render();
    static void LoadWallsFromFile(const char* file_name);
    ~Wall();
};

#endif  // !__WALL__
