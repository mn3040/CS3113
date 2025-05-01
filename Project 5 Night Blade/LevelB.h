#pragma once

#include "Scene.h"
#include "Effects.h"

enum class PlayerStateB {
    IDLE,
    WALKING,
    RUNNING,
    CROUCHING,
    CROUCH_WALKING,
    ATTACKING
};

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    static constexpr int ENEMY_COUNT = 15;
    bool m_animation_complete = true;
    float m_current_anim_timer = 0.0f;
    float m_current_anim_duration = 0.0f;

    GLuint font_texture_id;

    // ————— DESTRUCTOR ————— //
    ~LevelB();

    // ————— METHODS ————— //
    void set_crouching(bool v) { m_is_crouching = v; }
    void set_attacking(bool v) { m_is_attacking = v; }
    void set_running(bool v) { m_is_running = v; }
    void set_walking(bool v) { m_is_walking = v; }
    void set_rolling(bool v) { m_is_rolling = v; }
    void set_kicking(bool v) { m_is_kicking = v; }

    Effects* get_effects() override { return effects; }

    // key release tracking
    bool is_attack_key_released() const { return m_attack_key_released; }
    void set_attack_key_released(bool value) { m_attack_key_released = value; }
    bool is_kick_key_released() const { return m_kick_key_released; }
    void set_kick_key_released(bool value) { m_kick_key_released = value; }

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;

private:
    Effects* effects;

    // ————— ATTRIBUTES ————— //
    int* m_current_animation = nullptr;
    int m_current_texture = -1;


    bool m_previous_attacking = false;
    float m_combo_timer = 0.0f;
    const float COMBO_WINDOW = 0.5f;
    bool m_combo_active = false;

    float m_shake_timer = 0.0f;
    float m_shake_intensity = 0.0f;

    bool m_started_attack_effects = false;

    bool m_attack_key_released = true;  // track if Z key has been released
    bool m_kick_key_released = true;    // track if C key has been released

    PlayerStateB m_player_state;
    bool m_is_attacking;
    bool m_is_walking;
    bool m_is_running;
    bool m_is_crouching;
    bool m_is_rolling = false;
    bool m_is_kicking = false;
    float m_attack_timer;
    const float ATTACK_DURATION = 0.5f;
    const float ATTACK_COOLDOWN = 1.0f;
    static constexpr int MAX_FRAMES = 10;

    Entity* m_kick_effect = nullptr;
    float m_kick_timer = 0.0f;
    bool m_show_kick = false;
    const float KICK_DURATION = 0.25f;

    // for stuck detection
    float m_stuck_timer = 0.0f;
    glm::vec3 m_last_position = glm::vec3(0.0f);
    const float STUCK_TIMEOUT = 1.5f; // 1.5 second of not moving = stuck

	float m_animation_speeds[6]; // different speeds for different animations

	// player animation frames
    int m_walking[1][8];
    int m_running[1][10];
    int m_attacking[1][7];
    int m_idle[1][11];
    int m_crouch_walk[1][10];
    int m_rolling[1][7];
    int m_kick[1][6];
    int m_jump[1][4];
    int m_hurt[1][6];

    // texture IDs
    GLuint m_walking_texture_id;
    GLuint m_crouch_walk_texture_id;
    GLuint m_running_texture_id;
    GLuint m_attacking_texture_id;
    GLuint m_idle_texture_id;
    GLuint m_rolling_texture_id;
    GLuint m_kick_texture_id;
    GLuint m_jump_texture_id;
    GLuint m_hurt_texture_id;

    // enemy animation textures
    GLuint m_enemy_attack_texture_id;
    GLuint m_enemy_idle_texture_id;
    GLuint m_enemy_run_texture_id;
    GLuint m_enemy_jump_texture_id;

    // enemy animation frames
    int m_enemy_attack[1][5];
    int m_enemy_idle[1][5];
    int m_enemy_run[1][6];
    int m_enemy_bat[1][5];
    int m_enemy_jump[1][5];

    // enemy animation state tracking
    bool m_enemy_is_attacking[ENEMY_COUNT + 1];
    bool m_enemy_is_running[ENEMY_COUNT + 1];
    bool m_enemy_is_jumping[ENEMY_COUNT + 1];
    float m_enemy_animation_timers[ENEMY_COUNT + 1];
    float m_enemy_attack_cooldown[ENEMY_COUNT + 1];
    const float ENEMY_ATTACK_DURATION = 0.5f;
    const float ENEMY_ATTACK_COOLDOWN = 1.5f;

    // enemy state helper functions
    void switch_enemy_animation(Entity* enemy, int* animation, int texture_id, float delta_time, int frames, int cols, float speed, bool force_reset);
    void update_enemy_state(int enemy_index, float delta_time);
    void initialize_enemy_animations();

    GLuint m_new_enemy_texture_id;

    // sprite display/hurt animation
    bool m_player_hurt;
    float m_hurt_animation_time;
    int m_blink_count;
    const float BLINK_DURATION = 0.1f;
    const int NUM_BLINKS = 5;

    // player state helper methods
    void switch_animation(int* animation, int texture_id, float delta_time, int frames, int cols, float speed);
    void update_player_state(float delta_time);
};