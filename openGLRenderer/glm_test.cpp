#include "glm_test.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
void test_matrices() {
	/*
	vec is a column vector
	1.0
	0.0
	0.0
	1.0

	trans will be our 4x4 matrix for transformation, glm::mat4(1.0f) will generate a 4x4 identity matrix, which looks like this
	1.0, 0.0, 0.0, 0.0
	0.0, 1.0, 0.0, 0.0
	0.0, 0.0, 1.0, 0.0
	0.0, 0.0, 0.0, 1.0

	glm translate will generate a translation matrix from trans (which is assigned TO trans) that looks like this
	1.0, 0.0, 0.0, 1.0
	0.0, 1.0, 0.0, 1.0
	0.0, 0.0, 1.0, 0.0
	0.0, 0.0, 0.0, 1.0

	then we can multiply to get our final vector, which should be
	2.0
	1.0
	0.0
	1.0
	*/
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	//rotate 90 degrees around the z axis, then scale by 0.5, then translate by (1, 1, 0)
	//order of operations matter, and typically we want translation to be performed last
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = trans * vec;
	std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
}