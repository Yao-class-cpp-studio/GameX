#include "GameX/physics/collision.h"

#include <iostream>

namespace GameX::Physics {

bool DetectCollision(const Sphere &sphere1,
                     const Sphere &sphere2,
                     Collision &collision) {
  glm::vec3 distance = sphere2.position - sphere1.position;
  float distance_length = glm::length(distance);
  float penetration = sphere1.radius + sphere2.radius - distance_length;
  if (penetration < 0.0f) {
    return false;
  }

  if (distance_length < 0.0001f) {
    collision.point = sphere1.position;
    collision.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    collision.penetration = penetration;
    return true;
  }

  collision.point =
      sphere1.position +
      distance * (sphere1.radius - penetration / 2.0f) / distance_length;
  collision.normal = distance / distance_length;
  collision.penetration = penetration;
  return true;
}

bool DetectCollision(const Sphere &sphere,
                     const Cube &cube,
                     Collision &collision) {
  glm::vec3 cube_to_sphere = sphere.position - cube.position;
  glm::vec3 closest_point = cube.position;
  float distance = 0.0f;
  float penetration = 0.0f;

  for (int i = 0; i < 3; ++i) {
    distance = glm::dot(cube_to_sphere, cube.orientation[i]);
    if (distance > cube.side_length / 2.0f) {
      distance = cube.side_length / 2.0f;
    } else if (distance < -cube.side_length / 2.0f) {
      distance = -cube.side_length / 2.0f;
    }
    closest_point += distance * cube.orientation[i];
  }

  distance = glm::length(closest_point - sphere.position);
  penetration = sphere.radius - distance;
  if (penetration < 0.0f) {
    return false;
  }

  if (distance < 0.0001f) {
    collision.point = sphere.position;
    collision.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    collision.penetration = penetration;
    return true;
  }

  collision.point = sphere.position + (closest_point - sphere.position) *
                                          (sphere.radius - penetration / 2.0f) /
                                          distance;
  collision.normal = glm::normalize(closest_point - sphere.position);
  collision.penetration = penetration;
  return true;
}

bool SolveCollision(RigidBody &body1,
                    RigidBody &body2,
                    const Collision &collision) {
  //  std::cout << "Solve Collision..." << std::endl;
  //  std::cout << "body1 (pos): " << body1.position.x << " " <<
  //  body1.position.y
  //            << " " << body1.position.z << std::endl;
  //  std::cout << "body1 (vel):" << body1.velocity.x << " " << body1.velocity.y
  //            << " " << body1.velocity.z << std::endl;
  //  std::cout << "body1 (ang):" << body1.angular_velocity.x << " "
  //            << body1.angular_velocity.y << " " << body1.angular_velocity.z
  //            << std::endl;
  //
  //  std::cout << "body2 (pos): " << body2.position.x << " " <<
  //  body2.position.y
  //            << " " << body2.position.z << std::endl;
  //  std::cout << "body2 (vel):" << body2.velocity.x << " " << body2.velocity.y
  //            << " " << body2.velocity.z << std::endl;
  //  std::cout << "body2 (ang):" << body2.angular_velocity.x << " "
  //            << body2.angular_velocity.y << " " << body2.angular_velocity.z
  //            << std::endl;
  //
  //  std::cout << "collision (point): " << collision.point.x << " "
  //            << collision.point.y << " " << collision.point.z << std::endl;
  //  std::cout << "collision (normal): " << collision.normal.x << " "
  //            << collision.normal.y << " " << collision.normal.z << std::endl;
  //  std::cout << "collision (penetration): " << collision.penetration
  //            << std::endl;

  glm::vec3 r1 = collision.point - body1.position;
  glm::vec3 r2 = collision.point - body2.position;
  glm::vec3 relative_velocity =
      body2.velocity + glm::cross(body2.angular_velocity, r2) - body1.velocity -
      glm::cross(body1.angular_velocity, r1);
  float velocity_along_normal = glm::dot(relative_velocity, collision.normal);
  if (velocity_along_normal > -0.0001f) {
    return false;
  }

  glm::mat3 inverse_inertia1 =
      body1.orientation * body1.inertia_inv * glm::transpose(body1.orientation);
  glm::mat3 inverse_inertia2 =
      body2.orientation * body2.inertia_inv * glm::transpose(body2.orientation);

  float alpha = 0.0f;
  if (!std::isinf(body1.mass)) {
    alpha += 1.0f / body1.mass;
  }
  if (!std::isinf(body2.mass)) {
    alpha += 1.0f / body2.mass;
  }

  float e = std::min(body1.elasticity, body2.elasticity);
  float j =
      -(1.0f + e) * velocity_along_normal /
      (alpha +
       glm::dot(
           collision.normal,
           glm::cross(inverse_inertia1 * glm::cross(r1, collision.normal), r1) +
               glm::cross(inverse_inertia2 * glm::cross(r2, collision.normal),
                          r2)));

  glm::vec3 impulse = j * collision.normal;
  body1.velocity -= impulse / body1.mass;
  body1.angular_velocity -= inverse_inertia1 * glm::cross(r1, impulse);
  body2.velocity += impulse / body2.mass;
  body2.angular_velocity += inverse_inertia2 * glm::cross(r2, impulse);

  float friction = std::sqrt(body1.friction * body1.friction +
                             body2.friction * body2.friction);
  glm::vec3 tangent =
      relative_velocity -
      glm::dot(relative_velocity, collision.normal) * collision.normal;

  if (glm::length(tangent) > 0.0001f) {
    tangent = glm::normalize(tangent);
    float jt =
        -glm::dot(relative_velocity, tangent) /
        (alpha +
         glm::dot(
             tangent,
             glm::cross(inverse_inertia1 * glm::cross(r1, tangent), r1) +
                 glm::cross(inverse_inertia2 * glm::cross(r2, tangent), r2)));
    if (jt > j * friction) {
      jt = j * friction;
    } else if (jt < -j * friction) {
      jt = -j * friction;
    }
    glm::vec3 friction_impulse = jt * tangent;
    body1.velocity -= friction_impulse / body1.mass;
    body1.angular_velocity -=
        inverse_inertia1 * glm::cross(r1, friction_impulse);
    body2.velocity += friction_impulse / body2.mass;
    body2.angular_velocity +=
        inverse_inertia2 * glm::cross(r2, friction_impulse);
  }

  return true;
}

}  // namespace GameX::Physics
