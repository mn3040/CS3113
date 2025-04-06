#include "Menu.h"
#include "Utility.h"

constexpr char FONT_PATH[] = "assets/font.png",
MENU_BACKGROUND_PATH[] = "assets/menu_background.jpg";

Menu::Menu() {
	// set up the game state
	m_game_state.map = nullptr;
	m_game_state.player = nullptr;
	m_game_state.enemies = nullptr;
	m_game_state.bgm = nullptr;
	m_game_state.jump_sfx = nullptr;
	m_game_state.next_scene_id = -0;
	// set up the number of enemies
	m_number_of_enemies = 0;
}

Menu::~Menu() {
    delete effects;
}

void Menu::initialise() {
	// load the font texture
	font_texture_id = Utility::load_texture(FONT_PATH);
    menu_background_texture_id = Utility::load_texture(MENU_BACKGROUND_PATH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	effects = new Effects(glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f), glm::mat4(1.0f));
	effects->start(FADEIN, 0.5f);
}

void Menu::update(float delta_time) {
    if (effects) {
        effects->update(delta_time);
    }
}


void Menu::render(ShaderProgram* program) {
    // background
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(5.0f, -3.75f, 0.0f));
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

    glBindTexture(GL_TEXTURE_2D, menu_background_texture_id);
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());

    Utility::draw_text(program, font_texture_id, "Rise of the AI", 0.5f, 0.02f, glm::vec3(-2.5f, 2.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Press Enter to Start", 0.5f, 0.02f, glm::vec3(-3.5f, 0.5f, 0.0f));



    // fade-in effect render
    if (effects) {
        effects->render();
    }
}

