#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

static GLuint shader_load_single(const char *path, GLuint type) {
    GLuint shader; 
    std::string source;
    int result;

    shader = glCreateShader(type);

    std::ifstream f(path);
    if(!f) {
        std::cerr << "Couldn't load shader \"" << path << "\"\n";
        return 0;
    }

    source = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    const char *sources[] = {source.c_str()};
    glShaderSource(shader, 1, sources, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(!result) {
        char infolog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infolog);

        std::cerr << "Couldn't compile shader \"" << path << "\" - Infolog (max 1024 chars):\n" << infolog;
        return 0;
    }

    return shader;
}

namespace Renderer {
    int Shader::load(const char *vertex_path, const char *fragment_path) {
        GLuint program, vertex, fragment;
        int result;

        vertex = shader_load_single(vertex_path, GL_VERTEX_SHADER);
        fragment = shader_load_single(fragment_path, GL_FRAGMENT_SHADER);

        if(!vertex || !fragment)
            return -1;
        
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &result);
        if(!result)
            return -1;

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        this->program = program;
        return 0;
    }

    void Shader::use() {
        glUseProgram(this->program);
    }
        
    void Shader::setMatrix4(const char *name, glm::mat4 v) {
        GLuint matLoc = glGetUniformLocation(this->program, name);
        glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(v));
    }
}
