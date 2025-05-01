#include "Effects.h"

Effects::Effects(glm::mat4 projection_matrix, glm::mat4 view_matrix)
    : m_current_effect(NONE),
    m_alpha(1.0f),
    m_effect_speed(1.0f),
    m_size(10.0f),
    m_shake_intensity(0.0f),
    m_flash_size(1.0f),
    m_flash_active(false),
    m_shake_active(false)
{
    // Non-textured Shader for basic effects
    m_shader_program.load("shaders/vertex.glsl", "shaders/fragment.glsl");
    m_shader_program.set_projection_matrix(projection_matrix);
    m_shader_program.set_view_matrix(view_matrix);

	// flash shader program
    m_flash_shader_program.load("shaders/vertex_flash.glsl", "shaders/fragment_flash.glsl");
    m_flash_shader_program.set_projection_matrix(projection_matrix);
    m_flash_shader_program.set_view_matrix(view_matrix);
}

void Effects::draw_overlay()
{
    glUseProgram(this->m_shader_program.get_program_id());

    float vertices[] =
    {
        -5.0f, -3.75f,
         5.0f, -3.75f,
         5.0f,  3.75f,

        -5.0f, -3.75f,
         5.0f,  3.75f,
        -5.0f,  3.75f
    };

    glVertexAttribPointer(m_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(m_shader_program.get_position_attribute());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(m_shader_program.get_position_attribute());
}

void Effects::draw_flash_overlay()
{
    glUseProgram(this->m_flash_shader_program.get_program_id());

    float vertices[] =
    {
        -5.0f, -3.75f,
         5.0f, -3.75f,
         5.0f,  3.75f,

        -5.0f, -3.75f,
         5.0f,  3.75f,
        -5.0f,  3.75f
    };

    glVertexAttribPointer(m_flash_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(m_flash_shader_program.get_position_attribute());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(m_flash_shader_program.get_position_attribute());
}

void Effects::start(EffectType effect_type, float effect_speed)
{
    m_effect_speed = effect_speed;

    switch (effect_type)
    {
    case NONE:
        // Reset all effects
        m_current_effect = NONE;
        m_shake_active = false;
        m_flash_active = false;
        m_shake_intensity = 0.0f;
        m_alpha = 0.0f;
        break;

    case FADEIN:
        m_current_effect = FADEIN;
        m_alpha = 1.0f;
        break;

    case FADEOUT:
        m_current_effect = FADEOUT;
        m_alpha = 0.0f;
        break;

    case GROW:
        m_current_effect = GROW;
        m_size = 0.0f;
        break;

    case SHRINK:
        m_current_effect = SHRINK;
        m_size = 10.0f;
        break;

    case SHAKE:
        // trigger shake effect
        m_shake_active = true;
		m_shake_intensity = 0.2f * effect_speed; // adjusted intensity
        break;

    case FLASH:
		// trigger flash effect
        m_flash_active = true;
		m_alpha = 0.2f * effect_speed;
        m_flash_size = 1.0f;
        break;
    }
}


void Effects::update(float delta_time)
{
	// handle all effects
    switch (m_current_effect)
    {
    case NONE:
        break;

    case FADEIN:
        m_alpha -= delta_time * m_effect_speed;
        if (m_alpha <= 0) {
            m_alpha = 0.0f;
            m_current_effect = NONE;
        }
        break;

    case FADEOUT:
        if (m_alpha < 1.0f) {
            m_alpha += delta_time * m_effect_speed;
        }
        if (m_alpha >= 1.0f) {
            m_alpha = 1.0f;
        }
        break;

    case GROW:
        if (m_size < 10.0f) {
            m_size += delta_time * m_effect_speed;
        }
        if (m_size >= 10.0f) {
            m_size = 10.0f;
            m_current_effect = NONE;
        }
        break;

    case SHRINK:
        if (m_size >= 0.0f) {
            m_size -= delta_time * m_effect_speed;
        }
        if (m_size <= 0.0f) {
            m_size = 0.0f;
            m_current_effect = NONE;
        }
        break;
    }

	// handle shake effect
    if (m_shake_active) {
        if (m_shake_intensity > 0.0f) {
            // faster decay for shake effect
            m_shake_intensity -= delta_time * m_effect_speed * 2.0f;
            if (m_shake_intensity <= 0.0f) {
                m_shake_intensity = 0.0f;
                m_shake_active = false;
            }
        }
    }

    // handle flash effect
    if (m_flash_active) {
        // use faster decay for flash effect
        m_alpha *= pow(0.01f, delta_time * m_effect_speed * 2.0f);
        m_flash_size += delta_time * 1.5f;

        if (m_alpha <= 0.01f) {
            m_alpha = 0.0f;
            m_flash_active = false;
        }
    }
}

void Effects::render()
{
	// had to find a way to restore OpenGL state
	// was causing issues with other shaders
    GLint original_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &original_program);

    GLint original_blend_enabled;
    glGetIntegerv(GL_BLEND, &original_blend_enabled);

    GLint original_blend_src, original_blend_dst;
    glGetIntegerv(GL_BLEND_SRC, &original_blend_src);
    glGetIntegerv(GL_BLEND_DST, &original_blend_dst);

	// handle all effects
    switch (m_current_effect)
    {
    case GROW:
    case SHRINK:
    case FADEOUT:
    case FADEIN:
    {
        // Use our shader program
        glUseProgram(m_shader_program.get_program_id());

        // Set blend mode for fade effects
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::mat4 model_matrix = glm::mat4(1.0f);
        model_matrix = glm::scale(model_matrix,
            glm::vec3(m_size,
                m_current_effect != GROW && m_current_effect != SHRINK ?
                m_size : m_size * 0.75f,
                0.0f));

        m_shader_program.set_model_matrix(model_matrix);
        m_shader_program.set_colour(0.0f, 0.0f, 0.0f, m_alpha);
        draw_overlay();
    }
    break;

    case NONE:
        // Nothing to render for NONE
        break;
    }

    // handle FLASH effect - use additive blending to create a bright flash
    // instead of a dark overlay
    if (m_flash_active && m_alpha > 0.01f) {
        // use the flash shader
        glUseProgram(m_flash_shader_program.get_program_id());

        glm::mat4 flash_model = glm::scale(glm::mat4(1.0f), glm::vec3(m_flash_size, m_flash_size, 1.0f));
        m_flash_shader_program.set_model_matrix(flash_model);

		// set blend mode for flash effect
        m_flash_shader_program.set_colour(1.0f, 1.0f, 1.0f, m_alpha * 0.5f);
        draw_flash_overlay();
    }

	// after rendering effects, restore OpenGL state
    if (original_blend_enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(original_blend_src, original_blend_dst);
    }
    else {
        glDisable(GL_BLEND);
    }

    glUseProgram(original_program);
}


// apply shake effect to the view matrix
glm::mat4 Effects::apply_shake(glm::mat4 view_matrix)
{
    if (m_shake_active && m_shake_intensity > 0.0f) {
        float x_shake = ((rand() % 1000) / 1000.0f - 0.5f) * m_shake_intensity;
        float y_shake = ((rand() % 1000) / 1000.0f - 0.5f) * m_shake_intensity;
        view_matrix = glm::translate(view_matrix, glm::vec3(x_shake, y_shake, 0.0f));
    }
    return view_matrix;
}