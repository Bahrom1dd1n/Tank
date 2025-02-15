#ifndef __GAME__
#define __GAME__

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include "Texture.h"
class Object;
class Game {
   private:
    SDL_Renderer* main_renderer = nullptr;  // main main_renderer of the window
    SDL_Window* window = nullptr;

    unsigned long current_time = 0;  // time passed since window is opened (in milliseconds)
    int time_elapsed = 0;            // time passed since last render of window (in milliseconds)
    int frame_delay = 20;
    Texture texture;
    // coordinates of top left point of the battle field, and dimensions of
    // visible field.
    SDL_FRect frame = {0.0F, 0.0F, 0.0F, 0.0F};
    SDL_FPoint start_point = {0.0F,
                              0.0F};  // starting position of drawing seamles(continious) texture
    SDL_FPoint previous_point = {0.0F, 0.0F};
    SDL_FPoint target_position = {0.0F, 0.0F};
    Object& target;
    int row = 0, col = 0;  // number of col and row to fill screeen with texture
    bool running = false;

   public:
    bool moving = false;
    float dx = 0.0f, dy = 0.0f;  // speed of movement
    inline SDL_Renderer* GetRenderer() const { return this->main_renderer; }
    inline const SDL_FRect& GetFrame() const { return this->frame; };
    inline const int& GetTimeElapsed() const { return time_elapsed; }
    inline const unsigned long& GetCurrentTime() const { return current_time; }

    Game(int width, int height, Object& target, const SDL_FPoint& target_posiotion);

    void SetBackgroundTexture(const char* texture_path);
    void Render();
    void Move();
    void Start();

    ~Game() {
        if (this->texture.texture) SDL_DestroyTexture(this->texture.texture);
    }
};

#endif  // !__GAME__
