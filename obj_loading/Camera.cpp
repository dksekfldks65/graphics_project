#include "Camera.h"
#include <stdio.h>

const kmuvcl::math::vec3f Camera::center_position() const
{

  kmuvcl::math::vec3f center_position(position_(0) + front_dir_(0),
                 position_(1) + front_dir_(1),
                 position_(2) + front_dir_(2));
  return center_position;
}

// TODO: fill up the following functions properly
void Camera::fly(float delta){
  position_(1) += delta;
}

void Camera::move_forward(float delta)
{
  position_+=delta*front_dir_;
}

void Camera::move_backward(float delta)
{
  position_-=delta*front_dir_;
}

void Camera::move_left(float delta)
{
  position_-=delta*right_dir_;
}

void Camera::move_right(float delta)
{
  position_+=delta*right_dir_;
}

void Camera::rotate_left(float delta)  //front와 right 벡터 동시에 회전 시켜야한다.
{
  kmuvcl::math::mat4x4f R = kmuvcl::math::rotate(delta, 0.0f,1.0f, 0.0f);
  kmuvcl::math::vec4f right(right_dir_(0), right_dir_(1), right_dir_(2)), front(front_dir_(0), front_dir_(1), front_dir_(2)); //3차원을 4차원 벡터로 만듬

  right = R * right;
  front = R * front;

  for(int i=0; i<3; i++)
  {
    right_dir_(i) = right(i);
    front_dir_(i) = front(i);
  }
}

void Camera::rotate_right(float delta)
{
  kmuvcl::math::mat4x4f R = kmuvcl::math::rotate(-delta, 0.0f,1.0f, 0.0f);
  kmuvcl::math::vec4f right(right_dir_(0), right_dir_(1), right_dir_(2)), front(front_dir_(0), front_dir_(1), front_dir_(2));

  right = R * right;
  front = R * front;

  for(int i=0; i<3; i++)
  {
    right_dir_(i) = right(i);
    front_dir_(i) = front(i);
  }
}
