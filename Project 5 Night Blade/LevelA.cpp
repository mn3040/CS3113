#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 40
#define LEVEL_HEIGHT 16

// constants for the level
extern int g_player_lives;
extern bool g_game_won;

// sprite sheet file paths
constexpr const char WALKING_FILEPATH[] = "assets/sprites final/casual_walk.png",
CROUCH_WALK_FILEPATH[] = "assets/sprites final/crouch walk.png",
RUNNING_FILEPATH[] = "assets/sprites final/run.png",
ATTACKING_FILEPATH[] = "assets/sprites final/attack.png",
IDLE_FILEPATH[] = "assets/sprites final/idle.png",
ROLLING_FILEPATH[] = "assets/sprites final/roll.png",
KICK_FILEPATH[] = "assets/sprites final/kick.png",
JUMP_FILEPATH[] = "assets/sprites final/jump.png",
HURT_FILEPATH[] = "assets/sprites final/hurt on ground.png";


// katana zero-style level layout
unsigned int LEVEL_DATA[] = {
    42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
    42, 0, 0, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 11, 11, 11, 11, 0, 0, 0, 0, 17, 0, 0, 0, 11, 11, 11, 11, 0, 0, 0, 0, 22, 0, 0, 0, 11, 42,
    42, 36, 36, 36, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 36, 36, 36, 0, 0, 0, 0, 0, 0, 0, 0, 36, 36, 36, 36, 0, 0, 0, 0, 0, 0, 0, 0, 36, 42,
    42, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42,
    42, 0, 11, 11, 0, 11, 11, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 11, 11, 0, 0, 0, 0, 22, 0, 0, 0, 11, 11, 0, 0, 0, 0, 0, 11, 11, 0, 0, 42,
    42, 0, 36, 36, 0, 36, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 36, 0, 0, 0, 0, 0, 0, 0, 0, 36, 36, 0, 0, 0, 0, 0, 36, 36, 0, 0, 42,
    42, 24, 0, 0, 0, 0, 0, 0, 11, 11, 11, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 42,
    42, 0, 0, 0, 11, 11, 0, 11, 11, 0, 0, 17, 0, 0, 0, 0, 22, 0, 0, 0, 11, 11, 0, 0, 0, 0, 0, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42,
    42, 0, 0, 0, 36, 36, 0, 36, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 36, 0, 0, 0, 0, 0, 36, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42,
    42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 11, 11, 11, 0, 0, 0, 24, 0, 0, 0, 0, 0, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 42,
    42, 0, 0, 0, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 42,
    42, 0, 0, 0, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 0, 0, 0, 0, 42,
    42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 12, 12, 0, 0, 0, 0, 0, 0, 0, 42,
    42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42,
    42, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 42
};





LevelA::~LevelA()
{
	// freeing up the resources
    delete [] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
	Mix_FreeChunk(m_game_state.hurt_sfx);
	Mix_FreeChunk(m_game_state.coin_sfx);
	Mix_FreeChunk(m_game_state.winner_sfx);
	Mix_FreeChunk(m_game_state.slash_sfx);
    Mix_FreeChunk(m_game_state.kick_sfx);
	delete effects;

}

void LevelA::initialise()
{
	// ----— INITIALISING THE LEVEL ————— //
    // loading the map
    GLuint map_texture_id = Utility::load_texture("assets/tileset_industrial.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 9, 9);

    // loading the player
    GLuint player_texture_id = Utility::load_texture(IDLE_FILEPATH);

    // texture file mappings
    m_idle_texture_id = Utility::load_texture("assets/sprites final/idle.png");
    m_running_texture_id = Utility::load_texture("assets/sprites final/run.png");
    m_walking_texture_id = Utility::load_texture("assets/sprites final/casual_walk.png");
    m_attacking_texture_id = Utility::load_texture("assets/sprites final/attack.png");
    m_crouch_walk_texture_id = Utility::load_texture("assets/sprites final/crouch walk.png");
    m_jump_texture_id = Utility::load_texture("assets/sprites final/jump.png");
    m_rolling_texture_id = Utility::load_texture("assets/sprites final/roll.png");
    m_kick_texture_id = Utility::load_texture("assets/sprites final/kick.png");
    m_hurt_texture_id = Utility::load_texture("assets/sprites final/hurt on ground.png");


    // animation frame arrays
	for (int i = 0; i < 11; i++) m_idle[0][i] = i;
    for (int i = 0; i < 10; i++) m_running[0][i] = i;
    for (int i = 0; i < 8; i++)  m_walking[0][i] = i;
    for (int i = 0; i < 7; i++)  m_attacking[0][i] = i;
    for (int i = 0; i < 10; i++) m_crouch_walk[0][i] = i;
    for (int i = 0; i < 7; i++)  m_rolling[0][i] = i;
    for (int i = 0; i < 6; i++)  m_kick[0][i] = i;
    for (int i = 0; i < 4; i++)  m_jump[0][i] = i;
    for (int i = 0; i < 6; i++)  m_hurt[0][i] = i;

    // In initialise()
    m_animation_speeds[0] = 0.1f;  // idle
    m_animation_speeds[1] = 0.05f; // attack (faster)
    m_animation_speeds[2] = 0.04f; // run (faster)
    m_animation_speeds[3] = 0.05f; // roll (faster)
    m_animation_speeds[4] = 0.05f; // kick (faster)
    m_animation_speeds[5] = 0.08f; // walking



	// player state initialization
	m_player_state = PlayerState::IDLE;
	m_is_attacking = false;
	m_is_crouching = false;
	m_is_running = false;
	m_attack_timer = 0.0f;

	m_player_hurt = false;
    m_hurt_animation_time = 0.0f;
    m_blink_count = 0;

	// loading the effects
    effects = new Effects(glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f), glm::mat4(1.0f));
    effects->start(FADEIN, 0.5f);


	// loading the sounds
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.jump_sfx = Mix_LoadWAV("assets/sounds/jump.wav");
    m_game_state.hurt_sfx = Mix_LoadWAV("assets/sounds/hurt.wav");
	m_game_state.slash_sfx = Mix_LoadWAV("assets/sounds/slash.wav");
    m_game_state.winner_sfx = Mix_LoadWAV("assets/sounds/winner.wav");
	m_game_state.kick_sfx = Mix_LoadWAV("assets/sounds/kick.wav");


    int player_walking_animation[4][4] =
    {
        { 3, 4, 5, -1}, // left
        { 6, 7, 8, -1}, // right
        { 9, 10, 11, -1},   // forward
        { 0, 1, 2, -1}  // back
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -9.8f, 0.0f);  // more "arc" in air

	// player initialization
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        3,                         // animation frame amount
        0,                         // current animation index
        3,                         // animation column amount
        4,                         // animation row amount
        0.8f,                      // width
        0.8f,                      // height
        PLAYER
    );

    // adjust player's initial position to start at the beginning
    m_game_state.player->set_position(glm::vec3(2.5f, -1.5f, 0.0f));
    m_game_state.player->set_lives(g_player_lives);


    // always assign the sound to player after checking
    if (m_game_state.jump_sfx) {
        m_game_state.player->set_jump_sfx(m_game_state.jump_sfx);
    }
    if (m_game_state.hurt_sfx) {
        m_game_state.player->set_hurt_sfx(m_game_state.hurt_sfx);
    }

    m_game_state.player->set_jumping_power(7.0f); 

	// --- ENEMY INITIALIZATION --- //
    // loading the different enemy textures
    int enemy_walking_animation[4][4] =
    {
        { 3, 4, 5, -1}, // left
        { 6, 7, 8, -1}, // right
        { 9, 10, 11, -1},   // forward
        { 0, 1, 2, -1}  // back
    };

	// loading the enemy textures
    GLuint enemy_texture_id = Utility::load_texture("assets/sprites final/enemy_idle.png");
	GLuint bat_texture_id = Utility::load_texture("assets/sprites final/red eye bat.png");

    // texture file mappings
	m_enemy_idle_texture_id = Utility::load_texture("assets/sprites final/enemy_idle.png");
	m_enemy_attack_texture_id = Utility::load_texture("assets/sprites final/enemy_attack.png");
	m_enemy_run_texture_id = Utility::load_texture("assets/sprites final/enemy_run.png");
	m_enemy_jump_texture_id = Utility::load_texture("assets/sprites final/enemy_jump.png");

	// animation frame arrays
	for (int i = 0; i < 4; i++) m_enemy_idle[0][i] = i;
	for (int i = 0; i < 5; i++) m_enemy_attack[0][i] = i;
	for (int i = 0; i < 6; i++) m_enemy_run[0][i] = i;
	for (int i = 0; i < 4; i++) m_enemy_jump[0][i] = i;
	for (int i = 0; i < 5; i++) m_enemy_bat[0][i] = i;


    initialize_enemy_animations();

	// enemy state initialization
    m_game_state.enemies = new Entity[ENEMY_COUNT];

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(
            enemy_texture_id,         // texture id (default human enemy)
            1.0f,                     // speed
            glm::vec3(0.0f, -9.81f, 0.0f), // gravity
            0.0f,                     // no jump
            enemy_walking_animation,  // animation
            0.0f,                     // animation time
            3,                        // animation frames
            0,                        // starting index
            3,                        // cols
            4,                        // rows
            1.25f,                     // width
            1.25f,                    // height
            ENEMY                     // entity type
        );
        m_game_state.enemies[i].set_ai_type(GUARD); // default AI type
        m_game_state.enemies[i].set_ai_state(AIState::IDLE);
    }
    
	// here we set the positions of the enemies, their AI types, and lives
	// first floor enemies
    m_game_state.enemies[0].set_position(glm::vec3(10.0f, -2.0f, 0.0f));
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_lives(2);

    m_game_state.enemies[1].set_position(glm::vec3(25.0f, -2.0f, 0.0f));
    m_game_state.enemies[1].set_ai_type(WALKER);
    m_game_state.enemies[1].set_lives(1);

	// second floor enemies
    m_game_state.enemies[2].set_position(glm::vec3(15.0f, -5.0f, 0.0f));
    m_game_state.enemies[2].set_ai_type(GUARD);
    m_game_state.enemies[2].set_lives(2);

	// this enemy is a flyer - assign bat texture
    m_game_state.enemies[3].set_position(glm::vec3(29.0f, -5.0f, 0.0f));
    m_game_state.enemies[3].set_ai_type(FLYER);
    m_game_state.enemies[3].set_texture_id(bat_texture_id);
    m_game_state.enemies[3].set_animation_indices(m_enemy_bat[0], 5);
    m_game_state.enemies[3].set_animation_cols(5);
    m_game_state.enemies[3].set_animation_rows(1);
    m_game_state.enemies[3].set_animation_frames(5);
    m_game_state.enemies[3].set_lives(2);

	// third floor enemies
    m_game_state.enemies[4].set_position(glm::vec3(12.0f, -8.0f, 0.0f));
    m_game_state.enemies[4].set_ai_type(GUARD);
    m_game_state.enemies[4].set_lives(3);
    m_game_state.enemies[5].set_position(glm::vec3(25.0f, -8.0f, 0.0f));
    m_game_state.enemies[5].set_ai_type(WALKER);
    m_game_state.enemies[5].set_lives(2);
    m_game_state.enemies[6].set_position(glm::vec3(20.0f, -12.0f, 0.0f));
    m_game_state.enemies[6].set_ai_type(GUARD);
    m_game_state.enemies[6].set_lives(3);

	// this enemy is a flyer - assign bat texture
    m_game_state.enemies[7].set_position(glm::vec3(30.0f, -12.0f, 0.0f));
    m_game_state.enemies[7].set_ai_type(FLYER);
    m_game_state.enemies[7].set_texture_id(bat_texture_id);
    m_game_state.enemies[7].set_animation_indices(m_enemy_bat[0], 5);
    m_game_state.enemies[7].set_animation_cols(5);
    m_game_state.enemies[7].set_animation_rows(1);
    m_game_state.enemies[7].set_animation_frames(5);
    m_game_state.enemies[7].set_lives(3);

	// final floor enemies
    m_game_state.enemies[8].set_position(glm::vec3(30.0f, -13.0f, 0.0f));
    m_game_state.enemies[8].set_ai_type(GUARD);
    m_game_state.enemies[8].set_lives(4);

    m_game_state.enemies[9].set_position(glm::vec3(35.0f, -13.0f, 0.0f));
    m_game_state.enemies[9].set_ai_type(GUARD);
	m_game_state.enemies[9].set_lives(5);  // this enemy is tougher

    m_number_of_enemies = ENEMY_COUNT;
    m_current_animation = nullptr;
    m_current_texture = -1;

	// --- HOSTAGE INITIALIZATION --- //
	// hostage texture
	GLuint hostage_texture_id = Utility::load_texture("assets/sprites final/hostage.png");

    int hostage_animation[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

    m_game_state.hostage = new Entity(
        hostage_texture_id,     // texture
        0.0f,                   // speed (doesn't move)
        2.0f, 2.0f,             // width, height
        HOSTAGE                 // type
    );

	// set the hostage's position
    m_game_state.hostage->set_position(glm::vec3(37.0f, -13.0f, 0.0f));
    m_game_state.hostage->set_animation_indices(hostage_animation, 8);
    m_game_state.hostage->set_animation_index(0);
    m_game_state.hostage->set_animation_cols(8);
    m_game_state.hostage->set_animation_rows(1);
    m_game_state.hostage->set_animation_frames(8);
    m_game_state.hostage->set_animation_speed(0.12f);
}


// this function is used to switch the enemy animation
void LevelA::switch_enemy_animation(Entity* enemy, int* animation, int texture_id, float delta_time, int frames, int cols, float speed, bool force_reset = false) {
	// only update if animation or texture changed
    if (enemy->get_texture_id() != texture_id || force_reset) {
        enemy->set_animation_index(0);
        enemy->set_animation_time(0.0f);
    }

    enemy->set_texture_id(texture_id);
    enemy->set_animation_cols(cols);
    enemy->set_animation_rows(1);
    enemy->set_animation_frames(frames);
    enemy->set_animation_indices(animation, frames);

    enemy->set_animation_speed((speed > 0.0f) ? speed : 0.1f);
}

// this function is used to update the enemy state
void LevelA::update_enemy_state(int enemy_index, float delta_time) {
	// get the enemy entity
    Entity* enemy = &m_game_state.enemies[enemy_index];

	// check if the enemy is active
    if (!enemy->get_is_active()) return;

	// update the enemy's animation timer
    if (m_enemy_attack_cooldown[enemy_index] > 0) {
        m_enemy_attack_cooldown[enemy_index] -= delta_time;
    }

	// check if the enemy is a flyer
    if (enemy->get_ai_type() == FLYER) {
        // for flyers, we only use the bat animation
        // so no need to change animations based on state
        return;
    }

	// this is only for the human enemies
	// check if the enemy is attacking
    if (m_enemy_is_attacking[enemy_index]) {
        m_enemy_animation_timers[enemy_index] += delta_time;
        if (m_enemy_animation_timers[enemy_index] >= ENEMY_ATTACK_DURATION) {
            m_enemy_is_attacking[enemy_index] = false;
            m_enemy_animation_timers[enemy_index] = 0.0f;
        }
        switch_enemy_animation(enemy, m_enemy_attack[0], m_enemy_attack_texture_id, delta_time, 5, 5, 0.05f);
        return;
    }

    // check if the enemy is jumping
    if (m_enemy_is_jumping[enemy_index] || enemy->get_velocity().y > 0.1f) {
        switch_enemy_animation(enemy, m_enemy_jump[0], m_enemy_jump_texture_id, delta_time, 4, 4, 0.08f);
        return;
    }

	// check if the enemy is running
    if (fabs(enemy->get_velocity().x) > 0.1f) {
        enemy->set_facing_left(enemy->get_velocity().x < 0);
        switch_enemy_animation(enemy, m_enemy_run[0], m_enemy_run_texture_id, delta_time, 6, 6, 0.08f);
        return;
    }

    switch_enemy_animation(enemy, m_enemy_idle[0], m_enemy_idle_texture_id, delta_time, 4, 4, 0.1f);


	// check if the enemy is in attack state
    if (enemy->get_ai_state() == ATTACKING && !m_enemy_is_attacking[enemy_index]
        && m_enemy_attack_cooldown[enemy_index] <= 0) {

        m_enemy_is_attacking[enemy_index] = true;
        m_enemy_animation_timers[enemy_index] = 0.0f;
        m_enemy_attack_cooldown[enemy_index] = ENEMY_ATTACK_COOLDOWN;
    }
}


// this function is used to initialize the enemy animations
void LevelA::initialize_enemy_animations() {
	// initialize enemy animations
    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_enemy_is_attacking[i] = false;
        m_enemy_is_running[i] = false;
        m_enemy_is_jumping[i] = false;
        m_enemy_animation_timers[i] = 0.0f;
        m_enemy_attack_cooldown[i] = 0.0f;
    }
}


// this function is used to switch the player animation
void LevelA::switch_animation(int* animation, int texture_id, float delta_time, int frames, int cols, float speed) {
	// only update if animation or texture changed
    if (animation != m_current_animation || texture_id != m_current_texture) {
        m_current_animation = animation;
        m_current_texture = texture_id;
        bool was_facing_left = m_game_state.player->is_facing_left();
        m_game_state.player->set_texture_id(texture_id);
        m_game_state.player->set_animation_cols(cols);
        m_game_state.player->set_animation_rows(1);
        m_game_state.player->set_animation_frames(frames);
        m_game_state.player->set_animation_index(0);
        m_game_state.player->set_animation_time(0.0f);
        m_game_state.player->set_animation_indices(animation, frames);
		// set the animation speed
        float anim_speed;
		if (speed > 0.0f) {
			anim_speed = speed;
		}
		else {
            anim_speed = 0.1f;
		}

        m_game_state.player->set_animation_time(anim_speed);
        m_game_state.player->set_facing_left(was_facing_left);
    }
}

// this function is used to update the player state
void LevelA::update_player_state(float delta_time) {
	// combo window duration
    if (m_previous_attacking) {
        m_combo_timer += delta_time;
        if (m_combo_timer > COMBO_WINDOW) {
			// reset combo state
            m_previous_attacking = false;
            m_combo_timer = 0.0f;
            m_combo_active = false;
        }
    }

    // set combo effects
    if (m_combo_active && effects) {
        effects->start(SHAKE, 1.0f);
        effects->start(FLASH, 0.5f);
    }

	// check if the player is attacking and then quickly kicking
    if (m_previous_attacking && m_is_kicking) {
        m_combo_active = true;
    }

	// update the player's animation timer
    if (!m_animation_complete) {
        m_current_anim_timer += delta_time;
        if (m_current_anim_timer >= m_current_anim_duration) {
            m_animation_complete = true;
            m_current_anim_timer = 0.0f;

			// reset the animation
            m_started_attack_effects = false;
        }
    }

	// check if the player is hurt
    if (m_player_hurt) {
        switch_animation(m_hurt[0], m_hurt_texture_id, delta_time, 6, 6, 0.08f);
        return;
    }

	// check if the player is attacking
    if (m_is_attacking) {
		// only trigger effects once per attack animation
        if (!m_started_attack_effects) {
            if (effects) {
                if (m_combo_active) {
                    effects->start(SHAKE, 1.0f);
                    effects->start(FLASH, 0.5f);
                }
                else {
                    effects->start(SHAKE, 0.5f);
                    effects->start(FLASH, 0.3f);
                }
            }
            m_started_attack_effects = true;

            // play sound effect
            if (m_game_state.slash_sfx) {
                Mix_PlayChannel(-1, m_game_state.slash_sfx, 0);
            }
        }

        // regular attack animation
        switch_animation(m_attacking[0], m_attacking_texture_id, delta_time, 7, 7, 0.05f);

        // set animation completion tracking
        m_animation_complete = false;
        m_current_anim_duration = ATTACK_DURATION;
        m_current_anim_timer = 0.0f;

        return;
    }

	// check if the player is kicking
    if (m_is_kicking) {
        // only trigger effects once per kick animation
        if (!m_started_attack_effects) {
            if (effects) {
                effects->start(SHAKE, 0.8f);
                effects->start(FLASH, 0.4f);
            }
            m_started_attack_effects = true;
            // play sound effect
			if (m_game_state.kick_sfx) {
				Mix_PlayChannel(-1, m_game_state.kick_sfx, 0);
			}
        }

        switch_animation(m_kick[0], m_kick_texture_id, delta_time, 6, 6, 0.05f);

        // set animation completion tracking
        m_animation_complete = false;
        m_current_anim_duration = KICK_DURATION;
        m_current_anim_timer = 0.0f;

        return;
    }

    // only reset this flag if we're not attacking or kicking
    m_started_attack_effects = false;

    if (m_is_rolling) {
        switch_animation(m_rolling[0], m_rolling_texture_id, delta_time, 7, 7, 0.05f);
        m_is_rolling = false;
        return;
    }

    if (m_game_state.player->get_velocity().y > 0.1f) {
        switch_animation(m_jump[0], m_jump_texture_id, delta_time, 4, 4, 0.08f);
        return;
    }

    if (m_is_crouching && m_game_state.player->get_movement().x != 0) {
        switch_animation(m_crouch_walk[0], m_crouch_walk_texture_id, delta_time, 10, 10, 0.08f);
        return;
    }

    if (m_is_running) {
        switch_animation(m_running[0], m_running_texture_id, delta_time, 10, 10, 0.04f);
        return;
    }

    if (m_game_state.player->get_movement().x != 0) {
        switch_animation(m_walking[0], m_walking_texture_id, delta_time, 8, 8, 0.08f);
        return;
    }

    // idle animation
    switch_animation(m_idle[0], m_idle_texture_id, delta_time, 11, 11, 0.1f);
}

void LevelA::update(float delta_time) {
    if (effects) effects->update(delta_time);

    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    update_player_state(delta_time);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        Entity& enemy = m_game_state.enemies[i];
        enemy.update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

        update_enemy_state(i, delta_time);

		// handle blinking effect for the enemy
        if (enemy.get_entity_type() == ENEMY && enemy.get_is_active()) {
            if (enemy.is_invincible()) {
                enemy.set_invincibility_timer(enemy.get_invincibility_timer() + delta_time);
                if (enemy.get_invincibility_timer() >= 1.0f) {
                    enemy.set_invincible(false);
                    enemy.set_invincibility_timer(0.0f);
                }
            }
		}       
		// handle enemy attack
        if (enemy.get_ai_state() == ATTACKING && !m_enemy_is_attacking[i] && m_enemy_attack_cooldown[i] <= 0) {
            m_enemy_is_attacking[i] = true;
            m_enemy_animation_timers[i] = 0.0f;
            m_enemy_attack_cooldown[i] = ENEMY_ATTACK_COOLDOWN;

            // only damage the player if close enough and not already invincible
            glm::vec3 player_pos = m_game_state.player->get_position();
            glm::vec3 enemy_pos = enemy.get_position();

            float x_dist = fabs(player_pos.x - enemy_pos.x);
            float y_dist = fabs(player_pos.y - enemy_pos.y);

            if (x_dist < 1.0f && y_dist < 1.0f && !m_game_state.player->is_invincible()) {
                m_game_state.player->take_damage(10);
                g_player_lives = m_game_state.player->get_lives();
                m_game_state.player->set_invincible(true);
                m_game_state.player->set_invincibility_timer(0.0f);

                // play hurt sound
                if (m_game_state.hurt_sfx) {
                    Mix_PlayChannel(-1, m_game_state.hurt_sfx, 0);
                }
            }
        }

    }

	// handle player attacks on enemies
    if (m_is_attacking || m_is_kicking) {
        glm::vec3 player_pos = m_game_state.player->get_position();
        bool facing_left = m_game_state.player->is_facing_left();

        float attack_range = 1.0f; // hitbox range
        glm::vec3 hit_pos = player_pos;
        hit_pos.x += (facing_left ? -attack_range : attack_range);

		// calculate damage based on attack type
        int base_damage;
        int combo_multiplier;

        if (m_is_attacking) { base_damage = 1; }
        else { base_damage = 2; }

        if (m_combo_active) { combo_multiplier = 2; }
        else { combo_multiplier = 1; }

        int total_damage = base_damage * combo_multiplier;

		// check for enemy collisions while attacking or kicking
        for (int i = 0; i < ENEMY_COUNT; i++) {
            Entity& enemy = m_game_state.enemies[i];
            if (!enemy.get_is_active()) continue;
            
            glm::vec3 e_pos = enemy.get_position();
            float dist = fabs(hit_pos.x - e_pos.x);

            if (dist < (enemy.get_width() / 2.0f + 0.5f) && fabs(player_pos.y - e_pos.y) < 1.0f) {
                if (!enemy.is_invincible()) {
                    int lives = enemy.get_lives() - total_damage;
                    enemy.set_lives(lives);
                    enemy.set_invincible(true);
                    enemy.set_invincibility_timer(0.0f);

                    if (lives <= 0) {
                        enemy.deactivate();
                        enemy.set_position(glm::vec3(999.0f, -999.0f, 0.0f)); // remove from collision space
                    }

                    // play enemy hurt sound
                    if (m_game_state.hurt_sfx) {
                        Mix_PlayChannel(-1, m_game_state.hurt_sfx, 0);
                    }

                    // if it's an attack, store the attacking state for combo tracking
                    if (m_is_attacking) {
                        m_previous_attacking = true;
                        m_combo_timer = 0.0f;
                    }
                }
            }
        }

        // reset combo states
        if (m_is_attacking) {
            m_previous_attacking = true;
            m_combo_timer = 0.0f;
        }
        if (m_is_kicking) {
            m_combo_active = false;
        }
    }

	// check if animation is complete
    if (m_animation_complete) {
        m_is_attacking = false;
        m_is_kicking = false;
    }

    if (m_game_state.hostage && m_game_state.hostage->get_is_active()) {
        m_game_state.hostage->update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);
    }

    if (m_game_state.hostage && m_game_state.hostage->get_is_active()) {
        if (m_game_state.player->check_collision(m_game_state.hostage)) {
            // trigger win condition: hostage rescued!
            m_game_state.hostage->deactivate();

            // play winner sound
            Mix_PlayChannel(-1, m_game_state.winner_sfx, 0);

			// set has_won to true, basically the level is completed
            m_game_state.has_won = true;
        }
    }
}


void LevelA::render(ShaderProgram* g_shader_program)
{
    glm::vec3 player_position = m_game_state.player->get_position();

    glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-player_position.x, -player_position.y, 0.0f));

    // apply a shake
    if (effects) {
        view_matrix = effects->apply_shake(view_matrix);
    }
    g_shader_program->set_view_matrix(view_matrix);

	// now render the map and player
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_game_state.enemies[i].render(g_shader_program);
    }

    if (m_game_state.hostage && m_game_state.hostage->get_is_active()) {
        m_game_state.hostage->render(g_shader_program);
    }

    if (m_game_state.has_won && !g_game_won) {
        Utility::draw_text(g_shader_program, Utility::load_texture("assets/font.png"),
            "RESCUED!", 0.5f, -0.25f,
            glm::vec3(m_game_state.player->get_position().x - 0.8f,
                m_game_state.player->get_position().y + 1.5f, 0));
    }

	// had to save the current shader program
    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    
	// render the effects
    if (effects) {
        effects->render();
    }

    // restore game shader if it was changed
    glUseProgram(current_program);

	// set the view matrix for the shader program
    g_shader_program->set_view_matrix(view_matrix);
}



