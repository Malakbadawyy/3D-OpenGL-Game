#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

float PlayerX = 1;
float PlayerY = -0.5;
float PlayerZ = 0;
float playerAngle = 0;

float wallColor[3] = { 0.5f, 0.2f, 0.7f }; // Initial wall color

bool isAnimationActive = false;
float cupRotationAngle = 0.0f;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {


		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

void drawGround(double thickness) {
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glColor3f(0.5 , 0.4 ,0.4); // Set the ground color
	glutSolidCube(2);
	glPopMatrix();
}
void drawWall(double thickness) {
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glColor3fv(wallColor); // Set the wall color
	glutSolidCube(2);
	glPopMatrix();

	
}


void changeWallColor(int value) {
	// Change the wall color here
	wallColor[0] = static_cast<float>(rand()) / RAND_MAX; // Random value for red component
	wallColor[1] = static_cast<float>(rand()) / RAND_MAX; // Random value for green component
	wallColor[2] = static_cast<float>(rand()) / RAND_MAX; // Random value for blue component

	glutPostRedisplay(); // Trigger a redraw to update the color
	glutTimerFunc(2000, changeWallColor, 0); // Change color every 2000 milliseconds (2 seconds)
}

void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}
void drawJackPart() {
	glPushMatrix();
	glScaled(0.2, 0.2, 1.0);
	glutSolidSphere(1, 15, 15);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 1.2);
	glutSolidSphere(0.2, 15, 15);
	glTranslated(0, 0, -2.4);
	glutSolidSphere(0.2, 15, 15);
	glPopMatrix();
}
void drawJack() {
	glPushMatrix();
	drawJackPart();
	glRotated(90.0, 0, 1, 0);
	drawJackPart();
	glRotated(90.0, 1, 0, 0);
	drawJackPart();
	glPopMatrix();
}
void drawTable(double topWid, double topThick, double legThick, double legLen) {
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95 * topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camera.look();
}

void drawHuman(float x, float y, float z) {
	glPushMatrix();
	glTranslated(x, y, z); // Translate to the specified position
	glRotatef(playerAngle, 0.0, 1.0, 0.0);

	glScaled(0.5,0.5,0.5);
    // Head (First Primitive)
    glPushMatrix();
	glColor3f(0.9 , 0.7 , 0.6);
    glTranslated(0.0, 1.2, 0.0);
    glutSolidSphere(0.2, 20, 20); // Smaller head
    glPopMatrix();

	// Body - Torso  (Second Primitive)
	glPushMatrix();
	glTranslated(0.0, 0.8, 0.0);

	// Front Side of Body Cube
	glPushMatrix();
	glColor3f(1, 0, 0); // Set the color for the front side
	glutSolidCube(0.2); // Make the front side slightly smaller
	glPopMatrix();

	// Back Side of Body Cube
	glPushMatrix();
	glTranslated(0.0, 0.0, -0.2); // Move to the back side
	glColor3f(0, 1, 0); // Set a different color for the back side
	glutSolidCube(0.4); // Make the back side larger or different
	glPopMatrix();

	glPopMatrix();

    // Limbs - Arms  (Third Primitive)
    glPushMatrix();
	glColor3f(0.9, 0.7, 0.6);
    glTranslated(0.3, 1.0, 0.0); // Right arm
    glScaled(0.15, 0.6, 0.15); // Smaller arms
    glutSolidCube(1.0);
    glPopMatrix();

	//(Fourth Primitive)
    glPushMatrix();
    glTranslated(-0.3, 1.0, 0.0); // Left arm
    glScaled(0.15, 0.6, 0.15); // Smaller arms
    glutSolidCube(1.0);
    glPopMatrix();

    // Limbs - Legs (Fifth Primitive)
    glPushMatrix();
	glColor3f(0.2, 0.1 ,0.6);
    glTranslated(0.15, 0.4, 0.0); // Right leg
    glScaled(0.2, 0.5, 0.15); // Smaller legs
    glutSolidCube(1.0);
    glPopMatrix();
	// Limbs - Legs (Sixth Primitive)
    glPushMatrix();
	glColor3f(0.2, 0.1, 0.6);
    glTranslated(-0.15, 0.4, 0.0); // Left leg
    glScaled(0.2, 0.5, 0.15); // Smaller legs
    glutSolidCube(1.0);
    glPopMatrix();
	glPopMatrix();
}

void drawCarousel() {

	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);

	// Main Pole
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslated(0.0, 0.0, 0.0);
	glScaled(0.1, 3.0, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	// Top Roof
	glPushMatrix();
	glColor3f(0.8f, 0.0f, 0.0f);
	glTranslated(0.0, 1.5, 0.0);
	glScaled(1.5, 0.1, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();

	// Base Platform
	glPushMatrix();
	glColor3f(0.3f, 0.3f, 0.8f);
	glTranslated(0.0, -0.2, 0.0);
	glScaled(1.8, 0.2, 1.8);
	glutSolidCube(1.0);
	glPopMatrix();

	// Support Columns
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslated(0.0, 0.8, 1.0);
	glScaled(0.1, 1.5, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslated(0.0, 0.8, -1.0);
	glScaled(0.1, 1.5, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslated(0.0, 0.8, 1.0);
	glScaled(0.1, 1.5, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslated(0.0, 0.8 , -1.0);
	glScaled(0.1, 1.5, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();

}

void drawSmallCup(float angle) {
	glPushMatrix();
	glRotatef(angle, 0, 1, 0);
	glTranslatef(1.2, 0, 0);
	glColor3f(0.2f, 0.8f, 0.2f);
	glutSolidTeapot(0.1);
	glPopMatrix();
}

void drawTeapot() {
	glPushMatrix();
	glColor3f(0.8f, 0.2f, 0.2f);
	glutSolidTeapot(0.5);
	glPopMatrix();
}

void updateCupRotation(int value) {
	// Update the cup rotation angle
	cupRotationAngle += 1.0f; // Adjust the rotation speed as needed
	if (cupRotationAngle > 360.0f) {
		cupRotationAngle -= 360.0f; // Keep the angle within 360 degrees
	}

	glutPostRedisplay(); // Request a redraw
	glutTimerFunc(25, updateCupRotation, 0); // Set the next timer callback
}
void drawCup(float angle) {
	// Cup Body
	glPushMatrix();
	glRotatef(angle, 0, 1, 0);
	glColor3f(1.0f, 0.8431f, 0.0f);
	glutSolidCone(0.5, 1.0, 20, 20); // Cup body as a cone
	glPopMatrix();

	// Top Cover
	glPushMatrix();
	glColor3f(1.0f, 0.8431f, 0.0f);
	glTranslated(0.0, 1.0, 0.0);
	glutSolidSphere(0.5, 20, 20); // Top cover as a sphere
	glPopMatrix();

	// Handle (a simple rectangular handle)
	glPushMatrix();
	glColor3f(1.0f, 0.8431f, 0.0f);
	glRotated(-90, 0, 1, 0);
	glTranslated(-0.5, 0.5, 0.0);
	glScaled(0.1, 0.3, 0.1);
	glutSolidCube(1.0); // Handle as a cube
	glPopMatrix();
}

void drawFirstGame() {
	//// Base
	glPushMatrix();
	glColor3f(0.3f, 0.3f, 0.8f);
	glTranslated(0.0, -0.2, 0.0);
	glScaled(1.8, 0.2, 1.8);
	glutSolidCube(1.0);
	glPopMatrix();

	//// Big teapot in the middle
	//drawTeapot();

	int numCups = 8; // Number of small cups
	for (int i = 0; i < numCups; ++i) {
		float angle = i * (360.0 / numCups) + cupRotationAngle; // Apply rotation angle
		glPushMatrix();
		glScaled(0.5, 1, 0.5);
		drawCup(angle);
		glPopMatrix();
	}
}



void drawTree(float x, float y, float z) {
	static float sphereOffsets[3] = { 0.0, 0.0, 0.0 }; // Store initial sphere offsets

	glPushMatrix();
	glTranslated(x, y, z); // Translate to the specified position

	// Trunk - Cuboid
	glColor3f(0.5, 0.35, 0.05); // Brown color for the trunk

	glPushMatrix();
	glScaled(0.2, 2.0, 0.2); // Adjust the dimensions of the trunk
	glutSolidCube(1.0);
	glPopMatrix();

	// Foliage - Spheres
	glTranslated(0.0, 1.0, 0.0); // Move to the top of the trunk
	glColor3f(0.0, 0.6, 0.1); // Green color for the foliage

	if (isAnimationActive) {
		// If animation is active, modify sphere positions
		for (int i = 0; i < 3; ++i) {
			// Offset each sphere's position using a sine function for vibration effect
			sphereOffsets[i] = sin(glutGet(GLUT_ELAPSED_TIME) * 0.001 + i * 0.5) * 0.1;
		}
	}

	glTranslated(0.0, 0.0 + sphereOffsets[0], 0.0);
	glutSolidSphere(0.5, 20, 20); // Largest foliage sphere

	glTranslated(0.0, 0.2 + sphereOffsets[1], 0.0);
	glutSolidSphere(0.3, 20, 20); // Middle foliage sphere

	glTranslated(0.0, 0.1 + sphereOffsets[2], 0.0);
	glutSolidSphere(0.4, 20, 20); // Smallest foliage sphere

	glPopMatrix();
}



void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glPushMatrix();
	//glTranslated(0.4, 0.4, 0.6);
	//glRotated(45, 0, 0, 1);
	//glScaled(0.08, 0.08, 0.08);
	//drawJack();
	//glPopMatrix();
	//glPushMatrix();
	//glTranslated(0.6, 0.38, 0.5);
	//glRotated(30, 0, 1, 0);
	//glutSolidTeapot(0.08);
	//glPopMatrix();
	//glPushMatrix();
	//glTranslated(0.25, 0.42, 0.35);
	//glutSolidSphere(0.1, 15, 15);
	//glPopMatrix();
	//glPushMatrix();
	//glTranslated(0.4, 0.0, 0.4);
	//drawTable(0.6, 0.02, 0.02, 0.3);
	//glPopMatrix();
	glPushMatrix();
	glTranslated(0, -0.5, 0);
	glScaled(2,2, 2);
	drawGround(0.02);
	glPopMatrix();


	//Draw Left Wall
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(-0.25, 1, 0);
	glScaled(0.5, 2, 2);
	drawWall(0.02);
	glPopMatrix();

	//Draw Left Fence
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(0.52, 1 , 1);
	glScaled(0.02, 0.02,2);
	glColor3f(0.4, 0, 0.2); // Set the wall color
	glutSolidCube(2);
	glPopMatrix();

	//Draw Back Fence
	glPushMatrix();
	glRotated(90, 0.0, 1.0, 0.0);
	glTranslated(1, 0.52, 1);
	glScaled(0.02, 0.02, 2);
	glColor3f(0.4, 0, 0.2); // Set the wall color
	glutSolidCube(2);
	glPopMatrix();

	//Draw Right Fence
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(0.52, -3, 1);
	glScaled(0.02, 0.02, 2);
	glColor3f(0.4, 0, 0.2); // Set the wall color
	glutSolidCube(2);
	glPopMatrix();


	//Draw Back Wall
	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(0,1 , -0.25);
	glScaled(2,2, 0.5);
	drawWall(0.02);
	glPopMatrix();

	//Draw Right Wall
	glPushMatrix();
	glTranslated(3,-0.25,0);
	glRotated(90, 0.0, 0, 1.0);
	glScaled(0.5, 2, 2);
	drawWall(0.02);
	glPopMatrix();


	//Draw Player
	glPushMatrix();
	drawHuman(PlayerX,PlayerY,PlayerZ);
	glPopMatrix();




	//glPushMatrix();
	//drawCarousel();
	//glPopMatrix();


	//Draw Goal
	glPushMatrix();
	glTranslated(0,-0.25,0);
	glScaled(0.5, 0.5, 0.5);
	drawFirstGame();
	glPopMatrix();

	glPushMatrix();
	glScaled(0.6, 0.75, 0.6);

	drawTree(2.30, -0.5, -0.75);
	glPopMatrix();

	glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 't':
	case 'T':
		// Top View
		camera.eye = Vector3f(0.0f, 1.0f, 0.0f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 0.0f, -1.0f);
		break;
	case 'v':
	case 'V':
		// Side View
		camera.eye = Vector3f(1.0f, 0.0f, 0.0f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 1.0f, 0.0f);
		break;
	case 'f':
	case 'F':
		// Front View
		camera.eye = Vector3f(0.0f, 0.0f, 1.0f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 1.0f, 0.0f);
		break;
	case 'n':
		// Move forward
		if (PlayerZ + d <= 2.75) // Ensure player does not exceed wall height
		{
			PlayerZ += d;
			playerAngle = 0;
		}
		break;
	case 'h':
		// Move backwards
		if (PlayerZ - d >= -0.75) // Ensure player does not go below the floor
		{
			PlayerZ -= d;
			playerAngle = 180;

		}
		break;
	case 'b':
		// Move left
		if (PlayerX - d >= -0.75) // Ensure player does not go beyond left wall
		{
			PlayerX -= d;
			playerAngle = -90;

		}

		break;
	case 'm':
		// Move right
		if (PlayerX + d <= 2.75) // Ensure player does not go beyond right wall
		{
			PlayerX += d;
			playerAngle = 90;

		}
		break;
	case 'p':
		// Toggle animation on 'p' key press
		isAnimationActive = !isAnimationActive;
		break;


	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);
	glutTimerFunc(25, updateCupRotation, 0); // Start the cup rotation animation
	glutCreateWindow("Amusement Park");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutTimerFunc(0, changeWallColor, 0); // Start the color-changing timer
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}
