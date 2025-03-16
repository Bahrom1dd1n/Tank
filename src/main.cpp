#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <cmath>

#include "Bullet.h"
#include "Game.h"
#include "Object.h"
#include "Tank.h"
int main(int argc, char* argv[]) {
    Game game(1200, 700, SDL_WINDOW_FULLSCREEN);
    game.LoadMap("map");
    Tank& tank = Tank::Create(&game, {0, 0}, 0);
    game.SetTarget(&tank, {game.GetFrame().w / 2, game.GetFrame().h / 2});
    game.SetBackgroundTexture("assets/sand.png");
    tank.RotateBy(40);
    game.Start();
    return 0;
}
