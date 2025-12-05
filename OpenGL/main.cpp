#include "GameController.h"
#include "ToolWindow.h"
#include <windows.h>

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main()
{
    // Create and show the tool window
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    OpenGL::ToolWindow^ toolWindow = gcnew OpenGL::ToolWindow();
    toolWindow->Show();

    // Initialize and run the game
    GameController::GetInstance().Initialize();
    GameController::GetInstance().RunGame();

    return 0;
}
