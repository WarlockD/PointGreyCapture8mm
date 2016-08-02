#pragma once
#include "glext.h"
#include <vector>

class GLShape {
	int maxVerts;
	std::vector<GLfloat> _vertexMesh;
	std::vector<GLubyte> _colorMesh;
	int usedVerts;

	GLenum drawMode;

	float lastX, lastY;
	unsigned char lastR, lastG, lastB, lastA;

//	BL2DTurtle * turtle;

	bool useAlpha;
	bool useSmoothing;
	struct BL2DVectorFontLines
	{
		int nIndicies;
		GLfloat i[40];
	} ;
	// for text
	float textKern;     // 255.0 = one 'em' (default is 32.0) (additional space between chars)
	float textWidth;    // (default is 10.0)
	float textHeight;   // (default is 20.0)
	virtual void render() = 0;
	//-(void)clearData;

#pragma mark - colors
	void render() {
		glDisable(GL_DEPTH_TEST);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertexMesh.data());
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorMesh.data());
	}
	// doesn't add a point or change the last point,
	// this sets for the next point(s) added. [0..255]
	//void setColorR:(unsigned char)pR G : (unsigned char)pG B : (unsigned char)pB;
//void setColorR:(unsigned char)pR G : (unsigned char)pG B : (unsigned char)pB A : (unsigned char)pA;



//void addX( float)pX Y : (float)pY;
//void addRandomPointW:(float)pWidth H : (float)pHeight;
//void repeatPoint;

//void addLineX0:(float)x0 Y0 : (float)y0  X1 : (float)x1 Y1 : (float)y1;
//void addTriangleX0:(float)x0 Y0 : (float)y0 X1 : (float)x1 Y1 : (float)y1 X2 : (float)x2 Y2 : (float)y2;
//void addRectangleX0:(float)x0 Y0 : (float)y0  X1 : (float)x1 Y1 : (float)y1;
};