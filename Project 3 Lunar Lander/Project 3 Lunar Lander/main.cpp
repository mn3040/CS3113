#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "Entity.h"
#include <string>
#include <vector>
#include <ctime>
#include "cmath"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 640 * 2,
WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED = 0.1765625f,
BG_GREEN = 0.17265625f,
BG_BLUE = 0.1609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr GLint NUMBER_OF_TEXTURES = 1,
LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

// ————— STRUCTS AND ENUMS —————//
enum AppStatus { RUNNING, TERMINATED };
enum FilterType { NEAREST, LINEAR };

struct GameState { 
    Entity* lander; 
    bool thrust_key_pressed = false;
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix, bg_matrix, platform_matrix, meteor_matrix, game_over_matrix;

constexpr float MINIMUM_COLLISION_DISTANCE = 1.0f;
constexpr int FONTBANK_SIZE = 16;

bool g_game_win = false, 
meteor_moving_right = true,
g_game_over = false,
meteor_moving_up = true,
moving_platform_moving_right = true;

float g_previous_ticks = 0.0f,
g_game_over_time = 0.0f,
g_game_win_time = 0.0f,
meteor_speed = 1.0f,
meteor_gravity = -4.9f;

// platforms positions, scales, and matrices
// stored in vectors for easy iteration
std::vector<glm::vec3> g_platform_positions = {
    glm::vec3(-3.0f, -1.5f, 0.0f),  // left platform
    glm::vec3(0.0f, -2.0f, 0.0f),   // center platform
    glm::vec3(3.0f, -1.5f, 0.0f)    // right platform
};

std::vector<glm::vec3> g_platform_scales = {
    glm::vec3(0.125f, 0.125f, 0.0f),    // left platform
    glm::vec3(0.125f, 0.125f, 0.0f),    // center platform
    glm::vec3(0.125f, 0.125f, 0.0f),    // right platform
};

std::vector<glm::mat4> g_platform_matrices = {
	glm::mat4(1.0f),    // left platform
	glm::mat4(1.0f),    // center platform
	glm::mat4(1.0f)	 // right platform
};

glm::vec3 moving_platform_position = glm::vec3(-2.0f, 0.0f, 0.0f);
glm::vec3 moving_platform_velocity = glm::vec3(2.0f, 0.0f, 0.0f);
glm::mat4 moving_platform_matrix = glm::mat4(1.0f);


glm::vec3 meteor_position = glm::vec3(-3.0f, 2.0f, 0.0f);
glm::vec3 game_over_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 meteor_velocity = glm::vec3(2.5f, 2.5f, 0.0f); // initial velocity

// ————— FILEPATHS ————— //
constexpr char BACKGROUND_FILEPATH[] = "assets/sky.png";
constexpr char PLATFORM_FILEPATH[] = "assets/platform.png";
constexpr char METEOR_FILEPATH[] = "assets/meteor.png";
constexpr char GAME_OVER_FILEPATH[] = "assets/game_over.gif";
constexpr char WIN_MESSAGE_FILEPATH[] = "assets/you_win.png";
constexpr char FONTSHEET_FILEPATH[] = "assets/font.png";

GLuint g_win_message_texture_id;
GLuint g_background_texture_id;
GLuint g_platform_texture_id;
GLuint meteor_texture_id;
GLuint g_gameOver_texture_id;
GLuint g_font_texture_id;

// ———— FUNCTION PROTOTYPES ———— //
void initialise();
void process_input();
void update();
void render();
void shutdown();
void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id);
void draw_text(ShaderProgram* shader_program, GLuint font_texture_id, std::string text,
	float font_size, float spacing, glm::vec3 position);
void init_platforms();


// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath, FilterType filterType)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
        STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
        filterType == NEAREST ? GL_NEAREST : GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Lunar Lander!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

    if (g_display_window == nullptr)
    {
        std::cerr << "Error: SDL window could not be created.\n";
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ————— LOAD LUNAR LANDER SPRITES ————— //
    std::vector<GLuint> lander_textures = {
        load_texture("assets/lander_idle.png", NEAREST),   // IDLE spritesheet
        load_texture("assets/lander_thrust.png", NEAREST), // THRUST spritesheet
        load_texture("assets/lander_crash.png", NEAREST)   // CRASH spritesheet
    };

    g_background_texture_id = load_texture(BACKGROUND_FILEPATH, NEAREST);
    meteor_texture_id = load_texture(METEOR_FILEPATH, NEAREST);
    g_platform_texture_id = load_texture(PLATFORM_FILEPATH, NEAREST);
	g_gameOver_texture_id = load_texture(GAME_OVER_FILEPATH, NEAREST);
	g_win_message_texture_id = load_texture(WIN_MESSAGE_FILEPATH, NEAREST);
    g_font_texture_id = load_texture(FONTSHEET_FILEPATH, NEAREST);


    std::vector<std::vector<int>> lander_animations = {
        {0},       // IDLE animation frames
        {0, 1, 2}, // THRUST animation frames
        {0}        // CRASH animation frames
    };

    g_game_state.lander = new Entity(
        lander_textures,
        lander_animations
    );

    g_game_state.lander->set_position(glm::vec3(0.0f, 2.0f, 0.0f));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void init_platforms() {
    for (int i = 0; i < g_platform_positions.size(); i++) {
        g_platform_matrices[i] = glm::mat4(1.0f);
        g_platform_matrices[i] = glm::translate(g_platform_matrices[i], g_platform_positions[i]);
        g_platform_matrices[i] = glm::scale(g_platform_matrices[i], g_platform_scales[i]);
    }
}

void process_input()
{
    SDL_Event event;
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                g_app_status = TERMINATED;
                break;
            case SDLK_UP:
				// start thrust
                g_game_state.thrust_key_pressed = true;
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_UP:
				// stop thrust
                g_game_state.thrust_key_pressed = false;
                g_game_state.lander->stop_thrust();
                break;
            }
            break;
        }
    }

    // handle continuous key presses (rotate left/right)
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        g_game_state.lander->rotate_left(delta_time);
    }

    if (keys[SDL_SCANCODE_RIGHT]) {
        g_game_state.lander->rotate_right(delta_time);
    }

    // apply thrust continuously while the up key is held down
    if (g_game_state.thrust_key_pressed) {
        g_game_state.lander->apply_thrust();
    }
}



void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    if (g_game_win) {
		// if the player wins, display the win message for 2 seconds
        if (ticks - g_game_win_time > 3.0f) {
            g_app_status = TERMINATED;
        }
        return;
    }

    if (g_game_over) {
		// if the player loses, display the game over message for 2 seconds
        if (ticks - g_game_over_time > 3.0f) {
            g_app_status = TERMINATED;
        }
        return;
    }

	if (delta_time > 0.016f) delta_time = 0.016f; // clamp delta time

	// apply gravity to the meteor
    meteor_velocity.y += meteor_gravity * delta_time;

	// update meteor position using projectile motion equations (physics)
    meteor_position.x += meteor_velocity.x * delta_time;
    meteor_position.y += meteor_velocity.y * delta_time + 0.5f * meteor_gravity * delta_time * delta_time;

    // if meteor falls too low, reset and switch sides
    if (meteor_position.y < -3.0f) {
		meteor_moving_right = !meteor_moving_right; // switch sides

        if (meteor_moving_right) {
            meteor_position = glm::vec3(-3.0f, 2.0f, 0.0f); // start from the left
            meteor_velocity = glm::vec3(2.5f, 2.5f, 0.0f);  // move right
        }
        else {
            meteor_position = glm::vec3(3.0f, 2.0f, 0.0f);  // start from the right
            meteor_velocity = glm::vec3(-2.5f, 2.5f, 0.0f); // move left
        }
    }
    
    // move the platform horizontally
    moving_platform_position.x += moving_platform_velocity.x * delta_time;

    // reverse movement if platform reaches boundary
    if (moving_platform_position.x > 2.0f) {
        moving_platform_moving_right = false;
        moving_platform_velocity.x = -2.0f; // move left
    }
    else if (moving_platform_position.x < -2.0f) {
        moving_platform_moving_right = true;
        moving_platform_velocity.x = 2.0f; // move right
    }

	// update lander
    g_game_state.lander->apply_gravity(delta_time);
    g_game_state.lander->update(delta_time, g_platform_positions, 
        meteor_position, moving_platform_position);

	// check for collision with the meteor or platform
    if (g_game_state.lander->meteor_collision || g_game_state.lander->is_crashed()) {
        g_game_over = true;
        g_game_over_time = ticks;
    }
	// check for collision/landing with the platforms
    if (g_game_state.lander->win) {
        g_game_win = true;
        g_game_win_time = ticks;
    }
    
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_text(ShaderProgram* shader_program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    shader_program->set_model_matrix(model_matrix);
    glUseProgram(shader_program->get_program_id());

    glVertexAttribPointer(shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(shader_program->get_position_attribute());

    glVertexAttribPointer(shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(shader_program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(shader_program->get_position_attribute());
    glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}


void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up background vertices
    float vertices[] = {
        -5.0f, -3.75f,  // bottom left
        5.0f, -3.75f,   // bottom right
        5.0f, 3.75f,    // top right

        -5.0f, -3.75f,  // bottom left
        5.0f, 3.75f,    // top right
        -5.0f, 3.75f    // top left
    };

    float texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

	// draw background
    glm::mat4 bg_matrix = glm::mat4(1.0f);
    bg_matrix = glm::scale(bg_matrix, glm::vec3(1.0f, 1.0f, 0.0f));
    draw_object(bg_matrix, g_background_texture_id);

	// draw platforms
	init_platforms();  
    for (glm::mat4& platform_matrix : g_platform_matrices) {
        draw_object(platform_matrix, g_platform_texture_id);
    }

	// draw moving platform
    moving_platform_matrix = glm::mat4(1.0f);
    moving_platform_matrix = glm::translate(moving_platform_matrix, moving_platform_position);
    moving_platform_matrix = glm::scale(moving_platform_matrix, glm::vec3(0.125f, 0.125f, 0.0f));
    draw_object(moving_platform_matrix, g_platform_texture_id);
    
	// draw meteor
    glm::mat4 meteor_matrix = glm::mat4(1.0f);
    meteor_matrix = glm::translate(meteor_matrix, meteor_position);
    meteor_matrix = glm::scale(meteor_matrix, glm::vec3(0.125f, 0.125f, 0.0f));
    draw_object(meteor_matrix, meteor_texture_id);

    // draw win message
    if (g_game_win) {
        draw_text(&g_shader_program, g_font_texture_id, "MISSION ACCOMPLISHED!", 0.4f, 0.01f,
            glm::vec3(-4.0f, 0.0f, 0.0f));
    }

	// draw game over message
    if (g_game_over) {
        draw_text(&g_shader_program, g_font_texture_id, "MISSION FAILED", 0.4f, 0.01f,
            glm::vec3(-2.5f, 0.0f, 0.0f));

    }

    // draw lander
    g_game_state.lander->render(&g_shader_program);

    // draw fuel status - and get it to show the percentage
    std::string fuel_text = "FUEL: " + std::to_string(int(g_game_state.lander->get_fuel())) + "%";
    draw_text(&g_shader_program, g_font_texture_id, fuel_text, 0.2, 0.005f,
        glm::vec3(-4.5f, -2.6f, 0.0f));

    std::string velocity_text = "SPEED: " + std::to_string(int(glm::length(g_game_state.lander->get_velocity()) * 100)) + " M/S";
    draw_text(&g_shader_program, g_font_texture_id, velocity_text, 0.2f, 0.005f,
        glm::vec3(1.5f, -2.6f, 0.0f));

	if (g_game_state.lander->is_thrusting()) {
		std::string thruster_text = "THRUST: ON";
        draw_text(&g_shader_program, g_font_texture_id, thruster_text, 0.2f, 0.005f,
            glm::vec3(-1.5f, -2.6f, 0.0f));
	} else {
		std::string thruster_text = "THRUST: OFF";
		draw_text(&g_shader_program, g_font_texture_id, thruster_text, 0.2f, 0.005f,
			glm::vec3(-1.5f, -2.6f, 0.0f));
	}

    // draw game title
    draw_text(&g_shader_program, g_font_texture_id, "LUNAR LANDER", 0.4f, 0.01f,
        glm::vec3(-2.0f, 3.0f, 0.0f));

    // Disable vertex arrays
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}


void shutdown()
{
    SDL_Quit();
    delete   g_game_state.lander;
}


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