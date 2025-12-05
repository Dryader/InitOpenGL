#ifndef STANDARD_INCLUDES_H
#define STANDARD_INCLUDES_H

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#define M_ASSERT(_cond, _msg) \
if (!(_cond)) { \
    OutputDebugStringA("ASSERTION FAILED: "); \
    OutputDebugStringA(_msg); \
    OutputDebugStringA("\n"); \
    MessageBoxA(NULL, _msg, "Assertion Failed", MB_OK | MB_ICONERROR); \
    glfwTerminate(); \
    std::exit(1); \
}
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>


#include <ft2build.h>
#include FT_FREETYPE_H

#include "Singleton.h"
#include "Resolution.h"

using namespace std;

#endif
