#pragma once

class Shader
{
public:
	enum STATUS {SUCCESS, FILE_NOT_FOUND, EMPTY_FILE, READ_ERROR};
	unsigned int ShaderObject;
private:
	STATUS mStatus;
	char* mName;
	char* mSource;
	unsigned long mLength;
	unsigned int mShaderType;
public:
	Shader();
	~Shader();
	STATUS Read(const char *filename, unsigned int type);
	int Compile();
	int ReadAndCompile(const char* filename, unsigned int type);
	void Release();
};

unsigned int InitShaderProgram(const char* vName, const char* fName);