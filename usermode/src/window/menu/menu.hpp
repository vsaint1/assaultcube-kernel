#pragma once
#include "../window_manager.h"
#include "../../features/globals.h"

void show_menu(GLFWwindow *window) {
  ImGuiIO &io = ImGui::GetIO();

  if (settings::show_menu)
    glfwFocusWindow(window);

  if (WindowManager::mouse_state(window, GLFW_MOUSE_BUTTON_LEFT)) {
    io.MouseDown[0] = true;
    io.MouseClicked[0] = true;
    io.MouseClickedPos[0].x = io.MousePos.x;
    io.MouseClickedPos[0].y = io.MousePos.y;
  } else
    io.MouseDown[0] = false;

  glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);

  ImGui::SetNextWindowSize(ImVec2(500.0f, 400.0f), ImGuiCond_Once);
  ImGui::Begin(("TempleOS"), &settings::show_menu, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);

  ImGui::BeginTabBar(("##tabs"), ImGuiTabBarFlags_None);
  ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2 - 100, 50));
  ImGui::Text("Hello world");
  ImGui::Checkbox("Performance", &settings::misc::performance);

  ImGui::EndTabBar();
  ImGui::End();
}