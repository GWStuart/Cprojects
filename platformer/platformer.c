#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>


#define WIDTH 800
#define HEIGHT 600

/*
* render the screen in its current state
*/
void render_screen(SDL_Renderer *renderer, SDL_FRect *obstacles, int obstacle_count) {
    // fill the background
    SDL_SetRenderDrawColor(renderer, 0x12, 0x0c, 0x36, 0xff);
    SDL_RenderClear(renderer);

    // render the obstacles
    SDL_SetRenderDrawColor(renderer, 0xf7, 0xc1, 0x2d, 0xff);
    for (int i=0; i<obstacle_count; i++) {
        SDL_RenderFillRect(renderer, obstacles + i);
    }
}


int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Platformer", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // array containing the obstacles
    SDL_FRect obstacles[] = {
        {200, 200, 200, 200},
        {500, 500, 20, 20}
    };
    int obstacle_count = sizeof(obstacles) / sizeof(obstacles[0]);

    SDL_Event event;
    int run = 1;
    while (run) {
        while (SDL_PollEvent(&event)) {
            // check for a quit event
            if (event.type == SDL_EVENT_QUIT) { run = 0; }

            // check for key presses
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                // q key quits the platformer
                if (event.key.key == SDLK_Q) { run = 0; }
            }
        }

        render_screen(renderer, obstacles, obstacle_count);
        SDL_RenderPresent(renderer);  // update the screen
    }
}
