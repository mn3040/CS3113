#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include <vector>
#include <iostream>

// constructor
Entity::Entity()
    : m_position(0.0f), m_velocity(0.0f), m_acceleration(0.0f),
    m_scale(0.5f, 0.5f, 0.0f), m_model_matrix(1.0f),
    m_fuel(100.0f), m_landed(false), m_crashed(false),
    m_animation_time(0.0f), m_animation_frames(0),
    m_animation_index(0), m_animation_cols(0), m_animation_rows(0),
    m_animation_indices(nullptr), m_current_animation(IDLE)
{
}

// parameterized constructor
Entity::Entity(std::vector<GLuint> texture_ids, std::vector<std::vector<int>> animations)
    : m_position(0.0f), m_velocity(0.0f), m_acceleration(0.0f),
    m_scale(0.5f, 0.5f, 0.0f),
    m_model_matrix(1.0f),
    m_fuel(100.0f), m_landed(false), m_crashed(false),
    m_texture_ids(texture_ids), m_animations(animations),
    m_animation_time(0.0f),
    m_animation_frames(1),
    m_animation_cols(1),
    m_animation_rows(1),
    m_current_animation(IDLE),
    m_animation_indices(nullptr)
{
	// initialize the animation indices and set the initial animation state
    if (!m_animations.empty() && !m_animations[0].empty()) {
        m_animation_indices = m_animations[m_current_animation].data();
    }
}

Entity::~Entity() {}

// apply thrust to the lander
void Entity::apply_thrust()
{
    if (m_fuel > 0 && !m_landed && !m_crashed)
    {
		// calculate thrust vector based on current rotation
        float angle_rad = glm::radians(m_rotation + 90.0f);
        float thrust_x = THRUST_POWER * cos(angle_rad);
        float thrust_y = THRUST_POWER * sin(angle_rad);

		// add thrust to acceleration
        m_acceleration.x += thrust_x;
        m_acceleration.y += thrust_y;

		// reduce fuel
        m_fuel -= FUEL_BURN_RATE;

		// stop thrust if fuel runs out
        if (m_fuel < 0) {
            m_fuel = 0;
            stop_thrust();
            return;
        }

		// set thrusting flag and animation state
        m_thrusting = true;
        set_animation_state(THRUST);
    }
    else if (m_fuel <= 0 && m_thrusting)
    {
		// if fuel runs out while thrusting, stop thrust
        stop_thrust();
    }
}

void Entity::apply_gravity(float delta_time)
{
    if (!m_landed && !m_crashed)
    {
		// apply gravity to the entity
        m_acceleration.y += GRAVITY;
    }
}

// stop thrusting
void Entity::stop_thrust()
{
    if (m_thrusting) {
        m_thrusting = false;
        set_animation_state(IDLE);

		// reset animation index and change acceleration
        m_animation_index = 0;
        m_acceleration.x = 0.0f;
        m_acceleration.y = GRAVITY;
    }
}


// gradually increase rotation velocity when rotating left (counter-clockwise)
void Entity::rotate_left(float delta_time)
{
    if (!m_landed && !m_crashed)
    {
		// increase rotational velocity in positive direction (with a max cap)
        m_rotation_velocity += m_rotation_acceleration * delta_time;
        if (m_rotation_velocity > m_max_rotation_velocity)
            m_rotation_velocity = m_max_rotation_velocity;
    }
}

// gradually increase rotation velocity when rotating right (clockwise)
void Entity::rotate_right(float delta_time)
{
    if (!m_landed && !m_crashed)
    {
		// increase rotational velocity in negative direction (with a max cap)
        m_rotation_velocity -= m_rotation_acceleration * delta_time;
        if (m_rotation_velocity < -m_max_rotation_velocity)
            m_rotation_velocity = -m_max_rotation_velocity;
    }
}

// update rotation based on rotational velocity
void Entity::update_rotation(float delta_time)
{
    if (!m_landed && !m_crashed)
    {
		// update rotation based on velocity
        m_rotation += m_rotation_velocity * delta_time;

		// keep rotation within 0-360 degrees
        if (m_rotation >= 360.0f) {
            m_rotation -= 360.0f;
        }
        else if (m_rotation < 0.0f) {
            m_rotation += 360.0f;
        }

		// decelerate rotation velocity
        if (m_rotation_velocity > 0) {
            m_rotation_velocity -= m_rotation_deceleration * delta_time;
            if (m_rotation_velocity < 0) m_rotation_velocity = 0;
        }
        else if (m_rotation_velocity < 0) {
            m_rotation_velocity += m_rotation_deceleration * delta_time;
            if (m_rotation_velocity > 0) m_rotation_velocity = 0;
        }
    }
}

// check for collision with the ground or platforms
void Entity::check_collision(float ground_height, glm::vec3& object_position, bool isMeteor) {
	// if it's a meteor, check for collision with the meteor
    if (isMeteor) {
		// calculate distance between the entity and the meteor
        float collision_threshold = 0.2f;
        float x_distance = fabs(m_position.x - object_position.x);
        float y_distance = fabs(m_position.y - object_position.y);

        if (x_distance < collision_threshold && y_distance < collision_threshold) {
            m_crashed = true;
            win = false;
            meteor_collision = true;
            m_current_animation = CRASHED;
            

            m_velocity = glm::vec3(0.0f);
            m_acceleration = glm::vec3(0.0f);
        }
    }
    else {
		// check for collision with the ground or platforms
        float platform_width = 0.5f;
        float platform_height = 0.3f;

		// check if the entity is above the platform
        bool above_platform =
            (m_position.x >= object_position.x - platform_width) &&
            (m_position.x <= object_position.x + platform_width);

		// check if the entity is below the platform
        if (above_platform && m_position.y <= ground_height + platform_height) {
			// check for safe landing
            float landing_speed = glm::length(m_velocity);
            float landing_angle = fabs(fmod(m_rotation, 360.0f));
            if (landing_angle > 180.0f) landing_angle = 360.0f - landing_angle;

            if (landing_speed > SAFE_LANDING_SPEED || (landing_angle > 30.0f && landing_angle < 330.0f)) {
                m_crashed = true;
                win = false;
                m_current_animation = CRASHED;
            }
            else {
                m_position.y = ground_height + platform_height;
                m_velocity = glm::vec3(0.0f);
                m_acceleration = glm::vec3(0.0f);
                m_landed = true;
                win = true;
            }
        }
		// check if the entity has crashed into the ground
        else if (m_position.y <= -2.95f) { 
            m_crashed = true;
			win = false;
            m_current_animation = CRASHED;
            m_position.y = -2.95f;
            m_velocity = glm::vec3(0.0f);
            m_acceleration = glm::vec3(0.0f);
        }
    }
}

void Entity::set_animation_state(Animation new_animation)
{
	// check if the animation state has changed
    if (m_current_animation != new_animation) {
        m_current_animation = new_animation;
        m_animation_index = 0;

		// set the number of frames, columns, and rows based on the animation state
        if (m_current_animation == IDLE || m_current_animation == CRASHED) {
            m_animation_frames = 1;
            m_animation_cols = 1;
            m_animation_rows = 1;
        }
        else if (m_current_animation == THRUST) {
			// the thrust animation has 3 frames
            m_animation_frames = 3;
            m_animation_cols = 3;
            m_animation_rows = 1;
        }
    }
}


// Update draw_sprite_from_texture_atlas to correctly handle thrust animation
void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program)
{
    GLuint current_texture = m_texture_ids[m_current_animation];

	// set the current texture based on the current animation state
    if (m_current_animation == THRUST && m_thrusting) {
		current_texture = m_texture_ids[1]; // use the thrust texture
    }
    else if (m_current_animation == IDLE) {
		current_texture = m_texture_ids[0]; // use the idle texture
    }
    else if (m_current_animation == CRASHED) {
		current_texture = m_texture_ids[2]; // use the crashed texture
    }

	// calculate texture coordinates based on the current animation frame
    float u_coord = (float)(m_animation_index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(m_animation_index / m_animation_cols) / (float)m_animation_rows;

    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width,
        v_coord, u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, current_texture);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::update(float delta_time, std::vector<glm::vec3>& g_platform_positions, 
    glm::vec3& meteor_position, glm::vec3& moving_platform_position) {
	// apply gravity
    if (!m_landed && !m_crashed)
    {
        m_velocity += m_acceleration * delta_time;
        m_position += m_velocity * delta_time; 
		m_velocity *= 0.998f; // apply drag

        update_rotation(delta_time);

        m_acceleration = glm::vec3(0.0f);
    }

	// collision check against platforms and meteor
    for (glm::vec3& platform_position : g_platform_positions) {
        check_collision(platform_position.y, platform_position, false);
        ;
    }

    // check for landing on the moving platform
    check_collision(moving_platform_position.y, moving_platform_position, false);

	// Check for collision with the meteor
    check_collision(-2.95f, meteor_position, true);


	// update animation
    m_animation_time += delta_time;
	float frames_per_second = 1.0f / SECONDS_PER_FRAME;
    if (m_animation_time >= frames_per_second) {
        m_animation_time = 0.0f;
        m_animation_index++;
        if (m_animation_index >= m_animation_frames) {
            m_animation_index = 0;
        }
    }

	// update the model matrix
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}


// Render the entity
void Entity::render(ShaderProgram* program)
{
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != nullptr)
        draw_sprite_from_texture_atlas(program);
}
