#include "model.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Renderer {
    Mesh::Mesh(std::vector<float> data, std::vector<GLuint> indices) {
        this->indices_num = indices.size();

        glGenVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);
        glCreateBuffers(1, &ebo);
        
        glBindVertexArray(vao);

        glNamedBufferStorage(vbo, data.size() * sizeof(float), data.data(), 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Tex coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Tex coords
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
        
    int Mesh::loadTexture(const char *path) {
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
        if(!data)
            return -1;

        glGenTextures(1, &this->texture);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum texFormat = channels == 4 ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, texFormat, 
                width, height, 0, texFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return 0;
    }

    void Mesh::draw(Shader& shader) {
        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        glBindVertexArray(this->vao);
        //glDrawArrays(GL_TRIANGLES, 0, this->vert_num);
        glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

    Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<float> meshData;
        std::vector<GLuint> meshIndices;

        // Vertices' positions and normals
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            const aiVector3D pos = mesh->mVertices[i]; // Position
            const aiVector3D norm = mesh->mNormals[i]; // Normal

            meshData.push_back(pos.x);
            meshData.push_back(pos.y);
            meshData.push_back(pos.z);

            meshData.push_back(norm.x);
            meshData.push_back(norm.y);
            meshData.push_back(norm.z);

            if(mesh->mTextureCoords[0]) {
                const aiVector3D texcords = mesh->mTextureCoords[0][i];

                meshData.push_back(texcords.x);
                meshData.push_back(texcords.y);
            } else {
                meshData.push_back(0.f);
                meshData.push_back(0.f);
            }

        }

        // Indices
        for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                meshIndices.push_back(face.mIndices[j]);
        }

        return Mesh(meshData, meshIndices);
    }
    
    void Model::processModelNode(aiNode *node, const aiScene *scene) {
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            processModelNode(node->mChildren[i], scene);
        }
    }

    int Model::load(const char *path) {
        Assimp::Importer importer;
        //const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            return -1;

        processModelNode(scene->mRootNode, scene);
        return 0;
    }

    void Model::draw(Shader& shader) {
        for(unsigned int i = 0; i < meshes.size(); i++) {
            meshes[i].draw(shader);
        }
    }
}
