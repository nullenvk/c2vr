#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Renderer {
    class Shader {
    public:
        GLuint program;

        int load(const char *vertex_path, const char *fragment_path);
        void use();
        
        void setMatrix4(const char *name, glm::mat4 v);
    };
}
