/*
 * spline.cpp
 *
 *  Created on: 12/08/2014
 *      Author: Reuben Blake Pepperell
 */

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <glm/glm.hpp>
// My definition
#include "include/define.h"
#include "include/G308_Geometry.h"

using namespace std;
using namespace glm;

vec3 cameraPosition;

float zoomFactor = 10;

bool drawingSphere = false;
vector<vec3> cps; //stores the control points
vector<vec3> spline; //stores all of the points that construct the spline
vector<vec3> vps; //store control points for velocity curve
vector<vec3> vspline; //store all of the points that construct the velocity spline

int windowWidth = 800, windowHeight = 600;
//static int view_state = 0;

int ix = -1; //index
int splineIndex = 0;
const float DEG2RAD = 3.14159 / 180;

int window_1;
int window_2;

vec3 findWorldCoordinates(int, int);
void drawCircle(float);
void G308_SetLight();
void drawSpline();
void calculateSpline();
void drawControlPoints();
void display();
int selectPoint(float, float);
void mouseDrag(int, int);

/*
 * Velocity curve window
 */

void reshape_alt(int, int);
void display_alt();

void drawCircle(float radius) {

	glPushMatrix();
	if (cps.size() > 3) {

		glTranslatef(spline[splineIndex].x, spline[splineIndex].y, -1);

		glutSolidSphere(0.03, 100, 100);

	}

	glPopMatrix();
}
// Set View Position
void G308_SetLight() {
	float direction[] = { 200, 200, -10.0f, 1.0f };
	float diffintensity[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}


void G308_SetLight_2() {
	float direction[] = { 200, 200, -10.0f, 1.0f };
	float diffintensity[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT1, GL_POSITION, direction);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT1);
}


void calculateSpline() {

	if (cps.size() > 3) {

		for (vector<vec3>::size_type i = 1; i != cps.size() - 2; i++) {
			for (int k = 0; k < 50; k++) {
				//50 points
				float t = k * 0.02; //Interpolation parameter
				float splineX, splineY;
				vec3 splinePoint;

				splineX = cps[i].x + 0.5 * t * (-cps[i - 1].x + cps[i + 1].x)
						+ pow(t, 2)
								* (cps[i - 1].x - 2.5 * cps[i].x
										+ 2 * cps[i + 1].x - 0.5 * cps[i + 2].x)
						+ pow(t, 3)
								* (-0.5 * cps[i - 1].x + 1.5 * cps[i].x
										- 1.5 * cps[i + 1].x
										+ 0.5 * cps[i + 2].x);

				splineY = cps[i].y + 0.5 * t * (-cps[i - 1].y + cps[i + 1].y)
						+ pow(t, 2)
								* (cps[i - 1].y - 2.5 * cps[i].y
										+ 2 * cps[i + 1].y - 0.5 * cps[i + 2].y)
						+ pow(t, 3)
								* (-0.5 * cps[i - 1].y + 1.5 * cps[i].y
										- 1.5 * cps[i + 1].y
										+ 0.5 * cps[i + 2].y);

				splinePoint.x = splineX;
				splinePoint.y = splineY;
				splinePoint.z = -1;

				spline.push_back(splinePoint);

			}
		}

	}

}

void drawSpline() {
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (vector<vec3>::size_type i = 0; i != spline.size(); i++) {

		glVertex3f(spline[i].x, spline[i].y, -1);

	}
	glEnd();
}

void drawControlPoints() {
	//draw the control points for our CRSpline
	glPointSize(10.0);
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (vector<vec3>::size_type i = 0; i != cps.size(); i++) {
		glVertex3f(cps[i].x, cps[i].y, -1);
	}
	glEnd();
}

void drawVelocityPoints() {

	//draw the control points for our CRSpline
	glPointSize(10.0);
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (vector<vec3>::size_type i = 0; i != vps.size(); i++) {
		glVertex2f(vps[i].x, vps[i].y);
	}
	glEnd();

}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE); //enabled GL normalize to automagically normalize vectors
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(1.0f, 0.0f, 0.0f); /* set object color as red */
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	drawCircle(0.01);
	//draw the control points for our CRSpline
	drawControlPoints();
	//need at least 4 control points
	if (cps.size() > 3) {
		drawSpline();
	}

	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
	glFlush();
}

/*
 * find the index of selected point # NO LONG WORKS IN 3D SPACE
 */
int selectPoint(float xp, float yp) {
	for (vector<vec3>::size_type i = 0; i != cps.size(); i++) {
		if (fabs(xp - cps[i].x) < 0.2 && fabs(yp - cps[i].y) < 0.2) {
			return i;
		}
	}
	return -1;
}

void mouseDrag(int xp, int yp) {

	vec3 worldCoords = findWorldCoordinates(xp, yp);

	if (ix > -1) {
		cps[ix].x = worldCoords.x;
		cps[ix].y = worldCoords.y;
		cps[ix].z = -1;

		spline.clear();
		calculateSpline();
	}
	glutPostRedisplay();
}

vec3 findWorldCoordinates(int xp, int yp) {

	GLint viewport[4]; //var to hold the viewport info
	GLdouble modelview[16]; //var to hold the modelview info
	GLdouble projection[16]; //var to hold the projection matrix info
	GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
	GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview); //get the modelview info
	glGetDoublev( GL_PROJECTION_MATRIX, projection); //get the projection matrix info
	glGetIntegerv( GL_VIEWPORT, viewport); //get the viewport info

	winX = (float) xp;
	winY = (float) (viewport[3] - (float) yp);
	winZ = 0;

	//get the world coordinates from the screen coordinates
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX,
			&worldY, &worldZ);

	vec3 world;
	world.x = worldX;
	world.y = worldY;
	world.z = worldZ;

	return world;

}

void mouse(int button, int state, int xp, int yp) {
	if (button == 3) { //wheel up
		zoomFactor *= 1.1;
	} else if (button == 4) { //wheel down
		zoomFactor *= 0.9;
	}
	printf("x: %d, y: %d\n", xp, yp);

	vec3 worldCoords = findWorldCoordinates(xp, yp);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		vec3 newPoint;
		newPoint.x = (float) worldCoords.x;
		newPoint.y = (float) worldCoords.y;
		newPoint.z = (float) worldCoords.z;
		cps.push_back(newPoint);
		//recalculate the spline as it has new points
		spline.clear();
		calculateSpline();

	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)  //Pick
		ix = selectPoint((float) worldCoords.x, (float) worldCoords.y);
	glutPostRedisplay();
}

void mouse_2(int button, int state, int xp, int yp) {


	float xWorld = (float)(xp*8.0)/windowWidth;
	float yWorld = (float)(windowHeight-yp)*6.0/windowHeight;
	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN)
	{

		   vec3 newPoint;
		   newPoint.x = xWorld;
		   newPoint.y = yWorld;
		   vps.push_back(newPoint);

	}
//	else if(button==GLUT_RIGHT_BUTTON && state ==GLUT_DOWN)  //Pick
//			indx = picked(xWorld, yWorld);
	glutPostRedisplay();

	printf("x: %d, y: %d\n", xp, yp);

}


void keyboard(unsigned char key, int x, int y) {
	if (key == 'c' || key == 'C')
		cps.clear();

	if (key == 's' || key == 'S') {

		if (splineIndex <= spline.size()) {
			splineIndex++;
		} else {
			splineIndex = 0;
		}

	}

	glutPostRedisplay();
}

void initialize(void) {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) windowWidth / (double) windowHeight,
			G308_ZNEAR_3D, G308_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
			cameraPosition.x, cameraPosition.y, cameraPosition.z - zoomFactor,
			0.0, 1.0, 0.0);
}

void initialize_2(void) {

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 8.0, 0.0, 6.0);

}


void reshape(int wid, int hgt) {
	windowWidth = wid;
	windowHeight = hgt;
	glViewport(0, 0, wid, hgt);
}

/*
 * Velocity curve window callbacks
 */

void reshape_alt(int wid, int hgt) {
	windowWidth = wid;
	windowHeight = hgt;
	glViewport(0, 0, wid, hgt);
}

void display_alt() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_NORMALIZE); //enabled GL normalize to automagically normalize vectors
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glColor3f(1.0f, 0.0f, 0.0f); /* set object color as red */
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		drawVelocityPoints();

		glDisable(GL_NORMALIZE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);

		glutSwapBuffers();
		glFlush();

}

int main(int argc, char **argv) {
	cameraPosition.x = 0;
	cameraPosition.y = 0;
	cameraPosition.z = 0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
	window_1 = glutCreateWindow("CRSpline biz");
	G308_SetLight();
	initialize();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);
	glutKeyboardFunc(keyboard);

	window_2 = glutCreateWindow("Velocity biz");
	G308_SetLight_2();
	initialize_2();
	glutDisplayFunc(display_alt);
	glutReshapeFunc(reshape_alt);
	glutMouseFunc(mouse_2);


	glutMainLoop();
	return 0;
}

