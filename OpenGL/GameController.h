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
    Shader m_shaderSkybox;
    vector<Mesh> m_meshes;
    Camera m_camera;
    Skybox m_skybox;
    GLuint vao;
    PostProcessor m_postProcessor;
    double m_gameTime;
    Mesh m_waterPlane;
    Mesh m_spaceship;
    Mesh m_asteroids;
    bool m_previousWaterSceneEnabled = false;
    bool m_previousSpaceSceneEnabled = false;
};

#endif // GAME_CONTROLLER_H
