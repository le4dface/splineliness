/*
 * Spline.h
 *
 *  Created on: 10/09/2014
 *      Author: reuben
 */

#ifndef SPLINE_H_
#define SPLINE_H_

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <glm/glm.hpp>
// My definition
#include "include/define.h"

using namespace std;
using namespace glm;

class Spline {

private:

vector<vec3> cps; //stores the control points
vector<vec3> spline; //stores all of the points that construct the spline
vector<float> segList; //list of arc lengths at increasing position on the spline
float splineLength; //approximate length of the spline
int numPoints; //number of points between control points
int splineIndex; //current index of spline



float calculateDist(int,int); //approximate length of an arc segment
vec3 calculateCMRPoint(vec3, vec3, vec3, vec3, float);

public:
	void calculateArcLengths(); //
	void calculateSpline();
	void drawControlPoints();
	void drawSpline();
	int selectPoint(float, float);
	void insertPoint(const vec3&);
	void insertBetween(const vec3&);
	vector<float>* getSegList();
	float getSplineLength();
	int getSplineIndex();
	void setSplineIndex(int);
	bool isMono(const vec3&, const vec3&, const vec3&);
	vector<vec3>* getControlPoints();
	vector<vec3>* getSpline();

	Spline();
	~Spline();

};

#endif /* SPLINE_H_ */
