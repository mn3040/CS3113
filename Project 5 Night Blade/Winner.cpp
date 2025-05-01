#include "Winner.h"
#include "Utility.h"

constexpr char FONT_PATH[] = "assets/font.png";
extern bool g_game_won;
constexpr char WINNER_BG_PATH[] = "assets/background_industrial.jpg";

Winner::Winner() : time_elapsed(0.0f) {
    m_game_state.map = nullptr;
    m_game_state.player = nullptr;
    effects = nullptr;
}

Winner::~Winner() {
    delete effects;
}

void Winner::initialise() {
    font_texture_id = Utility::load_texture(FONT_PATH);
    background_texture_id = Utility::load_texture(WINNER_BG_PATH);
    effects = new Effects(glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f), glm::mat4(1.0f));
    effects->start(FADEIN, 0.5f);
    time_elapsed = 0.0f;

    // Play victory sound if available
    if (m_game_state.winner_sfx) {
        Mix_PlayChannel(-1, m_game_state.winner_sfx, 0);
    }

    // Reset the victory state
    g_game_won = false; // Reset the win flag here
}


void Winner::update(float delta_time) {
    time_elapsed += delta_time;

    // Add periodic effects for visual interest
    if (effects) {
        effects->update(delta_time);

        // Add a flash effect every few seconds for celebration
        if (fmod(time_elapsed, 3.0f) < 0.1f) {
            effects->start(FLASH, 0.3f);
        }
    }
}

void Winner::render(ShaderProgram* program) {
    // Render background
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(10.0f, 7.5f, 1.0f));
    program->set_model_matrix(model_matrix);

    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    float tex_coords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    glBindTexture(GL_TEXTURE_2D, background_texture_id);
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());

	// creating a floating effect for the title
    float title_offset = 0.1f * sin(time_elapsed * 2.0f);
    Utility::draw_text(program, font_texture_id, "VICTORY!", 1.0f, 0.03f,
        glm::vec3(-3.3f, 2.0f + title_offset, 0.0f));
    Utility::draw_text(program, font_texture_id, "All hostages rescued!", 0.4f, 0.02f,
        glm::vec3(-4.4f, 0.8f, 0.0f));
    Utility::draw_text(program, font_texture_id, "You have completed your mission", 0.4f, 0.01f,
        glm::vec3(-4.5f, 0.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "and saved the day!", 0.4f, 0.01f,
        glm::vec3(-3.2f, -0.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Press Q to quit", 0.4f, 0.02f,
        glm::vec3(-2.5f, -1.0f, 0.0f));
    if (effects) effects->render();
}