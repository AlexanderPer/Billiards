#include "Dynamics.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/gtc/type_ptr.hpp>

BallDynamics::BallDynamics() : Pos(0.0), Radius(1.0)
{
	timePrev = glutGet(GLUT_ELAPSED_TIME);
	startVelocity = 0.008f;
	deceleration = 0.0000003f;
	Velocity = glm::vec2(0);
	BallUpVec = glm::vec3(0, 1, 0);
	BallRightVec =  glm::vec3(1, 0, 0);
}

void BallDynamics::Init(unsigned int shaderProg)
{
	shaderProgram = shaderProg;
	int locRad = glGetUniformLocation(shaderProg, "ballRadius");
	if (locRad > -1)
		glUniform1f(locRad, Radius);
}

void BallDynamics::Update()
{	
	ProcessMoving();
	int locPos = glGetUniformLocation(shaderProgram, "ballPos");
	if (locPos > -1)
		glUniform2fv(locPos, 1, glm::value_ptr(Pos));
	
	glm::vec3 currUpVec = currQuatOrientation * BallUpVec;
	glm::vec3 currRightVec = currQuatOrientation * BallRightVec;

	int locUp = glGetUniformLocation(shaderProgram, "ballUpVec");
	if (locUp > -1)
		glUniform3fv(locUp, 1, glm::value_ptr(currUpVec));

	int locRight = glGetUniformLocation(shaderProgram, "ballRightVec");
	if (locRight > -1)
		glUniform3fv(locRight, 1, glm::value_ptr(currRightVec));
}

void BallDynamics::ProcessMoving()
{
	int time = glutGet(GLUT_ELAPSED_TIME);
	int timeDiff = time - timePrev;
	timePrev = time;

	float AbsVelocity = glm::length(Velocity);
	if (AbsVelocity > 0)
	{
		float decelerCoeff = (AbsVelocity - float(timeDiff) * deceleration) / AbsVelocity;
		if (decelerCoeff > 0)
			Velocity *= decelerCoeff;
		else
			Velocity = glm::vec2(0);
		glm::vec2 delta = float(timeDiff) * Velocity;
		Pos += delta;

		glm::vec3 deltaEulerAngles(delta.y, 0, -delta.x);
		glm::quat deltaQuat = glm::quat(deltaEulerAngles);

		currQuatOrientation = deltaQuat * currQuatOrientation;
	}
}

void BallDynamics::Strike(glm::vec2 camPos)
{
	glm::vec2 strikeDir = glm::normalize(Pos - camPos);
	Velocity = strikeDir * startVelocity;
}

TableDynamics::TableDynamics()
{
	size = 12;
}

void TableDynamics::Init(unsigned int shaderProg, CameraControl *cc)
{
	ball.Init(shaderProg);
	cam = cc;
	int locSize = glGetUniformLocation(shaderProg, "tableSize");
	if (locSize > -1)
		glUniform1f(locSize, size);
}

void TableDynamics::Update()
{
	ball.Update();
	
	 if ( (ball.Pos.x <= (-size / 2 + ball.Radius)) || (ball.Pos.x >= (size / 2 - ball.Radius)) )
		 ball.Velocity.x = -ball.Velocity.x;

	 if ( (ball.Pos.y <= (-size / 2 + ball.Radius)) || (ball.Pos.y >= (size / 2 - ball.Radius)) )
		 ball.Velocity.y = -ball.Velocity.y;
}

void TableDynamics::StrikeBall()
{
	ball.Strike(glm::vec2(cam->eye.x, cam->eye.z));
}