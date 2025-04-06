#include "Scene.h"
#include "Effects.h"


class LevelA : public Scene {
private:
    Effects* effects;

public:
    // ————— STATIC ATTRIBUTES ————— //
    static constexpr int ENEMY_COUNT = 5;
    
	GLuint font_texture_id;

    // ————— DESTRUCTOR ————— //
    ~LevelA();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
