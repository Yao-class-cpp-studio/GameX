#include "GameBall/logic/units/regular_ball.h"

#include "GameBall/core/game_ball.h"
#include "GameBall/logic/world.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <unistd.h>
#include <list>
#include <string.h>
#endif

namespace global{
#ifdef __linux__
  #define PORT 7000
  std::string tempmsg;
  int soc;
  socklen_t len;
  std::list<int> li;
  std::string message;
  struct sockaddr_in servaddr;
  std::string mode;

  void Server_getConn() {
    while (1) {
      int conn = accept(soc, (struct sockaddr*)&servaddr, &len);
      li.push_back(conn);
    }
  }

  void GetData() {
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (global::mode == "server")
      while(1) {
        std::list<int>::iterator it;
        for(it=li.begin(); it!=li.end(); ++it){
          fd_set rfds;
          FD_ZERO(&rfds);
          int maxfd = 0;
          int retval = 0;
          FD_SET(*it, &rfds);
          if(maxfd < *it)  maxfd = *it;
          retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
          retval = 1;
          if(retval == -1) printf("select error\n");
          else if(retval == 0) {}
          else {
            char buf[1024];
            memset(buf, 0 ,sizeof(buf));
            long len = recv(*it, buf, sizeof(buf), 0);
            message = std::string(buf);
          }
        }
      }
    if (global::mode == "client")
      while (1){
        char buf[1024];
        long len = recv(soc, buf, sizeof(buf), 0);
        message = std::string(buf);
      }
  }

  void SendMessage(std::string msg) {
    if (mode == "server") {
      std::list<int>::iterator it;
      for(it=li.begin(); it!=li.end(); ++it)
        send(*it, msg.c_str(), msg.length(), 0);
    }
    if (mode == "client") 
      send(soc, msg.c_str(), msg.length(), 0);
  }
#endif
}
namespace GameBall::Logic::Units {
inline std::string float_to_string(float ft) {
  std::string ret = "";
  for (int i = 3; i >= -4; --i) ret += char(int(ft / pow(10, i)) % 10 + '0');
  return ret;
}

inline std::string vec_to_string(glm::vec3 vec) {
  return (float_to_string(vec.x) + float_to_string(vec.y) + float_to_string(vec.z));
}

inline std::string mat_to_string(glm::mat3 mat) {
  return (vec_to_string(mat[0]) + vec_to_string(mat[1]) + vec_to_string(mat[2]));
}

inline float string_to_float(const char *hd) {
  float ret = 0;
  for (int i = 0; i < 8; ++i) ret *= 10, ret += int(*(hd + i) - '0');
  ret /= 10000;
  return ret;
}

RegularBall::RegularBall(World *world,
                         uint64_t player_id,
                         const glm::vec3 &position,
                         float radius,
                         float mass)
    : Unit(world, player_id) {
  radius_ = radius;
  mass_ = mass;
  position_ = position;
  auto physics_world = world_->PhysicsWorld();
  sphere_id_ = physics_world->CreateSphere();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.position = position_;
  sphere.SetRadiusMass(radius_, mass_);
  sphere.orientation = orientation_;
  sphere.velocity = velocity_;
  sphere.angular_velocity = glm::vec3{0.0f};
  sphere.elasticity = 1.0f;
  sphere.friction = 10.0f;
  sphere.gravity = glm::vec3{0.0f, -9.8f, 0.0f};
}

RegularBall::~RegularBall() {
  ;
}

SYNC_ACTOR_FUNC(RegularBall) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  actor->SetMass(1.0f);
  actor->SetGravity(glm::vec3{0.0f, -9.8f, 0.0f});
  actor->SetTransform(glm::mat3{radius_});
  actor->SetMotion(position_, velocity_, orientation_, augular_momentum_);
  actor->SetMomentOfInertia(sphere.inertia[0][0]);
}

void RegularBall::UpdateTick() {
  float delta_time = world_->TickDeltaT();
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);

  
//  auto owner = world_->GetPlayer(player_id_);
#ifdef __linux__
  if (global::mode != "client" && global::mode != "server")
#endif
//  if (owner) {
//    if (UnitId() == owner->PrimaryUnitId()) {
//      auto input = owner->TakePlayerInput();
//
//      glm::vec3 forward = glm::normalize(glm::vec3{input.orientation});//
//      glm::vec3 right =
//          glm::normalize(glm::cross(forward, glm::vec3{0.0f, 1.0f, 0.0f}));
//
//      glm::vec3 moving_direction{};
//
//      float angular_acceleration = glm::radians(2880.0f);
//
//      if (input.move_forward) {
//        moving_direction -= right;
//      }
//      if (input.move_backward) {
//       moving_direction += right;
//     }
//     if (input.move_left) {
//       moving_direction -= forward;
//     }
//     if (input.move_right) {
//       moving_direction += forward;
//     }

//     if (glm::length(moving_direction) > 0.0f) {
//       moving_direction = glm::normalize(moving_direction);
//       sphere.angular_velocity +=
//           moving_direction * angular_acceleration * delta_time;
//     }

//     if (input.brake) {
//       sphere.angular_velocity = glm::vec3{0.0f};
//     }
//   }
// }
#ifdef __linux__
  if (global::mode == "server")
    if (owner) 
      if (UnitId() == owner->EnemyUnitId()) {
        global::tempmsg += vec_to_string(position_) + vec_to_string(velocity_) + mat_to_string(orientation_) + vec_to_string(augular_momentum_);
        global::SendMessage(global::tempmsg);
        auto input = owner->GetPlayerInput();
        input = global::message;
        global::message = "";
        glm::vec3 forward = glm::normalize(glm::vec3{input.orientation});
        glm::vec3 right =
            glm::normalize(glm::cross(forward, glm::vec3{0.0f, 1.0f, 0.0f}));

        glm::vec3 moving_direction{};

        float angular_acceleration = glm::radians(2880.0f);

        if (input.move_forward) {
          moving_direction -= right;
        }
        if (input.move_backward) {
          moving_direction += right;
        }
        if (input.move_left) {
          moving_direction -= forward;
        }
        if (input.move_right) {
          moving_direction += forward;
        }

        if (glm::length(moving_direction) > 0.0f) {
          moving_direction = glm::normalize(moving_direction);
          sphere.angular_velocity +=
              moving_direction * angular_acceleration * delta_time;
        }

        if (input.brake) {
          sphere.angular_velocity = glm::vec3{0.0f};
        }
      }
  if (global::mode == "server")
    if (owner) {
      if (UnitId() == owner->PrimaryUnitId()) {
        global::tempmsg = vec_to_string(position_) + vec_to_string(velocity_) + mat_to_string(orientation_) + vec_to_string(augular_momentum_);
        auto input = owner->TakePlayerInput();

        glm::vec3 forward = glm::normalize(glm::vec3{input.orientation});
        glm::vec3 right =
            glm::normalize(glm::cross(forward, glm::vec3{0.0f, 1.0f, 0.0f}));

        glm::vec3 moving_direction{};

        float angular_acceleration = glm::radians(2880.0f);

        if (input.move_forward) {
          moving_direction -= right;
        }
        if (input.move_backward) {
          moving_direction += right;
        }
        if (input.move_left) {
          moving_direction -= forward;
        }
        if (input.move_right) {
          moving_direction += forward;
        }

        if (glm::length(moving_direction) > 0.0f) {
          moving_direction = glm::normalize(moving_direction);
          sphere.angular_velocity +=
              moving_direction * angular_acceleration * delta_time;
        }

        if (input.brake) {
          sphere.angular_velocity = glm::vec3{0.0f};
        }
      }
    }
  if (global::mode != "client") {
#endif

  sphere.velocity *= std::pow(0.5f, delta_time);
  sphere.angular_velocity *= std::pow(0.2f, delta_time);

  position_ = sphere.position;
  velocity_ = sphere.velocity;
  orientation_ = sphere.orientation;
  augular_momentum_ = sphere.inertia * sphere.angular_velocity;
#ifdef __linux__
  }
  if (global::mode == "client")
  if (owner) 
    if (UnitId() == owner->PrimaryUnitId()) {
      global::SendMessage(std::string(owner->TakePlayerInput()));
      SetMotion(global::message.c_str() + 144);
    }
  if (global::mode == "client")
    if (owner) 
      if (UnitId() == owner->EnemyUnitId()) {
        global::SendMessage(std::string(owner->TakePlayerInput()));
        SetMotion(global::message.c_str());
      }
    
#endif
}

void RegularBall::SetMass(float mass) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius_, mass);
  mass_ = mass;
}

void RegularBall::SetGravity(const glm::vec3 &gravity) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.gravity = gravity;
}

void RegularBall::SetRadius(float radius) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.SetRadiusMass(radius, mass_);
  radius_ = radius;
}

void RegularBall::SetMotion(const glm::vec3 &position,
                            const glm::vec3 &velocity,
                            const glm::mat3 &orientation,
                            const glm::vec3 &angular_momentum) {
  auto physics_world = world_->PhysicsWorld();
  auto &sphere = physics_world->GetSphere(sphere_id_);
  sphere.position = position;
  sphere.velocity = velocity;
  sphere.orientation = orientation;
  sphere.angular_velocity = sphere.inertia_inv * angular_momentum;
  position_ = position;
  velocity_ = velocity;
  orientation_ = orientation;
  augular_momentum_ = angular_momentum;
}

void RegularBall::SetMotion(const char *hd) {
  for (int i = 0; i < 3; ++i) position_[i] = string_to_float(hd + 8 * i);
  for (int i = 0; i < 3; ++i) velocity_[i] = string_to_float(hd + 8 * i + 24);
  for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) orientation_[i][j] = string_to_float(hd + 24 * i + 8 * j + 48);
  for (int i = 0; i < 3; ++i) augular_momentum_[i] = string_to_float(hd + 8 * i + 120);

  return ;
}

glm::vec3 RegularBall::Position() const {
  return position_;
}

glm::vec3 RegularBall::Velocity() const {
  return velocity_;
}

glm::mat3 RegularBall::Orientation() const {
  return orientation_;
}

glm::vec3 RegularBall::AngularMomentum() const {
  return augular_momentum_;
}

}  // namespace GameBall::Logic::Units