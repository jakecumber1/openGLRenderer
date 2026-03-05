#pragma once
#include <glm/glm.hpp>
class Camera {
public:
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	glm::mat4 view;
	float cameraSpeed = 0.05f;
	float sensitivity = 0.1f;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float fov = 45.0f;
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f));
	//moves camera to position, return view matrix
	glm::mat4 moveCamera(glm::vec3 direction);
	glm::mat4 getView();
};