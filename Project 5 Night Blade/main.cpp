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
#include "Instructions.h"
#include "Winner.h"
#include "Loser.h"
#include "Effects.h"
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
Instructions* g_instructions;
Winner* g_winner;
Loser* g_loser;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;
Menu *g_menu;
bool in_menu = true;
int g_player_lives = 30;

// background music
Mix_Music* g_background_music = nullptr;

// font and background textures
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

// switch to the next scene
void switch_to_scene(Scene* scene)
{
    if (g_current_scene != nullptr && g_current_scene->get_state().player != nullptr)
    {
        g_player_lives = g_current_scene->get_state().player->get_lives();
    }

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
    g_display_window = SDL_CreateWindow("Night Blade",
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

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    font_texture_id = Utility::load_texture(FONT_PATH);
    g_background_texture_id = Utility::load_texture("assets/background_industrial.jpg");

    
	// —–— SCENES ————— //
	g_menu = new Menu();
    g_instructions = new Instructions();
    g_winner = new Winner();
    g_loser = new Loser();
	g_level_a = new LevelA();
	g_level_b = new LevelB();
    g_level_c = new LevelC();
	switch_to_scene(g_menu);
    
	// ----— AUDIO ————— //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_background_music = Mix_LoadMUS("assets/sounds/chaos.mp3");
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
                        if (in_menu) {
							// if the user is in the menu, start the game
                            in_menu = false;
                            switch_to_scene(g_level_a);
                        }
                        break;

                    case SDLK_i:
						// if the user is in the menu, show instructions
                        if (g_current_scene == g_menu) {
                            switch_to_scene(g_instructions);
                        }
                        break;

                    case SDLK_b:
						// if the user is in the instructions, go back to the menu
                        if (g_current_scene == g_instructions) {
                            switch_to_scene(g_menu);
                        }
                        break;

					// if the user either presses space or up arrow, jump
                    case SDLK_SPACE:
						if (g_current_scene->get_state().player != nullptr) {
							g_current_scene->get_state().player->jump();
                            Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
						}
						break; 

                    case SDLK_UP:
                        if (g_current_scene->get_state().player != nullptr) {
                            g_current_scene->get_state().player->jump();
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
		// check if the user is pressing the keys and set the player state accordingly
        if (g_current_scene == g_level_a) {
            if (key_state[SDL_SCANCODE_DOWN]) g_level_a->set_crouching(true);
            else g_level_a->set_crouching(false);

            if (key_state[SDL_SCANCODE_Z]) g_level_a->set_attacking(true);
            else g_level_a->set_attacking(false);

            if (key_state[SDL_SCANCODE_LSHIFT]) g_level_a->set_running(true);
            else g_level_a->set_running(false);

            if (key_state[SDL_SCANCODE_X]) g_level_a->set_rolling(true);
            else g_level_a->set_rolling(false);

            if (key_state[SDL_SCANCODE_C]) g_level_a->set_kicking(true);
            else g_level_a->set_kicking(false);
        }
        else if (g_current_scene == g_level_b) {
            if (key_state[SDL_SCANCODE_DOWN]) g_level_b->set_crouching(true);
            else g_level_b->set_crouching(false);

            if (key_state[SDL_SCANCODE_Z]) g_level_b->set_attacking(true);
            else g_level_b->set_attacking(false);

            if (key_state[SDL_SCANCODE_LSHIFT]) g_level_b->set_running(true);
            else g_level_b->set_running(false);

            if (key_state[SDL_SCANCODE_X]) g_level_b->set_rolling(true);
            else g_level_b->set_rolling(false);

            if (key_state[SDL_SCANCODE_C]) g_level_b->set_kicking(true);
            else g_level_b->set_kicking(false);
		}
		else if (g_current_scene == g_level_c) {
			if (key_state[SDL_SCANCODE_DOWN]) g_level_c->set_crouching(true);
			else g_level_c->set_crouching(false);

			if (key_state[SDL_SCANCODE_Z]) g_level_c->set_attacking(true);
			else g_level_c->set_attacking(false);

			if (key_state[SDL_SCANCODE_LSHIFT]) g_level_c->set_running(true);
			else g_level_c->set_running(false);

			if (key_state[SDL_SCANCODE_X]) g_level_c->set_rolling(true);
			else g_level_c->set_rolling(false);

			if (key_state[SDL_SCANCODE_C]) g_level_c->set_kicking(true);
			else g_level_c->set_kicking(false);
		}

        Entity* player = g_current_scene->get_state().player;

        // check if running (shift key is held)
        bool is_running = key_state[SDL_SCANCODE_LSHIFT];

        float movement_speed;
		if (is_running) {
			movement_speed = 1.5f;
			player->set_running(true);
		}
		else {
			movement_speed = 1.0f;
			player->set_running(false);
		}
		// check if the user is pressing the left or right arrow keys
        if (key_state[SDL_SCANCODE_LEFT]) {
            player->set_movement(glm::vec3(-movement_speed, 0.0f, 0.0f));
            player->set_facing_left(true);

            if (g_current_scene == g_level_a) {
                g_level_a->set_walking(true);
                g_level_a->set_running(is_running);
            }
            else if (g_current_scene == g_level_b) {
                g_level_b->set_walking(true);
                g_level_b->set_running(is_running);
            }
			else if (g_current_scene == g_level_c) {
				g_level_c->set_walking(true);
				g_level_c->set_running(is_running);
			}

        }
        else if (key_state[SDL_SCANCODE_RIGHT]) {
            player->set_movement(glm::vec3(movement_speed, 0.0f, 0.0f));
            player->set_facing_left(false);

            if (g_current_scene == g_level_a) {
                g_level_a->set_walking(true);
                g_level_a->set_running(is_running);
            }
            else if (g_current_scene == g_level_b) {
                g_level_b->set_walking(true);
                g_level_b->set_running(is_running);
            }
			else if (g_current_scene == g_level_c) {
				g_level_c->set_walking(true);
				g_level_c->set_running(is_running);
			}
        }
        else {
            player->set_movement(glm::vec3(0.0f));

            if (g_current_scene == g_level_a) {
                g_level_a->set_walking(false);
                g_level_a->set_running(false);
            }
            else if (g_current_scene == g_level_b) {
                g_level_b->set_walking(false);
                g_level_b->set_running(false);
            }
            else if (g_current_scene == g_level_c) {
                g_level_c->set_walking(false);
                g_level_c->set_running(false);
            }
        }

        // normalize movement to ensure diagonal movement isn't faster
        if (glm::length(player->get_movement()) > movement_speed)
            player->normalise_movement();
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
        if (player != nullptr) {
            // if player runs out of lives, lose
            if (player->get_lives() <= 0) {
                g_game_lost = true;
            }

            if (g_current_scene == g_level_a) {
                if (g_current_scene->get_state().has_won) {
                    // add a small delay before switching scenes
                    g_win_timer += FIXED_TIMESTEP;
                    if (g_win_timer >= 2.0f) {  // 2-second delay
                        switch_to_scene(g_level_b);  // switch to the next level
                        g_win_timer = 0.0f;
                        g_game_won = false;
                    }
                }
                else if (player->get_position().y < -100.0f) {
                    // fell off map without rescuing hostage = instant loss
                    g_game_lost = true;
                }
            }
            else if (g_current_scene == g_level_b) {
                if (g_current_scene->get_state().has_won) {
                    // optionally add a small delay before switching scenes
                    g_win_timer += FIXED_TIMESTEP;
                    if (g_win_timer >= 2.0f) {  // 2-second delay
                        switch_to_scene(g_level_c);  // switch to the next level
                        g_win_timer = 0.0f;
                        g_game_won = false;
                    }
                }
                else if (player->get_position().y < -100.0f) {
                    // fell off map without rescuing hostage = instant loss
                    g_game_lost = true;
                }
            }
            else if (g_current_scene == g_level_c) {
                if (g_current_scene->get_state().has_won) {
                    // set victory flag after completing level C
                    g_win_timer += FIXED_TIMESTEP;
                    if (g_win_timer >= 2.0f) {
                        g_game_won = true; // this will trigger transition to winner scene
                    }
                }
                else if (player->get_position().y < -50.0f) {
                    // fell off map without rescuing hostage = instant loss
                    g_game_lost = true;
                }
            }
        }

        // handle transition to winner scene
        if (g_game_won && g_current_scene != g_winner) {
            g_win_timer += FIXED_TIMESTEP;
            if (g_win_timer >= 1.0f) {
                switch_to_scene(g_winner);
            }
            g_game_won = false;
        }

        // handle transition to loser scene
        if (g_game_lost && g_current_scene != g_loser) {
            g_loss_timer += FIXED_TIMESTEP;
            if (g_loss_timer >= 1.0f) {
                switch_to_scene(g_loser);
            }
        }

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);

    if (g_current_scene->get_state().player != nullptr) {
        float player_x = g_current_scene->get_state().player->get_position().x;
        float player_y = g_current_scene->get_state().player->get_position().y;

        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-player_x, -player_y, 0.0f));
    }
    else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
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

    if (g_current_scene->get_effects()) {
        g_current_scene->get_effects()->render();
    }

	// ————— RENDERING THE HUD ————— //
    // draw UI elements with a fixed view
    if (g_current_scene->get_state().player != nullptr) {
        // save the current view matrix
        glm::mat4 ui_view_matrix = glm::mat4(1.0f);
        g_shader_program.set_view_matrix(ui_view_matrix);

		// render the player lives
        glm::vec3 lives_position = glm::vec3(-4.5f, -3.0f, 0.0f);

        float health_perentage = (g_current_scene->get_state().player->get_lives() / 30.0f) * 100.0f;
        Utility::draw_text(&g_shader_program, font_texture_id,
            "Health: " + std::to_string((int)health_perentage) + "%",
            0.5f, 0.0015625f, lives_position);

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
    delete g_winner;
    delete g_loser;
	delete g_instructions;
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
