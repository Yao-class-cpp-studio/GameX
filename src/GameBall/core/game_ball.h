#pragma once

#include "GameBall/core/asset_manager.h"
#include "GameBall/core/camera_third_person.h"
#include "GameBall/core/utils.h"
#include "GameBall/logic/logic.h"

namespace GameBall {

typedef GameX::Base::ApplicationSettings GameSettings;

class GameBall : public GameX::Base::Application {
 public:
  GameBall(const GameSettings &settings);

  ~GameBall();

  void OnInit() override;

  void OnUpdate() override;

  void OnRender() override;

  void OnCleanup() override;

  class AssetManager *AssetManager() {
    return asset_manager_.get();
  }

  GameX::Graphics::PScene Scene() {
    return scene_.get();
  }

  template <class ActorType, class... Args>
  Actor *CreateActor(uint64_t object_id, Args &&...args) {
    auto actor = new ActorType(this, std::forward<Args>(args)...);
    actors_.insert({object_id, actor});
    return actors_.at(object_id);
  }

  Actor *GetActor(uint64_t object_id) {
    if (!actors_.count(object_id)) {
      return nullptr;
    }
    return actors_.at(object_id);
  }

  void CursorPosCallback(double xpos, double ypos) override;

  CameraControllerThirdPerson *CameraController() {
    return camera_controller_.get();
  }

 private:
  friend class Logic::Manager;
  friend class Logic::World;

  GameX::Graphics::UScene scene_;
  GameX::Graphics::UFilm film_;
  uint64_t synced_world_version_{0};
  std::unique_ptr<Logic::Manager> logic_manager_;
  std::unique_ptr<class AssetManager> asset_manager_;
  std::map<uint64_t, Actor *> actors_;
  uint64_t synced_logic_world_version_{0};

  GameX::Graphics::UCamera camera_;
  GameX::Graphics::UAmbientLight ambient_light_;
  GameX::Graphics::UDirectionalLight directional_light_;

  std::unique_ptr<CameraControllerThirdPerson> camera_controller_;

  uint64_t primary_player_id_{0};
  uint64_t primary_player_primary_unit_object_id_{0};
  uint64_t large_player_id_{0};
  uint64_t small_player_id_{0};

  bool ignore_next_mouse_move_{true};

  std::unique_ptr<Logic::PlayerInputController> player_input_controller_;
};
}  // namespace GameBall
