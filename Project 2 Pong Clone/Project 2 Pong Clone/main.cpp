#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

/**
* Author: Matthew Nunez
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 2.0f;

constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
// yellow is amber ufo
constexpr char AMBER_SPRITE_FILEPATH[] = "../assets/amber_ufo.png",
// pink is blue ufo
BLUE_SPRITE_FILEPATH[] = "../assets/blue_ufo.png",
GAME_OVER_SPRITE_FILEPATH[] = "../assets/game_over.gif",
BALL_SPRITE_FILEPATH[] = "../assets/meteor.png",
BACKGROUND_FILEPATH[] = "../assets/background.png",
BLUE_WINNER_FILEPATH[] = "../assets/blue_wins.png",
AMBER_WINNER_FILEPATH[] = "../assets/red_wins.png",
ROTATING_BACKGROUND_FILEPATH[] = "../assets/rotating_background.jpg";



constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr glm::vec3 INIT_SCALE_AMBER = glm::vec3(1.0f, 1.0f, 0.0f),
INIT_POS_AMBER = glm::vec3(-3.0f, 0.0f, 0.0f),
INIT_SCALE_BLUE = glm::vec3(1.0f, 1.0f, 1.0f),
INIT_POS_BLUE = glm::vec3(3.0f, 0.0f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_blue_matrix, g_projection_matrix, g_amber_matrix, g_game_over_matrix, bg_matrix;

float g_previous_ticks = 0.0f;

GLuint g_blue_texture_id;
GLuint g_amber_texture_id;
GLuint g_ball_texture_id;
GLuint g_gameOver_texture_id;
GLuint g_background_texture_id;
GLuint g_blue_winner_texture_id;
GLuint g_amber_winner_texture_id;
GLuint g_rotating_background_texture_id;


constexpr float MOVEMENT_SPEED = 3.0f; // speed of drop


// ball and ufo dimensions
constexpr float ballHalfWidth = 0.1f;
constexpr float ballHalfHeight = 0.1f;
constexpr float ufoHalfWidth = 0.25f;
constexpr float ufoHalfHeight = 0.25f;

// computed the background boundaries:
float bgHalfWidth = 3.5f;
float bgHalfHeight = 2.5f;
float bottomLimit = -3.75 + ufoHalfHeight;
float topLimit = 3.75 - ufoHalfHeight;

// game state variables
bool g_one_player_mode = false;  // false means 2-player mode by default
float g_blue_up_down_direction = 1.0f; // 1.0 means moving upward, -1.0 means downward
bool g_gameOver = false;
bool g_winner; // 0 is amber, 1 is blue
float g_gameOverTime;
int g_active_ball_count = 1; // default 1 ball


// ufo positions and movements
glm::vec3 g_blue_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_blue_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_amber_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_amber_movement = glm::vec3(0.0f, 0.0f, 0.0f);

// ball positions, velocities, and ball matrices
glm::vec3 g_ball_position_1 = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_position_2 = glm::vec3(-1.0f, 1.0f, 0.0f);
glm::vec3 g_ball_position_3 = glm::vec3(1.0f, -1.0f, 0.0f);

glm::vec3 g_ball_velocity_1 = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 g_ball_velocity_2 = glm::vec3(-1.0f, -1.0f, 0.0f);
glm::vec3 g_ball_velocity_3 = glm::vec3(1.0f, -1.0f, 0.0f);

glm::mat4 g_ball_matrix_1 = glm::mat4(1.0f);
glm::mat4 g_ball_matrix_2 = glm::mat4(1.0f);
glm::mat4 g_ball_matrix_3 = glm::mat4(1.0f);

glm::mat4 g_rotating_bg_matrix = glm::mat4(1.0f); // rotating background matrix
float g_rotation_angle = 0.0f; // rotation angle in radians


// compute the actual center positions for the paddles:
glm::vec3 g_blue_center = INIT_POS_BLUE + g_blue_position;
glm::vec3 g_amber_center = INIT_POS_AMBER + g_amber_position;


glm::vec3 g_amber_scale = glm::vec3(0.0f, 0.0f, 0.0f);  // scale trigger vector
glm::vec3 g_amber_size = glm::vec3(1.0f, 1.0f, 0.0f);  // scale accumulator vector


void initialise();
void process_input();
void update();
void render();
void shutdown();
void update_ball(glm::vec3& ball_position, glm::vec3& ball_velocity, glm::mat4& ball_matrix, float delta_time);
void clamp_ball(glm::vec3& ball_position, glm::vec3& ball_velocity);
void handle_collision(glm::vec3& ball_position, glm::vec3& ball_velocity, glm::vec3 paddle_position);
void update_ball(glm::vec3& ball_position, glm::vec3& ball_velocity, glm::mat4& ball_matrix, float delta_time);

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Pong Clone!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);


    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

	// setting up the matrices
	g_game_over_matrix = glm::mat4(1.0f);
    g_blue_matrix = glm::mat4(1.0f);
    g_amber_matrix = glm::mat4(1.0f);
    g_amber_matrix = glm::translate(g_amber_matrix, glm::vec3(1.0f, 1.0f, 0.0f));

	// setting up the background matrix
    g_amber_position += g_amber_movement;
    g_blue_position += g_blue_movement;

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
	// loading textures
    g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);
    g_blue_texture_id = load_texture(BLUE_SPRITE_FILEPATH);
    g_amber_texture_id = load_texture(AMBER_SPRITE_FILEPATH);
    g_gameOver_texture_id = load_texture(GAME_OVER_SPRITE_FILEPATH);
    g_background_texture_id = load_texture(BACKGROUND_FILEPATH);
    g_blue_winner_texture_id = load_texture(BLUE_WINNER_FILEPATH);
    g_amber_winner_texture_id = load_texture(AMBER_WINNER_FILEPATH);
    g_rotating_background_texture_id = load_texture(ROTATING_BACKGROUND_FILEPATH);



    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_amber_movement.x = 0.0f;
    g_blue_movement.x = 0.0f;
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q:
                // Quit the game with a keystroke
                g_app_status = TERMINATED;
                break;

            case SDLK_t:
                g_one_player_mode = !g_one_player_mode;  // change mode
                // reset user-controlled movement for blue ufo:
                g_blue_movement.y = 0.0f;
                // reset if entering one-player mode:
                if (g_one_player_mode) {
                    g_blue_up_down_direction = 1.0f;
                }
                break;

            case SDLK_1:
                g_active_ball_count = 1;
                break;

            case SDLK_2:
                g_active_ball_count = 2;
                break;

            case SDLK_3:
                g_active_ball_count = 3;
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_W])
    {
        g_amber_movement.y = 1.0f;
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        g_amber_movement.y = -1.0f;
    }
    
    if (key_state[SDL_SCANCODE_UP])
    {
        g_blue_movement.y = 1.0f;
	}
	else if (key_state[SDL_SCANCODE_DOWN])
	{
        g_blue_movement.y = -1.0f;
	}

    if (!g_one_player_mode) {
		// in two-player mode, checking user input for blue ufo movement:
        if (key_state[SDL_SCANCODE_UP]) {
            g_blue_movement.y = 1.0f;
        }
        else if (key_state[SDL_SCANCODE_DOWN]) {
            g_blue_movement.y = -1.0f;
        }
    }
    else {
        // in one-player mode, ignore user input for ufo:
        g_blue_movement.y = 0.0f;
    }
}

// update the ball position
void update_ball(glm::vec3& ball_position, glm::vec3& ball_velocity, glm::mat4& ball_matrix, float delta_time) {
    ball_position += ball_velocity * 0.67f * MOVEMENT_SPEED * delta_time;
	// check if the ball is out of bounds
    if (ball_position.x > bgHalfWidth - ballHalfWidth || ball_position.x < -bgHalfWidth + ballHalfWidth)
        ball_velocity.x = -ball_velocity.x;
    if (ball_position.y > bgHalfHeight - ballHalfHeight || ball_position.y < -bgHalfHeight + ballHalfHeight)
        ball_velocity.y = -ball_velocity.y;

    ball_matrix = glm::mat4(1.0f);
    ball_matrix = glm::translate(ball_matrix, ball_position);
    ball_matrix = glm::scale(ball_matrix, glm::vec3(0.5f, 0.5f, 1.0f));
}
// clamp the ball to the background
void clamp_ball(glm::vec3& ball_position, glm::vec3& ball_velocity) {
	// basically check if the ball is out of bounds and if it is, reverse the velocity
    if (ball_position.y >= bgHalfHeight - ballHalfHeight) {
        ball_position.y = bgHalfHeight - ballHalfHeight - 0.01f;
        if (ball_velocity.y > 0) {
            ball_velocity.y = -ball_velocity.y; 
        }
    }
    if (ball_position.y <= -bgHalfHeight + ballHalfHeight) {
        ball_position.y = -bgHalfHeight + ballHalfHeight + 0.01f;
        if (ball_velocity.y < 0) { 
            ball_velocity.y = -ball_velocity.y; 
        }
    }
}


// handle collision between the ball and the ufo
void handle_collision(glm::vec3& ball_position, glm::vec3& ball_velocity, glm::vec3 ufo_position) {
	// calculate the distance between the ball and the ufo
    float x_distance = fabs(ball_position.x - ufo_position.x) - (ballHalfWidth + ufoHalfWidth);
    float y_distance = fabs(ball_position.y - ufo_position.y) - (ballHalfHeight + ufoHalfHeight);

    if (x_distance < 0 && y_distance < 0) { // when collision is detected
        if (ball_velocity.x > 0 && ball_position.x < ufo_position.x) {
            // ball moving right, hitting left side of ufo
            ball_velocity.x = -ball_velocity.x;
        }
        else if (ball_velocity.x < 0 && ball_position.x > ufo_position.x) {
            // ball moving left, hitting right side of ufo
            ball_velocity.x = -ball_velocity.x;
        }
    }
}


bool is_ball_out(glm::vec3& ball_position) {
	// check if the ball is out of bounds
    return (ball_position.x >= bgHalfWidth + ballHalfWidth || ball_position.x <= -bgHalfWidth - ballHalfWidth);
}



void update()
{
    // --- DELTA TIME CALCULATIONS --- //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

	// --- ROTATING BACKGROUND --- //
    g_rotation_angle += 0.2f * delta_time; // adjust speed by changing the multiplier
	// keep angle within 0 to 2pi
    if (g_rotation_angle > 2 * M_PI) {
        g_rotation_angle -= 2 * M_PI;
    }


    // --- ACCUMULATOR LOGIC --- //
    if (g_one_player_mode) {
        // move the blue paddle using the auto direction.
        g_blue_position.y += g_blue_up_down_direction * MOVEMENT_SPEED * delta_time;

        // bounce off the top and bottom boundaries.
        if (g_blue_position.y >= bgHalfHeight - ufoHalfHeight) {
            g_blue_position.y = bgHalfHeight - ufoHalfHeight;
            g_blue_up_down_direction = -1.0f;  // to move down
        }
        if (g_blue_position.y <= -bgHalfHeight + ufoHalfHeight) {
            g_blue_position.y = -bgHalfHeight + ufoHalfHeight;
            g_blue_up_down_direction = 1.0f;   // to move up
        }
    }
    else {
        // 2-player mode: update using user input.
        g_blue_position += g_blue_movement * MOVEMENT_SPEED * delta_time;
    }

    if (!g_gameOver) {
		// update the ball positions
        if (g_active_ball_count >= 1) { 
            update_ball(g_ball_position_1, g_ball_velocity_1, g_ball_matrix_1, delta_time); 
        }
        if (g_active_ball_count >= 2) {
            update_ball(g_ball_position_2, g_ball_velocity_2, g_ball_matrix_2, delta_time);
        }
        if (g_active_ball_count == 3) {
            update_ball(g_ball_position_3, g_ball_velocity_3, g_ball_matrix_3, delta_time);
        }
    }

    g_amber_position += g_amber_movement * MOVEMENT_SPEED * delta_time;
    g_amber_size += g_amber_scale * MOVEMENT_SPEED * delta_time;

	// clamp the balls to the background
    if (g_active_ball_count >= 1) { 
        clamp_ball(g_ball_position_1, g_ball_velocity_1); 
    }
    if (g_active_ball_count >= 2) {
        clamp_ball(g_ball_position_2, g_ball_velocity_2);
    }
    if (g_active_ball_count == 3) {
        clamp_ball(g_ball_position_3, g_ball_velocity_3);
    }


    // clamp the ufos to the background
    if (g_blue_position.y > bgHalfHeight - ufoHalfHeight) {
        g_blue_position.y = bgHalfHeight - ufoHalfHeight;
    }
    if (g_blue_position.y < -bgHalfHeight + ufoHalfHeight) {
        g_blue_position.y = -bgHalfHeight + ufoHalfHeight;
    }

    if (g_amber_position.y > bgHalfHeight - ufoHalfHeight) {
        g_amber_position.y = bgHalfHeight - ufoHalfHeight;
    }
    if (g_amber_position.y < -bgHalfHeight + ufoHalfHeight) {
        g_amber_position.y = -bgHalfHeight + ufoHalfHeight;
    }

    // --- TRANSLATION --- //
    g_blue_matrix = glm::mat4(1.0f);
    g_blue_matrix = glm::translate(g_blue_matrix, INIT_POS_BLUE);
    g_blue_matrix = glm::translate(g_blue_matrix, g_blue_position);

    g_amber_matrix = glm::mat4(1.0f);
    g_amber_matrix = glm::translate(g_amber_matrix, INIT_POS_AMBER);
    g_amber_matrix = glm::translate(g_amber_matrix, g_amber_position);

    // --- SCALING --- //
    g_blue_matrix = glm::scale(g_blue_matrix, INIT_SCALE_BLUE);
    g_amber_matrix = glm::scale(g_amber_matrix, INIT_SCALE_AMBER);

    // --- COLLISION LOGIC --- //
	// compute the actual center positions for the ufo:
    glm::vec3 g_blue_center = INIT_POS_BLUE + g_blue_position;
    glm::vec3 g_amber_center = INIT_POS_AMBER + g_amber_position;

    if (g_active_ball_count >= 1) {
        handle_collision(g_ball_position_1, g_ball_velocity_1, g_blue_center);
        handle_collision(g_ball_position_1, g_ball_velocity_1, g_amber_center);
    }
    if (g_active_ball_count >= 2) {
        handle_collision(g_ball_position_2, g_ball_velocity_2, g_blue_center);
        handle_collision(g_ball_position_2, g_ball_velocity_2, g_amber_center);
    }
    if (g_active_ball_count == 3) {
        handle_collision(g_ball_position_3, g_ball_velocity_3, g_blue_center);
        handle_collision(g_ball_position_3, g_ball_velocity_3, g_amber_center);
    }


	bool any_ball_out = false; // to check if any ball is out
    int last_ball_out_direction = 0; // 1 = right, -1 = left

    if (g_active_ball_count >= 1 && (g_ball_position_1.x > bgHalfWidth - .1 || g_ball_position_1.x < -bgHalfWidth + .1)) {
        any_ball_out = true;
        if (g_ball_position_1.x > 0) {
            last_ball_out_direction = 1;
        }
        else {
            last_ball_out_direction = -1;
        }
    }
    if (g_active_ball_count >= 2 && (g_ball_position_2.x > bgHalfWidth - .1 || g_ball_position_2.x < -bgHalfWidth + .1)) {
        any_ball_out = true;
        if (g_ball_position_2.x > 0) {
            last_ball_out_direction = 1;
        }
        else {
            last_ball_out_direction = -1;
        }
    }
    if (g_active_ball_count == 3 && (g_ball_position_3.x > bgHalfWidth - .1 || g_ball_position_3.x < -bgHalfWidth + .1)) {
        any_ball_out = true;
        if (g_ball_position_3.x > 0) {
            last_ball_out_direction = 1;
        }
        else {
            last_ball_out_direction = -1;
        }
    }

    if (any_ball_out && !g_gameOver) {
		// if any ball is out, the game is over
		if (last_ball_out_direction == 1) {
			g_winner = 1;
		}
		else {
			g_winner = 0;
		}
        g_gameOver = true;
        g_gameOverTime = ticks;
    }

    if (g_gameOver) {
        // wait for 3 seconds before ending the game
        if (ticks - g_gameOverTime > 2.0f) {
            g_app_status = TERMINATED;
        }
    }

}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

	// draw the rotating background
    glm::mat4 rotating_bg_matrix = glm::mat4(1.0f);
    rotating_bg_matrix = glm::rotate(rotating_bg_matrix, g_rotation_angle, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the Z-axis
    rotating_bg_matrix = glm::scale(rotating_bg_matrix, glm::vec3(12.0f, 12.0f, 1.0f)); // Scale it to fit
    draw_object(rotating_bg_matrix, g_rotating_background_texture_id);

	// draw the pixelated game background
    glm::mat4 bg_matrix = glm::mat4(1.0f);
    bg_matrix = glm::scale(bg_matrix, glm::vec3(7.0f, 5.0f, 1.0f));
    draw_object(bg_matrix, g_background_texture_id);

	// Draw the ufos and balls
    draw_object(g_amber_matrix, g_amber_texture_id);
    draw_object(g_blue_matrix, g_blue_texture_id);

    if (g_active_ball_count >= 1) {
        draw_object(g_ball_matrix_1, g_ball_texture_id);
    }
	if (g_active_ball_count >= 2) {
		draw_object(g_ball_matrix_2, g_ball_texture_id);
	}
	if (g_active_ball_count == 3) {
		draw_object(g_ball_matrix_3, g_ball_texture_id);
	}

    if (g_gameOver) {
        GLuint winner_texture_id;

        if (g_winner == 0) {
            winner_texture_id = g_blue_winner_texture_id;
        }
        else {
            winner_texture_id = g_amber_winner_texture_id;
        }

        // display Winner Message
        glm::mat4 winner_matrix = glm::mat4(1.0f);
        winner_matrix = glm::translate(winner_matrix, glm::vec3(0.0f, 2.9f, 0.0f));
        winner_matrix = glm::scale(winner_matrix, glm::vec3(1.5f, 1.5f, 1.0f));
        draw_object(winner_matrix, winner_texture_id);

        // display "Game Over" Message
        g_game_over_matrix = glm::mat4(1.0f);
        g_game_over_matrix = glm::scale(g_game_over_matrix, glm::vec3(2.0f, 2.0f, 1.0f));
        draw_object(g_game_over_matrix, g_gameOver_texture_id);
    }

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}