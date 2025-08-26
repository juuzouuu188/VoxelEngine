#pragma once
#include <glad/glad.h>      
#include <GLFW/glfw3.h>     

class Window {


public:
	Window(int width, int height, const char* title);
	~Window();

	GLFWwindow* getGLFWwindow() const { return window; }
	bool shouldClose() const { return glfwWindowShouldClose(window); }
	void swapBuffers() { glfwSwapBuffers(window); }
	void pollEvents() { glfwPollEvents(); }



private:
	GLFWwindow* window;
};
