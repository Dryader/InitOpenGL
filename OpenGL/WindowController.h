#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H

#include "StandardIncludes.h"

class WindowController : public Singleton<WindowController>
{
public:
    WindowController();
    ~WindowController() override;

    GLFWwindow* GetWindow()
    {
        if (m_window == nullptr) { NewWindow(); }
        return m_window;
    }

    void NewWindow();
    Resolution GetResolution();

private:
    GLFWwindow* m_window;
};

#endif
