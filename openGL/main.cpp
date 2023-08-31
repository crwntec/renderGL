#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "callbacks.h"
#include <vector>
#include <time.h>
#include <stdarg.h>
#include <cassert>

#define GL_LOG_FILE "gl.log"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int g_gl_width = WINDOW_WIDTH;
int g_gl_height = WINDOW_HEIGHT;


bool restart_gl_log() {
    FILE* file;
    if (fopen_s(&file, GL_LOG_FILE, "w") != 0) {
        fprintf(stderr,
            "ERROR: could not open GL_LOG_FILE log file %s for writing\n",
            GL_LOG_FILE);
        return false;
    }
    time_t now;
    time(&now);
    char date[26];
    ctime_s(date, sizeof(date), &now);
    fprintf(file, "GL_LOG_FILE log. local time %s", date);
    fclose(file);
    return true;
}

bool gl_log(const char* message, ...) {
    va_list argptr;
    FILE* file;
    if (fopen_s(&file, GL_LOG_FILE, "a") != 0) {
        fprintf(stderr,
            "ERROR: could not open GL_LOG_FILE %s file for appending\n",
            GL_LOG_FILE);
        return false;
    }
    va_start(argptr, message);
    vfprintf(file, message, argptr);
    va_end(argptr);
    fclose(file);
    return true;
}

bool gl_log_err(const char* message, ...) {
    va_list argptr;
    FILE* file;
    if (fopen_s(&file, GL_LOG_FILE, "a") != 0) {
        fprintf(stderr,
            "ERROR: could not open GL_LOG_FILE %s file for appending\n",
            GL_LOG_FILE);
        return false;
    }

    va_start(argptr, message);
    vfprintf(file, message, argptr);
    va_end(argptr);

    va_start(argptr, message);
    vfprintf(stderr, message, argptr);
    va_end(argptr);

    fclose(file);
    return true;
}

void glfw_error_callback(int error, const char* description) {
    gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

void glfw_size_callback(GLFWwindow* window, int width, int height) {
    g_gl_width = width;
    g_gl_height = height;
    //Perspective matrix update
}

std::string readShaderCode(const std::string& filename) {
    std::ifstream shaderFile(filename);
    if (!shaderFile.is_open()) {
        std::cerr << "Error: Unable to open shader file " << filename << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

int main() {
    assert(restart_gl_log());
    gl_log("Starting GLFW %s\n", glfwGetVersionString());
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    gl_log("Creating Window\n");
    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Test", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    /*Set Callbacks*/
    gl_log("Setting Callbacks\n");
    glfwSetKeyCallback(window, key_callback);

    gl_log("Starting GLEW with openGL version %s\n",glGetString(GL_VERSION));
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
    gl_log("Creating Buffers\n");
    // Create VertexBuffers
    std::vector<float> points = {
    -0.5f,  0.5f,  0.0f,
     0.5f, -0.5f,  0.0f,
    -0.5f, -0.5f,  0.0f,
    -0.5f,  0.5f,  0.0f,
     0.5f,  0.5f,  0.0f,
     0.5f, -0.5f,  0.0f,
    };

    std::vector<float> obj2 = {
         0.8f,  0.5f,  0.0f,
         0.8f, -0.5f,  0.0f,
        -0.8f, -0.5f,  0.0f,
    };

    std::vector<GLuint> vbos(2);
    std::vector<GLuint> vaos(2);

    for (int i = 0; i < 2; ++i) {
        glGenBuffers(1, &vbos[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
        glBufferData(GL_ARRAY_BUFFER, (i == 0 ? points : obj2).size() * sizeof(float), (i == 0 ? points : obj2).data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vaos[i]);
        glBindVertexArray(vaos[i]);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[i]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    gl_log("Loading and Compiling Shaders\n");
    //Create and Compile Shaders
    std::string vertex_shader = readShaderCode("test_vs.glsl");
    std::string fragment_shader = readShaderCode("test_fs.glsl");

    const char* vertexShaderSource = vertex_shader.c_str();
    const char* fragmentShaderSource = fragment_shader.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    GLint screenSizeUniform = glGetUniformLocation(shader_programme, "screenSize");
    /* Loop until the user closes the window */
    gl_log("openGL initilized. Ready to render \n");
    while (!glfwWindowShouldClose(window))
    {
        // Set the clear color and clear the buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        glViewport(0, 0, g_gl_width, g_gl_height);

        // Use the shader program
        glUseProgram(shader_programme);
        glUniform2f(screenSizeUniform, g_gl_width, g_gl_height);

        for (int i = 0; i < 2; ++i) {
            glBindVertexArray(vaos[i]);
            glDrawArrays(GL_TRIANGLES, 0, (i == 0 ? points : obj2).size() / 3);
        }

        // Unbind VAOs and shader program
        glBindVertexArray(0);
        glUseProgram(0);

        // Update other events like input handling
        glfwPollEvents();

        // Swap the front and back buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
