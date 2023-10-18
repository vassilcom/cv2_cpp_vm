
#include <glad/glad.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

class win
{
private:
    /* data */
public:
    win(/* args */);
    ~win();

    int init();
    bool loop();
    void pre_renser();
    void post_renser();

    GLFWwindow* window;



};
