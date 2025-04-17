#include "Model.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <queue>
using namespace std;


Model* disk;
Model* padLeft;
Model* padRight;

bool isBraking = false;
float rotationSpeed = 300.0f;
float rotationAngle = 0.0f;
float padOffset = 0.2f; // 바깥쪽
float padLeftRotation = 0.0f;
float padRightRotation = 0.0f;



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE) {
        isBraking = (action != GLFW_RELEASE);
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT) {
            padLeftRotation += 5.0f;  // 왼쪽 패드 회전 증가
        }
        if (key == GLFW_KEY_RIGHT) {
            padRightRotation -= 5.0f; // 오른쪽 패드 회전 감소
        }
    }
}



void init() {
    disk = new Model("models/disk_fixed.obj");
    padLeft = new Model("models/pad_left_fixed.obj");
    padRight = new Model("models/pad_right_fixed.obj");
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);

}

void update(float deltaTime) {
    // 디스크 회전 제어
    if (isBraking) {
        rotationSpeed -= 400.0f * deltaTime;
        padOffset = std::max(padOffset - 0.5f * deltaTime, 0.05f); // 안쪽으로
    } else {
        rotationSpeed = std::min(rotationSpeed + 100.0f * deltaTime, 300.0f);
        padOffset = std::min(padOffset + 0.5f * deltaTime, 0.2f); // 원위치
    }

    rotationAngle += rotationSpeed * deltaTime;
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // [1] PROJECTION: 카메라 렌즈 설정
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 400.0);

    // [2] MODELVIEW: 카메라 위치 설정
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        0.0, 0.0, 15.0,    // 카메라 위치
        0.0, 0.0, 0.0,     // 바라보는 중심
        0.0, 1.0, 0.0      // 위쪽 방향
    );



    // // 디스크
    // glPushMatrix();
    // glColor3f(0.0f, 0.0f, 1.0f); // 파란색
    // glScalef(1.0f, 1.0f, 1.0f);
    // disk->draw();
    // glPopMatrix();

    // 디스크 회전
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f); 
    glRotatef(rotationAngle, 0, 0, 1);
    glScalef(1.0f, 1.0f, 1.0f);
    disk->draw();
    glPopMatrix();

    // 왼쪽 패드 이동 + 회전
    glPushMatrix();
    glTranslatef(-padOffset, 0.0f, 0.0f);
    glRotatef(padLeftRotation, 0, 0, 1);  // Z축 회전
    glScalef(1.0f, 1.0f, 1.0f);
    padLeft->draw();
    glPopMatrix();

    // 오른쪽 패드 이동 + 회전
    glPushMatrix();
    glTranslatef(padOffset, 0.0f, 0.0f);
    glRotatef(padRightRotation, 0, 0, 1); // Z축 회전
    glScalef(1.0f, 1.0f, 1.0f);
    padRight->draw();
    glPopMatrix();


    glutSwapBuffers();
}

int main() {
    // GLFW 초기화
    if (!glfwInit()) {
        return -1;
    }

    // OpenGL 2.1 context 요청
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Brake Simulation", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glEnable(GL_DEPTH_TEST); // Z-buffer

    init(); // 모델 로드

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        update(deltaTime);
        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}