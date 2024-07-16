#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGL/Renderer.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"   
#include "OpenGL/VertexArray.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"
#include "OpenGL/BackEnd/BackEnd.h"

#include <glm/glm.hpp>

#include <iostream>

int main(void)
{ 
    float positions[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    BackEnd::Init();

    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 2);
    layout.Push(GL_FLOAT, 2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    Shader shader("res/shaders/vertex.shader", "res/shaders/frag.shader");
    shader.Bind();
    //shader.SetVec4("color", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

    Texture texture("res/textures/marble.jpg");
    texture.Bind();
    shader.SetInt("u_texture", 0);

    shader.Unbind();
    va.Unbind();
    vb.Unbind();
    ib.Unbind();

    Renderer renderer;

    /* Loop until the user closes the window */
    while (BackEnd::IsWindowOpen())
    {
        /* Polls for events */
        BackEnd::BeginFrame();

        /* Clear screen  */
        renderer.Clear();

        /* Drawing */
        renderer.Draw(va, ib, shader);

        /* Swap front and back buffers */
        BackEnd::EndFrame();
    }

    BackEnd::CleanUpGLFW();
    return 0;
}