#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#define WM_RENDER_OPENGL (WM_USER + 1)

#define GL_CHECK(x) x; {int error = 0; if((error = glGetError())) { std::cout << "GL Error: " << error << std::endl; throw std::runtime_error("OpenGL Error"); }}

#include <stdio.h>
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <future>
#include <stack>
#include <queue>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glad/glad.h"
