#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>


void RenderLightSource(unsigned int& LightSourceVAO, unsigned int& LightSourceVBO)
{
    if (LightSourceVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  2.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  2.0f, 0.0f, 1.0f, 1.0f,
             1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &LightSourceVAO);
        glGenBuffers(1, &LightSourceVBO);
        glBindVertexArray(LightSourceVAO);
        glBindBuffer(GL_ARRAY_BUFFER, LightSourceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(LightSourceVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderGround(unsigned int& GroundVAO, unsigned int& GroundVBO)
{
    if (GroundVAO == 0)
    {
        // float quadVertices[] = {
        //     // positions        // texture Coords
        //     -6.0f,  -0.3f, -0.5f, 0.0f, 1.0f,
        //     -6.0f, -0.3f, 12.0f, 0.0f, 0.0f,
        //      6.0f,  -0.3f, -0.5f, 1.0f, 1.0f,
        //      6.0f, -0.3f, 12.0f, 1.0f, 0.0f,
        // };
         float quadVertices[] = {
            // positions        // texture Coords
            -30.0f,  -0.3f, -60.0f, 0.0f, 1.0f,
            -30.0f, -0.3f, 60.0f, 0.0f, 0.0f,
             30.0f,  -0.3f, -60.0f, 1.0f, 1.0f,
             30.0f, -0.3f, 60.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &GroundVAO);
        glGenBuffers(1, &GroundVBO);
        glBindVertexArray(GroundVAO);
        glBindBuffer(GL_ARRAY_BUFFER, GroundVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(GroundVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}