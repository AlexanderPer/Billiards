#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "ScreenRect.h"
#include "Texture.h"
#include "CameraControl.h"
#include "Dynamics.h"

RectSurface* RectScreen;
CameraControl CamControl;
TableDynamics TblDynamics;

unsigned int shaderProgram;

int windowWidth = 800;
int windowHeight = 600;
float antialiasing = 0.0f;

using namespace std;

char VertexShaderName[] = "shaders\\vert.glsl";
char FragmentShaderName[] = "shaders\\frag.glsl";
char TextureFileName[] = "tex\\ball_tex.bmp";

GLuint BallTexture;

void initTexture() 
{	
	glActiveTexture(GL_TEXTURE0);
	BallTexture = LoadTextureBMP(TextureFileName);
	
	glBindTexture(GL_TEXTURE_2D, BallTexture);

	int texLoc = glGetUniformLocation(shaderProgram, "ballTexture");
	if (texLoc>-1)
		glUniform1ui(texLoc, GL_TEXTURE0);
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
		
	shaderProgram = InitShaderProgram(VertexShaderName, FragmentShaderName);	
	glUseProgram(shaderProgram);	
		
	RectScreen = new RectSurface((float)windowWidth, (float)windowHeight);
	int locAA = glGetUniformLocation(shaderProgram, "antialiasing");
	if (locAA > -1)
		glUniform1f(locAA, antialiasing);
	RectScreen->Init(shaderProgram);
	CamControl.Init(shaderProgram);
	TblDynamics.Init(shaderProgram, &CamControl);
	initTexture();
}

void reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	RectScreen->SetResolution((float)windowWidth, (float)windowHeight);

	glViewport(0, 0, width, height);
	glm::mat4 mv = glm::ortho(-1.f, 1.f, -1.f, 1.f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&mv[0][0]);	
}

void display()
{
	glClearColor(0.f, 0.f, 1.f, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	CamControl.Update();
	TblDynamics.Update();
	RectScreen->Draw();

	glutSwapBuffers();  
}

//IdleFunction
void update()
{
	//make animation
	glutPostRedisplay();
}

void keyboard(unsigned char key, int mx, int my)
{
	CamControl.PressKey(key);
	glutPostRedisplay();
}

void keyboard_up(unsigned char key, int mx, int my)
{
	CamControl.UpKey(key);
	glutPostRedisplay();
}

void specialKey(int key, int x, int y) {
	CamControl.PressKey(key);
	glutPostRedisplay();
}

void specialKey_up(int key, int x, int y) {
	CamControl.UpKey(key);
	glutPostRedisplay();
}

void mouse(int button, int mode,int posx, int posy)
{
	if ((button == GLUT_LEFT_BUTTON) && (mode == GLUT_DOWN))
		TblDynamics.StrikeBall();
}

void emptydisplay() {}

void printHelp() {
  std::cout << "\nCamera control keys:\n"            
			<< "press 'a' to left move\n"
			<< "press 'd' to right move\n"
			<< "press 'w' to forward move\n"
			<< "press 's' to backward move\n"
			<< "press 'PgUp' to up move\n"
			<< "press 'PgDn' to down move\n"
			<< "press Left Button Mouse to strike ball\n"
			<< "----------------\n"
			<< "Startup options:\n"
			<< "[a] - enable antialiasing\n";      
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
		if (argv[1][0] == 'a')
			antialiasing = 1;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Billiards");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutReshapeWindow(windowWidth, windowHeight);

	glutIdleFunc(update);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboard_up);
	glutSpecialFunc(specialKey);
	glutSpecialUpFunc(specialKey_up);
	glutMouseFunc(mouse);

	GLenum err = glewInit() ; 
	if (GLEW_OK != err) { 
		std::cerr << "Error: " << glewGetString(err) << std::endl; 
	}
	const char * slVer = (const char *) glGetString ( GL_SHADING_LANGUAGE_VERSION );
	cout << "GLSL Version: " << slVer << endl;

	try
	{
		init();
	}
	catch (const char *str)
	{
		cout << "Error During Initialiation: " << str << endl;		
		glDeleteTextures(1, &BallTexture);
		glutDisplayFunc(emptydisplay);
		glutMainLoop();
		return -1;
	}
	printHelp();
	try
	{
		glutMainLoop();
	}
	catch (const char *str)
	{
		cout << "Error During Main Loop: " << str << endl;
	}
	glDeleteTextures(1, &BallTexture);
	delete RectScreen;
	return 0;
}