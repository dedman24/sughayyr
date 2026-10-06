#include "renderer/renderer.h"

// stdlib includes.
#include "stdio.h"
#include "stdbool.h"

// posix includes.
#include "unistd.h"

// objectives:
//  draw simple stuff to screen.                                X
//  abstract everything into its own function.                  X
//  worry about 3d graphics l8r on.

void framebuffer_size_callback(GLFWwindow* const restrict window, const int width, const int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window){
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
    usleep(200000);
    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);

    if(polygonMode[0] == GL_FILL)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if(polygonMode[0] == GL_LINE)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

int main() {
  if(!glfwInit()){
    fprintf(stderr, "Failed to initialize GLFW.\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "sughayyr game engine v0.1.", NULL, NULL);
  if(!window){
    fprintf(stderr, "ERROR: 'glfwCreateWindow' failed.\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)){
    fprintf(stderr, "ERROR: 'gladLoadGLLoader' failed.\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }
  glViewport(0, 0, 800, 600);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
// shader program.
  const GLuint shaderProgram = shader_compile("resources/shaders/vertex.vert", "resources/shaders/fragment.frag");
  if(!shaderProgram){
    glfwDestroyWindow(window);
    fprintf(stderr, "ERROR: could not create shader program.\n");
    return 1;
  }

  modelT* const restrict model = model_load("resources/models/square.obj", "resources/textures/container.jpg");
  if(!model){
    fprintf(stderr, "ERROR: 'model_load' failed.\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
  }

  glUseProgram(shaderProgram);
// draws stuff to screen luv <333.
  while(!glfwWindowShouldClose(window)){
  // input.
    processInput(window);
  // rendering.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

  // draw our first triangle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->id_texture);

    glBindVertexArray(model->VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

  // check call events & swap buffers.
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  model_destroy(model);

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
