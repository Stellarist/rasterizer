#pragma once

#include "global.hpp"

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
private:
	direct_t position;
	direct_t front;
	direct_t up;
	direct_t right;
	direct_t world_up;

	float movement_speed;
	float mouse_sensitivity;

	float zoom;
	float yaw;
	float pitch;
	float fov;

	void updateCameraVectors();

public:
	Camera(direct_t position = direct_t(0.0f, 0.0f, 0.0f),
		direct_t up = direct_t(0.0f, 1.0f, 0.0f),
		float yaw = -90.0f,
		float pitch = 0.0f);
	Camera(float x_pos, float y_pos, float z_pos,
		float x_up, float y_up, float z_up,
		float yaw, float pitch);

	direct_t getPosition();
	direct_t getFront();
	matrix_t getView();
	matrix_t getProjection();

	void processKeyboard(CameraMovement direction, float delta_time);
	void processMouseMovement(float x_ofs, float y_ofs, bool constrain_pitch = true);
	void processMouseScroll(float y_ofs);
};

inline direct_t Camera::getPosition()
{
	return position;
}

inline direct_t Camera::getFront()
{
	return front;
}

inline matrix_t Camera::getView()
{
	return Geometry::lookAt(position, position + front, up);
}

inline matrix_t Camera::getProjection()
{
	return Geometry::perspective(Geometry::radians(zoom), fov, 0.1f, 100.0f);
}
