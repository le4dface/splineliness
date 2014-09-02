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
using namespace std;
using namespace glm;
 
vector<vec3> cps; //stores the control points
vector<vec3> spline; //stores all of the points that construct the spline

int windowWidth=800, windowHeight=600;
int ix = -1; //index



void calculateSpline() {

	if(cps.size() > 3) {

	for (vector<vec3>::size_type i = 1; i != cps.size() - 2; i++) {
			for (int k = 0; k < 50; k++) {
				//50 points
				float t = k * 0.02; //Interpolation parameter
				float splineX, splineY;
				vec3 splinePoint;

				splineX =
					  cps[i].x + 0.5 * t * (-cps[i - 1].x + cps[i + 1].x) +
					  pow(t,2) * (cps[i - 1].x - 2.5 * cps[i].x + 2 * cps[i + 1].x - 0.5 * cps[i + 2].x) +
					  pow(t,3) * (-0.5 * cps[i - 1].x + 1.5 * cps[i].x - 1.5 * cps[i + 1].x + 0.5 * cps[i + 2].x);

				splineY =
						cps[i].y + 0.5 * t * (-cps[i - 1].y + cps[i + 1].y) +
						pow(t,2) * (cps[i - 1].y - 2.5 * cps[i].y + 2 * cps[i + 1].y - 0.5 * cps[i + 2].y) +
						pow(t,3) * (-0.5 * cps[i - 1].y + 1.5 * cps[i].y - 1.5 * cps[i + 1].y + 0.5 * cps[i + 2].y);

				splinePoint.x = splineX;
				splinePoint.y = splineY;
				splinePoint.z = 0;

				spline.push_back(splinePoint);

			}
	}

	}

}


void drawSphere() {



}

void drawSpline() {
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (vector<vec3>::size_type i = 0; i != spline.size(); i++) {

		glVertex3f(spline[i].x, spline[i].y, 0);

	}
	glEnd();
}

void drawControlPoints() {
	//draw the control points for our CRSpline
	glPointSize(6.0);
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (vector<vec3>::size_type i = 0; i != cps.size(); i++) {
		glVertex3f(cps[i].x, cps[i].y, 0);
	}
	glEnd();
}

void display(void) 
{

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//draw the control points for our CRSpline
	drawControlPoints();
	//need at least 4 control points
	if(cps.size() > 3)
	{
		drawSpline();
	}
	glFlush(); 
}


/*
 * find the index of selected point
 */
int selectPoint(float xp, float yp)
{
	for(vector<vec3>::size_type i = 0; i != cps.size(); i++) {
		if(fabs(xp - cps[i].x) < 0.2 && fabs(yp - cps[i].y) < 0.2) {
			return i;
		}
	}
	return -1;
}


void mouseDrag(int xp, int yp) {
	float xScene = (float)(xp*8.0)/windowWidth;
	float yScene = (float)(windowHeight-yp)*6.0/windowHeight;
	if(ix > -1)
    {
		cps[ix].x = xScene;
		cps[ix].y = yScene;

		spline.clear();
		calculateSpline();
    }
	glutPostRedisplay();
}

void mouse(int button, int state, int xp, int yp)
{
	float xScene = (float)(xp*8.0)/windowWidth;
	float yScene = (float)(windowHeight-yp)*6.0/windowHeight;
	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN)
	{

		   vec3 newPoint;
		   newPoint.x = xScene;
		   newPoint.y = yScene;
		   newPoint.z = 0;
		   cps.push_back(newPoint);
		   //recalculate the spline as it has new points
		   spline.clear();
		   calculateSpline();

	} 
	else if(button==GLUT_RIGHT_BUTTON && state ==GLUT_DOWN)  //Pick
			ix = selectPoint(xScene, yScene);
	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
    if(key == 'c' || key == 'C') cps.clear();
    glutPostRedisplay();
}

void initialize(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 8.0, 0.0, 6.0);
}


void reshape(int wid, int hgt)
{
	windowWidth = wid;
	windowHeight = hgt;
	glViewport(0, 0, wid, hgt);
}


int main(int argc, char **argv)
{ 
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0,0);
	glutCreateWindow("CRSpline biz");
	initialize();
	glutDisplayFunc(display);
    glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseDrag);
    glutKeyboardFunc(keyboard);
	glutMainLoop(); 
	return 0;
}

