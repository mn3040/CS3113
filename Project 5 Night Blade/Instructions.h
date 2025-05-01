#pragma once
#include "Scene.h"
#include "Effects.h"
#include "ShaderProgram.h"

class Instructions : public Scene {
private:
    GLuint font_texture_id;
    GLuint background_texture_id;
    Effects* effects;

public:
    Instructions();
    ~Instructions();
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
#pragma once
