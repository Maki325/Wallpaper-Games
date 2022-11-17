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
    GL_CHECK(glAttachShader(m_shaderProgram, vertexShader));
    GL_CHECK(glAttachShader(m_shaderProgram, fragmentShader));
    GL_CHECK(glLinkProgram(m_shaderProgram));
    // check for linking errors
    GL_CHECK(glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &m_success));
    if (!m_success) {
      glGetProgramInfoLog(m_shaderProgram, 512, NULL, m_infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << m_infoLog << std::endl;
    }

    GL_CHECK(glDeleteShader(vertexShader));
    GL_CHECK(glDeleteShader(fragmentShader));
  }

  void ShaderProgram::Use() {
    GL_CHECK(glUseProgram(m_shaderProgram));
  }

  void ShaderProgram::LoadFloat(const char* variable, float f)
  {
    IsValidVariable(variable);
    unsigned int transformLoc = glGetUniformLocation(m_shaderProgram, variable);
    GL_CHECK(glUniform1f(transformLoc, f));
  }

  void ShaderProgram::LoadVec3(const char* variable, glm::vec3& vec3)
  {
    IsValidVariable(variable);
    unsigned int transformLoc = glGetUniformLocation(m_shaderProgram, variable);
    GL_CHECK(glUniform3f(transformLoc, vec3.x, vec3.y, vec3.z));
  }

  void ShaderProgram::LoadVec4(const char* variable, glm::vec4& vec4)
  {
    IsValidVariable(variable);
    unsigned int transformLoc = glGetUniformLocation(m_shaderProgram, variable);
    GL_CHECK(glUniform4f(transformLoc, vec4.r, vec4.g, vec4.b, vec4.a));
  }

  void ShaderProgram::LoadMatrix4f(const char* variable, float *p_mat4)
  {
    IsValidVariable(variable);
    unsigned int transformLoc = glGetUniformLocation(m_shaderProgram, variable);
    GL_CHECK(glUniformMatrix4fv(transformLoc, 1, GL_FALSE, p_mat4));
  }

  void ShaderProgram::IsValidVariable(const char* variable)
  {
    size_t len = strnlen(variable, 255);
    for (size_t i = 0; i < len; i++)
    {
      if (!isalnum(variable[i]))
      {
        throw std::runtime_error("Unsupported character in shader uniform name!");
      }
    }
  }

  unsigned int ShaderProgram::GenerateShader(const char *file, int type) {
    unsigned int shaderId = glCreateShader(type);

    std::ifstream vertexStream(file);
    std::stringstream buffer;
    buffer << vertexStream.rdbuf();
    const std::string contents = buffer.str();
    const char* ptr = contents.c_str();

    GL_CHECK(glShaderSource(shaderId, 1, &ptr, NULL));

    GL_CHECK(glCompileShader(shaderId));

    GL_CHECK(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &m_success));
    if (!m_success)
    {
      GL_CHECK(glGetShaderInfoLog(shaderId, 512, NULL, m_infoLog));
      std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << m_infoLog << std::endl;
    }

    return shaderId;
  }
}
