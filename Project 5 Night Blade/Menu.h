#pragma once

#include "Effects.h"
#include "Scene.h"
#include "ShaderProgram.h"

class Menu : public Scene {
private:
	// ————— ATTRIBUTES ————— //
	// we need to load the font texture
	GLuint font_texture_id;
	GLuint menu_background_texture_id;
	Effects* effects;  // NEW: fade-in effect
	float total_time = 0.0f;


public:
	Menu();
	~Menu();
	void initialise() override;
	void update(float delta_time) override;
	void render(ShaderProgram *program) override;
};
