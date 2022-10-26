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

    void LoadMatrix4f(const char* variable, float *p_mat4);
  private:
    unsigned int m_shaderProgram = 0;

    int m_success = 0;
    char m_infoLog[512] = {0};
  };
}
