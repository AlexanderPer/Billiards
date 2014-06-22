#include <iostream>
#include "ScreenRect.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

Object3D::Object3D(void):
	dataCount(0),
	indexesCount(0),
	pData(0),
	pIndexes(0)
{
	size = glm::vec2(1.0);
	center = glm::vec3(0.0);
}

Object3D::~Object3D(void)
{
	//glDeleteBuffers(2, vbo);
	if (pData)
	{
		delete[] pData;
		delete[] pIndexes;
	}
}

void Object3D::CreateArrays()
{
	if (pData)
	{
		delete[] pData;
		delete[] pIndexes;
	}
	pData = new VertexData [dataCount];
	pIndexes = new unsigned int [indexesCount];
}

void Object3D::Init(unsigned int programId)
{	
	shaderProgramId = programId;
	
	glGenBuffers(2, &vbo[0]);
	
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	glBufferData( GL_ARRAY_BUFFER, dataCount * sizeof(VertexData), pData, GL_STATIC_DRAW );
	
	int bufferSize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    if(dataCount * sizeof(VertexData) != bufferSize) {
		std::cerr << "Array buffer init error!" << std::endl;
		return;
	}
		
	glEnable(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesCount * sizeof(unsigned int), pIndexes, GL_STATIC_DRAW);

	bufferSize = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
    if(indexesCount * sizeof(unsigned int) != bufferSize) {
		std::cerr << "Element array buffer init error!" << std::endl;
	}
}

void Object3D::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );	
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glDrawElements(GL_TRIANGLES, indexesCount, GL_UNSIGNED_INT, 0);

}

RectSurface::RectSurface(float width, float height) : mWidth(width), mHeight(height)
{
	size = glm::vec2(2.0);
	InitData();
	CreateArrays();
	GenerateData();
}

RectSurface::~RectSurface(void)
{
}

void RectSurface::SetResolution(float width, float height)
{
	mWidth = width;
	mHeight = height;
}

void RectSurface::Draw()
{
	int resLoc = glGetUniformLocation(shaderProgramId, "iResolution");
	if (resLoc > -1)
		glUniform3f(resLoc, mWidth, mHeight, 0.0f);
	Object3D::Draw();
}

void RectSurface::InitData()
{
	dataCount = 4; 
	unsigned int nTriangles = 2;
	indexesCount = 3 * nTriangles;
}

void RectSurface::GenerateData()
{
	for (unsigned int i = 0; i < dataCount; i++)
	{
		pData[i] = glm::vec3(float((i % 2) ? center.x + size.x / 2 : center.x - size.x / 2), 
			float((i > 1) ? center.y + size.y / 2 : center.y - size.y / 2), 
			center.z);
	}
	pIndexes[0] = 0; pIndexes[1] = 1; pIndexes[2] = 3;
	pIndexes[3+0] = 0; pIndexes[3+1] = 2; pIndexes[3+2] = 3;
}
