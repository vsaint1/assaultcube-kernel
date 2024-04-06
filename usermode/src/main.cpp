#include "driver/driver.h"
#include "window/menu/menu.hpp"

int main(int argc, char *argv[]) {

  static WindowManager manager;

  if (!manager.create("old_but_gold"))
    return -1;

  UNICODE_STRING name;
  RtlInitUnicodeString(&name, L"ac_client.exe");
  int pid = driver.get_process_id(name);
  
  auto module_base = driver.get_module_base(name);

  auto local_player_offset = driver.find_pattern_offset("8B 0D ? ? ? ? 56 57 8B 3D", 2);
  uint32_t local_player = driver.readv<uint32_t>(module_base + local_player_offset);

  while (!manager.should_close()) {

    if (WindowManager::key_state(manager.m_window, GLFW_KEY_END) & 1)
      exit(0);

    manager.pool_events();

    manager.begin_frame();

    int health = driver.readv<int>(local_player + 0xEC);
    std::string player_name = driver.read_str(local_player + 0x205);


    auto fmt = std::format("Name: {}, Health: {}", player_name.c_str(), health);
    ImGui::GetBackgroundDrawList()->AddText(ImVec2(100, 200), IM_COL32_WHITE, fmt.c_str());

    if (GetAsyncKeyState(VK_INSERT) & 1)
      settings::show_menu = !settings::show_menu;

    if (settings::show_menu)
      show_menu(manager.m_window);
    else
      glfwSetWindowAttrib(manager.m_window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);

    if (settings::misc::performance)
      manager.performance_metrics();

    manager.render();
  }

  manager.cleanup();

  return 0;
}