#pragma once

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include "../utils/macros.h"
#include <Windows.h>

class WindowManager {

  int m_width, m_height;

public:

  GLFWwindow *m_window;

  bool create(const char *window_name);

  bool should_close();

  void pool_events();

  void render();

  void begin_frame();

  void cleanup();

  void performance_metrics();

  static bool mouse_state(GLFWwindow *window, int key);

  static bool key_state(GLFWwindow *window, int key);

  int get_width() { return this->m_width;}
  
  int get_height() { return this->m_height; }

private:
  std::pair<float, float> get_target_window_size(const char *window_name);

};