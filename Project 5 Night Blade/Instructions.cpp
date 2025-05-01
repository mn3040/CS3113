#include "Instructions.h"
#include "Utility.h"

constexpr char FONT_PATH[] = "assets/font.png";
constexpr char INSTRUCTIONS_BG_PATH[] = "assets/instructions.jpg"; // or use the same as menu

Instructions::Instructions() {
    m_game_state.map = nullptr;
    m_game_state.player = nullptr;
    effects = nullptr;
}

Instructions::~Instructions() {
    delete effects;
}

void Instructions::initialise() {
    font_texture_id = Utility::load_texture(FONT_PATH);
    background_texture_id = Utility::load_texture(INSTRUCTIONS_BG_PATH);
    effects = new Effects(glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f), glm::mat4(1.0f));
    effects->start(FADEIN, 0.5f);
}

void Instructions::update(float delta_time) {
    if (effects) effects->update(delta_time);
}

void Instructions::render(ShaderProgram* program) {
    // render background
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

	// render text
    Utility::draw_text(program, font_texture_id, "How to Play", 0.6f, 0.02f, glm::vec3(-2.8f, 2.5f, 0.0f));

    Utility::draw_text(program, font_texture_id, "Move: Arrow Keys", 0.4f, 0.02f, glm::vec3(-3.6f, 1.4f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Jump: SPACE", 0.4f, 0.02f, glm::vec3(-3.6f, 0.9f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Run: SHIFT", 0.4f, 0.02f, glm::vec3(-3.6f, 0.4f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Attack: Z", 0.4f, 0.02f, glm::vec3(-3.6f, -0.1f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Roll: X", 0.4f, 0.02f, glm::vec3(-3.6f, -0.6f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Kick: C", 0.4f, 0.02f, glm::vec3(-3.6f, -1.1f, 0.0f));

    Utility::draw_text(program, font_texture_id, "Goal: Rescue all hostages", 0.35f, 0.01f, glm::vec3(-4.25f, -1.8f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Press B to return", 0.4f, 0.02f, glm::vec3(-3.3f, -2.4f, 0.0f));

    if (effects) effects->render();
}
