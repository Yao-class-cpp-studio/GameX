#include "GameBall/core/game_ball.h"

#include <atomic>
#include <queue>
#include <sstream>

#include "GameBall/core/actors/actors.h"
#include "GameBall/core/p2pnode.h"
#include "GameBall/logic/obstacles/obstacles.h"
#include "GameBall/logic/units/units.h"

namespace GameBall {

struct InputPacket{
  uint64_t player_id = -1;
  Logic::PlayerInput input;
};

std::thread listener;
std::queue<InputPacket> input_queue;
std::atomic<bool> is_running(true);

std::string PlayerInputToString(const Logic::PlayerInput &input) {
  std::string str;
  str += input.move_forward ? '1' : '0';
  str += input.move_backward ? '1' : '0';
  str += input.move_left ? '1' : '0';
  str += input.move_right ? '1' : '0';
  str += input.speed_up ? '1' : '0';
  str += input.brake ? '1' : '0';
  str += input.left_arrow ? '1' : '0';
  str += input.right_arrow ? '1' : '0';
  str += input.grow ? '1' : '0';
  str += input.shrink ? '1' : '0';
  str += input.restart ? '1' : '0';

  std::ostringstream oss;
  oss << "(" << input.orientation.x << "," << input.orientation.y << ","
      << input.orientation.z << ")";
  str += oss.str();

  return str;
}

Logic::PlayerInput StringToPlayerInput(const std::string &str) {
  Logic::PlayerInput input;
  input.move_forward = (str[0] == '1');
  input.move_backward = (str[1] == '1');
  input.move_left = (str[2] == '1');
  input.move_right = (str[3] == '1');
  input.speed_up = (str[4] == '1');
  input.brake = (str[5] == '1');
  input.left_arrow = (str[6] == '1');
  input.right_arrow = (str[7] == '1');
  input.grow = (str[8] == '1');
  input.shrink = (str[9] == '1');
  input.restart = (str[10] == '1');

  std::istringstream iss(str.substr(11));
  char left_bracket, comma1, comma2, right_bracket;
  iss >> left_bracket >> input.orientation.x >> comma1 >> input.orientation.y >>
      comma2 >> input.orientation.z >> right_bracket;

  return input;
}

static const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
  }

  return ret;
}

std::vector<unsigned char> base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::vector<unsigned char> ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = std::find(base64_chars, base64_chars + 64, char_array_4[i]) - base64_chars;

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret.push_back(char_array_3[i]);
      i = 0;
    }
  }

  if (i) {
    for (j = 0; j < i; j++)
      char_array_4[j] = std::find(base64_chars, base64_chars + 64, char_array_4[j]) - base64_chars;

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

    for (j = 0; (j < i - 1); j++)
      ret.push_back(char_array_3[j]);
  }

  return ret;
}

void GameBall::receivePackets() {
  std::cout << "Server started." << std::endl;
  auto world_ = logic_manager_->World();

  while (is_running) {
    auto [msg, c_ip, c_port] = world_->game_node_.receive();

    if (msg == "join") {
      {
        std::lock_guard<std::mutex> lock(logic_manager_->logic_mutex_);
        auto new_player = world_->CreatePlayer();
        auto new_unit = world_->CreateUnit<Logic::Units::RegularBall>(
            new_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);

        new_player->setNetInfo(c_ip, c_port);
        new_player->SetPrimaryUnit(new_unit->UnitId());

        std::cout << "New client joined: " << c_ip << ":" << c_port
                  << std::endl;

        world_->game_node_.send(std::to_string(new_player->PlayerId()), c_ip,
                                c_port);
      }
      continue;
    }

    size_t pos = msg.find(':');
    if (!(pos != std::string::npos && pos > 0 && std::all_of(msg.begin(), msg.begin() + pos, ::isdigit))) {
      continue;
    }
    uint64_t id = std::stoull(msg.substr(0, pos));
    std::string command = msg.substr(pos + 1);

    if (command == "quit") {
      {
        std::lock_guard<std::mutex> lock(logic_manager_->logic_mutex_);

        world_->UnregisterUnit(world_->GetPlayer(id)->PrimaryUnitId());
        world_->RemoveUnit(world_->GetPlayer(id)->PrimaryUnitId());

        world_->UnregisterPlayer(id);
        world_->RemovePlayer(id);

        world_->game_node_.send("Bye", c_ip, c_port);
      }
      continue;
    }

    InputPacket inputPacket;
    inputPacket.player_id = id;
    inputPacket.input = StringToPlayerInput(command);
    input_queue.push(inputPacket);
  }
  std::cout << "Server stopped." << std::endl;
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

  if (settings_.mode == "server") {
    // Node on,  User input off, Host on
    world->game_node_.is_server = true;
    world->game_node_.initialize(settings_.port);

  } else if (settings_.mode == "client") {
    // Node on,  User input on,  Host off

    world->game_node_.is_server = false;
    world->game_node_.initialize(settings_.port);

  } else if (settings_.mode == "room") {
    // Node on,  User input on,  Host on

    world->game_node_.is_server = true;
    world->game_node_.initialize(settings_.port);

  } else if (settings_.mode == "local") {
    // Node off, User input on,  Host off

    world->game_node_.is_server = false;
    // Do nothing here, just like before.
  }

  float ground_size = 40.0f;

  scene_->SetEnvmapImage(asset_manager_->ImageFile("textures/envmap.hdr"));

  ambient_light_ = scene_->CreateLight<GameX::Graphics::AmbientLight>();
  ambient_light_->SetLight(glm::vec3{0.3});

  directional_light_ = scene_->CreateLight<GameX::Graphics::DirectionalLight>();
  directional_light_->SetLight(glm::vec3{1.0f}, glm::vec3{3.0f, 2.0f, 1.0f});

  auto primary_obstacle = world->CreateObstacle<Logic::Obstacles::Block>(
      glm::vec3{0.0f, -ground_size, 0.0f},
      std::numeric_limits<float>::infinity(), false, 2 * ground_size);

  if (settings_.mode == "room") {
    // Server only performs calculations, no rendering and playing.

    auto primary_player = world->CreatePlayer();
    //auto enemy_player = world->CreatePlayer();

    auto primary_unit = world->CreateUnit<Logic::Units::RegularBall>(
        primary_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);
    //auto enemy_unit = world->CreateUnit<Logic::Units::RegularBall>(
    //    enemy_player->PlayerId(), glm::vec3{-5.0f, 1.0f, 0.0f}, 1.0f, 1.0f);

    primary_player_id_ = primary_player->PlayerId();

    primary_player->SetPrimaryUnit(primary_unit->UnitId());

    std::thread t1(&GameBall::receivePackets, this);
    listener = std::move(t1);
    listener.detach();
  }

  if (settings_.mode == "local"){
    auto primary_player = world->CreatePlayer();
    auto enemy_player = world->CreatePlayer();

    auto primary_unit = world->CreateUnit<Logic::Units::RegularBall>(
        primary_player->PlayerId(), glm::vec3{0.0f, 1.0f, 0.0f}, 1.0f, 1.0f);
    auto enemy_unit = world->CreateUnit<Logic::Units::RegularBall>(
        enemy_player->PlayerId(), glm::vec3{-5.0f, 1.0f, 0.0f}, 1.0f, 1.0f);

    primary_player_id_ = primary_player->PlayerId();

    primary_player->SetPrimaryUnit(primary_unit->UnitId());
  }

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

  is_running.store(false);
  if (listener.joinable()) {
    listener.join();
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

    if (!input_queue.empty()) {
      auto inputPacket = input_queue.front();
      input_queue.pop();
      auto player = logic_manager_->world_->GetPlayer(inputPacket.player_id);
      if (player) {
        player->SetInput(inputPacket.input);
      }
    }
  }

  // Generate State data;
  float ball_data[48];
  auto client_list = logic_manager_->world_->player_map_;
  auto ball_num = client_list.size();
  std::string msg_broadcast;

  for (auto &pair : client_list) {
    auto player = pair.second;
    if (player && player->PlayerId() != primary_player_id_) {
      auto unit = logic_manager_->world_->GetUnit(player->PrimaryUnitId());
      if (unit) {
        auto sphere_id = player->PrimaryUnitId();
        auto &sphere =
            logic_manager_->world_->PhysicsWorld()->GetSphere(sphere_id);
        auto regular_ball = dynamic_cast<Logic::Units::RegularBall*>(logic_manager_->world_->GetUnit(sphere_id));
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
      msg_broadcast += std::to_string(player->PrimaryUnitId()) + ":" + base64_encode(reinterpret_cast<unsigned char*>(ball_data), sizeof(float)*48) + ",";
    }
  }

  for (auto &pair : client_list) {
    auto player = pair.second;
    if (player) {
      logic_manager_->world_->game_node_.send(msg_broadcast, player->GetIp(), player->GetPort());
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

void GameBall::ScrollCallback(double xoffset, double yoffset) {
  if (!ignore_next_mouse_move_) {
    camera_controller_->CursorScroll(xoffset, yoffset);
  }
  ignore_next_mouse_move_ = false;
}

}  // namespace GameBall
