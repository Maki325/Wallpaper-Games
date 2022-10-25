#include "pch.h"
#include "ShaderProgram.h"

namespace WallpaperAPI
{
  ShaderProgram::ShaderProgram() {}
  ShaderProgram::ShaderProgram(const char *vertex, const char *fragment)
  {
    Create(vertex, fragment);
  }

  void ShaderProgram::Create(const char* vertex, const char* fragment)
  {
    if (m_shaderProgram) return;
    unsigned int vertexShader = GenerateShader(vertex, GL_VERTEX_SHADER);
    unsigned int fragmentShader = GenerateShader(fragment, GL_FRAGMENT_SHADER);

    // link shaders
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    // check for linking errors
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &m_success);
    if (!m_success) {
      glGetProgramInfoLog(m_shaderProgram, 512, NULL, m_infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << m_infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
  }

  void ShaderProgram::Use() {
    glUseProgram(m_shaderProgram);
  }

  unsigned int ShaderProgram::GenerateShader(const char *file, int type) {
    unsigned int shaderId = glCreateShader(type);

    std::ifstream vertexStream(file);
    std::stringstream buffer;
    buffer << vertexStream.rdbuf();
    const std::string contents = buffer.str();
    const char* ptr = contents.c_str();

    glShaderSource(shaderId, 1, &ptr, NULL);

    glCompileShader(shaderId);

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &m_success);
    if (!m_success)
    {
      glGetShaderInfoLog(shaderId, 512, NULL, m_infoLog);
      std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << m_infoLog << std::endl;
    }

    return shaderId;
  }
}
