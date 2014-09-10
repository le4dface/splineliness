/*
 * main.cpp
 *
 *  Created on: 12/08/2014
 *      Author: Reuben Blake Pepperell
 */

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <glm/glm.hpp>
#include <map>
// My definition
#include "include/define.h"
#include "Spline.h"

using namespace std;
using namespace glm;

vec3 cameraPosition;

float zoomFactor = 10;

int xAxisOff = 20;
int yAxisOff = 20;

int windowWidth = 800, windowHeight = 600;

int ix = -1; //index

int window_1; //animation window
int window_2; //curve map window

float distanceCovered; //distance obj is along spline

Spline animSpline, curveSpline;

/*
 * utilities
 */
vec3 findWorldCoordinates(int, int);
void drawCircle(Spline &s, float);
vec3 transformClick2D(float, float);
void redisplay(); //switch focus between windows
void calculateProgress();

/*
 *animation window
 */
void initialize();
void reshape(int, int);
void display();
void mouse(int, int, int, int);
void mouseDrag(int, int);
void keyboard(unsigned char, int, int);

/*
 *curvemap window
 */
void drawAxis();
void initialize_vps();
void initialize_2();
void G308_SetLight_2();
void reshape_alt(int, int);
void display_alt();
void mouse_2(int, int, int, int);
void mouseDrag_2(int, int);
void keyboard_2(unsigned char, int, int);

/*
 * Begin Animation Window
 */

void G308_SetLight() {
	float direction[] = { 200, 200, -10.0f, 1.0f };
	float diffintensity[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
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

	drawCircle(animSpline, 0.01);
	//draw the control points for our CRSpline
	animSpline.drawControlPoints();
	vector<vec3> *ctrlPoints = animSpline.getControlPoints();
	//need at least 4 control points
	if (ctrlPoints->size() > 3) {
		animSpline.drawSpline();
	}

	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
	glFlush();
}

void reshape(int wid, int hgt) {
	windowWidth = wid;
	windowHeight = hgt;
	glViewport(0, 0, wid, hgt);
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

//Begin Callback functions for animation
void mouse(int button, int state, int xp, int yp) {
	if (button == 3) { //wheel up
		zoomFactor *= 1.1;
	} else if (button == 4) { //wheel down
		zoomFactor *= 0.9;
	}
	printf("x: %d, y: %d\n", xp, yp);

	vec3 worldCoords = findWorldCoordinates(xp, yp);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		vector<vec3> *ctrlPoints = animSpline.getControlPoints();
		vector<vec3> *splinePoints = animSpline.getSpline();

		ctrlPoints->push_back(worldCoords);
		//recalculate the spline as it has new points
		splinePoints->clear();
		animSpline.calculateSpline();

		if (ctrlPoints->size() > 3) {
			animSpline.calculateArcLengths();
		}

	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)  //Pick
		ix = animSpline.selectPoint((float) worldCoords.x,
				(float) worldCoords.y);
	glutPostRedisplay();
}

void mouseDrag(int xp, int yp) {
	vec3 worldCoords = findWorldCoordinates(xp, yp);
	vector<vec3> *ctrlPoints = animSpline.getControlPoints();
	vector<vec3> *splinePoints = animSpline.getSpline();

	if (ix > -1) {

		ctrlPoints->at(ix).x = worldCoords.x;
		ctrlPoints->at(ix).y = worldCoords.y;
		ctrlPoints->at(ix).z = -1;

		splinePoints->clear();
		animSpline.calculateSpline();

	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'c' || key == 'C')
		animSpline.getControlPoints()->clear();
	glutPostRedisplay();
}

//End Callback functions for animation

/*
 * End Animation Window
 */

/*
 * Begin Curve Window
 */

void G308_SetLight_2() {
	float direction[] = { 200, 200, -10.0f, 1.0f };
	float diffintensity[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glLightfv(GL_LIGHT1, GL_POSITION, direction);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT1);
}

void drawAxis() {

	vec3 v1 = findWorldCoordinates(20, (600 - 20));
	vec3 v2 = findWorldCoordinates(20, 0);

	vec3 h1 = v1;
	vec3 h2 = findWorldCoordinates(800, (600 - 20));

	//draw y axis
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);

	glVertex3f(v1.x, v1.y, -1);
	glVertex3f(v2.x, v2.y, -1);

	glEnd();

	//draw x axis
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);

	glVertex3f(h1.x, h1.y, -1);
	glVertex3f(h2.x, h2.y, -1);

	glEnd();

}

void initialize_vps() {

	vector<vec3> *ctrlPoints = curveSpline.getControlPoints();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	vec3 p1, p2, p3, pn;
	p1.x = viewport[0] + xAxisOff;
	p1.y = viewport[3] - yAxisOff;

	p1 = findWorldCoordinates(p1.x, p1.y);

	p2.x = p1.x;
	p2.y = p1.y;

	pn.x = viewport[2] - xAxisOff;
	pn.y = viewport[1] + yAxisOff;

	pn = findWorldCoordinates(pn.x, pn.y);

	ctrlPoints->push_back(p1);
	ctrlPoints->push_back(p2);
	ctrlPoints->push_back(pn);

	curveSpline.drawControlPoints();
	curveSpline.calculateSpline();

}

void initialize_2(void) {

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

	vector<vec3> *ctrlPoints = curveSpline.getControlPoints();
	drawAxis();
	curveSpline.drawControlPoints();

	if (ctrlPoints->size() > 3) {
		curveSpline.drawSpline();
	}

	drawCircle(curveSpline, 0.01);

	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
	glFlush();

}

void reshape_alt(int wid, int hgt) {
	windowWidth = wid;
	windowHeight = hgt;
	glViewport(0, 0, wid, hgt);
}

//Callback function for curvemap
void mouse_2(int button, int state, int xp, int yp) {

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

		vector<vec3> *ctrlPoints = curveSpline.getControlPoints();
		vector<vec3> *splinePoints = curveSpline.getSpline();

		curveSpline.insertBetween(newPoint);
		//recalculate the spline as it has new points
		splinePoints->clear();
		curveSpline.calculateSpline();

		if (ctrlPoints->size() > 3) {
			curveSpline.calculateArcLengths();
		}

	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)  //Pick
		ix = curveSpline.selectPoint((float) worldCoords.x,
				(float) worldCoords.y);
	glutPostRedisplay();

}

void mouseDrag_2(int xp, int yp) {

	vec3 worldCoordinates = findWorldCoordinates(xp, yp);
	vector<vec3> *ctrlPoints = curveSpline.getControlPoints();
	vector<vec3> *spline = curveSpline.getSpline();

	if (ix > -1) {

		ctrlPoints->at(ix).x = worldCoordinates.x;
		ctrlPoints->at(ix).y = worldCoordinates.y;
		ctrlPoints->at(ix).z = -1;

		spline->clear();
		curveSpline.calculateSpline();

	}
	glutPostRedisplay();
}

void keyboard_2(unsigned char key, int x, int y) {

	vector<vec3> *ctrlPoints = curveSpline.getControlPoints();
	vector<vec3> *velSpline = curveSpline.getSpline();
	vector<float> *segList = animSpline.getSegList();

	int velIndex = curveSpline.getSplineIndex();

	if (key == 'c' || key == 'C') {
		ctrlPoints->clear();
		initialize_vps();
	}

	if (key == 's' || key == 'S') {

		if (velIndex < velSpline->size() - 1) {
			float d = distanceCovered;
			//search through spline vector comparing expected value
			//to value at index.
			int theIndex;
			for (vector<vec3>::size_type i = 0; i != segList->size(); i++) {
				if (segList->at(i) > d) {
					theIndex = i;
					break;
				}
			}
			animSpline.setSplineIndex(theIndex);
			curveSpline.setSplineIndex(velIndex + 1);
			calculateProgress();

			redisplay();

		} else {
			animSpline.setSplineIndex(0);
			curveSpline.setSplineIndex(0);
		}
	}

	glutPostRedisplay();
}

//End Callback functions for curvemap

/*
 * End Curve Window
 */

/*
 * Begin Utilities
 */

void drawCircle(Spline &s, float r) {
	vector<vec3>* ctrlPoints = s.getControlPoints();
	glPushMatrix();
	if (ctrlPoints->size() > 3) {
		vector<vec3> *splinePoints = s.getSpline();
		int index = s.getSplineIndex();
		glTranslatef(splinePoints->at(index).x, splinePoints->at(index).y, -1);
		glutSolidSphere(r, 100, 100);
	}
	glPopMatrix();
}

void calculateProgress() {
	distanceCovered = 0;
	int index = curveSpline.getSplineIndex();
	vector<vec3> *velSpline = curveSpline.getSpline();

	float height = abs(
			velSpline->at(velSpline->size() - 1).y - velSpline->at(0).y);
	float current = abs(
			velSpline->at(velSpline->size() - 1).y - velSpline->at(index).y);

	float p = 1 - abs(current / height);
	printf("percent: %f\n", p);
	distanceCovered = abs(p * animSpline.getSplineLength());
}

vec3 transformClick2D(float x, float y) {
	vec3 p;
	p.x = (float) (x * 8 / windowWidth);
	p.y = (float) (windowHeight - y) * 6.0 / windowHeight;
	p.z = -1;

	return p;
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

void redisplay() {

	int currentWindow = glutGetWindow();
	glutSetWindow(window_1);
	glutPostRedisplay();
	glutSetWindow(window_2);
	glutPostRedisplay();
	glutSetWindow(currentWindow);

}

/*
 * End Utilities
 */

int main(int argc, char **argv) {

	//initialise a few things
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
	glutMotionFunc(mouseDrag_2);
	glutKeyboardFunc(keyboard_2);

	initialize_vps();

	glutMainLoop();
	return 0;
}

