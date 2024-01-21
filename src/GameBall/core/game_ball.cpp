#include "GameBall/core/game_ball.h"

#include <queue>

#include "GameBall/core/actors/actors.h"
#include "GameBall/logic/obstacles/obstacles.h"
#include "GameBall/logic/units/units.h"

namespace GameBall {

GameBall::GameBall(const GameSettings &settings)
    : GameX::Base::Application(settings) {
  auto extent = FrameExtent();
  float aspect = static_cast<float>(extent.width) / extent.height;
  scene_ = Renderer()->CreateScene();
  film_ = Renderer()->CreateFilm(extent.width, extent.height);
  logic_manager_ = std::make_unique<Logic::Manager>();
  asset_manager_ = std::make_unique<class AssetManager>(Renderer());

  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

GameBall::~GameBall() {
  asset_manager_.reset();
}

void GameBall::OnInit() {
  auto world = logic_manager_->World();

  scene_->SetEnvmapImage(asset_manager_->ImageFile("textures/envmap.hdr"));

  ambient_light_ = scene_->CreateLight<GameX::Graphics::AmbientLight>();
  ambient_light_->SetLight(glm::vec3{0.3});

  directional_light_ = scene_->CreateLight<GameX::Graphics::DirectionalLight>();
  directional_light_->SetLight(glm::vec3{1.0f}, glm::vec3{3.0f, 2.0f, 1.0f});

  auto primary_player = world->CreatePlayer();
  auto enemy_player = world->CreatePlayer();
  auto small_player = world->CreatePlayer();
  auto large_player = world->CreatePlayer();
  auto primary_unit = world->CreateUnit<Logic::Units::RegularBall>(
      primary_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);
  auto enemy_unit = world->CreateUnit<Logic::Units::RegularBall>(
      enemy_player->PlayerId(), glm::vec3{0.0f, 21.0f, -20.0f}, 1.0f, 1.0f);
  auto small_unit = world->CreateUnit<Logic::Units::RegularBall>(
      small_player->PlayerId(), glm::vec3{-12.7f, 0.7f, 20.0f}, 0.7f, 0.343f);
  auto large_unit = world->CreateUnit<Logic::Units::RegularBall>(
      large_player->PlayerId(), glm::vec3{0.0f, 22.0f, 20.0f}, 2.0f, 6.0f);
  auto primary_obstacle = world->CreateObstacle<Logic::Obstacles::Block>(
      glm::vec3{0.0f, -10.0f, 0.0f}, std::numeric_limits<float>::infinity(),
      false, 20.0f);

  primary_player_id_ = primary_player->PlayerId();

  primary_player->SetPrimaryUnit(primary_unit->UnitId());

  VkExtent2D extent = FrameExtent();
  float aspect = static_cast<float>(extent.width) / extent.height;
  camera_ = scene_->CreateCamera(glm::vec3{0.0f, 10.0f, 10.0f},
                                 glm::vec3{0.0f, 0.0f, 0.0f}, 45.0f, aspect,
                                 0.1f, 100.0f);
  camera_controller_ =
      std::make_unique<CameraControllerThirdPerson>(camera_.get(), aspect);

  player_input_controller_ =
      std::make_unique<Logic::PlayerInputController>(this);

  logic_manager_->Start();
}

void GameBall::OnCleanup() {
  logic_manager_->Stop();
  std::queue<Actor *> actors_to_remove;
  for (auto &actor : actors_) {
    actors_to_remove.push(actor.second);
  }

  while (!actors_to_remove.empty()) {
    auto actor = actors_to_remove.front();
    actors_to_remove.pop();
    actors_.erase(actor->SyncedLogicWorldVersion());
    delete actor;
  }
}

void GameBall::OnUpdate() {
  static auto last_time = std::chrono::steady_clock::now();
  auto current_time = std::chrono::steady_clock::now();
  float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(
                         current_time - last_time)
                         .count();
  last_time = current_time;

  auto player_input = player_input_controller_->GetInput();

  {
    std::lock_guard<std::mutex> lock(logic_manager_->logic_mutex_);
    logic_manager_->world_->SyncWorldState(this);
    primary_player_primary_unit_object_id_ = 0;
    auto primary_player = logic_manager_->world_->GetPlayer(primary_player_id_);
    if (primary_player) {
      auto primary_unit =
          logic_manager_->world_->GetUnit(primary_player->PrimaryUnitId());
      if (primary_unit) {
        primary_player_primary_unit_object_id_ = primary_unit->ObjectId();
      }
      primary_player->SetInput(player_input);
    }
  }

  std::queue<Actor *> actors_to_remove;
  for (auto &actor : actors_) {
    if (actor.second->SyncedLogicWorldVersion() ==
        synced_logic_world_version_) {
      actor.second->Update(delta_time);
    } else {
      actors_to_remove.push(actor.second);
    }
  }

  while (!actors_to_remove.empty()) {
    auto actor = actors_to_remove.front();
    actors_to_remove.pop();
    actors_.erase(actor->SyncedLogicWorldVersion());
    delete actor;
  }

  auto actor = GetActor(primary_player_primary_unit_object_id_);
  if (actor) {
    camera_controller_->SetCenter(actor->Position());
  }
  camera_controller_->Update(delta_time);
}

void GameBall::OnRender() {
  auto cmd_buffer = VkCore()->CommandBuffer();
  Renderer()->RenderPipeline()->Render(cmd_buffer->Handle(), *scene_, *camera_,
                                       *film_);

  OutputImage(cmd_buffer->Handle(), film_->output_image.get());
}
void GameBall::CursorPosCallback(double xpos, double ypos) {
  static double last_xpos = xpos;
  static double last_ypos = ypos;
  double dx = xpos - last_xpos;
  double dy = ypos - last_ypos;

  last_xpos = xpos;
  last_ypos = ypos;

  if (!ignore_next_mouse_move_) {
    camera_controller_->CursorMove(dx, dy);
  }

  ignore_next_mouse_move_ = false;
}

}  // namespace GameBall
