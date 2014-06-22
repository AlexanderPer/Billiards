#pragma once
#include <glm/glm.hpp>

class CameraControl
{
private:	
	glm::vec3 cen;
	glm::vec3 up;
	glm::mediump_ivec3 camMoveDir;
	int timePrev;
	unsigned int shaderProgram;

	void ProcessMoving();
public:
	glm::vec3 eye;
	CameraControl();
	void Init(unsigned int shaderProg);
	void PressKey(unsigned char key);	
	void PressKey(int key);
	void UpKey(unsigned char key);
	void UpKey(int key);
	void Update();
};