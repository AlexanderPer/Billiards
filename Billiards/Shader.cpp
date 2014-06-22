#include <cstring>
#include <iostream>

#include "Shader.h"
#include <GL/glew.h>

using namespace std;

Shader::Shader():
	mName(0),
	mLength(0),
	mSource(0),
	ShaderObject(0),
	mShaderType(0)
{
}

Shader::STATUS Shader::Read(const char* filename, unsigned int type)
{
	if (mName)
	{
		delete[] mName;
		delete[] mSource;
		mName = 0;
		mSource = 0;
	}		
	mName = new char [strlen(filename) + 1];
    strcpy(mName, filename);
	mName[strlen(filename)] = 0;

    FILE* file = fopen(mName, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		mLength = ftell(file);
		
		if (mLength == 0)
		{
			mSource=0;
			mStatus = EMPTY_FILE;
		}
		else
		{
			mSource = new char[mLength + 1];
			mSource[mLength] = 0;
			rewind(file);
			size_t actualLength = fread(mSource, 1, mLength, file);

			mStatus = (actualLength == size_t(mLength)) ? SUCCESS : READ_ERROR;
		}
		fclose(file);
	}
	else
	{
		mStatus = FILE_NOT_FOUND;
	}	

	mShaderType = type;
	ShaderObject = glCreateShader(mShaderType);
	if (mSource)
		glShaderSource(ShaderObject, 1, (const GLchar **) &mSource, (const GLint *) &mLength);

	return mStatus;
}

int Shader::Compile()
{
	int success;
	glCompileShader(ShaderObject);
	glGetShaderiv(ShaderObject, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		cerr << "Shader" << mShaderType << "compile error" << endl;
		GLint maxLength = 0;
        glGetShaderiv(ShaderObject, GL_INFO_LOG_LENGTH, &maxLength);
 
        char* errorLog = new char [maxLength];
        glGetShaderInfoLog(ShaderObject, maxLength, &maxLength, &errorLog[0]);
		cout << errorLog << endl;
		delete[] errorLog;

		glDeleteShader(ShaderObject);
	}
	else
		cout << "Shader" << " compilation succeed" << endl;

	
	return success;
}

int Shader::ReadAndCompile(const char* filename, unsigned int type)
{
	Read(filename, type);
	if (mStatus != Shader::SUCCESS)
	{
		cerr << "Error while reading shader. Invalid name or empty file." << endl;
		return 0;
	}
	return Compile();
	
}

void Shader::Release()
{
	if (ShaderObject > 0)
	{
		glDeleteShader(ShaderObject);
		ShaderObject = -1;
	}
}

Shader::~Shader()
{
	delete[] mName;
	delete[] mSource;
}

unsigned int InitShaderProgram(const char* vName, const char* fName)
{
	Shader vertex, fragment;
	unsigned int programObject;
	GLint success;
	success = vertex.ReadAndCompile(vName, GL_VERTEX_SHADER);
	if (!success)
	{
		throw "Vertex Compilation Error";
	}

	success = fragment.ReadAndCompile(fName, GL_FRAGMENT_SHADER);
	if (!success)
	{
		throw "Fragment Compilation Error";
	}

	programObject = glCreateProgram();
	
	glAttachShader(programObject, vertex.ShaderObject);
	glAttachShader(programObject, fragment.ShaderObject);

	glLinkProgram(programObject);
	glGetProgramiv (programObject, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLint maxLength = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &maxLength);

		char* errorLog = new char[maxLength];
		glGetProgramInfoLog(programObject, maxLength, &maxLength, &errorLog[0]);
		cout << errorLog << endl;
		delete[] errorLog;
		
		glDetachShader(programObject,vertex.ShaderObject);
		glDetachShader(programObject,fragment.ShaderObject);
		vertex.Release();
		fragment.Release();
		
		throw "Link Error";
	}
	return programObject;
}