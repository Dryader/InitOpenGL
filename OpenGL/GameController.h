#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "StandardIncludes.h"
#include "Mesh.h"
#include "Shader.h"
#include "WindowController.h"
#include "Camera.h"
#include "Skybox.h"

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
    Shader m_shaderColor;
    Shader m_shaderDiffuse;
    Shader m_shaderFont;
    Shader m_shaderSkybox;
    vector<Mesh> m_meshes;
    Mesh m_meshLight;
    Camera m_camera;
    Skybox m_skybox;
};

#endif // GAME_CONTROLLER_H
