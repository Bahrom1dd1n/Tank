#ifndef __FIELD__
#define __FIELD__

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include "Object.h"
#include "Texture.h"
#include "Window.h"
class Game {
   private:
    SDL_Renderer* main_renderer = nullptr;  // main renderer of the window
    SDL_Window* window = nullptr;

    unsigned long current_time = 0;  // time passed since window is opened (in milliseconds)
    int time_elapsed = 0;            // time passed since last render of window (in milliseconds)
    int frame_delay = 20;
    Texture texture;
    // coordinates of top left point of the battle field, and dimensions of visible field.
    SDL_FRect frame = {0.0F, 0.0F, 0.0F, 0.0F};
    SDL_FPoint start_point = {0.0F, 0.0F};  // starting position of drawing seamles(continious) texture
    SDL_FPoint previous_point = {0.0F, 0.0F};
    SDL_FPoint target_position = {0.0F, 0.0F};
    Object& target;
    int row = 0, col = 0;  // number of col and row to fill screeen with texture
    bool running = false;

   public:
    bool moving = false;
    float dx = 0.0f, dy = 0.0f;  // speed of movement
    SDL_Renderer* GetRenderer() const { return this->renderer; }
    inline const SDL_FRect& GetFrame() const { return this->frame; };

    Game(int width, int height, Object& target, const SDL_FPoint& target_posiotion) : target(target) {
        this->window = SDL_CreateWindow("Tank", 0, 20, frame.w, frame.h, window_flag);
        this->main_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }
    void SetBackgroundTexture(const char* texture_path) {
        if (!this->renderer) return;
        SDL_Surface* surf = IMG_Load(texture_path);
        this->texture = {SDL_CreateTextureFromSurface(renderer, surf), surf->w, surf->h};
        row = height / surf->h + 2;
        col = width / surf->w + 2;
        SDL_FreeSurface(surf);
    }
    void Render() {
        SDL_FRect dstrect;
        for (int i = 0; i < col; i++) {
            dstrect.x = start_point.x + i * dstrect.w;
            for (int j = 0; j < row; j++) {
                dstrect.y = start_point.y + j * dstrect.h;
                SDL_RenderCopyF(this->renderer, this->texture.texture, NULL, &dstrect);
            }
        }
    }

    void Move() {
        float dx = target.center.x - this->previous_point.x;
        float dy = target.center.y - this->previous_point.y;
        this->previous_point = target.center;
        if (dx || dy) {
            float x1 = frame.x;
            float y1 = frame.y;

            frame.x = target.center.x - target_position.x;
            frame.y = target.center.y - target_position.y;
            start_point.x -= frame.x - x1;
            start_point.y -= frame.y - y1;

            if (start_point.x > 0)
                start_point.x -= texture.width;
            else if (start_point.x < -texture.height)
                start_point.x += texture.width;

            if (start_point.y > 0)
                start_point.y -= texture.height;
            else if (start_point.y < -texture.height)
                start_point.y += texture.height;
        }
    }
    void Start() {
        this->running = true;
        while (running) {
            auto start = SDL_GetTicks();
            if (this->ondraw_frame) ondraw_frame(this->current_time, time_elapsed);
            this->time_elapsed = this->current_time - start;
            if (this->time_elapsed < this->frame_delay) {
                SDL_Delay(this->frame_delay - this->time_elapsed);
                this->time_elapsed = frame_delay;
            }
        }
    };

} inline const int& GetTimeElapsed() const {
    return time_elapsed;
}
inline const unsigned long& GetCurrentTime() const {
    return current_time;
}
~Game() {
    SDL_DestroyTexture(this->texture.texture);
}
}
;

#endif  // !__FIELD__
