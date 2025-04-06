#include "Scene.h"
#include "Effects.h"

class LevelB : public Scene {
private:
    Effects* effects;

public:

   static constexpr int ENEMY_COUNT = 6;


    // ————— DESTRUCTOR ————— //
    ~LevelB();

    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};