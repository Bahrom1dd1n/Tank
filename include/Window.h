#ifndef __WINDOW__
#define __WINDOW__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <ctime>
#include <functional>
class Window {
   private:
    static int window_number;
    SDL_Renderer* main_renderer = nullptr;  // main renderer of the window
    SDL_Window* window = nullptr;
    unsigned long current_time = 0;  // time passed since window is opened (in milliseconds)
    int time_elapsed = 0;            // time passed since last render of window (in milliseconds)
    int frame_delay = 20;
    int width = 0;   // width of the window
    int height = 0;  // height of the window
    bool running = false;
    unsigned short old_key = 0;
    void (*ondrawframe)(unsigned long current_time, short time_elapsed) = nullptr;
    void (*key_down)(unsigned short key) = nullptr;
    void (*mouse_down)(short mb, int x, int y) = nullptr;
    void (*mouse_move)(short mb, int x, int y) = nullptr;

   public:
    inline Window(const SDL_Rect& frame, const char* window_title,
                  const SDL_WindowFlags& window_flag = SDL_WINDOW_SHOWN) {
        this->window = SDL_CreateWindow(window_title, 0, 20, frame.w, frame.h, window_flag);
        this->main_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!window_number) SDL_InitSubSystem(SDL_INIT_VIDEO);
        window_number++;
    }
    inline SDL_Renderer* GetRenderer() const { return this->main_renderer; }
    inline ~Window() {
        SDL_DestroyRenderer(this->main_renderer);
        SDL_DestroyWindow(this->window);
        window_number--;
        if (!window_number) SDL_Quit();
    }
    inline void Start() {
        this->running = true;
        while (running) {
            auto start = SDL_GetTicks();
            if (this->ondraw_frame) ondraw_frame(this->current_time, time_elapsed);
            this->current_time = SDL_GetTicks();
            this->time_elapsed = this->current_time - start;
            if (this->time_elapsed < this->frame_delay) {
                SDL_Delay(this->frame_delay - this->time_elapsed);
                this->time_elapsed = frame_delay;
            }
        }
    };
    inline void Stop() { this->running = false; }
    inline const int& GetTimeElapsed() const { return this->time_elapsed; };
    inline const unsigned long& GetCurrentTime() { return this->current_time; };
};
inline int Window::window_number = 0;
#endif  //!__WINDOW__
