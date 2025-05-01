#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Effects.h"
#include "Map.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState
{
    // ————— GAME OBJECTS ————— //
    Map *map;
    Entity *player;
    Entity *enemies;
    Entity* hostage;
    
    // coins
	Entity* coins;
    
    // ————— AUDIO ————— //
    Mix_Chunk* hurt_sfx;
    Mix_Chunk* coin_sfx;
    Mix_Chunk* jump_sfx;
    Mix_Chunk* slash_sfx;
	Mix_Chunk* winner_sfx;
	Mix_Chunk* kick_sfx;
	Mix_Music* bgm;
    
	bool has_won = false;
    // ————— POINTERS TO OTHER SCENES ————— //
    int next_scene_id;
    Entity* platforms; // Add this line
    int platform_count;

};

class Scene {
protected:
    GameState m_game_state;
    
public:
    // ————— ATTRIBUTES ————— //
    int m_number_of_enemies = 1;
    
    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    // ————— GETTERS ————— //
    GameState const get_state() const { return m_game_state;             }
    // In Scene.h
    virtual Effects* get_effects() { return nullptr; }

    int const get_number_of_enemies() const { return m_number_of_enemies; }
};
