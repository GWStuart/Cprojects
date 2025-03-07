#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define WIDTH 610
#define HEIGHT 610
#define HD_RENDER_ITERATIONS 99999
#define LD_RENDER_ITERATIONS 10000

// define some global variables
int zoom = 55;
int fern_x = WIDTH / 2;
int fern_y = HEIGHT;

/*
* render the fern
*/
void render_fern(SDL_Renderer* renderer, int iterations, int clear_first) {
    // fill the background
    if (clear_first) {
        SDL_SetRenderDrawColor(renderer, 0x12, 0x0c, 0x36, 0xff);
        SDL_RenderClear(renderer);
    }

    int num;
    double point[] = {0, 0};

    SDL_SetRenderDrawColor(renderer, 0, 0xff, 0, 0xff);

    for (int i=0; i<iterations; i++) {
        SDL_RenderPoint(renderer, point[0] * zoom + fern_x, point[1] * -zoom + fern_y);
        num = rand() % 100;
        
        if (num == 1) {
            point[0] = 0;
            point[1] *= 0.16;
        } else if (num <= 86) {
            point[0] = 0.85*point[0] + 0.04*point[1];
            point[1] = -0.04*point[0] + 0.85*point[1] + 1.6;
        } else if (num <= 93) {
            point[0] = 0.2*point[0] - 0.26*point[1];
            point[1] = 0.23*point[0] + 0.22*point[1] + 1.6;
        } else {
            point[0] = -0.15*point[0] + 0.28*point[1];
            point[1] = 0.26*point[0] + 0.24*point[1] + 0.44;
        }

        // SDL_RenderPresent(renderer);  // update the screen
    }

    SDL_RenderPresent(renderer);  // update the screen
}


int main() {
    srand(time(NULL));  // initialise the random numbers

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Barnsley Fern", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // render the background
    SDL_SetRenderDrawColor(renderer, 0x12, 0x0c, 0x36, 0xff);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);  // update the screen

    // draw the fern
    render_fern(renderer, HD_RENDER_ITERATIONS, 1);


    SDL_RenderPresent(renderer);  // update the screen

    SDL_Event event;
    int mouse_down = 0;
    int inactivity = 0;
    int run = 1;
    while (run) {
        // run the event loop
        while (SDL_PollEvent(&event)) {
            // check for window close event
            if (event.type == SDL_EVENT_QUIT) { run = 0; }

            // check for a keypress event
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_Q) { run = 0; }
            }

            // check for mouse wheel events
            else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                int scroll = event.wheel.y;
                zoom += scroll;

                render_fern(renderer, LD_RENDER_ITERATIONS, 1);
                inactivity = 100000;
            }

            // check for mouse clicks
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                mouse_down = 1;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                mouse_down = 0;
            }

            // check for mouse movement
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (mouse_down) {
                    int xrel = event.motion.xrel;
                    int yrel = event.motion.yrel;
                    fern_x += xrel;
                    fern_y += yrel;

                    render_fern(renderer, LD_RENDER_ITERATIONS, 1);
                    inactivity = 100000;
                }
            }
        }

        if (inactivity > 0) {
            if (inactivity == 1) {
                render_fern(renderer, HD_RENDER_ITERATIONS, 0);
            }
            inactivity --;
        }
    }

    return 0;
}
