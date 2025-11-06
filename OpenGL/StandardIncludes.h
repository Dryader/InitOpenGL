#ifndef STANDARD_INCLUDES_H
#define STANDARD_INCLUDES_H

// Include standard headers
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

// Widows specific includes and defines
#ifdef _WIN32
#include <Windows.h>
#define M_ASSERT(_cond, _msg) \
if (!(_cond)) { OutputDebugStringA(_msg); std::abort(); glfwTerminate(); }
#endif

// Openg GL/Helper headers
#include <GL/glew.h>    // Include GLEW
#include <GLFW/glfw3.h> // Include GLFW
#include <glm/glm.hpp>  // Include GLM
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp> // Include GLM extensions


#include <ft2build.h>
#include FT_FREETYPE_H

#include "Singleton.h"
#include "Resolution.h"

using namespace std;

#endif // STANDARD_INCLUDES_H
