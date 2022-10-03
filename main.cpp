#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Camera.hpp"
#include "Shader.hpp"
#include "Utility.hpp"
#include <iostream>
#include <vector>
#include "LTCMap.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float current_frame = 0.0f;
float last_frame = 0.0f;
float delta_frame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

unsigned int LightSourceVAO = 0;
unsigned int LightSourceVBO = 0;
unsigned int GroundVAO = 0;
unsigned int GroundVBO = 0;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(1200, 900, "LTC", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    Shader PlaneLightSahder("../shader/PlaneLight.vs","../shader/PlaneLight.fs");
    Shader GroundSahder("../shader/Ground.vs","../shader/Ground.fs");
    glm::vec3 DiffuseColor = glm::vec3(1.0f,1.0f,1.0f);
    glm::vec3 SpecularColor = glm::vec3(0.23f,0.23f,0.23f);
    float LightIntensity = 1.0f;
    float Roughness = 0.25f;
    float Intensity = 1.1f;
    std::vector<glm::vec3> PolygonalLightVertexPos;
    PolygonalLightVertexPos.push_back(glm::vec3(-1.0f,2.0f,0.0f));
    PolygonalLightVertexPos.push_back(glm::vec3(1.0f,2.0f,0.0f));
    PolygonalLightVertexPos.push_back(glm::vec3(1.0f,0.0f,0.0f));
    PolygonalLightVertexPos.push_back(glm::vec3(-1.0f,0.0f,0.0f));

    glm::vec3 v1 = glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 v2 = glm::vec3(0.0f,2.0f,0.0f);
    glm::vec3 v3 = glm::vec3(0.0f,0.0f,3.0f);
    glm::mat3 m = glm::mat3(v1,v2,v3);
    std::cout<<m[0][0]<<std::endl;
  

    auto LTC = loadLTCTextures(); 
    bindTextures(LTC);
    GroundSahder.use();
    GroundSahder.setInt("LTC_1", 0);
    GroundSahder.setInt("LTC_2", 1);
    for(int i = 0; i < PolygonalLightVertexPos.size(); ++i) {
        GroundSahder.setVec3("PolygonalLightVertexPos[" + std::to_string(i) + "]",PolygonalLightVertexPos[i]);
    }

    // for(int i  = 0; i < 4*64*64; ++i) {
    //     if((i+1) % 4 == 3) {
    //         std::cout<<data1[i]<<std::endl;
    //     }
    // }

    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window))
    {
        current_frame = glfwGetTime();
        delta_frame = current_frame - last_frame;
        last_frame = current_frame;
        // input
        processInput(window);
        // render
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1500.0f);
        
        glm::vec3 LightSourceColor = glm::vec3(1.0f);
        PlaneLightSahder.use();
        PlaneLightSahder.setMat4("Model", model);
        PlaneLightSahder.setMat4("View", view);
        PlaneLightSahder.setMat4("Projection", projection);
        PlaneLightSahder.setFloat("LightIntensity", LightIntensity);
        PlaneLightSahder.setVec3("LightSourceColor", LightSourceColor);
        RenderLightSource(LightSourceVAO, LightSourceVBO);


        GroundSahder.use();
        GroundSahder.setMat4("Model", model);
        GroundSahder.setMat4("View", view);
        GroundSahder.setMat4("Projection", projection);
        GroundSahder.setVec3("CameraPos", camera.Position);
        GroundSahder.setVec3("DiffuseColor", DiffuseColor);
        GroundSahder.setVec3("SpecularColor", SpecularColor);
        GroundSahder.setFloat("Roughness", Roughness);
        GroundSahder.setFloat("Intensity", Intensity);
        RenderLightSource(LightSourceVAO, LightSourceVBO);
        RenderGround(GroundVAO, GroundVBO);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        camera.MovementSpeedUp();
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        camera.MovementSpeedDown();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, delta_frame);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, delta_frame);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, delta_frame);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, delta_frame);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}