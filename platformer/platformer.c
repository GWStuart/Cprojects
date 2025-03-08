#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>


#define FPS 60

#define WIDTH 800
#define HEIGHT 600
#define PLAYER_SIZE 20


// struct to hold player info
typedef struct {
    SDL_FRect* rect;
    double xvel;
    double yvel;
} Player;

/*
* render the screen in its current state
*/
void render_screen(SDL_Renderer *renderer, Player* player, SDL_FRect *obstacles, int obstacle_count) {
    // fill the background
    SDL_SetRenderDrawColor(renderer, 0x12, 0x0c, 0x36, 0xff);
    SDL_RenderClear(renderer);

    // render the obstacles
    SDL_SetRenderDrawColor(renderer, 0xf7, 0xc1, 0x2d, 0xff);
    for (int i=0; i<obstacle_count; i++) {
        SDL_RenderFillRect(renderer, obstacles + i);
    }

    // render the player
    SDL_SetRenderDrawColor(renderer, 0x3c, 0xd6, 0x65, 0xff);
    SDL_RenderFillRect(renderer, player->rect);
}


int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Platformer", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // array containing the obstacles
    SDL_FRect obstacles[] = {
        {0, 575, 500, 25},
        {200, 460, 100, 25}
    };
    int obstacle_count = sizeof(obstacles) / sizeof(obstacles[0]);

    // define the player
    SDL_FRect player_rect = {100, 440, PLAYER_SIZE, PLAYER_SIZE};
    Player player = {&player_rect, 0, 0};

    Uint32 frame_time;  // length of frame in miliseconds
    const Uint32 FRAME_DELAY = 1000 / FPS;  // desired frame time
    
    SDL_Event event;

    int run = 1;
    while (run) {
        frame_time = SDL_GetTicks();  // get the start time

        while (SDL_PollEvent(&event)) {
            // check for a quit event
            if (event.type == SDL_EVENT_QUIT) { run = 0; }

            // check for key presses
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                // q key quits the platformer
                if (event.key.key == SDLK_Q) { run = 0; }
            }
        }

        player_rect.y += player.yvel;
        player.yvel += 1;

        render_screen(renderer, &player, obstacles, obstacle_count);
        SDL_RenderPresent(renderer);  // update the screen

        // Cap the FPS
        frame_time = SDL_GetTicks() - frame_time;  // calculate frame duration
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }
}
