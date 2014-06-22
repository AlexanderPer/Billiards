#pragma once
#include <glm/glm.hpp>

typedef glm::vec3 VertexData;

// base object for 3d objects
class Object3D
{	
private:	
	unsigned int vbo[2];	
protected:
	unsigned int shaderProgramId;
	VertexData* pData;
	unsigned int dataCount;

	unsigned int* pIndexes;
	unsigned int indexesCount;

	glm::vec2 size;
	glm::vec3 center;
public:
	Object3D(void);
	virtual ~Object3D(void);	
	void Init(unsigned int programId); 
	virtual void Draw();
protected:
	void CreateArrays();
};

// full screen rectangle
class RectSurface : public Object3D
{
private:
	float mWidth;
	float mHeight;
public:
	RectSurface(float width, float height);
	~RectSurface(void);
	void SetResolution(float width, float height);
	virtual void Draw();
private:
	void InitData();
	void GenerateData();
};