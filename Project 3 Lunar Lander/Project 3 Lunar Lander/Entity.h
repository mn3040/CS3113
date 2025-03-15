#pragma once

// Include essential headers
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#include <vector>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <iostream>

// Animation states for the entity
enum Animation { IDLE, THRUST, CRASHED };

class Entity
{
private:
    // ————— TEXTURES & ANIMATIONS ————— //
    std::vector<GLuint> m_texture_ids;
    std::vector<std::vector<int>> m_animations;

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    float m_rotation;       // rotation angle in degrees
    bool m_thrusting;       // tracking if thrusting is active

	// rotation velocity and acceleration
	float m_rotation_velocity = 0.0f;       // rotation velocity in degrees per second
	float m_max_rotation_velocity = 40.0f;  // maximum rotation velocity
	float m_rotation_acceleration = 20.0f;  // rotation acceleration
	float m_rotation_deceleration = 10.0f;  // rotation deceleration

    glm::vec3 m_scale;
    glm::mat4 m_model_matrix;

    Animation m_current_animation;  // Current animation state
    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;
    int m_animation_frames;
    int m_animation_index;
    int m_animation_cols;
    int m_animation_rows;

	float m_fuel = 100.0f;  // fuel level
	bool m_landed; // has the ship landed?
    bool m_crashed; // did the ship crash?

public:
    // ————— CONSTANTS ————— //
    static constexpr int SECONDS_PER_FRAME = 6;
    bool meteor_collision = false;
	bool win;

    // physics constants - adjusted for better lunar lander feel
    static constexpr float GRAVITY = -0.5f;      // Lunar gravity (weaker than Earth)
    static constexpr float THRUST_POWER = 1.0f;  // Thrust force
    static constexpr float ROTATION_SPEED = 1.2f; // Degrees per update when rotating
    static constexpr float FUEL_BURN_RATE = 0.01f; // Fuel consumption rate
    static constexpr float SAFE_LANDING_SPEED = 0.3f; // Maximum safe landing speed

    // ————— CONSTRUCTORS & DESTRUCTOR ————— //
    Entity();
    Entity(std::vector<GLuint> texture_ids, std::vector<std::vector<int>> animations);
    ~Entity();

    // ————— METHODS ————— //
    void apply_gravity(float delta_time);
    void apply_thrust();
    void stop_thrust();
    void rotate_left(float delta_time);
    void rotate_right(float delta_time);
    void update_rotation(float delta_time);
    void check_collision(float ground_height, glm::vec3& object_position, bool isMeteor);
    void update(float delta_time, std::vector<glm::vec3>& g_platform_positions, 
        glm::vec3& meteor_position, glm::vec3& moving_platform_position);
    void render(ShaderProgram* program);
    void draw_sprite_from_texture_atlas(ShaderProgram* program);

    // Animation control
    void set_animation_state(Animation new_animation);

    // ————— GETTERS & SETTERS ————— //
    glm::vec3 get_position() const { return m_position; }
    glm::vec3 get_velocity() const { return m_velocity; }
    float get_fuel() const { return m_fuel; }
    float get_rotation() const { return m_rotation; }
    float get_rotation_velocity() const { return m_rotation_velocity; }

	// checking if lander has landed, crashed, or is thrusting
    bool is_landed() const { return m_landed; }
    bool is_crashed() const { return m_crashed; }
    bool is_thrusting() const { return m_thrusting; }

    void set_position(glm::vec3 new_position) { m_position = new_position; }
    void set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void set_fuel(float fuel) { m_fuel = fuel; }
    void set_rotation(float rotation) { m_rotation = rotation; }
};
