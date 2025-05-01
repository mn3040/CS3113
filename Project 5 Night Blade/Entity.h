#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include <SDL_mixer.h>
enum EntityType { PLAYER, PLATFORM, ENEMY, COIN, HOSTAGE, SLASH };
enum AIType     { WALKER, GUARD, FLYER           };
enum AIState    { WALKING, IDLE, ATTACKING };


enum AnimationDirection { LEFT, RIGHT, UP, DOWN };

class Entity
{
private:
	// ————— ENTITY ATTRIBUTES ————— //
    bool m_is_active = true;
    int m_walking[4][4]; // 4x4 array for walking animations
    int m_lives;
    float m_invincibility_timer = 0.0f;
    bool m_invincible = false;
    int m_max_jumps = 1;
    int m_jumps_left = 1;
    float     m_speed, m_jumping_power;
    bool m_is_jumping;
	bool m_is_running;

	float m_animation_speed = 0.1f;  // setting the animation speed
	int m_animation_indices_data[16] = { 0 }; // 16 indices for 4x4 array

    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;

	// ————— ANIMATION STATES ————— //
    bool m_is_attacking = false;
    bool m_is_crouching = false;


	// ————— SOUNDS ————— //
    Mix_Chunk* m_jump_sfx = nullptr;
    Mix_Chunk* m_hurt_sfx = nullptr;

    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    glm::mat4 m_model_matrix;

    // ————— TEXTURES ————— //
    GLuint    m_texture_id;

    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;

    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;

    float m_width = 1.0f,
          m_height = 1.0f;

    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 8;

    // ————— METHODS ————— //
    Entity();
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, int walking[4][4], float animation_time,
        int animation_frames, int animation_index, int animation_cols,
           int animation_rows, float width, float height, EntityType EntityType);
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType); // Simpler constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState); // AI constructor
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    // Overloading our methods to check for only the map
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);

    void ai_activate(Entity *player);
    void ai_walk();
    void ai_guard(Entity *player);
	void ai_fly();

	void take_damage(int damage) {
		if (m_invincible) return;
		m_lives -= damage;
		m_invincible = true;
		m_invincibility_timer = 0.0f;
		Mix_PlayChannel(-1, m_hurt_sfx, 0);
	}
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }

    bool m_facing_left = false;

    void set_facing_left(bool value) { m_facing_left = value; }
    bool is_facing_left() const { return m_facing_left; }


    void face_left() { m_animation_indices = m_walking[LEFT]; }
    void face_right() { m_animation_indices = m_walking[RIGHT]; }
    void face_up() { m_animation_indices = m_walking[UP]; }
    void face_down() { m_animation_indices = m_walking[DOWN]; }

    void move_left() { m_movement.x = -1.0f; face_left(); }
    void move_right() { m_movement.x = 1.0f;  face_right(); }
    void move_up() { m_movement.y = 1.0f;  face_up(); }
    void move_down() { m_movement.y = -1.0f; face_down(); }
    

    void const jump() { m_is_jumping = true; }
    void reset_jumps() { m_jumps_left = m_max_jumps; }


    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;   };
    AIType     const get_ai_type()        const { return m_ai_type;       };
    AIState    const get_ai_state()       const { return m_ai_state;      };
    int get_lives() const { return m_lives; }
    float const get_jumping_power() const { return m_jumping_power; }
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }
    bool get_is_active() const { return m_is_active; }
    float get_width() const { return m_width; }
    float get_height() const { return m_height; }
    float get_invincibility_timer() const { return m_invincibility_timer; }
    void unstick() {
        if (m_entity_type == PLAYER) {
			// unstick the player from the ground
            m_position.y += 0.2f;

            // reset all collision flags
            m_collided_left = false;
            m_collided_right = false;
            m_collided_top = false;
            m_collided_bottom = false;

            // also give a slight boost
            m_velocity.y = 2.0f;
        }
    }
    void activate()   { m_is_active = true;  };
    void deactivate() { m_is_active = false; };


    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type){ m_ai_type = new_ai_type;};
    void const set_ai_state(AIState new_state){ m_ai_state = new_state;};
    void set_lives(int lives) { m_lives = lives; }
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
	// setter for m_animation_indices
    void const set_animation_indices(int* new_indices, int frame_count) {
		// check if the new_indices is not null
        if (frame_count > 0) {
			// allocate a new array if necessary
            int max_frames = (frame_count < 16) ? frame_count : 16;

			// copy the new indices to our instance-specific data
            for (int i = 0; i < max_frames; i++) {
                m_animation_indices_data[i] = new_indices[i];
            }

			// set the instance-specific data to the new indices
            m_animation_indices = m_animation_indices_data;
        }
    }
    void set_animation_speed(float speed) { m_animation_speed = speed; }
    void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power;}
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }
    void set_jump_sfx(Mix_Chunk* jump) { m_jump_sfx = jump; }
    void set_hurt_sfx(Mix_Chunk* hurt) { m_hurt_sfx = hurt; }
    void set_attacking(bool value) { m_is_attacking = value; }
    void set_crouching(bool value) { m_is_crouching = value; }
    void set_running(bool value) { m_is_running = value; }
    void set_invincible(bool value) { m_invincible = value; }
    void set_invincibility_timer(float time) { m_invincibility_timer = time; }


    bool is_invincible() const { return m_invincible; }
    // setter for m_walking
    void set_walking(int walking[4][4])
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                m_walking[i][j] = walking[i][j];
            }
        }
    }
};

#endif // ENTITY_H
