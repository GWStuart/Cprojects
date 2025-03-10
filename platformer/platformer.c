#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>
#include <math.h>


#define FPS 60

#define WIDTH 800
#define HEIGHT 600

#define PLAYER_SIZE 20
#define ACCELERATION 1
#define FRICTION 0.85
#define JUMP_VEL 14

#define DEFAULT_CAMERA_SPEED 0.1


// struct to hold player info
typedef struct {
    float x;
    float y;
    float xvel;
    float yvel;
} Player;


// struct to hold camera info
// the x and y represent the top left point of the camera
typedef struct {
    float x;
    float y;
    float zoom;
} Camera;


// struct to hold obstacles
typedef struct {
    float x;
    float y;
    float w;
    float h;
} Obstacle;


/*
 * sets the camera's zoom to the given value
 * zooms in towards the centre of the screen
 */
void zoom_camera(Camera* camera, float zoom) {
    float scale_factor = zoom / camera->zoom;
    float offset_factor = (1 - 1/scale_factor) / 2 * 1/camera->zoom;

    // move the camera so that it is centred
    camera->x += offset_factor * WIDTH;
    camera->y += offset_factor * HEIGHT;

    // set the new zoom
    camera->zoom = zoom;
}

/*
 * Focuses the camera on the given x,y position (where x and y are global coordinate values)
 * This is done so that the centre of the camera is on (x,y)
 */
void focus_camera(Camera* camera, float x, float y, float zoom, float camera_speed) {
    // zoom the camera
    float dzoom = zoom - camera->zoom;
    if (dzoom) {
        zoom_camera(camera, camera->zoom + dzoom*camera_speed);
    }

    // calculate zoom adjustments
    float scale_factor = (1 - 1/camera->zoom) / 2;
    x += scale_factor * WIDTH;
    y += scale_factor * HEIGHT;

    // move the camera
    float dx = (x - WIDTH/2) - camera->x;
    float dy = (y - HEIGHT/2) - camera->y;
    camera->x += dx * camera_speed;
    camera->y += dy * camera_speed;
}

/*
 * Returns 1 if there is a collision with an obstacle and 0 otherwise
 * The object with which the player collided with will be stroed in the collision_rect
 *
 * The collision detection is non-inclusive of edges. I.e. if you are just touching it is not a collision
*/
int player_collision(Obstacle* collision_rect, Player* player, Obstacle* obstacles, int obstacle_count) {
    for (int i=0; i<obstacle_count; i++) {
        if (
                player->x + PLAYER_SIZE > obstacles[i].x &&
                player->x < obstacles[i].x + obstacles[i].w &&
                player->y + PLAYER_SIZE > obstacles[i].y &&
                player->y < obstacles[i].y + obstacles[i].h
            ) {

            *collision_rect = obstacles[i];
            return 1;
        }
    }

    return 0;  // no collision detected
}

/*
 * move the player horiztonaly by the specified dx
 */
void move_player(float dx, Obstacle* collision_rect, Player* player, Obstacle* obstacles, int obstacle_count) {
    player->x += dx;
    while (player_collision(collision_rect, player, obstacles, obstacle_count)) {
        player->xvel = 0;
        if (dx > 0) {  // moving right
            player->x = collision_rect->x - PLAYER_SIZE;
        } else {  // moving left
            player->x = collision_rect->x + collision_rect->w;
        }
    }
}

/*
* render the screen in its current state
*/
void render_screen(SDL_Renderer *renderer, Camera* camera, Player* player, Obstacle* obstacles, int obstacle_count) {
    // fill the background
    SDL_SetRenderDrawColor(renderer, 0x12, 0x0c, 0x36, 0xff);
    SDL_RenderClear(renderer);

    // define rect used to draw objects
    SDL_FRect rect;

    // render the obstacles
    SDL_SetRenderDrawColor(renderer, 0xf7, 0xc1, 0x2d, 0xff);
    for (int i=0; i<obstacle_count; i++) {
        Obstacle* obstacle = obstacles + i;  // get the current obstacle
        
        // calculate the render rect
        rect = (SDL_FRect) {
            camera->zoom * (obstacle->x - camera->x), 
            camera->zoom * (obstacle->y - camera->y),
            camera->zoom * obstacle->w,
            camera->zoom * obstacle->h
        };

        SDL_RenderFillRect(renderer, &rect);
    }

    // render the player
    SDL_SetRenderDrawColor(renderer, 0x3c, 0xd6, 0x65, 0xff);
    SDL_FRect player_render = {
        camera->zoom * (player->x - camera->x),
        camera->zoom * (player->y - camera->y),
        camera->zoom * PLAYER_SIZE,
        camera->zoom * PLAYER_SIZE
    };
    SDL_RenderFillRect(renderer, &player_render);
}


int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Platformer", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // array containing the obstacles
    Obstacle obstacles[] = {
        {0, 575, 500, 25},
        {200, 480, 100, 25},
        {425, 500, 25, 75},
        {300, 380, 100, 25},
        {400, 280, 100, 25},
        {300, 180, 100, 25},
    };
    int obstacle_count = sizeof(obstacles) / sizeof(obstacles[0]);

    // define the player
    Player player = {10, 440, 0, 0};
    int allow_motion = 0;

    // initialise the camera
    Camera camera = {0, 0, 1};
    focus_camera(&camera, 250, 350, 0.5, 1);

    float zoom = 1;
    float camera_speed = 0.05;
    int camera_speed_reset = 0;
    float default_camera_speed = DEFAULT_CAMERA_SPEED;

    Uint32 frame_time;  // length of frame in miliseconds
    const Uint32 FRAME_DELAY = 1000 / FPS;  // desired frame time
    
    int mouse_down = 0;  // 1 when mouse is down and 0 otherwise
    
    Obstacle collision_rect;

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

            // mouse wheel scrolls in and out
            else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                if (event.wheel.y >= 0 || zoom > 1) {
                    zoom += event.wheel.y / 2;
                } else {
                    zoom *= 0.75;
                }
            }

            // click and drag mouse to pan
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                mouse_down = 1;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                mouse_down = 0;
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (mouse_down) {
                    // camera_speed = 0.05;
                    camera.x -= event.motion.xrel;
                    camera.y -= event.motion.yrel;
                }
            }
        }

        // get key presses
        if (allow_motion) {
            if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
                player.y ++;  // push player into the ground
                if (player_collision(&collision_rect, &player, obstacles, obstacle_count)) {
                    player.yvel = -JUMP_VEL;  // give an upward velocity
                }
                player.y --;  // take the player back off
            }
            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
                player.xvel = 5;
            }
            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
                player.xvel = -5;
            }
        }

        // Move the player horiztonaly
        move_player(player.xvel, &collision_rect, &player, obstacles, obstacle_count);
        player.xvel *= FRICTION;

        // Move the player vertically
        player.y += player.yvel;
        player.yvel += ACCELERATION;

        while (player_collision(&collision_rect, &player, obstacles, obstacle_count)) {
            // positive yvel means player is falling
            if (player.yvel > 0) {
                player.y = collision_rect.y - PLAYER_SIZE;
            } else {  // negative yvel means the player is falling
                player.y = collision_rect.y + collision_rect.h;
            }
            player.yvel = 0;
        }

        // update the camera position
        if (!mouse_down) {
            focus_camera(&camera, player.x + PLAYER_SIZE/2, player.y + PLAYER_SIZE/2, zoom, camera_speed);
        }

        // camera speed return to default values
        if (camera_speed != default_camera_speed) {
            camera_speed_reset++;
            float n = log(0.01) / log(1 - camera_speed);
            if (camera_speed_reset >= n) {
                camera_speed = default_camera_speed;
                camera_speed_reset = 0;
                allow_motion = 1;
            }
        }

        render_screen(renderer, &camera, &player, obstacles, obstacle_count);
        SDL_RenderPresent(renderer);  // update the screen

        // Cap the FPS
        frame_time = SDL_GetTicks() - frame_time;  // calculate frame duration
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }
}
