#include "Camera.h"

#include <iostream>

Camera::Camera(glm::vec3 sphereCenter, GLfloat radius, GLfloat angleY, GLfloat angleXZ):
	sphereCenter(sphereCenter),
	radius(radius),
	angleY(angleY),
	angleXZ(angleXZ)
{
}

glm::mat4 Camera::getViewMatrix() const
{
	// sin cos come from?
	GLfloat x = radius * cos(glm::radians(angleY)) * cos(glm::radians(angleXZ));
	GLfloat y = radius * sin(glm::radians(angleY));
	GLfloat z = radius * cos(glm::radians(angleY)) * sin(glm::radians(angleXZ));

	return glm::lookAt(glm::vec3(x, y, z), sphereCenter, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::orbit(double xoffset, double yoffset)
{
	angleY += yoffset;
	angleXZ += xoffset;

	if (angleY < -89.0f)
		angleY = -89.0f;
	else if (angleY > 89.0f)
		angleY = 89.0f;

	if (angleXZ < 0.0f)
		angleXZ += 360.0f;
	else if (angleXZ >= 360.0f)
		angleXZ -= 360.0f;
	
	std::cout << angleY << ' ' << angleXZ << std::endl;
}

void Camera::zoom(double yoffset)
{
	radius += yoffset;

	if (radius < 0.0f)
		radius = 0.0f;

	std::cout << radius << std::endl;
}
