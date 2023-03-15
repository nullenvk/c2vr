#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <hidapi/hidapi.h>

#include "shader.hpp"
#include "model.hpp"
#include "ahrs.hpp"
#include "imu.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void glfw_error_callback(int code, const char *desc) {
    std::cerr << "GLFW Error - " << code << ": " << desc << std::endl;
}

GLFWwindow *init_win()
{
    glfwSetErrorCallback(glfw_error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "GLTest", NULL, NULL);
    if(!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }    

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glEnable(GL_DEPTH_TEST);

    return window;
}

int main() 
{
    IMUThread imuthread;
    if(imuthread.start()) return -1;

    GLFWwindow *window = init_win();
    if(!window)
        return -1;

    Renderer::Shader defaultShader;
    if(defaultShader.load("shaders/default.vert", "shaders/default.frag"))
        return -1;

    Renderer::Model model;
    if(model.load("models/cube.obj"))
        return -1;

    for(size_t i = 0; i < model.meshes.size(); i++) {
        if(model.meshes[i].loadTexture("textures/test.png"))
            return -1;
    }

    glm::mat4 matProjection, matView, matModel, matLocal;
    glm::vec3 CameraPos = glm::vec3(0,0,-3.f);

    while(!glfwWindowShouldClose(window)) {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        int winw, winh;
        glfwGetWindowSize(window, &winw, &winh);
        matProjection = glm::perspective(glm::radians(90.0f), (float)winw / (float)winh, 0.1f, 100.0f); 
    
        // Camera handling
        matView = glm::mat4(1.0f);
        //matView = glm::rotate(matView, CameraRot.y, glm::vec3(1,0,0));
        //matView = glm::rotate(matView, CameraRot.x, glm::vec3(0,1,0));
        matView = glm::translate(matView, CameraPos);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        matLocal = glm::mat4(1.0f);
        //matLocal = glm::scale(matLocal, glm::vec3(0.5, 0.5, 0.5));
        
        matModel = glm::mat4(1.0f);
        matModel = glm::translate(matModel, glm::vec3(0.f, 0.f, -3.f));
        matModel = matModel * glm::mat4_cast(imuthread.getQuat()); 

        defaultShader.setMatrix4("transform", matProjection * matView * matModel * matLocal);
        model.draw(defaultShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    imuthread.stop();
    glfwTerminate();
    return 0;
}
