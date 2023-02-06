#pragma once

namespace WallpaperAPI
{
  class ShaderProgram
  {
  public:
    ShaderProgram();
    ShaderProgram(const char *vertex, const char *fragment);
    void Create(const char* vertex, const char* fragment);
    unsigned int ShaderProgram::GenerateShader(const char *file, int type);
    void Use();

    void LoadFloat(const char* variable, float f);
    void LoadVec2(const char* variable, const glm::vec2& vec2);
    void LoadVec3(const char* variable, const glm::vec3& vec3);
    void LoadVec4(const char* variable, const glm::vec4& vec4);
    void LoadMatrix4f(const char* variable, float* p_mat4);
  private:
    void IsValidVariable(const char* variable);

    unsigned int m_shaderProgram = 0;

    int m_success = 0;
    char m_infoLog[512] = {0};
  };
}
