#pragma once
#include "Mesh.h"
#include "ToolWindow.h"

#pragma managed

void Mesh::GetSpecularSettings(float& _strength, float& _colorR, float& _colorG, float& _colorB)
{
    using namespace OpenGL;
    _strength = ToolWindow::SpecularStrength;
    _colorR = ToolWindow::SpecularColorR;
    _colorG = ToolWindow::SpecularColorG;
    _colorB = ToolWindow::SpecularColorB;
}

#pragma unmanaged

