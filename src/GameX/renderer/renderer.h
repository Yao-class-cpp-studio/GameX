#pragma once

#include <iostream>
#include <stack>

#include "GameX/renderer/camera.h"
#include "GameX/renderer/image.h"
#include "GameX/renderer/lights/lights.h"
#include "GameX/renderer/model.h"
#include "GameX/renderer/render_pipeline.h"
#include "GameX/renderer/scene.h"
#include "GameX/utils/utils.h"

namespace GameX::Base {
class Application;
}

namespace GameX::Graphics {

class Renderer {
 public:
  Renderer(Base::Application *app);

  ~Renderer();

  Base::Application *App() const {
    return app_;
  }

  void RegisterSyncObject(grassland::vulkan::DynamicObject *sync_object) {
    registered_sync_objects_.insert(sync_object);
  }

  void UnregisterSyncObject(grassland::vulkan::DynamicObject *sync_object) {
    registered_sync_objects_.erase(sync_object);
  }

  grassland::vulkan::RenderPass *DepthRenderPass() const {
    return depth_render_pass_.get();
  }

  class RenderPipeline *RenderPipeline() const {
    return render_pipeline_.get();
  }

  void SyncObjects() const;

  template <class... Args>
  [[nodiscard]] UScene CreateScene(Args &&...args) {
    return std::make_unique<Scene>(this, std::forward<Args>(args)...);
  }

  template <class... Args>
  [[nodiscard]] UStaticModel CreateStaticModel(Args &&...args) {
    return std::make_unique<StaticModel>(this, std::forward<Args>(args)...);
  }

  template <class... Args>
  [[nodiscard]] UAnimatedModel CreateAnimatedModel(Args &&...args) {
    return std::make_unique<AnimatedModel>(this, std::forward<Args>(args)...);
  }

  template <class... Args>
  [[nodiscard]] UImage CreateImage(Args &&...args) {
    return std::make_unique<Image>(this, std::forward<Args>(args)...);
  }

  [[nodiscard]] UFilm CreateFilm(int width, int height) {
    return RenderPipeline()->CreateFilm(width, height);
  }

  grassland::vulkan::Sampler *LinearSampler() const {
    return linear_sampler_.get();
  }

  grassland::vulkan::Sampler *NearestSampler() const {
    return nearest_sampler_.get();
  }

  grassland::vulkan::Sampler *AnisotropicSampler() const {
    return anisotropic_sampler_.get();
  }

  Image *WhiteImage() const {
    return white_image_.get();
  }

  Image *BlackImage() const {
    return black_image_.get();
  }

  Image *NormalMapImage() const {
    return normal_map_image_.get();
  }

  void AddReleaseCallback(std::function<void()> callback) {
    release_callbacks_.push(std::move(callback));
  }

 private:
  void CreateDepthRenderPass();

  void CreateRenderPipeline();

  void CreateDefaultImagesAndSamplers();

  Base::Application *app_;
  std::set<grassland::vulkan::DynamicObject *> registered_sync_objects_;
  std::unique_ptr<grassland::vulkan::RenderPass> depth_render_pass_;
  std::unique_ptr<class RenderPipeline> render_pipeline_;

  UImage white_image_;
  UImage black_image_;
  UImage normal_map_image_;

  std::unique_ptr<grassland::vulkan::Sampler> linear_sampler_;
  std::unique_ptr<grassland::vulkan::Sampler> nearest_sampler_;
  std::unique_ptr<grassland::vulkan::Sampler> anisotropic_sampler_;
  std::stack<std::function<void()>> release_callbacks_;
};
}  // namespace GameX::Graphics
