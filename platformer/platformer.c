#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>


#define FPS 60

#define WIDTH 800
#define HEIGHT 600

#define PLAYER_SIZE 20
#define ACCELERATION 1


// struct to hold player info
typedef struct {
    SDL_FRect* rect;
    double xvel;
    double yvel;
} Player;


/*
 * Returns 1 if there is a collision with an obstacle and 0 otherwise
 * The object with which the player collided with will be stroed in the collision_rect
 *
 * The collision detection is non-inclusive of edges. I.e. if you are just touching it is not a collision
*/
int player_collision(SDL_FRect* collision_rect, Player* player, SDL_FRect* obstacles, int obstacle_count) {
    for (int i=0; i<obstacle_count; i++) {
        if (player->rect->x + player->rect->w > obstacles[i].x &&
                player->rect->x < obstacles[i].x + obstacles[i].w &&
                player->rect->y + player->rect->h > obstacles[i].y &&
                player->rect->y < obstacles[i].y + obstacles[i].h) {

            *collision_rect = obstacles[i];
            return 1;
        }
    }

    return 0;  // no collision detected
}

/*
 * move the player horiztonaly by the specified dx
 */
void move_player(int dx, SDL_FRect* collision_rect, Player* player, SDL_FRect* obstacles, int obstacle_count) {
    player->rect->x += dx;
    while (player_collision(collision_rect, player, obstacles, obstacle_count)) {
        if (dx > 0) {  // moving right
            player->rect->x = collision_rect->x - PLAYER_SIZE;
        } else {  // moving left
            player->rect->x = collision_rect->x + collision_rect->w;
        }
    }
}

/*
* render the screen in its current state
*/
void render_screen(SDL_Renderer *renderer, Player* player, SDL_FRect* obstacles, int obstacle_count) {
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
        {200, 460, 100, 25},
        {425, 500, 25, 75},
        {300, 360, 100, 25},
        {400, 260, 100, 25},
        {300, 160, 100, 25},
    };
    int obstacle_count = sizeof(obstacles) / sizeof(obstacles[0]);

    // define the player
    SDL_FRect player_rect = {100, 440, PLAYER_SIZE, PLAYER_SIZE};
    Player player = {&player_rect, 0, 0};
    SDL_FRect collision_rect;

    Uint32 frame_time;  // length of frame in miliseconds
    const Uint32 FRAME_DELAY = 1000 / FPS;  // desired frame time
    
    const bool* keys = SDL_GetKeyboardState(NULL);  // get key presses
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

        // get key presses
        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
            player_rect.y ++;  // push player into the ground
            if (player_collision(&collision_rect, &player, obstacles, obstacle_count)) {
                player.yvel = -15;  // give an upward velocity
            }
            player_rect.y --;  // take the player back off
        }
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
            move_player(5, &collision_rect, &player, obstacles, obstacle_count);
        }
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
            move_player(-5, &collision_rect, &player, obstacles, obstacle_count);
        }

        player_rect.y += player.yvel;
        player.yvel += ACCELERATION;

        while (player_collision(&collision_rect, &player, obstacles, obstacle_count)) {
            // positive yvel means player is falling
            if (player.yvel > 0) {
                player_rect.y = collision_rect.y - PLAYER_SIZE;
            } else {  // negative yvel means the player is falling
                player_rect.y = collision_rect.y + collision_rect.h;
            }
            player.yvel = 0;
        }

        render_screen(renderer, &player, obstacles, obstacle_count);
        SDL_RenderPresent(renderer);  // update the screen

        // Cap the FPS
        frame_time = SDL_GetTicks() - frame_time;  // calculate frame duration
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }
}
