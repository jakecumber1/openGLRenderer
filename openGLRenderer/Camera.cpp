#include "Camera.h"
//required for lookAt
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
Camera::Camera(glm::vec3 pos, glm::vec3 target) {
	//define camera position, camera target, direction (pointing away from the direction actually)
	this->cameraPos = pos;
	this->cameraTarget = target;
	this->cameraFront = glm::normalize(cameraTarget - cameraPos);
	//define up (NOT the camera up, just a vector which points upward and camera right (cross product of up and cameraFront)
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->cameraRight = glm::normalize(glm::cross(up, cameraFront));
	//now we can find the camera's up vector by crossing the direction and right vectors
	this->cameraUp = glm::cross(cameraFront, cameraRight);

	/*
	look at definition
	R = right vector
	U = up vector
	D = direction vector
	P = camera position
	LookAt =
	|R.x R.y R.z 0|     |1  0  0  -P.x|
	|U.x U.y U.z 0|  *  |0  1  0  -P.y|
	|D.x D.y D.z 0|     |0  0  1  -P.z|
	|0   0   0   1|		|0  0  0   1  |

	glm has a function which handles this, glm::lookAt(cameraPos, cameraTarget (calculated dynamically w cameraPos + cameraFront later, cameraUp);
	*/
	this->view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	std::cout << "Camera created with members: " << std::endl;
	std::cout << "Camera Position: " << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
	std::cout << "Camera Target: " << cameraTarget.x << " " << cameraTarget.y << " " << cameraTarget.z << std::endl;
	std::cout << "Camera Front: " << cameraFront.x << " " << cameraFront.y << " " << cameraFront.z << std::endl;
	std::cout << "Camera Up: " << cameraUp.x << " " << cameraUp.y << " " << cameraUp.z << std::endl;
}
glm::mat4 Camera::moveCamera(glm::vec3 position) {
	this->cameraPos = position;
	//update the view matrix
	this->view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
	return this->view;
}
glm::mat4 Camera::getView() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->cameraFront = glm::normalize(front);
	this->view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);
	return this->view;
}