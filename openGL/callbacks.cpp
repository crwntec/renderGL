#include <GL/glew.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
	{
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_1:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            case GLFW_KEY_2:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            default:
                break;
        }
	}
}