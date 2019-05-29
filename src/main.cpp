#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "init.hh"
#include "program.hh"
#include "model.hh"
#include "camera.hh"


Camera camera;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (camera.first_mouse_move)
    {
        camera.last_mouse_x = xpos;
        camera.last_mouse_y = ypos;
        camera.first_mouse_move = false;
    }

    float offset_x = xpos - camera.last_mouse_x;
    float offset_y = camera.last_mouse_y - ypos;

    camera.last_mouse_x = xpos;
    camera.last_mouse_y = ypos;

    offset_x *= camera.sensitivity;
    offset_y *= camera.sensitivity;

    camera.yaw   += offset_x;
    camera.pitch += offset_y;

    if(camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if(camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    camera.front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front.y = sin(glm::radians(camera.pitch));
    camera.front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(camera.front);
    camera.right = glm::normalize(glm::cross(camera.front, glm::vec3(0.f, 1.f, 0.f)));
    camera.up    = glm::normalize(glm::cross(camera.right, camera.front));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.fov -= yoffset;

    if(camera.fov <= 1.0f)
        camera.fov = 1.0f;
    if(camera.fov >= 45.0f)
        camera.fov = 45.0f;
}

void process_input(GLFWwindow *window, float delta_time)
{
    float delta_speed = camera.speed * delta_time;

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.pos += delta_speed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.pos -= delta_speed * camera.front;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.pos += delta_speed * camera.right;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.pos -= delta_speed * camera.right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.pos += delta_speed * camera.up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.pos -= delta_speed * camera.up;
}


int main()
{
    // window variables
    int window_w = 800;
    int window_h = 600;

    // time variables
    float total_time = 0.f;
    float delta_time = 0.f;
    float last_time = 0.f;

    GLFWwindow *window = Init::init_all(window_w, window_h);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Program program("../shaders/vertex.glsl", "../shaders/fragment.glsl");


    Model obj("../resources/varia-suit/DolBarriersuit.obj");
    //Model obj("../resources/crysis-nano-suit-2/nanosuit.obj");
    //Model obj("../resources/cube/cube.obj");
    //Model obj("../resources/textured-cube/textured-cube.obj");

    // main loop
    while(!glfwWindowShouldClose(window))
    {
        // update delta_time
        total_time = glfwGetTime();
        delta_time = total_time - last_time;
        last_time = total_time;

        // input
        process_input(window, delta_time);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(program.program_id);

        // set uniforms
        program.set_float("total_time", total_time);
        program.set_float("delta_time", delta_time);
        // set lights
        program.set_vec3("ambient_light_color", 0.1f, 0.1f, 0.1f);
        program.set_vec3("diffuse_light_color", 1.0f, 1.0f, 1.0f);
        program.set_vec3("diffuse_light_position", 0.0f, 15.0f, 10.0f);

        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(-0.3, -9.f, 0.f));
        model = glm::rotate(model, total_time * glm::radians(20.f), glm::vec3(0.f, 1.f, 0.f));
        //model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        program.set_mat4("model", model);
        glm::mat4 normal_mat = glm::mat4(1.f);
        normal_mat = glm::rotate(normal_mat, total_time * glm::radians(20.f), glm::vec3(0.f, 1.f, 0.f));
        program.set_mat4("normal_mat", normal_mat);

        glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
        program.set_mat4("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)window_w/(float)window_h, 0.1f, 100.0f);
        program.set_mat4("projection", projection);


        obj.draw(program);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}