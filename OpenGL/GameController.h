#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "StandardIncludes.h"
#include "Mesh.h"
#include "Shader.h"

class GameController : public Singleton<GameController>
{
public:
    // Constructors / Destructors
    GameController();
    virtual ~GameController() { }

    // Methods
    void Initialize();
    void RunGame();

private:
	Shader m_shader;
    Mesh m_mesh;
};

#endif // GAME_CONTROLLER_H
