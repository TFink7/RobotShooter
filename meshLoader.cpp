#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>
#include "meshLoader.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <SOIL/SOIL.h>
#include <cmath>
#include <GL/glu.h>
#include <algorithm>




#define M_PI 3.14159265358979323846
#define BUFFER_OFFSET(i) ((void*)(i))

void drawLaser(const Laser& laser, GLUquadricObj* quadric);
void spawnRobotLaser(const Robot& robot);
void updateRobotAngles();
void updateRobotLasers();
BoundingSphere createRobotBoundingSphere(const Robot& robot);
BoundingSphere createCannonBoundingSphere(const GLfloat* vertices, int numVertices, float translateX, float translateY, float translateZ);
bool pointInsideBoundingSphere(float x, float y, float z, const BoundingSphere& bs);
void checkCollisionsAndUpdate();
void drawGameOverText();
void drawSkybox();
GLuint loadCubeMap(const char* fileNames[6]);
float calculateChargeProgress(float lastShotTime, float currentTime, float firingInterval);
void drawChargingBar(float chargeProgress);
void initializeRobotBoundingSpheres();

//texture setup
GLuint texture[4];




//Materials and lighting
GLfloat groundMat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat groundMat_diffuse[] = { 0.35, 0.35, 0.35, 1.0 };
//GLfloat groundMat_ambient[] = { 0.4, 0.8, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
//GLfloat groundMat_diffuse[] = { 0.4, 0.7, 0.4, 1.0 };
GLfloat groundMat_shininess[] = { 10.0 };

GLfloat robotBody_mat_ambient[] = { 0.0,0.0,0.0,1.0 };
GLfloat robotBody_mat_specular[] = { 0.45,0.55,0.45,1.0 };
GLfloat robotBody_mat_diffuse[] = { 0.35,0.1,0.1,1.0 };
GLfloat robotBody_mat_shininess[] = { 10.0 };


GLfloat bulletMat_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat bulletMat_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat bulletMat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat bulletMat_shininess[] = { 50.0f };

GLfloat light_position0[] = { 4.0, 8.0, -8.0, 1.0 };
GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient0[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat light_specular0[] = { 1.0, 1.0, 1.0, 1.0 };


GLfloat light_position1[] = { -4.0, 8.0, -8.0, 1.0 };
GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient1[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_specular1[] = { 1.0, 1.0, 1.0, 1.0 };



GLfloat robotArm_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat robotArm_mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat robotArm_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat robotArm_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gun_mat_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat gun_mat_shininess[] = { 100.0F };

GLfloat robotLowerBody_mat_ambient[] = { 0.05375f, 0.05f, 0.06625f, 0.82f };
GLfloat robotLowerBody_mat_diffuse[] = { 0.18275f, 0.17f, 0.22525f, 0.82f };
GLfloat robotLowerBody_mat_specular[] = { 0.332741f, 0.328634f, 0.346435f, 0.82f };
GLfloat robotLowerBody_mat_shininess[] = { 38.4F };

GLfloat lmKa[] = { 0.0, 0.0, 0.0, 0.0 };
GLfloat spot_direction[] = { 1.0, -1.0, -1.0 };
GLint spot_exponent = 30;
GLint spot_cutoff = 180;

GLfloat Kc = 1.0;
GLfloat Kl = 0.0;
GLfloat Kq = 0.0;

GLfloat material_Ka[] = { 0.5, 0.0, 0.0, 1.0 };
GLfloat material_Kd[] = { 0.4, 0.4, 0.5, 1.0 };
GLfloat material_Ks[] = { 0.8, 0.8, 0.0, 1.0 };
GLfloat material_Se = 20.0;

GLfloat quadMat_diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // Dark grey diffuse color
GLfloat quadMat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f }; // Dark grey ambient color
GLfloat quadMat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f }; // Specular color (keep it white for more natural reflections)
GLfloat quadMat_shininess[] = { 25.0f }; // Shininess value


//

GLuint program = 0;

GLint glutWindowWidth = 1600;
GLint glutWindowHeight = 800;
GLint viewportWidth = glutWindowWidth;
GLint viewportHeight = glutWindowHeight;




// screen window identifiers
int window3D;

int window3DSizeX = 1600, window3DSizeY = 800;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;


std::vector<Laser> lasers;
std::vector<Laser> robotLasers;
Robot robots[] = {
	Robot(60.0, 0.0, -35.0, -40.0, 0.0, 0.0, 0.0, 0.01, 0.5, false, Vector3D{60.0, 0.0, -35.0}),
	Robot(-20.0, 0.0, -35.0, 20.0, 0.0, 0.0, 0.0, 0.01, 0.5, false, Vector3D{-20.0, 0.0, -35.0}),
	Robot(-60.0, 0.0, -35.0, 45.0, 0.0, 0.0, 0.0, 0.01, 0.5, false, Vector3D{-60.0, 0.0, -35.0})
};

int numRobots = 3;
int hitRobotCount = 0;

std::vector<BoundingSphere> robotBoundingSpheres;
BoundingSphere cannonBoundingSphere;
bool cannonHit = false;



const float groundColor[] = { 9.0f, 0.5f, 0.4f, 1.0f };
const float cannonColor[] = { 0.0f, 5.0f, 0.0f, 1.0f };
GLuint indices[2048];
GLfloat vertices[1584];
GLfloat normals[1584];
GLfloat texCoords[1584];
bool Bullet = false;
bool gameRunning = false;

float robotBodyWidth = 4.0;
float robotBodyDepth = 4.0;
float robotBodyLength = 4.0;
float upperLegbase = 0.5 * robotBodyLength;
float upperLegTop = 0.5 * robotBodyWidth;
float upperLegHeight = 0.6 * robotBodyWidth;
float gunBase = robotBodyLength / 4;
float gunTop = robotBodyLength / 4;
float gunHeight = robotBodyDepth / 2;


float forward = 0.0;
float robotAngle = -16.0;
float hipAngle = 0.0f;
float hip = 0.0;
float kneeAngle = 0.0f;
float knee = 0.0;
float bodyAngle = 0.0;
float body = 0.0;
float gunAngle = 0.0;
float gun = 0.0;
float bodyRotationSpeed = 1.0f; 
int bodyRotationUpdateInterval = 100; 
float bodyAngleYaw = 0.0f;
float bodyAnglePitch = 0.0f;
float bodyAngleRoll = 0.0f;
float targetBodyAngleYaw = 0.0f;
float targetBodyAnglePitch = 0.0f;
float targetBodyAngleRoll = 0.0f;
int lastBodyRotationUpdateTime = 0;
int hipDirection = -1;
int kneeDirection = 1;


float minHip = -40.0;
float maxKnee = 60.0;



GLdouble fov = 60.0;
bool stop = true;
bool shooting = false;
bool gameOver = false;


int lastMouseX = -1;
int lastMouseY = -1;
int bullets = 0;

GLdouble projectileX = 0;
GLdouble projectileY = 0;
GLdouble projectileZ = 20.0;
GLdouble projectileVelocity = 10.0;
GLdouble projectilePitch = 0.0;
GLdouble projectileYaw = 0.0;

bool firing = false;


GLdouble lastUpdateTime = 0.0;
GLdouble currentTime = 0.0;


GLdouble eyeX = 0.0, eyeY = 3.0, eyeZ = 30.0;
GLdouble radius = eyeZ;
GLdouble zNear = 0.1, zFar = 100.0;
GLdouble yaw = 0.0;
GLdouble pitch = 0.0;
GLdouble frontX = 0.0;
GLdouble frontY = 0.0;
GLdouble frontZ = 0.0;
GLdouble upX = 0.0;
GLdouble upY = 1.0;
GLdouble upZ = 0.0;

GLdouble cannonX = eyeX;
GLdouble cannonY = eyeY - 2.0;
GLdouble cannonZ = eyeZ - 2.0;
GLdouble cannonPitch = 0.0;
GLdouble cannonYaw = 0.0;


int firstMouse = 1;
bool cannonAlive = false;

float lastBulletFiredTime = 0.0f;
float firingInterval = 0.5f;

const char* skyboxFileNames[] = {
	"right.jpg",
	"left.jpg",
	"top.jpg",
	"bottom.jpg",
	"back.jpg",
	"front.jpg"
};

GLuint skyboxTexture;
int currentScore = 0;
std::vector<int> highScores = { 0, 0, 0 };





int main(int argc, char* argv[])
{
	srand(static_cast<unsigned>(time(0)));
	setArrays();
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(glutWindowWidth, glutWindowHeight);
	glutInitWindowPosition(50, 100);
	// The 3D Window
	window3D = glutCreateWindow("Surface of Revolution");

	bool check = LoadGLTextures();
	if (!check)
	{
		printf("failed");
	}
	GLenum err2 = glewInit();
	if (GLEW_OK != err2) {
		fprintf(stderr, "Glew error: %s\n", glewGetErrorString(err2));
	}


	glutSetCursor(GLUT_CURSOR_NONE); // Hide the mouse cursor
	glutPositionWindow(900, 100);
	glutDisplayFunc(display3D);
	glutReshapeFunc(reshape3D);
	glutMouseWheelFunc(mouseScrollWheelHandler3D);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyHandler);
	glutPassiveMotionFunc(mouseMotionHandler);
	// Initialize the 3D system

	init3DSurfaceWindow();
	setShaders();
	printProgramInfoLog(program);

	
	glutTimerFunc(0, update, 0);

	// Annnd... ACTION!!
	glutMainLoop();
	
	return 0;
}

int currentButton;

/* shader reader */
/* creates null terminated string from file */

char* textFileRead(const char* fn) {

	FILE* fp;
	char* content = NULL;

	int count = 0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}


void setShaders() {

	GLuint v, f;
	char* vs, * fs;

	// Create shader handlers
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	// Read source code from files
	vs = textFileRead("vertexshader.vert");
	fs = textFileRead("fragmentshader.frag");

	const char* vv = vs;
	const char* ff = fs;

	// Set shader source
	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); free(fs);

	// Compile all shaders
	glCompileShader(v);
	glCompileShader(f);

	// Create the program
	program = glCreateProgram();

	// Attach shaders to program
	glAttachShader(program, v);
	glAttachShader(program, f);

	// Link and set program to use
	glLinkProgram(program);
	glUseProgram(program);
	GLint applyTextureLocation = glGetUniformLocation(program, "applyTexture");
	glUniform1i(applyTextureLocation, 1);
}

void init3DSurfaceWindow()
{

	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glViewport(0, 0, (GLsizei)window3DSizeX, (GLsizei)window3DSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(eyeX, eyeY, eyeZ, frontX, frontY, frontZ, upX, upY, upZ);

}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void reshape3D(int w, int h)
{
	glutWindowWidth = (GLsizei)w;
	glutWindowHeight = (GLsizei)h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, frontX, frontY, frontZ, upX, upY, upZ);

	glutPostRedisplay();
}

void display3D()
{
	glEnable(GL_COLOR_MATERIAL);
	GLUquadricObj* quadric;
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set up the Viewing Transformation (V matrix)	
	gluLookAt(eyeX, eyeY, eyeZ, frontX, frontY, frontZ, upX, upY, upZ);

	glDisable(GL_DEPTH_TEST);
	GLint isSkyboxUniformLocation = glGetUniformLocation(program, "isSkybox");
	glUniform1i(isSkyboxUniformLocation, 1);
	drawSkybox();
	glUniform1i(isSkyboxUniformLocation, 0); // Reset isSkybox to 0 for the rest of the scene
	glEnable(GL_DEPTH_TEST);

	

	for (int i = 0; i < numRobots; ++i) {
		const Robot& robot = robots[i];
		glPushMatrix();

		glTranslatef(robot.posX, robot.posY, robot.posZ);
		glRotatef(robot.angle, 0.0, 1.0, 0.0);
		glScalef(robot.scale, robot.scale, robot.scale);

		drawRobot(robot);
		glPopMatrix();

		robotBoundingSpheres[i].x = robot.posX;
		robotBoundingSpheres[i].y = robot.posY;
		robotBoundingSpheres[i].z = robot.posZ;
	}

	// Draw quad mesh
	
	drawGround();
	

	if (!cannonHit) {
		drawQuads();
	}
	

	// Position the sphere at the center of the bounding sphere
	glPushMatrix();
	glTranslatef(cannonBoundingSphere.x, cannonBoundingSphere.y, cannonBoundingSphere.z);

	// Draw a wireframe sphere with the same radius as the bounding sphere
	glutWireSphere(cannonBoundingSphere.radius, 12, 12);

	// Restore the transformation matrix
	glPopMatrix();


	for (const Laser& laser : lasers) {
		if(!gameOver){
			drawLaser(laser, quadric);
		}
		
	}

	for (const Laser& robotLaser : robotLasers) {

		if (!gameOver) {
			drawLaser(robotLaser, quadric);
		}

	}

	drawScoreboard(currentScore, highScores);
	
	

	if (gameOver)
	{
		drawGameOverText();
	}

	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float chargeProgress = calculateChargeProgress(lastBulletFiredTime, currentTime, firingInterval);
	drawChargingBar(chargeProgress);

	gluDeleteQuadric(quadric);

	glutSwapBuffers();
}



void drawGround()
{
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-80.0f, -4.0f, -80.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-80.0f, -4.0f, 80.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(80.0f, -4.0f, 80.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(80.0f, -4.0f, -80.0f);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}


void setArrays()
{
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;

	FILE* file = fopen("mesh.obj", "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		if (strcmp(lineHeader, "v") == 0)
		{
			fscanf(file, "%f %f %f\n", &vertices[i], &vertices[i + 1], &vertices[i + 2]);
			i += 3;
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			fscanf(file, "%f %f %f\n", &normals[j], &normals[j + 1], &normals[j + 2]);
			j += 3;
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			fscanf(file, "%f %f\n", &texCoords[l], &texCoords[l + 1]);
			l += 2;
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			int v[4], vt[4];
			fscanf(file, "%i/%i %i/%i %i/%i %i/%i\n", &v[0], &vt[0], &v[1], &vt[1], &v[2], &vt[2], &v[3], &vt[3]);
			indices[k] = v[0];
			indices[k + 1] = v[1];
			indices[k + 2] = v[2];
			indices[k + 3] = v[3];
			k += 4;
		}
	}
	cannonBoundingSphere = createCannonBoundingSphere(vertices, 528, cannonX, cannonY, cannonZ);
	initializeRobotBoundingSpheres();
}



void drawQuads()
{


	glPushMatrix();

	//glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);


	glTranslatef(cannonX, cannonY, cannonZ);
	glRotatef(cannonPitch, 1.0, 0.0, 0.0);
	glRotatef(cannonYaw, 0.0, 1.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);


	GLuint vboId;
	GLuint iboId;

	glGenBuffers(1, &vboId);
	glGenBuffers(1, &iboId);

	size_t vSize = sizeof vertices;
	size_t nSize = sizeof normals;
	size_t tSize = sizeof texCoords;


	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, vSize + nSize + tSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, normals);
	glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, tSize, texCoords);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);



	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);



	size_t nOffset = sizeof vertices;
	size_t tOffset = nOffset + sizeof normals;


	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	glNormalPointer(GL_FLOAT, 0, (void*)nOffset);
	glTexCoordPointer(2, GL_FLOAT, 0, (void*)tOffset);

	
	glDrawElements(GL_QUADS, 2048, GL_UNSIGNED_INT, (void*)0);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	//glPopAttrib();

	glPopMatrix();
	
}




void drawRobot(const Robot& robot) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glPushMatrix();
	//glTranslatef(robot.posX, robot.posY, robot.posZ);
	drawBody(robot);
	drawRightLeg();
	drawLeftLeg();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void drawBody(const Robot& robot)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);
	
	glPushMatrix();

	GLUquadricObj* sphere;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricTexture(sphere, GL_TRUE);

	glScalef(robotBodyWidth, robotBodyLength, robotBodyDepth);
	gluSphere(sphere, 1.0, 20, 20);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	glPushMatrix();

	glTranslatef(0.0, 0.0, (robotBodyLength));
	glRotatef(robot.gunAngleYaw, 0.0, 1.0, 0.0);
	glRotatef(robot.gunAnglePitch, 1.0, 0.0, 0.0);
	glRotatef(robot.gunAngleRoll, 0.0, 0.0, 1.0);

	GLUquadricObj* cylinder;
	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluQuadricTexture(cylinder, GL_TRUE);
	gluCylinder(cylinder, gunBase, gunTop, gunHeight, 20, 20);

	glPushMatrix();

	glTranslatef(0.0, 0.0, (gunHeight));

	GLUquadricObj* disk;
	disk = gluNewQuadric();
	gluQuadricDrawStyle(disk, GLU_FILL);
	gluQuadricNormals(disk, GLU_SMOOTH);
	gluQuadricTexture(disk, GL_TRUE);
	gluDisk(disk, 0.0, 1.0, 20, 20);

	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glTranslatef(0.0, 0.0, ((0.5 * gunHeight - 0.5 * gunBase)));
	glScalef(gunBase, gunBase, gunBase);
	gluSphere(sphere, 1.0, 20, 20);
	glPopMatrix();

	glPopMatrix();
	glPopMatrix();
	
}


void drawRightLeg()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	GLUquadricObj* sphere2;
	sphere2 = gluNewQuadric();
	gluQuadricDrawStyle(sphere2, GLU_FILL);
	gluQuadricNormals(sphere2, GLU_SMOOTH);
	gluQuadricTexture(sphere2, GL_TRUE);

	glPushMatrix();

	glTranslatef(-0.8 * robotBodyLength, 0.0, 0.0);
	glRotatef(hipAngle, 1.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);

	glPushMatrix();
	glScalef(upperLegbase, upperLegTop, upperLegHeight);
	GLUquadricObj* cylinder2;
	cylinder2 = gluNewQuadric();
	gluQuadricDrawStyle(cylinder2, GLU_FILL);
	gluQuadricNormals(cylinder2, GLU_SMOOTH);
	gluQuadricTexture(cylinder2, GL_TRUE);
	gluCylinder(cylinder2, 1.0, 1.0, 1.0, 20, 20);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.0, 0.0, 1.2 * upperLegbase);

	glPushMatrix();

	glScalef(upperLegbase, upperLegbase, upperLegbase);
	GLUquadricObj* disk2;
	disk2 = gluNewQuadric();
	gluQuadricDrawStyle(disk2, GLU_FILL);
	gluQuadricNormals(disk2, GLU_SMOOTH);
	gluQuadricTexture(disk2, GL_TRUE);
	gluDisk(disk2, 0.0, 1.0, 20, 20);

	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glPushMatrix();
	glTranslatef(-2.0, -0.5, -1.0);
	glRotatef(-kneeAngle, 0.0, 0.0, 1.0);
	glRotatef(50.0, 0.0, 0.0, 1.0);

	glPushMatrix();
	glScalef(3.0, 2.0, 1.0);
	gluSphere(sphere2, 1.0, 20, 20);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);
	glPushMatrix();

	glTranslatef(-2.0, -1.5, 0.0);
	glRotatef(90, 0.0, 0.0, 1.0);

	glPushMatrix();
	glScalef(4.0, 1.0, 1.0);

	gluSphere(sphere2, 1.0, 20, 20);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glPushMatrix();

	glTranslatef(-3.0, 0.0, 0.0);
	glRotatef(40, 0.0, 0.0, 1.0);

	glPushMatrix();
	glScalef(2.0, 1.0, 1.0);
	gluSphere(sphere2, 1.0, 20, 20);
	glPopMatrix();


	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();



}

void drawLeftLeg()
{

	GLUquadricObj* sphere3;
	sphere3 = gluNewQuadric();
	gluQuadricDrawStyle(sphere3, GLU_FILL);
	gluQuadricNormals(sphere3, GLU_SMOOTH);
	gluQuadricTexture(sphere3, GL_TRUE);

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glPushMatrix();

	glTranslatef(0.8 * robotBodyLength, 0.0, 0.0);
	glRotatef(hipAngle, 1.0, 0.0, 0.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);

	glPushMatrix();
	glScalef(upperLegbase, upperLegTop, upperLegHeight);
	GLUquadricObj* cylinder2;
	cylinder2 = gluNewQuadric();
	gluQuadricDrawStyle(cylinder2, GLU_FILL);
	gluQuadricNormals(cylinder2, GLU_SMOOTH);
	gluQuadricTexture(cylinder2, GL_TRUE);
	gluCylinder(cylinder2, 1.0, 1.0, 1.0, 20, 20);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.0, 0.0, 1.2 * upperLegbase);

	glPushMatrix();

	glScalef(upperLegbase, upperLegbase, upperLegbase);
	GLUquadricObj* disk2;
	disk2 = gluNewQuadric();
	gluQuadricDrawStyle(disk2, GLU_FILL);
	gluQuadricNormals(disk2, GLU_SMOOTH);
	gluQuadricTexture(disk2, GL_TRUE);
	gluDisk(disk2, 0.0, 1.0, 20, 20);

	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glPushMatrix();

	glTranslatef(2.0, -0.5, -1.0);
	glRotatef(kneeAngle, 0.0, 0.0, 1.0);
	glRotatef(-50.0, 0.0, 0.0, 1.0);

	glPushMatrix();
	glScalef(3.0, 2.0, 1.0);
	gluSphere(sphere3, 1.0, 20, 20);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glPushMatrix();

	glTranslatef(2.0, -1.5, 0.0);
	glRotatef(-90, 0.0, 0.0, 1.0);

	glPushMatrix();
	glScalef(4.0, 1.0, 1.0);
	gluSphere(sphere3, 1.0, 20, 20);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	glPushMatrix();

	glTranslatef(3.0, 0.0, 0.0);
	glRotatef(-40, 0.0, 0.0, 1.0);

	glPushMatrix();
	glScalef(2.0, 1.0, 1.0);
	gluSphere(sphere3, 1.0, 20, 20);
	glPopMatrix();


	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();


}

int LoadGLTextures()
{
	texture[0] = SOIL_load_OGL_texture
	(
		"robotjpg.bmp",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	if (texture[0] == 0)
		return false;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	texture[1] = SOIL_load_OGL_texture
	(
		"grass.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	if (texture[1] == 0)
		return false;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	texture[2] = SOIL_load_OGL_texture
	(
		"cannon.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	if (texture[2] == 0)
		return false;

	texture[3] = loadCubeMap(skyboxFileNames);

	if (texture[3] == 0)
		return false;

	return true;

}

// A few utility functions - use VECTOR3D.h or glm if you prefer
Vector3D crossProduct(Vector3D a, Vector3D b)
{
	Vector3D cross;

	cross.x = a.y * b.z - b.y * a.z;
	cross.y = a.x * b.z - b.x * a.z;
	cross.z = a.x * b.y - b.x * a.y;
	return cross;
}

float DotProduct(Vector3D lhs, Vector3D rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3D fourVectorAverage(Vector3D a, Vector3D b, Vector3D c, Vector3D d)
{
	Vector3D average;
	average.x = (a.x + b.x + c.x + d.x) / 4.0;
	average.y = (a.y + b.y + c.y + d.y) / 4.0;
	average.z = (a.z + b.z + c.z + d.z) / 4.0;
	return average;
}

Vector3D normalize(Vector3D a)
{
	GLdouble norm = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	Vector3D normalized;
	normalized.x = a.x / norm;
	normalized.y = a.y / norm;
	normalized.z = a.z / norm;
	return normalized;
}

float randomAngle(float minAngle, float maxAngle) {
	float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return minAngle + randomValue * (maxAngle - minAngle);
}


void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse)
{
	if (dir > 0) {
		fov -= 1.0;
		if (fov < 1.0) {
			fov = 1.0;
		}
	}
	else {
		fov += 1.0;
		if (fov > 179.0)
		{
			fov = 179.0;
		}
	}
	glutPostRedisplay();
}



void animationHandler2(int param)
{
	if (!stop && !gameOver)
	{

		const float hipSpeed = 2.0f * 2;
		const float kneeSpeed = 1.0f * 2;

		if (kneeAngle == maxKnee)
		{
			hipDirection = 1;
			kneeDirection = -1;
		}
		if (kneeAngle == 0.0f)
		{
			hipDirection = -1;
			kneeDirection = 1;
		}

		if (hipAngle > minHip && hipDirection == -1) {
			hipAngle += hipDirection * hipSpeed;
		}
		else if (hipAngle < 0.0f && hipDirection == 1) {
			hipAngle += hipDirection * hipSpeed * 2;
		}


		if (kneeAngle < maxKnee && kneeDirection == 1) {
			kneeAngle += kneeDirection * kneeSpeed;
		}
		else if (kneeAngle > 0.0f && kneeDirection == -1) {
			kneeAngle += kneeDirection * kneeSpeed * 2;
		}



		for (Robot& robot : robots) {
			// ... (other updates)

			// Update the robot's position
			if (robot.posZ < 35.0) {
				float angleRadians = robot.angle * M_PI / 180.0;
				robot.posX += robot.speed * sin(angleRadians) * 5.0;
				robot.posZ += robot.speed * cos(angleRadians) * 5.0;
			}
			else if (!robot.isRespawning) {
				robot.respawnTime = glutGet(GLUT_ELAPSED_TIME) + 5000;
				robot.isRespawning = true;
			}

			// Check if the robot should be respawned
			if (robot.isRespawning && glutGet(GLUT_ELAPSED_TIME) >= robot.respawnTime) {
				robot.isHit = false;
				robot.isRespawning = false;
				robot.scale = 0.5; // Reset the robot's scale
				robot.posX = robot.spawnPosition.x; // Set the robot's respawn position
				robot.posY = robot.spawnPosition.y;
				robot.posZ = robot.spawnPosition.z;
			}
		}


		checkCollisionsAndUpdate();
		updateRobotAngles();
		fireRobotLasers();
		updateRobotLasers();



		glutPostRedisplay();
		glutTimerFunc(16, animationHandler2, 0);
	}
}

void updateRobotAngles() {
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float timeStep = 0.01f;

	for (Robot& robot : robots) {
		// Update target angles periodically
		if (currentTime - robot.lastGunAngleUpdateTime > robot.gunAngleUpdateInterval) {
			robot.targetGunAngleYaw = randomAngle(-180.0, 180.0);
			robot.targetGunAnglePitch = randomAngle(-45.0, 45.0);
			robot.targetGunAngleRoll = randomAngle(-45.0, 45.0);
			robot.lastGunAngleUpdateTime = currentTime;
		}

		// Interpolate the gun angles smoothly
		robot.gunAngleYaw += (robot.targetGunAngleYaw - robot.gunAngleYaw) * robot.gunRotationSpeed * timeStep;
		robot.gunAnglePitch += (robot.targetGunAnglePitch - robot.gunAnglePitch) * robot.gunRotationSpeed * timeStep;
		robot.gunAngleRoll += (robot.targetGunAngleRoll - robot.gunAngleRoll) * robot.gunRotationSpeed * timeStep;
	}
}

void fireRobotLasers() {
    float shootProbability = 0.01; // Adjust this value to control how often the robots shoot
    for (size_t i = 0; i < numRobots; ++i) {
        Robot& robot = robots[i];
        float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        if (!robot.isHit && randomValue < shootProbability) {
            spawnRobotLaser(robot);
        }
    }
}




void drawLaser(const Laser& laser, GLUquadricObj* quadric) {

	
	glUseProgram(program);
	GLint applyTextureLocation = glGetUniformLocation(program, "applyTexture");
	glUniform1i(applyTextureLocation, 0);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glMaterialfv(GL_FRONT, GL_AMBIENT, bulletMat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bulletMat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bulletMat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, bulletMat_shininess);

	glPushMatrix();

	// Apply laser transformations (position and rotation)
	glTranslatef(laser.x, laser.y, laser.z);
	glRotatef(-laser.angleYaw, 0.0f, 1.0f, 0.0f);
	glRotatef(laser.anglePitch, 1.0f, 0.0f, 0.0f);

	GLUquadricObj* sphere;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	gluQuadricTexture(sphere, GL_TRUE);

	float bulletRadius = 0.2f; // Adjust the radius of the bullet
	gluSphere(sphere, bulletRadius, 16, 16);

	
	glPopMatrix();
	gluDeleteQuadric(sphere);

	glUniform1i(applyTextureLocation, 1);
}



void update(int value) {
	const float laserSpeed = 1.0f;

	for (size_t i = 0; i < lasers.size();) {
		Laser& laser = lasers[i];
		float yawRad = laser.angleYaw * M_PI / 180.0f;
		float pitchRad = laser.anglePitch * M_PI / 180.0f;
		laser.x += laserSpeed * sin(yawRad) * cos(pitchRad);
		laser.y += laserSpeed * sin(pitchRad);
		laser.z -= laserSpeed * cos(yawRad) * cos(pitchRad);

		if (laser.z < -100.0) {
			lasers.erase(lasers.begin() + i);
		}
		else {
			++i;
		}
	}

	

	glutPostRedisplay();
	glutTimerFunc(16, update, 0); // 60 FPS
}




void updateRobotLasers() {
	for (size_t i = 0; i < robotLasers.size(); ++i) {
		Laser& laser = robotLasers[i];
		float speed = 1.0f;
		float yawRad = laser.angleYaw * M_PI / 180.0f;
		float pitchRad = -laser.anglePitch * M_PI / 180.0f;

		laser.x += speed * sin(yawRad) * cos(pitchRad);
		laser.y += speed * sin(pitchRad);
		laser.z += speed * cos(yawRad) * cos(pitchRad);

		// Remove lasers that go out of range
		if (laser.z > 200.0f || laser.z < -200.0f || laser.y > 200.0f || laser.y < -200.0f || laser.x > 200.0f || laser.x < -200.0f) {
			robotLasers.erase(robotLasers.begin() + i);
			--i;
		}
	}
}


void spawnLaser() {

	Laser newLaser;
	newLaser.x = cannonX;
	newLaser.y = cannonY;
	newLaser.z = cannonZ;
	newLaser.angleYaw = -cannonYaw;
	newLaser.anglePitch = cannonPitch;

	// Adjust the initial position of the bullet to the nozzle
	float nozzleDistance = 2.5f; // Distance from the center of the cannon to the center of the nozzle
	float yawRad = newLaser.angleYaw * M_PI / 180.0f;
	float pitchRad = newLaser.anglePitch * M_PI / 180.0f;

	newLaser.x += nozzleDistance * sin(yawRad) * cos(pitchRad);
	newLaser.y += nozzleDistance * sin(pitchRad);
	newLaser.z -= nozzleDistance * cos(yawRad) * cos(pitchRad);

	lasers.push_back(newLaser);

}



void spawnRobotLaser(const Robot& robot) {
	Laser newLaser;
	newLaser.x = robot.posX;
	newLaser.y = robot.posY;
	newLaser.z = robot.posZ;
	newLaser.angleYaw = robot.gunAngleYaw + robot.angle;
	newLaser.anglePitch = robot.gunAnglePitch;

	// Adjust the initial position of the bullet to the nozzle
	float nozzleDistance = -4.0f;
	float turretOffsetZ = robotBodyLength + gunHeight + nozzleDistance;

	float yawRad = newLaser.angleYaw * M_PI / 180.0f;
	float pitchRad = -newLaser.anglePitch * M_PI / 180.0f;

	newLaser.x += turretOffsetZ * sin(yawRad) * cos(pitchRad);
	newLaser.y += turretOffsetZ * sin(pitchRad);
	newLaser.z += turretOffsetZ * cos(yawRad) * cos(pitchRad);

	robotLasers.push_back(newLaser);
}

void rotatePoint(float angleX, float angleY, float x, float y, float z, float& outX, float& outY, float& outZ) {
	float radiansX = angleX * (M_PI / 180.0);
	float radiansY = angleY * (M_PI / 180.0);

	// Rotate around the X-axis
	float newY = y * cos(radiansX) - z * sin(radiansX);
	float newZ = y * sin(radiansX) + z * cos(radiansX);

	// Rotate around the Y-axis
	float newX = x * cos(radiansY) + newZ * sin(radiansY);
	newZ = -x * sin(radiansY) + newZ * cos(radiansY);

	outX = newX;
	outY = newY;
	outZ = newZ;
}



BoundingSphere createRobotBoundingSphere(const Robot& robot) {
	BoundingSphere bs;
	bs.x = robot.posX;
	bs.y = robot.posY + robotBodyLength / 2; // Assuming the robot's origin is at its base
	bs.z = robot.posZ;
	bs.radius = (robotBodyWidth > robotBodyLength) ? robotBodyWidth / 2 : robotBodyLength / 2;
	bs.radius *= 1.5;
	return bs;
}



BoundingSphere createCannonBoundingSphere(const GLfloat* vertices, int numVertices, float translateX, float translateY, float translateZ) {
	BoundingSphere bs;

	// Calculate the center of the bounding sphere
	float centerX = 0;
	float centerY = 0;
	float centerZ = 0;
	for (int i = 0; i < numVertices * 3; i += 3) {
		centerX += vertices[i];
		centerY += vertices[i + 1];
		centerZ += vertices[i + 2];
	}
	centerX /= numVertices;
	centerY /= numVertices;
	centerZ /= numVertices;

	// Apply translation to the center
	centerX += translateX;
	centerY += translateY;
	centerZ += translateZ;

	bs.x = centerX;
	bs.y = centerY;
	bs.z = centerZ;

	// Calculate the radius of the bounding sphere
	float maxDistanceSquared = 0.0f;
	for (int i = 0; i < numVertices * 3; i += 3) {
		float dx = centerX - (vertices[i] + translateX);
		float dy = centerY - (vertices[i + 1] + translateY);
		float dz = centerZ - (vertices[i + 2] + translateZ);
		float distanceSquared = dx * dx + dy * dy + dz * dz;
		if (distanceSquared > maxDistanceSquared) {
			maxDistanceSquared = distanceSquared;
		}
	}
	bs.radius = std::sqrt(maxDistanceSquared) / 1.5;

	return bs;
}


bool pointInsideBoundingSphere(float x, float y, float z, const BoundingSphere& bs) {
	float dx = x - bs.x;
	float dy = y - bs.y;
	float dz = z - bs.z;
	float distanceSquared = dx * dx + dy * dy + dz * dz;
	return distanceSquared <= bs.radius * bs.radius;
}



void checkCollisionsAndUpdate() {
	// Check for collisions between bullets and robots
	if (numRobots == 0) {
		// All robots have been hit, no need to check for collisions
		return;
	}
	
	auto laserIt = lasers.begin();
	while (laserIt != lasers.end()) {
		bool laserErased = false;
		for (size_t j = 0; j < robotBoundingSpheres.size(); ++j) {
			if (pointInsideBoundingSphere(laserIt->x, laserIt->y, laserIt->z, robotBoundingSpheres[j])) {
				// If the robot is hit for the first time, start shrinking and stop firing
				if (j < numRobots && !robots[j].isHit && !robots[j].isRespawning) {
					robots[j].isHit = true;
					robots[j].scale = robots[j].scale * 0.9; // Start shrinking
					robots[j].respawnTime = glutGet(GLUT_ELAPSED_TIME) + 5000; // Set respawn time to 5 seconds later

					currentScore++;
				}

				// Remove the bullet
				laserIt = lasers.erase(laserIt);
				laserErased = true;
				break;
			}
		}
		if (!laserErased) {
			++laserIt;
		}
	}

	// Update the state of robots
	for (int i = 0; i < numRobots; ++i) {
		if (robots[i].isHit) {
			// Shrink the robot
			robots[i].scale *= 0.95;

			
			// If the robot has shrunk enough, remove it
			if (robots[i].scale < 0.01) {
				
				robots[i].scale = 1e-6; // Set the scale to a very small value
				robots[i].isRespawning = true;

			}
		}
	}
	if (!cannonHit) {
		auto robotLaserIt = robotLasers.begin();
		while (robotLaserIt != robotLasers.end()) {
			bool robotLaserErased = false;
			if (pointInsideBoundingSphere(robotLaserIt->x, robotLaserIt->y, robotLaserIt->z, cannonBoundingSphere)) {
				// The cannon is hit for the first time
				cannonHit = true;
				gameOver = true;

				// Remove the robot-fired laser
				robotLaserIt = robotLasers.erase(robotLaserIt);
				robotLaserErased = true;
			}
			if (!robotLaserErased) {
				++robotLaserIt;
			}
		}
	}
}






void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 't':
		//gunAngleX += 1.0;
		//if (gunAngleX > 90.0)
		//{
		//	gunAngleX = 90.0;
		//}
		//gunAngleY += 1.0;
		//if (gunAngleY > 90.0)
		//{
		//	gunAngleY = 90.0;
		//}
		break;
	case 'r':
		break;
	case 'R':
		restartGame();
		break;
	case 'w':
		if (stop) {
			stop = false;
			glutTimerFunc(16, animationHandler2, 0);
		}
		break;
	case 'W':
		hipAngle = 0.0f;
		kneeAngle = 0.0f;
		stop = true;
		break;
	case ' ':
		float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		if (!cannonHit && currentTime - lastBulletFiredTime >= firingInterval) {
			lastBulletFiredTime = currentTime;
			spawnLaser();
		}
		break;
	}
	glutPostRedisplay();   // Trigger a window redisplay
}

void specialKeyHandler(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		cannonYaw += 2.0;
		break;
	case GLUT_KEY_RIGHT:
		cannonYaw -= 2.0;
		break;
	case GLUT_KEY_UP:
		cannonPitch += 2.0;
		break;
	case GLUT_KEY_DOWN:
		cannonPitch -= 2.0;
		break;
	}
	glutPostRedisplay();
}

void mouseMotionHandler(int x, int y) {
	if (lastMouseX != -1 && lastMouseY != -1) {
		float deltaX = x - lastMouseX;
		float deltaY = y - lastMouseY;

		// Adjust the sensitivity of the mouse control
		float sensitivity = 0.1f;

		cannonYaw -= deltaX * sensitivity;
		cannonPitch -= deltaY * sensitivity;
		//updateCannonBoundingSphere();

		// Clamp the cannonPitch angle to avoid flipping the cannon
		if (cannonPitch >= 40.0) {
			cannonPitch = 40.0;
		}
		else if (cannonPitch <= -20.0f) {
			cannonPitch = -20.0f;
		}

		if (cannonYaw >= 90.0) {
			cannonYaw = 90.0;
		}
		else if (cannonYaw < -90.0) {
			cannonYaw = -90.0;
		}
	}

	lastMouseX = x;
	lastMouseY = y;

	glutPostRedisplay();
}


void restartGame() {
	// Update high scores if current score is greater than the lowest high score
	stop = true;
	gameOver = false;
	if (currentScore > highScores.back()) {
		for (int i = 0; i < highScores.size(); i++) {
			if (currentScore > highScores[i]) {
				highScores.insert(highScores.begin() + i, currentScore);
				highScores.pop_back();
				break;
			}
		}
	}

	// Reset current score
	currentScore = 0;
	lasers.clear();
	robotLasers.clear();
	// Reset robots to their starting positions
	for (Robot& robot : robots) {
		robot.posX = robot.spawnPosition.x;
		robot.posY = robot.spawnPosition.y;
		robot.posZ = robot.spawnPosition.z;
		robot.isHit = false;
		robot.isRespawning = false;
		robot.scale = 0.5;
	}

	cannonHit = false;
	cannonPitch = 0.0;
	cannonYaw = 0.0;
}



void renderText(float x, float y, void* font, const char* text) {
	glRasterPos2f(x, y);
	for (const char* c = text; *c != '\0'; c++) {
		glutBitmapCharacter(font, *c);
	}
}

int textLength(void* font, unsigned char* text) {
	int length = 0;
	for (unsigned char* c = text; *c != '\0'; c++) {
		length += glutBitmapWidth(font, *c);
	}
	return length;
}

void drawGameOverText() {
	const char* gameOverText = "GAME OVER";
	const char* restartText = "Press R to restart";

	int textLengthGameOver = textLength(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)gameOverText);
	int textLengthRestart = textLength(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)restartText);
	
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, glutWindowWidth, 0.0, glutWindowHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glColor3f(1.0f, 0.0f, 0.0f);

	// Draw GAME OVER
	glRasterPos2i(glutWindowWidth / 2 - textLengthGameOver / 2, glutWindowHeight / 2 + 30);
	for (const char* c = gameOverText; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}

	// Draw Press R to restart
	glRasterPos2i(glutWindowWidth / 2 - textLengthRestart / 2, glutWindowHeight / 2 - 30);
	for (const char* c = restartText; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glUseProgram(program);
}

void drawScoreboard(int currentScore, const std::vector<int>& highScores) {

	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, glutWindowWidth, 0.0, glutWindowHeight);

	glMatrixMode(GL_MODELVIEW); 
	glPushMatrix();
	glLoadIdentity();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	const float padding = 10.0f;
	float lineHeight = 18.0f;

	float highScoreY = padding + lineHeight;
	for (int i = 0; i < highScores.size(); ++i) {
		std::string highScoreText = "High Score " + std::to_string(i + 1) + ": " + std::to_string(highScores[i]);
		renderText(padding, glutWindowHeight - (highScoreY + lineHeight * i), GLUT_BITMAP_HELVETICA_18, highScoreText.c_str());
	}

	std::string scoreText = "Current Score: " + std::to_string(currentScore);
	renderText(padding, glutWindowHeight - (padding + lineHeight * (highScores.size() + 1)), GLUT_BITMAP_HELVETICA_18, scoreText.c_str());

	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glUseProgram(program);
}


void drawChargingBar(float chargeProgress) {
	int barWidth = 400;
	int barHeight = 40;
	int barMargin = 40;

	int filledWidth = static_cast<int>(barWidth * chargeProgress);
	float red = (1.0f - chargeProgress) * 2.0f;
	float green = chargeProgress * 2.0f;

	// Clamp the color values between 0 and 1
	red = std::min<float>(std::max<float>(red, 0.0f), 1.0f);
	green = std::min<float>(std::max<float>(green, 0.0f), 1.0f);

	// Set up the orthographic projection for 2D drawing
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(0);
	glDisable(GL_LIGHTING);

	// Draw the filled part of the charging bar
	glColor3f(red, green, 0.0f);
	glBegin(GL_QUADS);
	glVertex2i(barMargin, barMargin);
	glVertex2i(barMargin + filledWidth, barMargin);
	glVertex2i(barMargin + filledWidth, barMargin + barHeight);
	glVertex2i(barMargin, barMargin + barHeight);
	glEnd();

	// Draw the outline
	glColor3f(0.0f, 0.0f, 0.0f);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2i(barMargin, barMargin);
	glVertex2i(barMargin + barWidth, barMargin);

	glVertex2i(barMargin + barWidth, barMargin);
	glVertex2i(barMargin + barWidth, barMargin + barHeight);

	glVertex2i(barMargin + barWidth, barMargin + barHeight);
	glVertex2i(barMargin, barMargin + barHeight);

	glVertex2i(barMargin, barMargin + barHeight);
	glVertex2i(barMargin, barMargin);
	glEnd();

	glEnable(GL_LIGHTING);
	glUseProgram(program);

	// Restore the original matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}




float calculateChargeProgress(float lastShotTime, float currentTime, float firingInterval) {
	float timeSinceLastShot = currentTime - lastShotTime;
	float chargeProgress = timeSinceLastShot / firingInterval;
	return std::min<float>(chargeProgress, 1.0f);
}



void drawSkybox() {
	GLfloat skyboxVertices[] = {
		// Back face
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 // Front face
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		 -1.0f,  1.0f,  1.0f,
		 // Left face
		 -1.0f,  1.0f,  1.0f,
		 -1.0f,  1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f,  1.0f,
		 // Right face
		  1.0f,  1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f, -1.0f,
		  1.0f,  1.0f, -1.0f,
		  // Top face
		  -1.0f,  1.0f, -1.0f,
		  -1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f, -1.0f,
		   // Bottom face
		   -1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
		   -1.0f, -1.0f,  1.0f
	};
	
	GLfloat skyboxScale = 80.0f;

	glPushMatrix();
	// Position the skybox at the camera position (this will keep it centered around the camera)
	glTranslatef(eyeX, eyeY, eyeZ);

	// Scale the skybox
	glScalef(skyboxScale, skyboxScale, skyboxScale);

	
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture[3]);

	// Render the skybox
	glBegin(GL_QUADS);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			glTexCoord3fv(&skyboxVertices[(i * 4 + j) * 3]);
			glVertex3fv(&skyboxVertices[(i * 4 + j) * 3]);
		}
	}
	glEnd();

	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);
	

	glPopMatrix();
}

GLuint loadCubeMap(const char* fileNames[6]) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (GLuint i = 0; i < 6; i++) {
		int width, height;
		unsigned char* image = SOIL_load_image(fileNames[i], &width, &height, 0, SOIL_LOAD_RGB);
		if (image) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
		else {
			std::cout << "Failed to load texture: " << fileNames[i] << std::endl;
			SOIL_free_image_data(image);
			return 0;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);

	return textureID;
}

void initializeRobotBoundingSpheres() {
	for (int i = 0; i < numRobots; ++i) {
		// Get the robot from the robots array
		const Robot& robot = robots[i];

		// Create a bounding sphere for the robot
		BoundingSphere sphere = createRobotBoundingSphere(robot);

		// Add the bounding sphere to the robotBoundingSpheres vector
		robotBoundingSpheres.push_back(sphere);
	}
}