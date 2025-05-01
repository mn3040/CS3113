#include "Loser.h"
#include "Utility.h"

constexpr char FONT_PATH[] = "assets/font.png";
constexpr char LOSER_BG_PATH[] = "assets/background_industrial.jpg"; // or use a darker version

Loser::Loser() : time_elapsed(0.0f) {
    m_game_state.map = nullptr;
    m_game_state.player = nullptr;
    effects = nullptr;
}

Loser::~Loser() {
    delete effects;
}

void Loser::initialise() {
    font_texture_id = Utility::load_texture(FONT_PATH);
    background_texture_id = Utility::load_texture(LOSER_BG_PATH);
    effects = new Effects(glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f), glm::mat4(1.0f));

    // Start with a shake effect for losing
    effects->start(SHAKE, 0.7f);
    effects->start(FADEIN, 0.5f);

    time_elapsed = 0.0f;

    // Play defeat sound if available
    if (m_game_state.hurt_sfx) {
        Mix_PlayChannel(-1, m_game_state.hurt_sfx, 0);
    }
}

void Loser::update(float delta_time) {
    time_elapsed += delta_time;

    if (effects) {
        effects->update(delta_time);

        // Add periodic shake effects for a more dramatic feel
        if (fmod(time_elapsed, 4.0f) < 0.1f) {
            effects->start(SHAKE, 0.5f);
        }
    }
}

void Loser::render(ShaderProgram* program) {
    // Render background (slightly darker for defeat feel)
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

    // Draw title with slight animation
    float title_offset = 0.05f * sin(time_elapsed * 3.0f);
    Utility::draw_text(program, font_texture_id, "GAME OVER", 1.0f, 0.03f,
        glm::vec3(-4.0f, 2.0f + title_offset, 0.0f));

    // Draw defeat messages
    Utility::draw_text(program, font_texture_id, "Mission Failed!", 0.5f, 0.02f,
        glm::vec3(-3.5f, 0.8f, 0.0f));

    Utility::draw_text(program, font_texture_id, "The hostages weren't rescued", 0.3f, 0.01f,
        glm::vec3(-4.5f, 0.0f, 0.0f));

    Utility::draw_text(program, font_texture_id, "Press Q to quit", 0.4f, 0.02f,
        glm::vec3(-2.5f, -0.8f, 0.0f));

    // Render any active effects
    if (effects) effects->render();
}