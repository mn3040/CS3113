#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <vector>
#include <math.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "SDL_mixer.h"

enum EffectType {
    NONE,
    FADEIN,
    FADEOUT,
    GROW,
    SHRINK,
    SHAKE,
    FLASH
};

class Effects {
private:
    ShaderProgram m_shader_program;
	ShaderProgram m_flash_shader_program; // shader program for flash effect

    EffectType    m_current_effect;

    float m_flash_size;

    
    float m_alpha;
    float m_effect_speed;
    float m_size;
	bool m_flash_active;    // checking if flash is active
	bool m_shake_active;    // checking if shake is active
    float m_shake_intensity;



public:
    Effects(glm::mat4 projection_matrix, glm::mat4 view_matrix);
    glm::mat4 apply_shake(glm::mat4 view_matrix);
    void draw_overlay();
	void draw_flash_overlay();
    void start(EffectType effect_type, float effect_speed);
    void update(float delta_time);
    void render();
};