#include <string>
#include <vector>
typedef struct Vector2D
{
	GLdouble x, y;
	GLdouble nx, ny;
	GLdouble u; // running distance
} Vector2D;

typedef struct Vector3D
{
	GLdouble x, y, z;
} Vector3D;

typedef struct Laser {
	float x, y, z;
	float angleYaw, anglePitch;
	float initialCannonYaw, initialCannonPitch;
} Laser;

typedef struct BoundingSphere {
	float x, y, z; // Center coordinates
	float radius;
} BoundingSphere;


typedef struct Robot {
	double posX, posY, posZ;
	double angle;
	double gunAngleYaw, gunAnglePitch, gunAngleRoll;
	double speed;
	double scale;
	bool isHit;
	Vector3D spawnPosition;
	float respawnTime;
	bool isRespawning;

	double targetGunAngleYaw, targetGunAnglePitch, targetGunAngleRoll;
	unsigned int lastGunAngleUpdateTime;
	unsigned int gunAngleUpdateInterval;
	float gunRotationSpeed;

	Robot(double x = 0.0, double y = 0.0, double z = 0.0, double ang = 0.0, double gunYaw = 0.0, double gunPitch = 0.0, double gunRoll = 0.0, double speed = 0.0, double defScale = 0.5, bool hit = false, Vector3D spawnPos = Vector3D{ 0, 0, 0 })
		: posX(x), posY(y), posZ(z), angle(ang), gunAngleYaw(gunYaw), gunAnglePitch(gunPitch), gunAngleRoll(gunRoll), speed(speed),
		targetGunAngleYaw(gunYaw), targetGunAnglePitch(gunPitch), targetGunAngleRoll(gunRoll),
		lastGunAngleUpdateTime(0), gunAngleUpdateInterval(1000), gunRotationSpeed(0.1f), scale(defScale), isHit(hit), spawnPosition(spawnPos), isRespawning(false) {}
} Robot;



// Struct prototypes
typedef struct SubdivisionCurve SubdivisionCurve;
typedef struct Rect Rect;
typedef struct Button Button;
typedef struct Vertex Vertex;
// Function prototypes
void init3DSurfaceWindow();


/*

	3D Functions

*/






void display3D();
void reshape3D(int w, int h);
void specialKeyHandler(int key, int x, int y);
void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse);
void setArrays();
void mouseMotionHandler(int x, int y);
//void animationHandler1(int param);
//void keyboard(unsigned char key, int x, int y);
//void animationHandler3(int value);
//void fireLaser();
//void drawLasers();
//void updateLasers();
void update(int value);
//void drawLaser(const Laser& laser);
void update(int value);

Vector3D crossProduct(Vector3D a, Vector3D b);
float DotProduct(Vector3D lhs, Vector3D rhs);
Vector3D fourVectorAverage(Vector3D a, Vector3D b, Vector3D c, Vector3D d);
Vector3D normalize(Vector3D a);

float randomAngle(float minAngle, float maxAngle);
void drawGround();
void drawQuads();
void DrawBullet();
void animationHandler2(int param);
void drawRobot(const Robot& robot);
void drawBody(const Robot& robot);
void drawRightLeg();
void drawLeftLeg();
void keyboard(unsigned char key, int x, int y);
void printProgramInfoLog(GLuint obj);
void printShaderInfoLog(GLuint obj);
void setShaders();
char* textFileRead(const char* fn);
int LoadGLTextures();
void restartGame();
void fireRobotLasers();
void renderText(float x, float y, void* font, const char* text);
void drawScoreboard(int currentScore, const std::vector<int>& highScores);