#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Menu.h"


// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl",
           FONT_PATH[] = "assets/font.png";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //

// loading in the levels
Scene *g_current_scene;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC* g_level_c;
Menu *g_menu;
bool in_menu = true;
int g_player_lives = 3;

// background music
Mix_Music* g_background_music = nullptr;

GLuint font_texture_id;
GLuint g_background_texture_id;


SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

// ————— GAME STATE ————— //

bool g_game_won = false;
bool g_game_lost = false;
float g_loss_timer = 0.0f;
float g_win_timer = 0.0f;


void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Rise of the AI",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    font_texture_id = Utility::load_texture(FONT_PATH);
    g_background_texture_id = Utility::load_texture("assets/background.jpg");

    
    // ————— MENU ————— //
	// want to load the menu first
	g_menu = new Menu();
	g_level_a = new LevelA();
	g_level_b = new LevelB();
    g_level_c = new LevelC();
	switch_to_scene(g_menu);
    
	// ----— AUDIO ————— //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_background_music = Mix_LoadMUS("assets/sounds/skyline_pulse.mp3");
    if (!g_background_music) {
        std::cout << "Failed to load music: " << Mix_GetError() << std::endl;
    }
    else {
        Mix_PlayMusic(g_background_music, -1); // loop forever
        Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Set volume
    }

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
	// only process input if the game is not in the menu
    if (g_current_scene->get_state().player != nullptr) {
		g_current_scene->get_state().player->set_movement(glm::vec3(0, 0, 0));
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_RETURN:
						// switch to level a
						if (in_menu) {
							in_menu = false;
							switch_to_scene(g_level_a);
						}
						break;
                    // allow the user to jump
					case SDLK_SPACE:
						if (g_current_scene->get_state().player != nullptr) {
							g_current_scene->get_state().player->jump();
							// PLAY THE JUMP SOUND
                            Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
						}
						break;  
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    if (g_current_scene->get_state().player != nullptr) {
        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        Entity* player = g_current_scene->get_state().player;
		// ————— COLLISION DETECTION ————— //
        if (!g_game_won && !g_game_lost && player != nullptr) {
			// if the player is out of lives, set the game lost flag
            if (player->get_lives() <= 0) {
                g_game_lost = true;

            }
            // if player lost, give the player 3 seconds to see that they lost
            if (g_game_lost) {
                g_loss_timer += FIXED_TIMESTEP;
                if (g_loss_timer >= 3.0f) {
                    g_app_status = TERMINATED;
                    return;
                }
            }

			// if the player is out of bounds, switch to the next level
            if (g_current_scene == g_level_a && player->get_position().y < -8.0f) {
                g_player_lives = player->get_lives();
                switch_to_scene(g_level_b);
				g_level_b->get_state().player->set_lives(g_player_lives);
            } else if (g_current_scene == g_level_b &&
                player->get_position().y < -8.0f) {
                g_player_lives = player->get_lives();
                switch_to_scene(g_level_c);
                g_level_c->get_state().player->set_lives(g_player_lives);
            } else if (g_current_scene == g_level_c &&
                player->get_position().y < -8.0f) {
                g_player_lives = player->get_lives();
                g_game_won = true;
            }
			// if player won, start counting the timer
			if (g_game_won) {
				g_win_timer += FIXED_TIMESTEP;
				if (g_win_timer >= 3.0f) {
					g_app_status = TERMINATED;
					return;
				}
			}
        }
        delta_time -= FIXED_TIMESTEP;
    }

    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene->get_state().player != nullptr) {
        if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
	}
	else {
		g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
	}
}
// ————— RENDERING THE BACKGROUND ————— //
void render_background(const glm::mat4& view_matrix)
{
	// we want to render the background in the same position as the camera
	// so we need to invert the view matrix
    glm::mat4 background_matrix = glm::inverse(view_matrix);

    g_shader_program.set_model_matrix(background_matrix);

    float background_vertices[] = {
        -5.0f, -3.75f,   5.0f, -3.75f,   5.0f, 3.75f,
        -5.0f, -3.75f,   5.0f,  3.75f,  -5.0f, 3.75f
    };

    float tex_coords[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    glBindTexture(GL_TEXTURE_2D, g_background_texture_id);

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, background_vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
}



void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDER BACKGROUND ————— //
    render_background(g_view_matrix);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    // ————— MENU TEXT (only when in menu) ————— //
    if (g_current_scene == g_menu)
    {
		// save the current view matrix
		// we want to render the menu in the same position as the camera
        glm::mat4 ui_view_matrix = glm::mat4(1.0f);
        g_shader_program.set_view_matrix(ui_view_matrix);

        Utility::draw_text(&g_shader_program, font_texture_id,
            "Rise of the AI", 0.6f, 0.02f, glm::vec3(-4.0f, 0.2f, 0.0f));

        Utility::draw_text(&g_shader_program, font_texture_id,
            "Press Enter to Start", 0.4f, 0.02f, glm::vec3(-4.0f, -0.7f, 0.0f));

        // rest to original view matrix
        g_shader_program.set_view_matrix(g_view_matrix);
    }

    // draw UI elements with a fixed view
    if (g_current_scene->get_state().player != nullptr) {
        // save the current view matrix
        glm::mat4 ui_view_matrix = glm::mat4(1.0f);
        g_shader_program.set_view_matrix(ui_view_matrix);

		// render the player lives
        glm::vec3 lives_position = glm::vec3(-4.5f, -3.0f, 0.0f);

        Utility::draw_text(&g_shader_program, font_texture_id,
            "Lives: " + std::to_string(g_current_scene->get_state().player->get_lives()),
            0.5f, 0.0015625f, lives_position);

		// render the score 
        if (g_game_lost) {
            Utility::draw_text(&g_shader_program, font_texture_id,
                "You Lose", 1.0f, 0.05f, glm::vec3(-3.75f, 0.0f, 0.0f));
            g_app_status = TERMINATED;
        }
        else if (g_game_won) {
            Utility::draw_text(&g_shader_program, font_texture_id,
                "You Win", 1.0f, 0.05f, glm::vec3(-3.25f, 0.0f, 0.0f));
        }


        // restore the game view matrix
        g_shader_program.set_view_matrix(g_view_matrix);
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    // free the music resources
    if (g_background_music != nullptr) {
        Mix_FreeMusic(g_background_music);
    }
    SDL_Quit();
    
    // ————— DELETING LEVEL DATA (i.e. map, character, enemies...) ————— //
    delete g_level_a;
	delete g_level_b;
	delete g_level_c;
	delete g_menu;
}

// ————— GAME LOOP ————— //
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
