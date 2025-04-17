#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

Model::Model(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "[ERROR] Assimp: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "[INFO] Model loaded: " << path << std::endl;
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh* mesh) {
    std::cout << "[INFO] Processing mesh with " << mesh->mNumFaces << " faces" << std::endl;

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];

        if (face.mNumIndices != 3) {
            std::cout << "[WARN] Skipping non-triangle face" << std::endl;
            continue;
        }

        std::vector<float> triangle;
        for (unsigned int j = 0; j < 3; ++j) {
            unsigned int index = face.mIndices[j];
            aiVector3D v = mesh->mVertices[index];
            triangle.push_back(v.x);
            triangle.push_back(v.y);
            triangle.push_back(v.z);
        }
        vertices.push_back(triangle);
    }

    std::cout << "[INFO] Total triangles stored: " << vertices.size() << std::endl;
}

void Model::draw() {
    if (vertices.empty()) {
        std::cout << "[WARN] No geometry loaded in model!" << std::endl;
        return;
    }

    std::cout << "[INFO] Drawing model with " << vertices.size() << " triangles." << std::endl;

    glBegin(GL_TRIANGLES);
    for (const auto& tri : vertices) {
        for (size_t i = 0; i < tri.size(); i += 3) {
            glVertex3f(tri[i], tri[i + 1], tri[i + 2]);
        }
    }
    glEnd();
}
