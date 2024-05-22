#define GL_SILENCE_DEPRECATION

#include "gif_lib.h"
#include "stdio.h"
#include <GLFW/glfw3.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: vid2ascii <in_file>");
    return 0;
  }

  int error = 0;
  GifFileType *gif = DGifOpenFileName(argv[1], &error);
  if (!gif) {
    printf("Could not open file\n");
    return 1;
  }

  printf("width: %d\n", (int)gif->SWidth);

  GLFWwindow *window;

  if (!glfwInit()) {
    return -1;
  }

  window = glfwCreateWindow(640, 480, "vid2ascii", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  double y = -0.5;
  double inc = -0.01;

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, y);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, y);
    glEnd();

    y += inc;

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  return 0;
}
