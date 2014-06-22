#pragma once
#include <glm/glm.hpp>
#include "CameraControl.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class BallDynamics
{
public:
	glm::vec2 Velocity;
	glm::vec2 Pos;
	float Radius;	
private:
	glm::vec3 BallUpVec;
	glm::vec3 BallRightVec;
	glm::quat currQuatOrientation;
	unsigned int shaderProgram;	
	int timePrev;
	float startVelocity;
	float deceleration;

	void ProcessMoving();
public:
	BallDynamics();
	void Init(unsigned int shaderProg);
	void Update();
	void Strike(glm::vec2 camPos);
};

class TableDynamics
{
private:	
	BallDynamics ball;
	float size;
	CameraControl* cam;
public:
	TableDynamics();
	void Init(unsigned int shaderProg, CameraControl *cc);
	void Update();
	void StrikeBall();
};