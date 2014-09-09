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
#include <map>
// My definition
#include "include/define.h"
#include "include/G308_Geometry.h"
//#include "include/pointList.h"

using namespace std;
using namespace glm;

vec3 cameraPosition;

float zoomFactor = 10;

bool drawingSphere = false;
vector<vec3> cps; //stores the control points
vector<vec3> spline; //stores all of the points that construct the spline

vector<vec3> vps; //store control points for velocity curve
vector<vec3> vspline; //store all of the points that construct the velocity spline

vector<float> segList;
vector<float> uList;

int xAxisOff = 20;
int yAxisOff = 20;

float splineLen = 0.0f;
float vsplineLen = 0.0f;

float distAlong;

int numPoints = 50;
int windowWidth = 800, windowHeight = 600;
//static int view_state = 0;

int ix = -1; //index
int splineIndex = 0;
int vsplineIndex = 0;
const float DEG2RAD = 3.14159 / 180;

int window_1;
int window_2;

vec3 findWorldCoordinates(int, int);
void drawCircle(float);
void drawVelCircle(float);
void G308_SetLight();
void drawSpline();
void calculateSpline();
float calculateVelDist(int, int);
float calculateDist(int, int);
void calculateSplineLength();
vec3 calculateCMRPoint(vec3, vec3, vec3, vec3, float);
void drawControlPoints();
void display();
int selectPoint(float, float);
void mouseDrag(int, int);
void initialize_vps();
void redisplay();


/*
 * Velocity curve window
 */

void reshape_alt(int, int);
void display_alt();
//void calculateArcLength();

void drawCircle(float radius) {

	glPushMatrix();
	if (cps.size() > 3) {

		glTranslatef(spline[splineIndex].x, spline[splineIndex].y, -1);

		glutSolidSphere(radius, 100, 100);

	}

	glPopMatrix();
}

void drawVelCircle(float radius) {

	glPushMatrix();
	if(vps.size() > 3) {

		glTranslatef(vspline[vsplineIndex].x, vspline[vsplineIndex].y, 0);
		glutSolidSphere(radius, 100, 100);

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

vec3 calculateCMRPoint(vec3 a, vec3 b, vec3 c, vec3 d, float t) {

	float splineX, splineY;
	vec3 splinePoint;

	splineX =
			b.x + 0.5 * t * (-a.x + c.x) +
			pow(t, 2) * (a.x - 2.5 * b.x + 2 * c.x - 0.5 * d.x) +
			pow(t, 3) * (-0.5 * a.x + 1.5 * b.x - 1.5 * c.x + 0.5 * d.x);

	splineY = b.y + 0.5 * t * (-a.y + c.y)
			+ pow(t, 2) * (a.y - 2.5 * b.y + 2 * c.y - 0.5 * d.y)
			+ pow(t, 3) * (-0.5 * a.y + 1.5 * b.y - 1.5 * c.y + 0.5 * d.y);

	splinePoint.x = splineX;
	splinePoint.y = splineY;
	splinePoint.z = -1;

	return splinePoint;

}


void calculateVelSpline() {

	if (vps.size() > 3) {


		for (vector<vec3>::size_type i = 1; i != vps.size() - 2; i++) {
			for (int k = 0; k < numPoints; k++) {
				//50 points
				float t = k * 0.02; //Interpolation parameter

				vec3 splinePoint = calculateCMRPoint(vps[i-1], vps[i], vps[i+1], vps[i+2], t);

				vspline.push_back(splinePoint);

			}
		}

	}

}

void calculateSpline() {

	if (cps.size() > 3) {

		for (vector<vec3>::size_type i = 1; i != cps.size() - 2; i++) {
			for (int k = 0; k < numPoints; k++) {
				//50 points
				float t = k * 0.02; //Interpolation parameter



				vec3 splinePoint = calculateCMRPoint(cps[i-1], cps[i], cps[i+1], cps[i+2], t);

				spline.push_back(splinePoint);

			}
		}

	}

}

void drawSpline() {

//	printf("%d", spline.size());
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (vector<vec3>::size_type i = 0; i != spline.size(); i++) {

		glVertex3f(spline[i].x, spline[i].y, -1);

	}
	glEnd();
}

void drawVelSpline() {

//	printf("%d", spline.size());
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (vector<vec3>::size_type i = 0; i != vspline.size(); i++) {

		glVertex2f(vspline[i].x, vspline[i].y);

	}
	glEnd();
}


/*
 * For calculating the chord length of an arc
 */
float calculateDist(int indx, int nextIndx) {

	vec3 pos1 = spline[indx]; // start position
	vec3 pos2 = spline[nextIndx]; // end position

	float deltaX = pos2.x - pos1.x;
	float deltaY = pos2.y - pos1.y;

	float dist = sqrt(pow(deltaX,2) + pow(deltaY,2));

	return dist;

}

/*
 * For calculating the chord length of an arc
 */
float calculateVelDist(int indx, int nextIndx) {

	vec3 pos1 = vspline[indx]; // start position
	vec3 pos2 = vspline[nextIndx]; // end position

	float deltaX = abs(pos2.x - pos1.x);
	float deltaY = abs(pos2.y - pos1.y);

	float dist = sqrt((deltaX*deltaX) + (deltaY*deltaY));

	return dist;

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
	glPointSize(8.0);
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
		if (fabs(xp - cps[i].x) < 0.02 && fabs(yp - cps[i].y) < 0.02) {
			printf("index: %d\n", i);
			return i;
		}
	}
	return -1;
}

int selectVelPoint(float xp, float yp) {
	for (vector<vec3>::size_type i = 0; i != vps.size(); i++) {
		if (fabs(xp - vps[i].x) < 0.2 && fabs(yp - vps[i].y) < 0.2) {
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

void mouseDrag_2(int xp, int yp) {

	float xWorld = (float)(xp*8.0)/windowWidth;
	float yWorld = (float)(windowHeight-yp)*6.0/windowHeight;

	if (ix > -1) {
		vps[ix].x = xWorld;
		vps[ix].y = yWorld;
//		vps[ix].z = -1;

		vspline.clear();
		calculateVelSpline();
	}
	glutPostRedisplay();
}

vec3 transformClick2D(float x, float y) {

	vec3 p;
	p.x = (float) (x*8/windowWidth);
	p.y = (float) (windowHeight - y)*6.0/windowHeight;

	return p;


}

void drawAxis() {

	vec3 v1 = transformClick2D((float)(20), (float) (600-20));
	vec3 v2 = transformClick2D((float)(20), (float) (0));

	vec3 h1 = v1;
	vec3 h2 = transformClick2D((float)(800), (float)(600-20));

	//draw y axis
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);

		glVertex2f(v1.x, v1.y);
		glVertex2f(v2.x, v2.y);

	glEnd();

	//draw x axis
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);

		glVertex2f(h1.x, h1.y);
		glVertex2f(h2.x, h2.y);

	glEnd();

}

void initialize_vps() {

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	vec3 p1, p2, p3, pn;
	p1.x = viewport[0] + xAxisOff;
	p1.y = viewport[3] - yAxisOff;

	p1 = transformClick2D(p1.x, p1.y);

	p2.x = p1.x;
	p2.y = p1.y;

	pn.x = viewport[2] - xAxisOff;
	pn.y = viewport[1] + yAxisOff;

	pn = transformClick2D(pn.x, pn.y);

	vps.push_back(p1);
	vps.push_back(p2);
	vps.push_back(pn);


	drawVelocityPoints();
	calculateVelSpline();

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
		if(cps.size() > 3) {
			calculateSplineLength();
		}

	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)  //Pick
		ix = selectPoint((float) worldCoords.x, (float) worldCoords.y);
	glutPostRedisplay();
}

bool isMono(const vec3& location, const vec3& prevPoint, const vec3& endPoint) {
	return (location.x > prevPoint.x && location.y > prevPoint.y)
			&& (location.x <= endPoint.x && location.y <= endPoint.y);
}

void insertVelPoint(const vec3& location) {

	int prevIndx = vps.size() - 2;
	vec3 endPoint = vps.at(vps.size()-1);
	vec3 prevPoint = vps.at(prevIndx);

	//make sure it is monotonically increasing
	if (isMono(location, prevPoint, endPoint)) {
		//insert the point at the index one back from the end
		vps.insert(vps.begin() + (vps.size() - 1), location);
	}
	vspline.clear();
	calculateVelSpline();
}

void mouse_2(int button, int state, int xp, int yp) {


	if (button == 3) { //wheel up
		zoomFactor *= 1.1;
	} else if (button == 4) { //wheel down
		zoomFactor *= 0.9;
	}


	vec3 location = transformClick2D(xp,yp);

	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN)
	{
	   insertVelPoint(location);

	}
	else if(button==GLUT_RIGHT_BUTTON && state ==GLUT_DOWN) {
			ix = selectVelPoint(location.x, location.y);
			printf("ix: %d\n", ix);
	}

	glutPostRedisplay();

	printf("x: %d, y: %d\n", xp, yp);

}

void calculateDistAlong() {
	float highest = vspline[vspline.size() - 1].y;
	float p = vspline[vsplineIndex].y / highest;
	printf("p: %f\n", p);
	distAlong = p * splineLen;
	printf("dist along spline: %f\n", distAlong);
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'c' || key == 'C')
		cps.clear();

	if (key == 's' || key == 'S') {

//			calculateDistAlong();
//			float d = distAlong;
//			//search through spline vector comparing expected value
//			//to value at index.
//
//			int theIndex;
//
//			for (vector<vec3>::size_type i = 0; i != segList.size(); i++) {
//
//				if(segList[i] > d) {
//					theIndex = i;
//					break;
//				}
//
//			}
//
//			splineIndex = theIndex;


	}

	if(key == 'l' || key == 'L') {

		calculateSplineLength();




	}



	glutPostRedisplay();
}

void calculateSplineLength() {

	segList.clear();
	splineLen = 0;
//	segList.push_back(splineLen);


	int i = 1;
	while(i<spline.size() - 2) {
//		printf("x: %f, y: %f\n", cps[i].x, cps[i].y);

		splineLen += calculateDist(i, i + 1);
		printf("length: %f\n", splineLen);
		segList.push_back(splineLen);
		i++;
	}

//	for (vector<float>::size_type i = 0; i != segList.size(); i++) {
//
//			printf("seg num %d: %f\n", segList[i]);
//
//	}


}

void calculateVelLength() {
	vsplineLen = 0;
	int i = 1;
	while (i < vps.size() - 2) {
		printf("x: %f, y: %f\n", vps[i].x, vps[i].y);
		vsplineLen += calculateVelDist(i, i + 1);
		i += 1;
	}
	printf("v spline len: %f\n", vsplineLen);
}

void keyboard_2(unsigned char key, int x, int y) {
	if (key == 'c' || key == 'C')
		vps.clear();

	if (key == 's' || key == 'S') {

		if (vsplineIndex <= vspline.size()) {
			vsplineIndex++;

	//		calculateObjPos();
			calculateDistAlong();
			float d = distAlong;
			//search through spline vector comparing expected value
			//to value at index.

			int theIndex;

			for (vector<vec3>::size_type i = 0; i != segList.size(); i++) {

				if(segList[i] > d) {
					theIndex = i;
					break;
				}

			}

			splineIndex = theIndex;
			redisplay();

		} else {
			vsplineIndex = 0;
		}
	}

	if(key == 'l' || key == 'L') {

		calculateVelLength();

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

		drawAxis();

		drawVelocityPoints();

		if(vps.size() > 3) {
			drawVelSpline();
		}

		drawVelCircle(0.05);

		glDisable(GL_NORMALIZE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);

		glutSwapBuffers();
		glFlush();

}

void redisplay() {

	int currentWindow = glutGetWindow();
	glutSetWindow(window_1);
	glutPostRedisplay();
	glutSetWindow(window_2);
	glutPostRedisplay();
	glutSetWindow(currentWindow);



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
	glutMotionFunc(mouseDrag_2);
	glutKeyboardFunc(keyboard_2);

	initialize_vps();


	glutMainLoop();
	return 0;
}

