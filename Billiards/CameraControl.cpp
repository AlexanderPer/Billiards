#include "CameraControl.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/gtc/type_ptr.hpp>

CameraControl::CameraControl() : camMoveDir(0)
{
	eye = glm::vec3(0., 4., 10.);
	cen = glm::vec3(0.0, 0.0, 0.0);
	up = glm::vec3(0, 1, 0);
	timePrev = glutGet(GLUT_ELAPSED_TIME);
}

void CameraControl::Init(unsigned int shaderProg)
{
	shaderProgram = shaderProg;
}

void CameraControl::PressKey(unsigned char key)
{
	switch (key)
	{
	case 'w':
		camMoveDir.z = -1;
		break;
	case 's':
		camMoveDir.z = 1;
		break;
	case 'd':
		camMoveDir.x = 1;
		break;
	case 'a':
		camMoveDir.x = -1;
		break;
	}
}

void CameraControl::PressKey(int key)
{
	switch(key) {
	case GLUT_KEY_PAGE_UP:
          camMoveDir.y = 1;
          break;
	case GLUT_KEY_PAGE_DOWN:
          camMoveDir.y = -1;
          break;
	}
}

void CameraControl::UpKey(unsigned char key)
{
	switch (key)
	{
	case 'w':
		camMoveDir.z = 0;
		break;
	case 's':
		camMoveDir.z = 0;
		break;
	case 'd':
		camMoveDir.x = 0;
		break;
	case 'a':
		camMoveDir.x = 0;
		break;
	}
}

void CameraControl::UpKey(int key)
{
	switch(key) {
	case GLUT_KEY_PAGE_UP:
          camMoveDir.y = 0;
          break;
	case GLUT_KEY_PAGE_DOWN:
          camMoveDir.y = 0;
          break;
	}
}

void CameraControl::Update()
{
	ProcessMoving();
	int eyeLoc = glGetUniformLocation(shaderProgram, "eye");
	if (eyeLoc > -1)
		glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));
}

void CameraControl::ProcessMoving()
{
	float amount = 0.005f;
	int time = glutGet(GLUT_ELAPSED_TIME);
	int timeDiff = time - timePrev;
	timePrev = time;

	if (camMoveDir.z != 0)
	{
		glm::vec3 dist = eye - cen;
		glm::vec3 forward = glm::normalize(dist);		
		eye = eye + camMoveDir.z * amount * float(timeDiff) * forward;
		//cen = cen + camMoveDir.z * amount * float(timeDiff) * forward;
	}
	if (camMoveDir.x != 0)
	{
		glm::vec3 dist = eye - cen;
		glm::vec3 right = glm::normalize(glm::cross(dist, up));
		eye = eye - camMoveDir.x * amount * float(timeDiff) * right;
		//cen = cen - camMoveDir.x * amount * float(timeDiff) * right;
	}
	if (camMoveDir.y != 0)
	{
		eye = eye + camMoveDir.y * amount * float(timeDiff) * up;
		//cen = cen + camMoveDir.y * amount * float(timeDiff) * up;
	}
}