#pragma once
#include <vector>
#include <string>
#include <assimp/scene.h>
#include <GLUT/glut.h>

class Model {
public:
    Model(const std::string& path);
    void draw();

private:
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh);
    std::vector<std::vector<float>> vertices;
};
