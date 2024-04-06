#include "window_manager.h"
#include <thread>

bool WindowManager::create(const char *window_name) {

  if (!glfwInit())
    return false;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);
  glfwWindowHint(GLFW_FLOATING, 1);
  glfwWindowHint(GLFW_MAXIMIZED, 1);
  glfwWindowHint(GLFW_RESIZABLE, false);

  auto [width, height] = this->get_target_window_size(GAME_WINDOW_NAME);

  this->m_width = width;
  this->m_height = height;

  GLFWwindow *window = glfwCreateWindow(width, height, window_name, nullptr, nullptr);
  glfwSetWindowAttrib(window, GLFW_DECORATED, false);

  if (window == nullptr)
    return false;

  this->m_window = window;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(GLFW_TRUE);

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsClassic();

  ImGui_ImplGlfw_InitForOpenGL(window, false);

  ImGui_ImplOpenGL3_Init(GLSL_VERSION);

  return true;
}

bool WindowManager::should_close() { return glfwWindowShouldClose(this->m_window); }

void WindowManager::pool_events() {
  glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, 1);

  glfwPollEvents();
}

void WindowManager::render() {
  ImGui::Render();
  int display_w, display_h;

  glfwGetFramebufferSize(this->m_window, &display_w, &display_h);

  glViewport(0, 0, display_w, display_h);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(this->m_window);
}

void WindowManager::performance_metrics() {
  ImGui::SetNextWindowSize(ImVec2(300.0f, 80.0f), ImGuiCond_Once);
  ImGui::SetWindowPos(ImVec2(0, 0));

  ImGui::Begin(("Metrics - %s", "jose"));
  auto &io = ImGui::GetIO();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  int thread_count = std::thread::hardware_concurrency();
  ImGui::Text("Thread count: %d", thread_count);
  ImGui::End();
}

void WindowManager::begin_frame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void WindowManager::cleanup() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(this->m_window);
  glfwTerminate();
}

std::pair<float, float> WindowManager::get_target_window_size(const char *window_name) {

#if _WIN32
  HWND hwnd = FindWindowA(NULL, window_name);

  if (hwnd == NULL) {
    MessageBox(nullptr, L"Window Not found", L"ERROR", MB_ICONERROR);
    return {};
  }

  RECT rect;
  GetWindowRect(hwnd, &rect);

  return {rect.right - rect.left, rect.bottom - rect.top};
#else
  // TODO: Implement
  return {};

#endif
}

bool WindowManager::mouse_state(GLFWwindow *window, int key) { return glfwGetMouseButton(window, key) == GLFW_PRESS; }

bool WindowManager::key_state(GLFWwindow *window, int key) { return glfwGetKey(window, key); }