#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

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


bool m_stepped_on = false;
float m_disappear_timer = 0.0f;


void Entity::ai_activate(Entity *player)
{
    switch (m_ai_type)
    {
        case WALKER:
            ai_walk();
            break;
            
        case GUARD:
            ai_guard(player);
            break;
            
        case FLYER:
            ai_fly();
            break;
        default:
            break;
    }
}

void Entity::ai_walk()
{
	// simple walking AI motion
	m_movement.x = sin(SDL_GetTicks() / 1000.0f) * m_speed; // smooth oscillation
}

void Entity::ai_guard(Entity *player)
{
    float dx = player->get_position().x - m_position.x;
    switch (m_ai_state) {
        case IDLE:
            if (glm::distance(m_position, player->get_position()) < 3.0f) m_ai_state = WALKING;
            break;
            
        case WALKING:
            if (fabs(dx) > 0.1f) {  // prevent flip-flopping at equal positions
				if (dx > 0) {
					m_movement.x = 1.0f; // move right
					face_right();
				}
				else {
					m_movement.x = -1.0f; // move left
					face_left();
				}
			}
			else if (fabs(dx) < 0.1f) {
				m_movement.x = 0.0f;  // stop moving
				m_ai_state = IDLE;
			}
			else if (fabs(dx) < 1.0f) {
				m_movement.x = dx;  // move towards player
				m_ai_state = WALKING;
            }
            // to transition to attacking when very close
            if (fabs(dx) < 1.5f) {
                m_ai_state = ATTACKING;
                m_movement.x = 0.0f;  // stop moving when attacking
            }
            break;
            
        case ATTACKING:
            // return to walking after a brief attack
            m_movement.x = 0.0f;
            if (fabs(dx) > 1.5f) {
                m_ai_state = WALKING;
            }
            break;
            
        default:
            break;
    }
}

void Entity::ai_fly()
{
	// simple flying AI motion
    float time = (float)SDL_GetTicks() / 1000.0f; // convert to seconds
    m_movement = glm::vec3(0.0f, sin(time * 2.0f), 0.0f); // up/down sinusoidal
    m_animation_indices = m_walking[0];


}

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

// Parameterized constructor
Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[4][4], float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed),m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    face_down();
    set_walking(walking);
}

// Simpler constructor for partial initialization
Entity::Entity(GLuint texture_id, float speed,  float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < SECONDS_PER_FRAME; ++i)
        for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState): m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height),m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
// Initialize m_walking with zeros or any default value
for (int i = 0; i < SECONDS_PER_FRAME; ++i)
    for (int j = 0; j < SECONDS_PER_FRAME; ++j) m_walking[i][j] = 0;
}

Entity::~Entity() { }

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
            if (m_velocity.y > 0)
            {
                m_position.y -= (y_overlap + 0.001);
                m_velocity.y    = 0;

                // collision
                m_collided_top  = true;
                if (!m_invincible && m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY) {
                    m_lives--;
                    m_invincible = true;
                    m_invincibility_timer = 1.0f;
                };
            } else if (m_velocity.y < 0)
            {
                m_position.y      += y_overlap + 0.001;
                m_velocity.y       = 0;

				// collision
                m_collided_bottom  = true;
                if (!m_invincible && m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY) {
                    m_lives--;
                    m_invincible = true;
                    m_invincibility_timer = 0.0f;
                };
            }
        }
    }
}

void const Entity::check_collision_x(Entity *collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity *collidable_entity = &collidable_entities[i];
        
        if (check_collision(collidable_entity))
        {
			// checking collision with enemies
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            if (m_velocity.x > 0)
            {
                m_position.x -= (x_overlap + 0.001f);
                m_velocity.x      = 0;

				// collision
                m_collided_right  = true;
                if (!m_invincible && m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY) {
                    m_lives--;
                    m_invincible = true;
                    m_invincibility_timer = 0.0f;

					// collision sound
                    if (m_hurt_sfx) {
                        Mix_PlayChannel(-1, m_hurt_sfx, 0);
                    }
                }
            } else if (m_velocity.x < 0)
            {
                m_position.x += (x_overlap + 0.001f);
                m_velocity.x     = 0;
 
                // collision
                m_collided_left  = true;
                if (!m_invincible && m_entity_type == PLAYER && collidable_entity->get_entity_type() == ENEMY) {
                    m_lives--;
                    m_invincible = true;
                    m_invincibility_timer = 0.0f;

					// collision sound
                    if (m_hurt_sfx) {
                        Mix_PlayChannel(-1, m_hurt_sfx, 0);
                    }
                }
            }
        }
    }
}

void const Entity::check_collision_y(Map *map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    
    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    
    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
            m_position.y += penetration_y;
            m_velocity.y = 0;
            m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
        
    }

    // If we are colliding with the bottom, reset jumps
    if (m_collided_bottom) {
        reset_jumps();
    }

}

void const Entity::check_collision_x(Map *map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left  = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    // Add extra probes to check above and below
    glm::vec3 left_up = glm::vec3(m_position.x - (m_width / 2), m_position.y + 0.1f, m_position.z);
    glm::vec3 right_up = glm::vec3(m_position.x + (m_width / 2), m_position.y + 0.1f, m_position.z);
    glm::vec3 left_down = glm::vec3(m_position.x - (m_width / 2), m_position.y - 0.1f, m_position.z);
    glm::vec3 right_down = glm::vec3(m_position.x + (m_width / 2), m_position.y - 0.1f, m_position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        if (penetration_x > 0.05f) {
            m_position.x += penetration_x;
            m_velocity.x = 0;
            m_collided_left = true;
        }
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        // Add a slight grace margin for narrow gaps
        if (penetration_x > 0.05f) {
            m_position.x -= penetration_x;
            m_velocity.x = 0;
            m_collided_right = true;
        }
    }

    if (m_collided_left && m_collided_right && m_entity_type == PLAYER) {
        // Check if there's open space above us
        if (!map->is_solid(left_up, &penetration_x, &penetration_y) &&
            !map->is_solid(right_up, &penetration_x, &penetration_y)) {
            // Move up slightly to escape
            m_position.y += 0.1f;
            m_collided_left = false;
            m_collided_right = false;
        }
        // Or check if there's open space below us
        else if (!map->is_solid(left_down, &penetration_x, &penetration_y) &&
            !map->is_solid(right_down, &penetration_x, &penetration_y)) {
            // Move down slightly to escape
            m_position.y -= 0.1f;
            m_collided_left = false;
            m_collided_right = false;
        }
    }
}


void Entity::update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map)
{
    if (!m_is_active) return;

	// checking for collision with enemies and set a cooldown timer
    if (m_invincible) {
        m_invincibility_timer += delta_time;
        if (m_invincibility_timer >= 1.0f) {
            m_invincible = false;
            m_invincibility_timer = 0.0f;
        }
    }

	// reset collision flags
    m_collided_top    = false;
    m_collided_bottom = false;
    m_collided_left   = false;
    m_collided_right  = false;
    
    if (m_entity_type == ENEMY) ai_activate(player);

	// update animation
    if (m_animation_index >= m_animation_frames) {
        m_animation_index = 0;
    }

    // animate only if we have a row
    if (m_animation_indices != NULL)
    {
        m_animation_time += delta_time;
        if (m_animation_time >= m_animation_speed && m_animation_frames > 1)
        {
            m_animation_time = 0.0f;
            m_animation_index++;

            if (m_animation_index >= m_animation_frames)
            {
                m_animation_index = 0;
            }
        }
    }

	// update position
    m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;
    
	// jumping logic
    if (m_is_jumping && m_jumps_left > 0) {
        m_is_jumping = false;
        m_velocity.y = 7.0f;   // higher jump impulse
        m_jumps_left--;

        if (m_entity_type == PLAYER && m_jump_sfx) {
            Mix_PlayChannel(-1, m_jump_sfx, 0);
        }

    }
    
	// update position based on velocity
    m_position.y += m_velocity.y * delta_time;
    
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);
    
    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, glm::vec3(m_width, m_height, 1.0f));


}


void Entity::render(ShaderProgram* program) {
    if (!m_is_active) return;

    if (m_invincible) {
        int blink_phase = static_cast<int>(m_invincibility_timer * 10) % 2;
        if (blink_phase == 1) return; // skip draw for blinking effect
    }

    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL) {
        if (m_facing_left) {
            // Save original matrix
            glm::mat4 original_matrix = m_model_matrix;

            // Apply a horizontal flip
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
            program->set_model_matrix(m_model_matrix);

            // Draw the sprite
            draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);

            // Restore original matrix
            m_model_matrix = original_matrix;
            program->set_model_matrix(m_model_matrix);
        }
        else {
            draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        }
        return;
    }

    // Rest of rendering code remains the same
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
