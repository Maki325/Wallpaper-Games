#include "pch.h"
#include "Loader.h"
#include "Entity/Entity.h"
#include "Utils/Utils.h"
#include <string>
#include <charconv>

namespace WallpaperAPI
{
  namespace Loader
  {
    Model LoadObj(const std::string& path)
    {
      std::ifstream file(path);
      if (!file.is_open())
      {
        throw std::runtime_error("Couldn't open obj file: " + path);
      }
      std::vector<Vertex> vertices;
      std::vector<glm::vec3> positions;
      std::vector<glm::vec2> textures;
      std::vector<glm::vec3> normals;
      std::vector<glm::uvec3> indices;

      std::string line;
      while (std::getline(file, line)) {
        if (Utils::StartsWith(line, "vn"))
        {
          auto values = Utils::Split(line, " ");

          float x = 0, y = 0, z = 0;
          std::from_chars(values.at(1).data(), values.at(1).data() + values.at(1).size(), x);
          std::from_chars(values.at(2).data(), values.at(2).data() + values.at(2).size(), y);
          std::from_chars(values.at(3).data(), values.at(3).data() + values.at(3).size(), z);

          normals.push_back({ x, y, z });
        }
        else if (Utils::StartsWith(line, "vt"))
        {
          auto values = Utils::Split(line, " ");

          float x = 0, y = 0;
          std::from_chars(values.at(1).data(), values.at(1).data() + values.at(1).size(), x);
          std::from_chars(values.at(2).data(), values.at(2).data() + values.at(2).size(), y);

          textures.push_back({ x, y });
        }
        else if (Utils::StartsWith(line, "v"))
        {
          auto values = Utils::Split(line, " ");

          float x = 0, y = 0, z = 0;
          std::from_chars(values.at(1).data(), values.at(1).data() + values.at(1).size(), x);
          std::from_chars(values.at(2).data(), values.at(2).data() + values.at(2).size(), y);
          std::from_chars(values.at(3).data(), values.at(3).data() + values.at(3).size(), z);

          positions.push_back({ x, y, z });
          vertices.push_back({ { x, y, z }, {0, 0} });
        }
        else if (Utils::StartsWith(line, "f"))
        {
          auto faces = Utils::Split(line, " ");

          glm::uvec3 index(0);
          for (size_t i = 1; i < faces.size(); i++)
          {
            auto values = Utils::Split(faces.at(i), "/");

            int pos = 0, tex = 0, normal = 0;
            std::from_chars(values.at(0).data(), values.at(0).data() + values.at(0).size(), pos);
            std::from_chars(values.at(1).data(), values.at(1).data() + values.at(1).size(), tex);
            std::from_chars(values.at(2).data(), values.at(2).data() + values.at(2).size(), normal);

            index[i - 1] = pos - 1;
            vertices.at(pos - 1).texture = textures.at(tex - 1);
          }

          indices.push_back(index);
        }
      }
      file.close();

      return { vertices, indices };
    }
  }
}

