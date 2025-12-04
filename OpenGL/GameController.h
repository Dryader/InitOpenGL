#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "StandardIncludes.h"
#include "Mesh.h"
#include "Shader.h"
#include "PostProcessor.h"
#include "Camera.h"
#include "Skybox.h"

class GameController : public Singleton<GameController>
{
public:
    // Constructors / Destructors
    GameController();

    ~GameController() override
    {
    }

    // Methods
    void Initialize();
    void RunGame();

private:
    Shader m_shaderColor;
    Shader m_shaderDiffuse;
    Shader m_shaderFont;
    Shader m_shaderPost;
    Shader m_shaderWater;
    vector<Mesh> m_meshes;
    Camera m_camera;
    Skybox m_skybox;
    GLuint vao;
    PostProcessor m_postProcessor;
    double m_gameTime;
    Mesh m_waterPlane;
};

#endif // GAME_CONTROLLER_H
