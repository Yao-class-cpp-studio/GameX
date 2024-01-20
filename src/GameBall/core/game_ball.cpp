#include "GameBall/core/game_ball.h"

#include <atomic>
#include <queue>
#include <sstream>

#include "GameBall/core/actors/actors.h"
#include "GameBall/core/p2pnode.h"
#include "GameBall/logic/obstacles/obstacles.h"
#include "GameBall/logic/units/units.h"
#include "packet_tool.h"

namespace GameBall {

struct InputPacket {
  uint64_t player_id = -1;
  Logic::PlayerInput input;
};

std::thread listen_thread;
std::queue<InputPacket> input_queue;
extern std::map<uint64_t, std::queue<std::string>> video_stream_map;
extern std::map<uint64_t, uint64_t> room_id_to_client_id;
std::queue<uint64_t> remove_queue;
std::atomic<bool> is_running(true);

uint64_t data_id = 0;

void GameBall::receivePackets() {
  auto world_ = logic_manager_->World();

  // Room mode logics
  if (settings_.mode == "room") {
    std::cout << "Server started." << std::endl;

    while (is_running) {
      auto [msg, c_ip, c_port] = world_->game_node_.receive();

      if (msg == "join") {
        {
          std::lock_guard<std::mutex> lock(logic_manager_->logic_mutex_);
          auto new_player = world_->CreatePlayer();
          auto new_unit = world_->CreateUnit<Logic::Units::RegularBall>(
              new_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);

          new_player->SetNetInfo(c_ip, c_port);
          new_player->SetPrimaryUnit(new_unit->UnitId());

          world_->game_node_.send(std::to_string(new_player->PlayerId()), c_ip,
                                  c_port);
        }
        continue;
      }

      // Split the msg, and check if the command is quit.
      size_t pos = msg.find(':');
      if (!(pos != std::string::npos && pos > 0 &&
            std::all_of(msg.begin(), msg.begin() + pos, ::isdigit))) {
        continue;
      }
      uint64_t id = std::stoull(msg.substr(0, pos));
      std::string command = msg.substr(pos + 1);

      // (A Open Bug) TODO: Fix the bug of client quit
      // GameX still not have a proper scheme to
      // erase the ball picture when the client quit.
      // So we just remove the ball from the world.
      // without cleaning picture.
      // Hope someone can fix this in the future.

      if (command == "quit") {
        {
          std::lock_guard<std::mutex> lock(logic_manager_->logic_mutex_);

          remove_queue.push(id);
          world_->game_node_.send("Bye", c_ip, c_port);
        }
        continue;
      }

      // If the command is not quit, then it is a sync of user input, just push
      // it to the input queue.
      InputPacket inputPacket;
      inputPacket.player_id = id;
      inputPacket.input = StringToPlayerInput(command);
      {
        std::lock_guard<std::mutex> lock(logic_manager_->logic_mutex_);
        input_queue.push(inputPacket);
      }
    }
    std::cout << "Server stopped." << std::endl;
    return;
  }

  // Client Logics
  if (settings_.mode == "client") {
    std::cout << "Client started." << std::endl;
    while (is_running) {
      auto [msg, c_ip, c_port] = world_->game_node_.receive();
      std::map<uint64_t, bool> valid_ball_map;

      valid_ball_map[primary_player_id_] = true;

      std::stringstream ss(msg);
      std::string tmp_str;

      while (std::getline(ss, tmp_str, ',')) {
        std::lock_guard<std::mutex> lock(logic_manager_->logic_mutex_);

        std::stringstream fs(tmp_str);
        std::string idStr;
        std::string data;
        if (std::getline(fs, idStr, ':') && std::getline(fs, data)) {
          uint64_t id_in_room = std::stoull(idStr);

          // Check if the id is in the map
          if (room_id_to_client_id.find(id_in_room) ==
              room_id_to_client_id.end()) {
            auto new_player = world_->CreatePlayer();
            auto new_unit = world_->CreateUnit<Logic::Units::RegularBall>(
                new_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f,
                1.0f);

            new_player->SetPrimaryUnit(new_unit->UnitId());

            room_id_to_client_id[id_in_room] = new_player->PlayerId();
            video_stream_map[new_player->PlayerId()] =
                std::queue<std::string>();

            valid_ball_map[new_player->PlayerId()] = true;
          } else {
            valid_ball_map[room_id_to_client_id.at(id_in_room)] = true;
          }

          // Push the data to the video stream queue

          uint64_t real_id = room_id_to_client_id.at(id_in_room);
          video_stream_map.at(real_id).push(data);

          if (video_stream_map.at(real_id).size() > 128) {
            std::queue<std::string> emptyQueue;
            video_stream_map.at(real_id).swap(emptyQueue);
          } else if (video_stream_map.at(real_id).size() > 64) {
            video_stream_map.at(real_id).pop();
            // video_stream_map.at(real_id).pop();
          }
        }
      }

      // Check for lost ball by going through the video

      for (auto &pair : video_stream_map) {
        auto player = world_->GetPlayer(pair.first);
        if (!valid_ball_map[pair.first]) {
          // Clear the ball!
          video_stream_map.erase(player->PlayerId());
          for (auto it = room_id_to_client_id.begin();
               it != room_id_to_client_id.end();) {
            if (it->second == player->PlayerId()) {
              it = room_id_to_client_id.erase(it);
            } else {
              ++it;
            }
          }
          auto remove_unit_id = player->PrimaryUnitId();
          world_->RemovePlayer(player->PlayerId());
          world_->RemoveUnit(remove_unit_id);
        }
      }
    }
    std::cout << "Client stopped." << std::endl;
  }
}

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

  if (settings_.mode == "client") {
    world->game_node_.is_server = false;
    world->game_node_.initialize(DEFAULT_PORT + 1);

  } else if (settings_.mode == "room") {
    world->game_node_.is_server = true;
    world->game_node_.initialize(settings_.port);

  } else if (settings_.mode == "local") {
    world->game_node_.is_server = false;
    // Do nothing here, just like before.
  }

  if (settings_.mode == "room") {
    auto ip_list = getLocalIPs();
    for (auto &ip : ip_list) {
      std::cout << "Server IP: " << ip << std::endl;
    }
  }

  scene_->SetEnvmapImage(asset_manager_->ImageFile("textures/envmap.hdr"));

  ambient_light_ = scene_->CreateLight<GameX::Graphics::AmbientLight>();
  ambient_light_->SetLight(glm::vec3{0.3});

  directional_light_ = scene_->CreateLight<GameX::Graphics::DirectionalLight>();
  directional_light_->SetLight(glm::vec3{1.0f}, glm::vec3{3.0f, 2.0f, 1.0f});

  base64Init();

  if (settings_.mode == "room") {
    auto primary_player = world->CreatePlayer();

    auto primary_unit = world->CreateUnit<Logic::Units::RegularBall>(
        primary_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);

    primary_player_id_ = primary_player->PlayerId();

    primary_player->SetPrimaryUnit(primary_unit->UnitId());

    std::thread t1(&GameBall::receivePackets, this);
    listen_thread = std::move(t1);
    listen_thread.detach();
  }

  if (settings_.mode == "local") {
    auto primary_player = world->CreatePlayer();
    auto enemy_player = world->CreatePlayer();

    auto primary_unit = world->CreateUnit<Logic::Units::RegularBall>(
        primary_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);
    auto enemy_unit = world->CreateUnit<Logic::Units::RegularBall>(
        enemy_player->PlayerId(), glm::vec3{-5.0f, 1.0f, 0.0f}, 1.0f, 1.0f);

    primary_player_id_ = primary_player->PlayerId();

    primary_player->SetPrimaryUnit(primary_unit->UnitId());
  }

  if (settings_.mode == "client") {
    auto primary_player = world->CreatePlayer();

    auto primary_unit = world->CreateUnit<Logic::Units::RegularBall>(
        primary_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);

    primary_player_id_ = primary_player->PlayerId();

    primary_player->SetPrimaryUnit(primary_unit->UnitId());

    world->game_node_.send("join", settings_.address, settings_.port);
    auto [msg, c_ip, c_port] = world->game_node_.receive();

    data_id = std::stoull(msg);
    printf("Login Finished!\n");

    room_id_to_client_id[data_id] = primary_player->PlayerId();
    video_stream_map[primary_player->PlayerId()] = std::queue<std::string>();

    std::thread t1(&GameBall::receivePackets, this);
    listen_thread = std::move(t1);
    listen_thread.detach();
  }

  auto primary_obstacle = world->CreateObstacle<Logic::Obstacles::Block>(
      glm::vec3{0.0f, -10.0f, 0.0f}, std::numeric_limits<float>::infinity(),
      false, 20.0f);

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
  std::queue<uint64_t> actors_to_remove;

  for (auto &actor : actors_) {
    actors_to_remove.push(actor.first);
  }

  while (!actors_to_remove.empty()) {
    auto actor_id = actors_to_remove.front();
    actors_to_remove.pop();

    auto remove_actor_pointer = actors_.at(actor_id);
    actors_.erase(actor_id);
    if (remove_actor_pointer)
      delete remove_actor_pointer;
  }

  is_running = false;
  if (listen_thread.joinable()) {
    listen_thread.join();
  }

  if (settings_.mode == "client") {
    auto world = logic_manager_->World();
    ;
    world->game_node_.send(std::to_string(data_id) + ":quit", settings_.address,
                           settings_.port);
  }
}

Logic::PlayerInput last_input;

void GameBall::OnUpdate() {
  static auto last_time = std::chrono::steady_clock::now();
  auto current_time = std::chrono::steady_clock::now();
  float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(
                         current_time - last_time)
                         .count();
  last_time = current_time;

  auto player_input = player_input_controller_->GetInput();

  // Client only collect user inputs, and send them to the server
  if (settings_.mode == "client") {
    if (player_input != last_input) {
      last_input = player_input;
      auto send_input =
          std::to_string(data_id) + ":" + PlayerInputToString(player_input);
      logic_manager_->world_->game_node_.send(send_input, settings_.address,
                                              settings_.port);
    }
  }

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

    if (settings_.mode == "room") {
      if (!input_queue.empty()) {
        auto inputPacket = input_queue.front();
        input_queue.pop();
        auto player = logic_manager_->world_->GetPlayer(inputPacket.player_id);
        if (player) {
          player->SetInput(inputPacket.input);
        }
      }
    }

    if (settings_.mode == "room") {
      float ball_data[48];
      std::string msg_broadcast;

      while (!remove_queue.empty()) {
        auto remove_player_id = remove_queue.front();
        auto remove_unit_id =
            logic_manager_->world_->GetPlayer(remove_player_id)
                ->PrimaryUnitId();
        remove_queue.pop();
        logic_manager_->world_->RemovePlayer(remove_player_id);
        logic_manager_->world_->RemoveUnit(remove_unit_id);

        std::cout << "Player " << remove_player_id << " quit." << std::endl;
      }

      auto &client_list = logic_manager_->world_->player_map_;

      for (auto &pair : client_list) {
        auto player = pair.second;
        if (player) {
          auto unit = logic_manager_->world_->GetUnit(player->PrimaryUnitId());
          if (unit) {
            auto sphere_id = player->PrimaryUnitId();
            auto &sphere =
                logic_manager_->world_->PhysicsWorld()->GetSphere(sphere_id);
            auto regular_ball = dynamic_cast<Logic::Units::RegularBall *>(
                logic_manager_->world_->GetUnit(sphere_id));
            ball_data[0] = sphere.position.x;
            ball_data[1] = sphere.position.y;
            ball_data[2] = sphere.position.z;
            ball_data[3] = sphere.velocity.x;
            ball_data[4] = sphere.velocity.y;
            ball_data[5] = sphere.velocity.z;
            ball_data[6] = regular_ball->AngularMomentum().x;
            ball_data[7] = regular_ball->AngularMomentum().x;
            ball_data[8] = regular_ball->AngularMomentum().x;
            ball_data[9] = sphere.orientation[0][0];
            ball_data[10] = sphere.orientation[0][1];
            ball_data[11] = sphere.orientation[0][2];
            ball_data[12] = sphere.orientation[1][0];
            ball_data[13] = sphere.orientation[1][1];
            ball_data[14] = sphere.orientation[1][2];
            ball_data[15] = sphere.orientation[2][0];
            ball_data[16] = sphere.orientation[2][1];
            ball_data[17] = sphere.orientation[2][2];
          }
          msg_broadcast += std::to_string(player->PrimaryUnitId()) + ":" +
                           encode_state(ball_data) + ",";
        }
      }

      for (auto &pair : client_list) {
        auto player = pair.second;
        if (player && player->PlayerId() !=
                          primary_player_id_) {  // Do not send it to the host
          logic_manager_->world_->game_node_.send(
              msg_broadcast, player->GetIp(), player->GetPort());
        }
      }
    }

    // std::queue<Actor *> actors_to_remove;
    std::queue<uint64_t> actors_to_remove;
    for (auto &actor : actors_) {
      if (actor.second->SyncedLogicWorldVersion() ==
          synced_logic_world_version_) {
        actor.second->Update(delta_time);
      } else {
        // actors_to_remove.push(actor.second);
        actors_to_remove.push(actor.first);
      }
    }

    while (!actors_to_remove.empty()) {
      // auto actor = actors_to_remove.front();
      auto actor_id = actors_to_remove.front();
      actors_to_remove.pop();
      // actors_.erase(actor->SyncedLogicWorldVersion());
      auto remove_actor_pointer = actors_.at(actor_id);
      actors_.erase(actor_id);
      // delete actor;
      if (remove_actor_pointer)
        delete remove_actor_pointer;
    }

    auto actor = GetActor(primary_player_primary_unit_object_id_);
    if (actor) {
      camera_controller_->SetCenter(actor->Position());
    }
    camera_controller_->Update(delta_time);
  }
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
