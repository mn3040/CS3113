#include "Scene.h"
#include "Effects.h"

class LevelC : public Scene {
private:
	Effects* effects;

public:
    // ————— STATIC ATTRIBUTES ————— //
    static constexpr int ENEMY_COUNT = 10;

    // ————— DESTRUCTOR ————— //
    ~LevelC();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};