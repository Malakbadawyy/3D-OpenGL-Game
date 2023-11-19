#include<string>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <string>


#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

float PlayerX = 1;
float PlayerY = -0.5;
float PlayerZ = 0;
float playerAngle = 0;
float vibrationOffset = 0.0f; // Initialize the offset for vibration
bool swingClockwise = true; // Initial direction of rotation
float swingAngle = 0.0;
float carouselAngle = 0.0;


int gameWon = 0 ;
int gameLost = 0;
int goalCollected = 0;

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


void AnimationTimer(int value) {

	// Update the seesaw angle for animation

	if (swingClockwise) {
		swingAngle += 0.5; // Increment the rotation angle for clockwise rotation
		if (swingAngle >= 10) {
			swingClockwise = false; // Change direction to counterclockwise when reaching the maximum angle
		}
	}
	else {
		swingAngle -= 0.5; // Decrement the rotation angle for counterclockwise rotation
		if (swingAngle <= -10) {
			swingClockwise = true; // Change direction to clockwise when reaching the minimum angle
		}
	}
	carouselAngle += 0.5; // Increment the rotation angle for clockwise rotation




		// ask OpenGL to recall the display function to reflect the changes on the window
		glutPostRedisplay();

	if (isAnimationActive) {
		glutTimerFunc(8, AnimationTimer, 0);  // 16 milliseconds per frame (about 60 frames per second)
	}
}
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

void drawCylinder(GLfloat radius, GLfloat height, GLint slices, GLint stacks) {
	GLUquadric* quadric = gluNewQuadric();
	gluCylinder(quadric, radius, radius, height, slices, stacks);
	gluDeleteQuadric(quadric);
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
	glScalef(0.5, 0.6, 0.5);
	glTranslated(0, -0.5, 0);
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


	glPushMatrix();
	glRotatef(carouselAngle, 0, 1, 0);

	// Support Columns
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslated(0.0, 0.8, 0.7);
	glScaled(0.1, 1.5, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslated(0.0, 0.8, -0.7);
	glScaled(0.1, 1.5, 0.1);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslated(0.0, 0.2, -0.7);
	glScaled(0.8, 0.8, 0.8);
	glutSolidSphere(0.3, 20, 20);
	glPopMatrix();


	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslated(0.0, 0.2, 0.7);
	glScaled(0.8, 0.8, 0.8);
	glutSolidSphere(0.3, 20, 20);
	glPopMatrix();
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

void drawGoal() {
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

void drawBench() {
	glPushMatrix();
	glTranslated(2, -0.5, -0.75);
	glScaled(0.3, 0.3, 0.3);

	if (isAnimationActive) {
		// Calculate the vibration offset
		vibrationOffset = 0.05f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f); // Adjust the vibration speed here

		// Apply vibration transformation
		glTranslatef(0.0f, vibrationOffset, 0.0f);
	}
	// Bench base

	glPushMatrix();
	glColor3f(0.6f, 0.3f, 0.1f); // Brown color
	glTranslatef(1.0f, 0.75, 1.0f); // Positioning the base
	glScalef(2.0f, 0.1f, 0.5f); // Size of the base (length, height, width)
	glutSolidCube(1.0f);
	glPopMatrix();

	// Left side of the Bench
	glPushMatrix();
	glColor3f(0.8f, 0.5f, 0.3f); // Orange color
	glTranslatef(0.0f, 0.1f, 1.0f); // Positioning the left side
	glScalef(0.1f, 1.5f, 0.5f); // Size of the left side (width, length, height)
	glutSolidCube(1.0f);
	glPopMatrix();

	// Right side of the Bench
	glPushMatrix();
	glColor3f(0.8f, 0.5f, 0.3f); // Orange color
	glTranslatef(2.0f, 0.1f, 1.0f); // Positioning the right side
	glScalef(0.1f, 1.5f, 0.5f); // Size of the right side (width, length, height)
	glutSolidCube(1.0f);
	glPopMatrix();

	// Bench plank
	glPushMatrix();
	glColor3f(0.7f, 0.7f, 0.7f); // Grey color
	glTranslated(1.0f, 0, 1.0f); // Positioning the plank
	glScalef(2.0f, 0.1f, 0.2f); // Size of the plank (length, height, width)
	glutSolidCube(1.0f);
	glPopMatrix();

	glPopMatrix();
}

void drawSwingSet() {
	glPushMatrix();
	glTranslated(2, 0, 1.5);
	glScaled(0.1, 0.1, 0.1);

	// Ground
	glPushMatrix();
	glColor3f(0.2f, 0.8f, 0.2f); // Green color
	glTranslatef(0.0f, -4.2f, 0.0f);
	glScalef(8.0f, 0.2f, 3.0f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Support beams
	glPushMatrix();
	glColor3f(0.7f, 0.7f, 0.7f); // Gray color
	glTranslatef(-3.5f, -2.0f, 0.0f);
	glScalef(0.2f, 16.0f, 0.2f);
	glutSolidCube(1.0f);
	glTranslatef(35.0f, 0.0f, 0.0f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Top beam
	glPushMatrix();
	glColor3f(0.7f, 0.7f, 0.7f); // Gray color
	glTranslatef(0.0f, 6.0f, 0.0f);
	glScalef(8.0f, 0.2f, 0.2f);
	glutSolidCube(1.0f);
	glPopMatrix();



	glPushMatrix();
	glRotatef(swingAngle,0,0,1);
	// Swings
	glPushMatrix();
	glColor3f(0.8f, 0.6f, 0.2f); // Brown color
	glScaled(1, 2.8, 1);
	glTranslatef(-2.0f, 2.2f, 0);
	glRotatef(90, 1, 0, 0);
	drawCylinder(0.1, 2.5, 10, 10); // Swing rope
	glTranslatef(0.0f, 0.0f, 2.5f);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.8f, 0.6f, 0.2f); // Brown color
	glScaled(1, 2.8, 1);

	glTranslatef(2.0f, 2.2f, 0);
	glRotatef(90, 1, 0, 0);
	drawCylinder(0.1, 2.5, 10, 10); // Swing rope
	glTranslatef(0.0f, 0.0f, 2.5f);
	glPopMatrix();

	// Swing Base
	glPushMatrix();
	glColor3f(0.7 , 0.4 , 0.1); // gold color
	glTranslatef(0.0f, -0.7, 0.0f);
	glScalef(4.5f, 0.2f, 3.0f);
	glutSolidCube(1.0f);
	glPopMatrix();

	glPopMatrix();


	glPopMatrix();
}

void drawSeesaw() {
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
	// Seesaw base - Cube
	glPushMatrix();
	glColor3f(0.5f, 0.5f, 0.5f); // Gray color
	glTranslatef(0.0f, -1.5f, 0.0f);
	glScalef(4.0f, 0.2f, 1.0f);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Left plank - Cube
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.2f); // Light brown color
	glTranslatef(-1.5f, 0.2f, 0.0f);
	glScalef(1,1,1);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Right plank - Cube
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.2f); // Light brown color
	glTranslatef(1.5f, 0.2f, 0.0f);
	glScalef(1,1,1);
	glutSolidCube(1.0f);
	glPopMatrix();

	// Left support - Cylinder
	glPushMatrix();
	glColor3f(0.7f, 0.7f, 0.7f); // Gray color
	glTranslatef(-1.5f, -1.0f, 0.5f);
	GLUquadric* leftCylinder = gluNewQuadric();
	gluCylinder(leftCylinder, 0.1, 0.1, 1.0, 20, 20);
	gluDeleteQuadric(leftCylinder);
	glPopMatrix();

	// Right support - Cylinder
	glPushMatrix();
	glColor3f(0.7f, 0.7f, 0.7f); // Gray color
	glTranslatef(1.5f, -1.0f, 0.5f);
	GLUquadric* rightCylinder = gluNewQuadric();
	gluCylinder(rightCylinder, 0.1, 0.1, 1.0, 20, 20);
	gluDeleteQuadric(rightCylinder);
	glPopMatrix();

	// Seesaw plank - Box
	glPushMatrix();
	glColor3f(0.9f, 0.6f, 0.2f); // Light brown color
	glTranslatef(0.0f, 1.0f, 0.5f);
	glScalef(3.0f, 0.2f, 1.0f);
	glutSolidCube(1.0f);
	glPopMatrix();
	glPopMatrix();
}


void checkGameLost(int value) {
	if (!gameWon) {
		gameLost = 1; // Set gameLost to 1 after 60 seconds if the game has started and not won
		glutPostRedisplay();
	}
	
}
void checkGameWon(int value) {
	if (goalCollected) {
		gameWon = 1; // Set gameLost to 1 after 60 seconds if the game has started and not won
		glutPostRedisplay();
	}

}
void drawTrashCan() {
	glPushMatrix();
	glTranslated(0, 0, 1);
	glScaled(0.1, 0.1, 0.1);
	if (isAnimationActive) {
		// Calculate the vibration offset
		vibrationOffset = 0.05f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f); // Adjust the vibration speed here

		// Apply vibration transformation
		glTranslatef(0.0f, vibrationOffset, 0.0f);
	}
	// Trash can body - Cylinder
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glTranslated(0, 0, 1);
	glColor3f(0.4f, 0.4f, 0.4f); // Gray color
	GLUquadric* quadric = gluNewQuadric();
	gluCylinder(quadric, 1.0, 1.0, 4.0, 20, 20);
	gluDeleteQuadric(quadric);
	glPopMatrix();


	// Trash can decoration - Torus
	glPushMatrix();
	glTranslated(0, -2.5, 0);
	glScaled(0.7, 0.7, 0.7);
	if (isAnimationActive) {
		// Calculate the vibration offset
		vibrationOffset = 0.05f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f); // Adjust the vibration speed here

		// Apply vibration transformation
		glTranslatef(0.0f, vibrationOffset, 0.0f);
	}
	glColor3f(0.6f, 0.6f, 0.6f); // Light gray color
	glTranslatef(0.0f, 2.0f, 0.0f);
	glutSolidTorus(0.1, 1.2, 20, 20);
	glPopMatrix();

	// Trash can base - Cube
	glPushMatrix();
	glColor3f(0.6f, 0.6f, 0.6f); // Light gray color
	glTranslated(0, -2, 0);
	glTranslatef(0.0f, 2.0f, 0.0f);
	glTranslatef(0.0f, -1.0f, 0.0f);
	glScalef(2.0f, 0.1f, 2.0f);
	glutSolidCube(1.0f);
	glPopMatrix();
	glPopMatrix();
}


void Display() {
	if (PlayerX >= -0.1 && PlayerX <= 0.1 && PlayerZ >= 1.9 && PlayerZ <= 2.1) {
		goalCollected = 1; // Player reached the goal
	}
	if (gameLost && !gameWon) {
		// Display "Game Won" message
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); // Save the current projection matrix
		glLoadIdentity();
		gluOrtho2D(0, 1400, 0, 750);

		// Switch to modelview matrix in order to set the raster position
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix(); // Save the current modelview matrix
		glLoadIdentity();

		// Disable depth testing to ensure the text is not occluded
		glDisable(GL_DEPTH_TEST);



		// Set the color and position for the text
		glColor3f(1.0f, 0.0f, 0.0f); // Green text
		glRasterPos2f(500, 400); // Position the text

		// Display "Game Lost" text
		std::string text = "Game Lost!";
		for (unsigned int i = 0; i < text.length(); i++) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
		}
		// Re-enable depth testing
		glEnable(GL_DEPTH_TEST);

		// Restore the modelview matrix
		glPopMatrix();

		// Restore the projection matrix
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		// Switch back to modelview matrix
		glMatrixMode(GL_MODELVIEW);
	}

	else {
		if (gameWon) {
			// Display "Game Won" message
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix(); // Save the current projection matrix
			glLoadIdentity();
			gluOrtho2D(0, 1400, 0, 750);

			// Switch to modelview matrix in order to set the raster position
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix(); // Save the current modelview matrix
			glLoadIdentity();

			// Disable depth testing to ensure the text is not occluded
			glDisable(GL_DEPTH_TEST);



			// Set the color and position for the text
			glColor3f(0.0f, 1.0f, 0.0f); // Green text
			glRasterPos2f(500, 400); // Position the text

			// Display "Game Won" text
			std::string text = "Game Won!";
			for (unsigned int i = 0; i < text.length(); i++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
			}
			// Re-enable depth testing
			glEnable(GL_DEPTH_TEST);

			// Restore the modelview matrix
			glPopMatrix();

			// Restore the projection matrix
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			// Switch back to modelview matrix
			glMatrixMode(GL_MODELVIEW);

		}
		else {
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
			glScaled(2, 2, 2);
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
			glTranslated(0.52, 1, 1);
			glScaled(0.02, 0.02, 2);
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
			glTranslated(0, 1, -0.25);
			glScaled(2, 2, 0.5);
			drawWall(0.02);
			glPopMatrix();

			//Draw Right Wall
			glPushMatrix();
			glTranslated(3, -0.25, 0);
			glRotated(90, 0.0, 0, 1.0);
			glScaled(0.5, 2, 2);
			drawWall(0.02);
			glPopMatrix();


			//Draw Player
			glPushMatrix();
			drawHuman(PlayerX, PlayerY, PlayerZ);
			glPopMatrix();




	/*		glPushMatrix();
			drawSeesaw();
			glPopMatrix();*/


			//Draw Goal
			if (!goalCollected) {
				glPushMatrix();
				glTranslated(0, -0.25, 2);
				glScaled(0.5, 0.5, 0.5);
				drawGoal();
				glPopMatrix();
			}

			glPushMatrix();
			glScaled(0.6, 0.75, 0.6);

			drawTree(2.30, -0.5, -0.75);
			glPopMatrix();

			glPushMatrix();
			drawBench();
			glPopMatrix();

			glPushMatrix();
			drawSwingSet();
			glPopMatrix();

			glPushMatrix();
			drawTrashCan();
			glPopMatrix();

			glPushMatrix();
			drawCarousel();
			glPopMatrix();
		}
	}
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
		// start animation on 'p' key press
		if (!isAnimationActive) {
			glutTimerFunc(0, AnimationTimer, 0);
			isAnimationActive = true;
		}
		break;
	case 'o':
		if (isAnimationActive)
			isAnimationActive = false;
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
	glutTimerFunc(60000, checkGameWon, 0);
	glutTimerFunc(60000, checkGameLost, 0);

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
