#pragma once
#include "Scene.h"
#include "Effects.h"
#include "ShaderProgram.h"

class Loser : public Scene {
private:
    GLuint font_texture_id;
    GLuint background_texture_id;
    Effects* effects;
    float time_elapsed;

public:
    Loser();
    ~Loser();
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    Effects* get_effects() override { return effects; }
};