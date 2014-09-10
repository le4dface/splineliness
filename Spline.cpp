/*
 * Spline.cpp
 *
 *  Created on: 10/09/2014
 *      Author: reuben
 */

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <glm/glm.hpp>
#include "Spline.h"
// My definition
#include "include/define.h"

using namespace std;
using namespace glm;

Spline::Spline() {
	this->numPoints = 200;
	this->splineIndex = 0;
	this->splineLength = 0.0f;
}

Spline::~Spline() {
//do nothing
}

//calculates the points(x,y,z) along the spline from
//the control points
void Spline::calculateSpline() {
	if (cps.size() > 3) {
		for (vector<vec3>::size_type i = 1; i != cps.size() - 2; i++) {
			for (int k = 0; k < numPoints; k++) {
				float interp = (float) 1 / (float) numPoints;
				float t = k * interp; //Interpolation parameter
				vec3 splinePoint = calculateCMRPoint(cps[i-1], cps[i], cps[i+1], cps[i+2], t);
				spline.push_back(splinePoint);
			}
		}
	}
}



//calculates dist between points
float Spline::calculateDist(int indx, int nextIndx) {

	vec3 pos1 = spline[indx]; // start position
	vec3 pos2 = spline[nextIndx]; // end position

	float deltaX = pos2.x - pos1.x;
	float deltaY = pos2.y - pos1.y;

	float dist = sqrt(pow(deltaX,2) + pow(deltaY,2));

	return dist;


}

//find the length of the spline, and create arc length table
void Spline::calculateArcLengths() {

	segList.clear();
	splineLength = 0;
	int i = 1;
	segList.push_back(splineLength);
	while(i<spline.size() - 2) {
		splineLength += calculateDist(i, i + 1);
		segList.push_back(splineLength);
		i++;
	}
}

//calculate spline point(x,y,z)
vec3 Spline::calculateCMRPoint(vec3 a, vec3 b, vec3 c, vec3 d, float t) {
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
//draws control points set by user
void Spline::drawControlPoints() {

//	printf("draw control points");
	glPointSize(10.0);
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (vector<vec3>::size_type i = 0; i != cps.size(); i++) {
		glVertex3f(cps[i].x, cps[i].y, -1);
	}
	glEnd();
}

void Spline::drawSpline() {
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (vector<vec3>::size_type i = 0; i != spline.size(); i++) {
		glVertex3f(spline[i].x, spline[i].y, -1);
	}
	glEnd();
}

void Spline::insertBetween(const vec3& point) {
	int prevIndx = cps.size() - 2;
	vec3 endPoint = cps.at(cps.size()-1);
	vec3 prevPoint = cps.at(prevIndx);
	//make sure it is monotonically increasing
//	if (isMono(point, prevPoint, endPoint)) {
//		//insert the point at the index one back from the end
		cps.insert(cps.begin() + (cps.size() - 1), point);
//	}
	spline.clear();
	calculateSpline();
}

//select a point on the spline for dragging
//TODO migrate to SplineCtrl
int Spline::selectPoint(float xp, float yp) {
	for (vector<vec3>::size_type i = 0; i != cps.size(); i++) {
		if (fabs(xp - cps[i].x) < 0.02 && fabs(yp - cps[i].y) < 0.02) {
			return i;
		}
	}
	return -1;
}

//enforce monotonic
bool Spline::isMono(const vec3& location, const vec3& prevPoint, const vec3& endPoint) {
	return (location.x > prevPoint.x && location.y > prevPoint.y)
			&& (location.x <= endPoint.x && location.y <= endPoint.y);
}

vector<float>* Spline::getSegList() {
	return &segList;
}

float Spline::getSplineLength() {
	return splineLength;
}

void Spline::setSplineIndex(int i) {
	splineIndex = i;
}

int Spline::getSplineIndex() {
	return splineIndex;
}

vector<vec3>* Spline::getSpline() {
	return &spline;
}

vector<vec3>* Spline::getControlPoints() {
	return &cps;
}

