// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#ifdef WIN32
#include <tchar.h>
#include <Windows.h>
#include <stdint.h>
#endif

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <fstream>
#include <list>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

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

const int CHAR_ADVANCE = 7;
const int CHAR_LINE_HEIGHT = 13;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;


// TODO: reference additional headers your program requires here
