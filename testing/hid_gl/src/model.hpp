#pragma once
#include "shader.hpp"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Renderer{
    struct Texture {
        GLuint id;
    };

    class Mesh {
    private:
        GLuint vbo, vao, ebo, texture;
        size_t indices_num;

    public:
        Mesh(std::vector<float> data, std::vector<GLuint> indices);

        int loadTexture(const char *path);
        void draw(Shader& shader);
    };

    class Model {
    public:
        std::vector<Mesh> meshes;

        int load(const char *path);
        void draw(Shader& shader);
    private:
        void processModelNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    };
}
