#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 40
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/himothy.png",
PLATFORM_FILEPATH[] = "assets/platform.png",
ENEMY_FILEPATH[] = "assets/evil_man.png",
ENEMY2_FILEPATH[] = "assets/evil_man2.png",
ENEMY3_FILEPATH[] = "assets/evil_woman.png";

unsigned int LEVEL_DATA_C[] = {
    17, 0, 0, 0, 0, 17, 17, 0, 0, 0, 0, 17, 17, 0, 0, 0, 17, 17, 0, 0, 17, 17, 0, 0, 0, 17, 0, 0, 17, 0, 0, 0, 17, 17, 0, 0, 0, 17, 0, 0,
    17, 0, 17, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 54, 0, 0, 17, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 17, 0, 0, 54, 0, 0, 0, 0,
    17, 17, 0, 0, 0, 54, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 17, 0, 0, 0,
    17, 0, 0, 0, 17, 17, 0, 0, 0, 0, 17, 17, 0, 0, 0, 0, 17, 0, 0, 0, 17, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 17, 17, 0, 0, 17, 17, 0, 0, 0,
    17, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 17, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0,
    17, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 54, 0, 0, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11
};





LevelC::~LevelC()
{
	// freeing up the resources
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    delete[] m_game_state.platforms;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeChunk(m_game_state.hurt_sfx);
    Mix_FreeChunk(m_game_state.coin_sfx);
    delete effects;

}

void LevelC::initialise()
{
    // ----— INITIALISING THE LEVEL ————— //
    // loading the map
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA_C, map_texture_id, 1.0f, 8, 8);

	// loading the player
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    
	// loading the effects
    effects = new Effects(glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f), glm::mat4(1.0f));
    effects->start(FADEIN, 0.5f);

	// loading the sounds
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.jump_sfx = Mix_LoadWAV("assets/sounds/jump.wav");
    m_game_state.hurt_sfx = Mix_LoadWAV("assets/sounds/hurt.wav");
	m_game_state.coin_sfx = Mix_LoadWAV("assets/sounds/coin.wav");
	m_game_state.winner_sfx = Mix_LoadWAV("assets/sounds/winner.wav");


    int player_walking_animation[4][4] =
    {
        { 3, 4, 5, -1}, // left
        { 6, 7, 8, -1}, // right
        { 9, 10, 11, -1},   // forward
        { 0, 1, 2, -1}  // back
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -9.8f, 0.0f);  // more "arc" in air

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
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );

	// player is always facing right
    m_game_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
	// always assign the sound to player after checking
    if (m_game_state.jump_sfx) {
        m_game_state.player->set_jump_sfx(m_game_state.jump_sfx);
    }

    if (m_game_state.hurt_sfx) {
        m_game_state.player->set_hurt_sfx(m_game_state.hurt_sfx);
    }

    m_game_state.player->set_jumping_power(7.0f);  // jump is more responsive

	// --- ENEMY INITIALIZATION --- //
	// loading the different enemy textures
    std::vector<GLuint> enemy_texture_ids;
    enemy_texture_ids.push_back(Utility::load_texture(ENEMY_FILEPATH));
    enemy_texture_ids.push_back(Utility::load_texture(ENEMY2_FILEPATH));
    enemy_texture_ids.push_back(Utility::load_texture(ENEMY3_FILEPATH));

    int enemy_walking_animation[4][4] = {
        {3, 4, 5, -1},     // LEFT
        {6, 7, 8, -1},     // RIGHT
        {9,10,11, -1},     // UP
        {0, 1, 2, -1}      // DOWN
    };
    m_game_state.enemies = new Entity[ENEMY_COUNT];

	// enemy initialization
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        int texture_index = i % enemy_texture_ids.size();
        m_game_state.enemies[i] = Entity(
            enemy_texture_ids[texture_index],
            1.0f,
            glm::vec3(0.0f, -9.81f, 0.0f), // gravity
            0.0f,                          // no jump
            enemy_walking_animation,
            0.0f,
            3,    // animation frames
            0,    // starting index
            3,    // cols
            4,    // rows
            1.0f,
            1.0f,
            ENEMY
        );
        m_game_state.enemies[i].set_ai_type(GUARD);
        m_game_state.enemies[i].set_ai_state(IDLE);
    }

	// enemy 0: static guard near the beginning
    m_game_state.enemies[0].set_ai_type(GUARD);
    m_game_state.enemies[0].set_position(glm::vec3(4.0f, 1.0f, 0.0f));
	// enemy 1: walker
    m_game_state.enemies[1].set_ai_type(WALKER);
    m_game_state.enemies[1].set_position(glm::vec3(8.0f, 2.0f, 0.0f));
	// enemy 2: flyer
    m_game_state.enemies[2].set_ai_type(FLYER);
    m_game_state.enemies[2].set_position(glm::vec3(13.0f, 3.5f, 0.0f));
	// enemy 3: walker
    m_game_state.enemies[3].set_ai_type(WALKER);
    m_game_state.enemies[3].set_position(glm::vec3(18.0f, 1.0f, 0.0f));
	// enemy 4: guard
    m_game_state.enemies[4].set_ai_type(GUARD);
    m_game_state.enemies[4].set_position(glm::vec3(22.0f, 1.0f, 0.0f));
	// enemy 5: flyer
    m_game_state.enemies[5].set_ai_type(FLYER);
    m_game_state.enemies[5].set_position(glm::vec3(25.0f, 4.0f, 0.0f));
	// enemy 6: walker
    m_game_state.enemies[6].set_ai_type(WALKER);
    m_game_state.enemies[6].set_position(glm::vec3(28.0f, 1.0f, 0.0f));
	// enemy 7: guard
    m_game_state.enemies[7].set_ai_type(GUARD);
    m_game_state.enemies[7].set_position(glm::vec3(32.0f, 2.0f, 0.0f));
	// enemy 8: flyer
    m_game_state.enemies[8].set_ai_type(FLYER);
    m_game_state.enemies[8].set_position(glm::vec3(36.0f, 4.5f, 0.0f));
	// enemy 9: walker
    m_game_state.enemies[9].set_ai_type(WALKER);
    m_game_state.enemies[9].set_position(glm::vec3(38.0f, 1.0f, 0.0f));

    m_number_of_enemies = ENEMY_COUNT;
}

void LevelC::update(float delta_time)
{
	// update the effects
    if (effects) {
        effects->update(delta_time);
    }


    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

	// update each enemy
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }

	if (m_game_state.player->get_position().y <= -8.0f) {
		Mix_PlayChannel(-1, m_game_state.winner_sfx, 0);
	}

}


void LevelC::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
	// render each enemy
    for (int i = 0; i < m_number_of_enemies; i++) {
        m_game_state.enemies[i].render(g_shader_program);
    }

    if (effects) {
        effects->render();
    }


}