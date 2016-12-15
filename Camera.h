#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:

	Camera(glm::vec3 sphereCenter, GLfloat radius, GLfloat angleY = 0.0f, GLfloat angleXZ = 0.0f);
	glm::mat4 getViewMatrix() const;
	GLfloat getYFoV() const;
	void orbit(double xoffset, double yoffset);
	void zoom(double yoffset);

private:

	glm::vec3 sphereCenter;
	GLfloat radius;
	GLfloat angleY;
	GLfloat angleXZ;
};

#endif
