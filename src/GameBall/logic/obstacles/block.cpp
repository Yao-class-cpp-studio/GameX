#include "GameBall/logic/obstacles/block.h"

#include "GameBall/core/game_ball.h"
#include "GameBall/logic/world.h"

namespace GameBall::Logic::Obstacles {
short count=0;
Block::Block(World *world,
             glm::vec3 position,
             float mass,
             bool gravity,
             float side_length)
    : Obstacle(world),
      position_(position),
      side_length_(side_length),
      gravity_(gravity ? glm::vec3{0.0f, -9.8f, 0.0f} : glm::vec3{0.0}),
      mass_(mass) {
  auto physics_world = world_->PhysicsWorld();
  cude_id_ = physics_world->CreateCube();
  auto &cube = physics_world->GetCube(cude_id_);
  SetGravity(gravity_);
  SetMass(mass_);
  SetSideLength(side_length_);
  SetMotion(position_, velocity_, orientation_, L_);
  cube.elasticity = 0.5f;
  cube.friction = 0.5f;
}

SYNC_ACTOR_FUNC(Block) {
  actor->SetTransform(glm::mat3{side_length_});
  actor->SetMass(mass_);
  actor->SetInertiaTensor(J_);
  actor->SetGravity(gravity_);
  actor->SetMotion(position_, velocity_, orientation_, L_);
  if (ActorInitialize())
  if (count <2) {
    std::cout << "Choose a setting:\n";
    char input;
    std::cin >> input;
    switch (input) {
      case '1':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_ao_2k.jpg"));
        break;
      case '2':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_arm_2k.jpg"));
        break;
      case '3':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_diff_2k.jpg"));
        break;
      case '4':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_disp_2k.jpg"));
        break;
      case '5':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_nor_dx_2k.jpg"));
        break;
      case '6':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_nor_gl_2k.jpg"));
        break;
      case '7':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_rough_2k.jpg"));
        break;
      case '8':
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_spec_2k.jpg"));
        break;
      default:
        actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
            "textures/floor_tiles_06_2k/floor_tiles_06_diff_2k.jpg"));
    }
    count++;
    } else 
      actor->Entity()->SetAlbedoImage(app->AssetManager()->ImageFile(
        "textures/floor_tiles_06_2k/floor_tiles_06_diff_2k.jpg"));
    }
void Block::SetMass(float mass) {
  auto physics_world = world_->PhysicsWorld();
  auto &cube = physics_world->GetCube(cude_id_);
  mass_ = mass;
  cube.SetSideLengthMass(side_length_, mass_);
  J_ = cube.inertia;
}

void Block::SetGravity(const glm::vec3 &gravity) {
  auto physics_world = world_->PhysicsWorld();
  auto &cube = physics_world->GetCube(cude_id_);
  gravity_ = gravity;
  cube.gravity = gravity_;
}

void Block::SetMotion(const glm::vec3 &position,
                      const glm::vec3 &velocity,
                      const glm::mat3 &orientation,
                      const glm::vec3 &angular_momentum) {
  auto physics_world = world_->PhysicsWorld();
  auto &cube = physics_world->GetCube(cude_id_);

  orientation_ = orientation;
  position_ = position;
  velocity_ = velocity;
  L_ = angular_momentum;

  cube.position = position_;
  cube.velocity = velocity_;
  cube.orientation = orientation_;
  cube.angular_velocity = cube.inertia_inv * L_;
}

void Block::UpdateTick() {
  auto physics_world = world_->PhysicsWorld();
  auto &cube = physics_world->GetCube(cude_id_);
  position_ = cube.position;
  velocity_ = cube.velocity;
  orientation_ = cube.orientation;
  L_ = cube.inertia * cube.angular_velocity;
}

void Block::SetSideLength(float side_length) {
  side_length_ = side_length;
  auto physics_world = world_->PhysicsWorld();
  auto &cube = physics_world->GetCube(cude_id_);
  cube.SetSideLengthMass(side_length_, mass_);
  J_ = cube.inertia;
}
}  // namespace GameBall::Logic::Obstacles
