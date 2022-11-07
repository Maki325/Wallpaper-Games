#pragma once

#define GL_CHECK(x) x; {int error = 0; if((error = glGetError())) { std::cout << "GL Error: " << error << std::endl; throw std::runtime_error("OpenGL Error"); }}

#define INFO(x) std::cout << x << '\n';
