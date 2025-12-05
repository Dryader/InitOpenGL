#include "GameController.h"
#include "ToolWindow.h"
#include <windows.h>

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main()
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    OpenGL::ToolWindow^ toolWindow = gcnew OpenGL::ToolWindow();
    toolWindow->Show();

    GameController::GetInstance().Initialize();
    GameController::GetInstance().RunGame();

    return 0;
}
