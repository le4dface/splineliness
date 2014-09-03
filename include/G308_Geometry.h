//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// This software is provided 'as-is' for assignment of COMP308 
// in ECS, Victoria University of Wellington, 
// without any express or implied warranty. 
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#pragma once

#include "define.h"
#include <GL/glut.h>

class G308_Geometry
{
public:
	G308_Geometry(void);
	~G308_Geometry(void);

	// Array for Geometry
	G308_Point* m_pVertexArray;		// Vertex Array
	G308_Normal* m_pNormalArray;	// Normal Array
	G308_Normal* m_pVertexNormalArray; //vertex normal array;
	G308_Triangle* m_pTriangles;	// Trianglem_glGeomListMap Array
	G308_UVcoord* m_pUVArray;	    // Texture Coordinate Array

	// Data for Geoemetry
	int m_nNumPoint;
	int m_nNumUV;
	int m_nNumPolygon;
	int m_nNumNormal;

	int mode; // Which mode to display

	// Data for Rendering
	int m_glGeomListPoly;	// Display List for Polygon
	int m_glGeomListWire;	// Display List for Wireframe

public:
	void ReadOBJ(const char* filename);

	void CreateGLPolyGeometry(); // [Assignment1] Create GL Display List for Polygon Geometry
	void CreateGLWireGeometry(); // [Assignment1] Create GL Display List for Wireframe Geometry

	void RenderGeometry();     // mode : G308_SHADE_POLYGON, G308_SHADE_WIREFRAME
	void toggleMode(); //Switch between showing filled polygons and wireframes

	void calculateVertexNormals(); //precalculate all of the vertex normals
	void drawPolygons(); //draw the polygons
	void drawPolygonsWire(); //draw the polygons wireframe

	void crossProduct(const G308_Point& u, const G308_Point& v,
			G308_Normal& temp);//helper method for calculating vector cross product
	void findUV(const G308_Point& v2, const G308_Point& v1,
			const G308_Point& v3, G308_Point& u, G308_Point& v);//helper find edges of triangle UV
};



