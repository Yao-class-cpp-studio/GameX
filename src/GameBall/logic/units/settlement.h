#include <string>
#ifdef __APPLE__
#include <GLFW/glfw3.h>
#include <dispatch/dispatch.h>

void renderLoop(GLFWwindow* window) {
    // Set the start time
    double startTime = glfwGetTime();

    while (glfwGetTime() - startTime <= 2.0) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwSetWindowShouldClose(window, true);
    glfwDestroyWindow(window);
}

void createWindowAndRunLoop(std::string str) {
    GLFWwindow* window = glfwCreateWindow(600, 50, "window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        // Handle error appropriately
        return;
    }
    glfwSetWindowTitle(window, str.c_str());
    glfwMakeContextCurrent(window);

    // Run the rendering loop on the main thread
    renderLoop(window);
}

void showWindow(std::string str) {
    if (!glfwInit()) {
        return;
    }
    // Use dispatch_async to execute window creation on the main thread
    dispatch_async(dispatch_get_main_queue(), ^{
        createWindowAndRunLoop(str);
    });
}
#else
void showWindow(std::string str) {
    std::cout << str << std::endl;
}
#endif