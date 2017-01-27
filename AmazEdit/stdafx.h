// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#ifdef WIN32
#include <tchar.h>
#include <Windows.h>
#endif

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <fstream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std::chrono_literals;

using std::cout;
using std::cin;
using std::endl;
using std::cerr;



// TODO: reference additional headers your program requires here
