#pragma once

#include "GameX/physics/physics.h"
#include "GameX/renderer/renderer.h"
#include "GameX/utils/utils.h"

const uint16_t DEFAULT_PORT = 1115;

namespace GameX::Base {

struct ApplicationSettings {
  bool fullscreen{false};
  int width{-1};
  int height{-1};
  std::string mode;
  std::string address;
  int port{DEFAULT_PORT};
};

class Application {
 public:
  Application(const ApplicationSettings &settings);

  virtual ~Application();

  GLFWwindow *Window() const {
    return window_;
  }

  grassland::vulkan::Core *VkCore() const {
    return vk_core_.get();
  }

  class Graphics::Renderer *Renderer() const {
    return renderer_.get();
  }

  void Run();

  void OutputImage(VkCommandBuffer cmd_buffer, grassland::vulkan::Image *image);

  VkExtent2D FrameExtent() const {
    return VkCore()->SwapChain()->Extent();
  }

  virtual void CursorPosCallback(double xpos, double ypos) {
  }

  virtual void MouseButtonCallback(int button, int action, int mods) {
  }

  virtual void CursorEnterCallback(int entered) {
  }

  virtual void KeyCallback(int key, int scancode, int action, int mods) {
  }

  virtual void DropCallback(int count, const char **paths) {
  }

  virtual void ScrollCallback(double xoffset, double yoffset) {
  }

  virtual void FramebufferSizeCallback(int width, int height) {
  }

  virtual void WindowSizeCallback(int width, int height) {
  }

 protected:
  void Init();

  void Update();

  void Render();

  void Cleanup();

  virtual void OnInit() {
  }

  virtual void OnUpdate() {
  }

  virtual void OnRender() {
  }

  virtual void OnCleanup() {
  }

  ApplicationSettings settings_;

  GLFWwindow *window_;

  std::unique_ptr<grassland::vulkan::Core> vk_core_;

  std::unique_ptr<Graphics::Renderer> renderer_;
};
}  // namespace GameX::Base

#include "GameX/renderer/model.inl"
