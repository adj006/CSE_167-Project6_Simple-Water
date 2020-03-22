#define _CRT_SECURE_NO_WARNINGS

#include "Window.h"
#include <cmath>

using namespace std;

int Window::width = 512;   // set window width in pixels here
int Window::height = 512;   // set window height in pixels here

double time = 0;
int cx = 0;
int cy = 0;
int cz = 0;
int eMap = 0;
double delta = 0.0000001;

/// a structure to hold a control point of the surface
struct Point {
	float x;
	float y;
	float z;
};

Point pU;
Point pV;
Vector3 tanU;
Vector3 tanV;
Vector3 n0;
Vector3 n1;
Vector3 n2;
Vector3 n3;

Matrix4 glmatrix;

Point Points[4][4] = {
		{
			{ 10, 0, 10 },
			{ 5, 0, 10 },
			{ -5, 0, 10 },
			{ -10, 0, 10 }
		},
		{
			{ 10, 0, 5 },
			{ 5, 0, 5 },
			{ -5, 0, 5 },
			{ -10, 0, 5 }
		},
		{
			{ 10, 0, -5 },
			{ 5, 0, -5 },
			{ -5, 0, -5 },
			{ -10, 0, -5 }
		},
		{
			{ 10, 0, -10 },
			{ 5, 0, -10 },
			{ -5, 0, -10 },
			{ -10, 0, -10 }
		}
};

// the level of detail of the surface
unsigned int LOD = 100;

Point CalculateU(float t, int row) {

	// the final point
	Point p;

	// the t value inverted
	float it = 1.0f - t;

	// calculate blending functions
	float b0 = t*t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it*it*it;

	// sum the effects of the Points and their respective blending functions
	p.x = b0*Points[row][0].x +
		b1*Points[row][1].x +
		b2*Points[row][2].x +
		b3*Points[row][3].x;

	p.y = b0*Points[row][0].y +
		b1*Points[row][1].y +
		b2*Points[row][2].y +
		b3*Points[row][3].y;

	p.z = b0*Points[row][0].z +
		b1*Points[row][1].z +
		b2*Points[row][2].z +
		b3*Points[row][3].z;

	return p;
}

Point CalculateV(float t, Point* pnts) {
	Point p;

	// the t value inverted
	float it = 1.0f - t;

	// calculate blending functions
	float b0 = t*t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it*it*it;

	// sum the effects of the Points and their respective blending functions
	p.x = b0*pnts[0].x +
		b1*pnts[1].x +
		b2*pnts[2].x +
		b3*pnts[3].x;

	p.y = b0*pnts[0].y +
		b1*pnts[1].y +
		b2*pnts[2].y +
		b3*pnts[3].y;

	p.z = b0*pnts[0].z +
		b1*pnts[1].z +
		b2*pnts[2].z +
		b3*pnts[3].z;

	return p;
}

Point Calculate(float u, float v) {

	Point temp[4];

	// calculate each point on our final v curve
	temp[0] = CalculateU(u, 0);
	temp[1] = CalculateU(u, 1);
	temp[2] = CalculateU(u, 2);
	temp[3] = CalculateU(u, 3);

	return CalculateV(v, temp);
}

void Window::processNormalKeys(unsigned char key, int x, int y)
{
	//cout << "Hello" << endl;

	Matrix4 trans;
	trans.identity();

	Matrix4 rot;
	rot.identity();

	switch (key)
	{
	case 'x':
		//cx--;
		trans.makeTranslate(-1, 0, 0);
		Globals::m = trans * Globals::m;
		break;
	case 'X':
		//cx++;
		trans.makeTranslate(1, 0, 0);
		Globals::m = trans * Globals::m;
		break;
	case 'y':
		//cy--;
		trans.makeTranslate(0, -1, 0);
		Globals::m = trans * Globals::m;
		break;
	case 'Y':
		//cy++;
		trans.makeTranslate(0, 1, 0);
		Globals::m = trans * Globals::m;
		break;
	case 'z':
		//cz--;
		trans.makeTranslate(0, 0, -1);
		Globals::m = trans * Globals::m;
		break;
	case 'Z':
		//cz++;
		trans.makeTranslate(0, 0, 1);
		Globals::m = trans * Globals::m;
		break;
	case 'r':
		rot.makeRotateY(-5);
		Globals::m = Globals::m * rot;
		//glmatrix = glmatrix * rot;
		break;
	case 'R':
		rot.makeRotateY(5);
		Globals::m = Globals::m * rot;
		break;
	case 'e':
		if (eMap == 0)
			eMap == 1;
		else
			eMap == 0;
	}
}

/*
void Window::specialKeysProcess(int key, int x, int y)
{

	switch (key)
	{
	case GLUT_KEY_F1:
		glutDisplayFunc(displayCallback_cube);
		glutIdleFunc(idleCallback_cube);
		break;
	case GLUT_KEY_F2:
		glutDisplayFunc(displayCallback_house);
		glutIdleFunc(idleCallback_house);
		whichCamera = 1;
		break;
	case GLUT_KEY_F3:
		glutDisplayFunc(displayCallback_house);
		glutIdleFunc(idleCallback_house);
		whichCamera = 2;
		break;
	case GLUT_KEY_F4:
		parser("Bunny.xyz");
		glutDisplayFunc(displayCallback_bunny);
		glutIdleFunc(idleCallback_bunny);
		firstTime = 1;
		getMin();
		getMax();

		break;
	case GLUT_KEY_F5:
		parser("Dragon.xyz");
		glutDisplayFunc(displayCallback_dragon);
		glutIdleFunc(idleCallback_dragon);
		firstTime = 1;
		getMin();
		getMax();

		break;
	}
}
*/

//----------------------------------------------------------------------------
// Callback method called when system is idle.
void Window::idleCallback()
{
	displayCallback();         // call display routine to show the object
}

//----------------------------------------------------------------------------
// Callback method called by GLUT when graphics window is resized by the user
void Window::reshapeCallback(int w, int h)
{
	cerr << "Window::reshapeCallback called" << endl;
	width = w;
	height = h;
	glViewport(0, 0, w, h);  // set new viewport size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, double(width) / (double)height, 1.0, 1000.0); // set perspective projection viewing frustum
	//glTranslatef(0, 0, -30);    // move camera back 30 units so that it looks at the origin (or else it's in the origin)
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
}

//----------------------------------------------------------------------------
// Callback method called by GLUT when window readraw is necessary or when glutPostRedisplay() was called.
void Window::displayCallback()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);  // make sure we're in Modelview mode


	// set the camera position
	gluLookAt(0, 50, 50,	//	eye pos
		0, 0, 0,	//	aim point
		0, 1, 0);	//	up direction

	// Tell OpenGL what ModelView matrix to use:
	glmatrix = Globals::m;
	glmatrix.transpose();
	glLoadMatrixd(glmatrix.getPointer());


	glColor3f(0.0, 0.0, 1.0);		// This makes the cube green; the parameters are for red, green and blue. 
	glDisable(GL_COLOR_MATERIAL);
	
	// Draw all Bexier curves
	//glBegin(GL_QUADS);
	glBegin(GL_QUADS);
	
	// set the current drawing color to white to use original texture colors
	//glColor3f(1, 1, 1);



	if (time == 360)
		time = 0;
	cout << time << endl;
	//for animation
	//[0][0].y = 8*sin(time); time += .001;
	Points[0][1].y = 8*sin(time); time += .01;
	Points[0][2].y = 8*sin(time); time += .01;
	//Points[0][3].y = -8*sin(time); time += .001;
	//Points[1][0].y = 8*sin(time); time += .001;
	Points[1][1].y = 8*sin(time); time += .01;
	Points[1][2].y = 8*sin(time); time += .01;
	//Points[1][3].y = -8*sin(time); time += .001;
	//Points[2][0].y = 8*sin(time); time += .001;
	Points[2][1].y = 8*sin(time); time += .01;
	Points[2][2].y = 8*sin(time); time += .01;
	//Points[2][3].y = -8*sin(time); time += .001;
	//Points[3][0].y = 8*sin(time); time += .001;
	Points[3][1].y = 8*sin(time); time += .01;
	Points[3][2].y = 8*sin(time); time += .01;
	//Points[3][3].y = -8*sin(time); time += .001;

	// use the parametric time value 0 to 1
	for (int i = 0; i != LOD - 1; ++i) {

		// calculate the parametric u value
		float u0 = (float)i / (LOD - 1);
		float u1 = (float)(i + 1) / (LOD - 1);
		//float u2 = (float)(i + 2) / (LOD - 1);

		for (int j = 0; j != LOD - 1; ++j) {

			// calculate the parametric v value
			float v0 = (float)j / (LOD - 1);
			float v1 = (float)(j + 1) / (LOD - 1);
			//float v2 = (float)(j + 2) / (LOD - 1);

			// calculate the point on the surface
			//1st point
			Point p0 = Calculate(u0, v0);
			pU = Calculate(u0 + delta, v0);
			pV = Calculate(u0, v0 + delta);

			//tanU
			tanU.v3[0] = pU.x - p0.x;
			tanU.v3[1] = pU.y - p0.y;
			tanU.v3[2] = pU.z - p0.z;

			//tanV
			tanV.v3[0] = pV.x - p0.x;
			tanV.v3[1] = pV.y - p0.y;
			tanV.v3[2] = pV.z - p0.z;

			tanU.normalize();
			tanV.normalize();

			//1st normal
			n0 = tanU.cross(tanU, tanV);


			//2nd point
			Point p1 = Calculate(u1, v0);
			pU = Calculate(u1 + delta, v0);
			pV = Calculate(u1, v0 + delta);

			//tanU
			tanU.v3[0] = pU.x - p1.x;
			tanU.v3[1] = pU.y - p1.y;
			tanU.v3[2] = pU.z - p1.z;

			//tanV
			tanV.v3[0] = pV.x - p1.x;
			tanV.v3[1] = pV.y - p1.y;
			tanV.v3[2] = pV.z - p1.z;

			tanU.normalize();
			tanV.normalize();

			//2nd normal
			n1 = tanU.cross(tanU, tanV);


			//3rd point
			Point p2 = Calculate(u1, v1);
			pU = Calculate(u1 + delta, v1);
			pV = Calculate(u1, v1 + delta);

			//tanU
			tanU.v3[0] = pU.x - p2.x;
			tanU.v3[1] = pU.y - p2.y;
			tanU.v3[2] = pU.z - p2.z;

			//tanV
			tanV.v3[0] = pV.x - p2.x;
			tanV.v3[1] = pV.y - p2.y;
			tanV.v3[2] = pV.z - p2.z;

			tanU.normalize();
			tanV.normalize();

			//3rd normal
			n2 = tanU.cross(tanU, tanV);


			//4th point
			Point p3 = Calculate(u0, v1);
			pU = Calculate(u0 + delta, v1);
			pV = Calculate(u0, v1 + delta);

			//tanU
			tanU.v3[0] = pU.x - p3.x;
			tanU.v3[1] = pU.y - p3.y;
			tanU.v3[2] = pU.z - p3.z;

			//tanV
			tanV.v3[0] = pV.x - p3.x;
			tanV.v3[1] = pV.y - p3.y;
			tanV.v3[2] = pV.z - p3.z;

			tanU.normalize();
			tanV.normalize();

			//4th normal
			n3 = tanU.cross(tanU, tanV);


			// draw points and normals
			if ((i != (LOD - 1)) && (j != (LOD - 1)))
			{
				//bottom left corner
				glNormal3f(n0.v3[0], n0.v3[1], n0.v3[2]);
				glVertex3f(p0.x, p0.y, p0.z);

				//bottom right corner
				glNormal3f(n1.v3[0], n1.v3[1], n1.v3[2]);
				glVertex3f(p1.x, p1.y, p1.z);

				//top right corner
				glNormal3f(n2.v3[0], n2.v3[1], n2.v3[2]);
				glVertex3f(p2.x, p2.y, p2.z);

				//top left corner
				glNormal3f(n3.v3[0], n3.v3[1], n3.v3[2]);
				glVertex3f(p3.x, p3.y, p3.z);
			}
		}
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnd();
	
	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, Globals::texture[0]);
	// Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Select GL_MODULATE to mix texture with polygon color for shading:
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Use bilinear interpolation:
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glEnable(GL_COLOR_MATERIAL);

	glBegin(GL_QUADS);

	// specify texture coordinates for each vertex
	// note that textures are stored "upside down"

	//left wall
	glColor3f(1.0, 1.0, 1.0);

	glNormal3f(1, 0, 0);
	glTexCoord2f(0, 1); glVertex3f(-10, 0, 10);
	glTexCoord2f(1, 1); glVertex3f(-10, 0, -10);
	glTexCoord2f(1, 0); glVertex3f(-10, 20, -10);
	glTexCoord2f(0, 0); glVertex3f(-10, 20, 10);

	glEnd();

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, Globals::texture[1]);

	glBegin(GL_QUADS);
	//back wall
	glColor3f(1.0, 1.0, 1.0);

	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 1); glVertex3f(-10, 0, -10);
	glTexCoord2f(1, 1); glVertex3f(10, 0, -10);
	glTexCoord2f(1, 0); glVertex3f(10, 20, -10);
	glTexCoord2f(0, 0); glVertex3f(-10, 20, -10);
	glEnd();

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, Globals::texture[2]);
	glBegin(GL_QUADS);
	//right wall
	glColor3f(1.0, 1.0, 1.0);

	glNormal3f(-1, 0, 0);
	glTexCoord2f(0, 1); glVertex3f(10, 0, -10);
	glTexCoord2f(1, 1); glVertex3f(10, 0, 10);
	glTexCoord2f(1, 0); glVertex3f(10, 20, 10);
	glTexCoord2f(0, 0); glVertex3f(10, 20, -10);
	glEnd();

	////Set this texture to be the one we are working with
	//glBindTexture(GL_TEXTURE_2D, Globals::texture[3]);
	//glBegin(GL_QUADS);
	////front wall
	//glColor3f(1.0, 1.0, 1.0);
	//
	//glNormal3f(0, 0, 1);
	//glTexCoord2f(0, 1); glVertex3f(-10, 0, 10);
	//glTexCoord2f(1, 1); glVertex3f(10, 0, 10);
	//glTexCoord2f(1, 0); glVertex3f(10, 20, 10);
	//glTexCoord2f(0, 0); glVertex3f(-10, 20, 10);
	//glEnd();

	// Set this texture to be the one we are working with
	glBindTexture(GL_TEXTURE_2D, Globals::texture[4]);
	glBegin(GL_QUADS);
	//top wall
	glColor3f(1.0, 1.0, 1.0);

	glNormal3f(0, -1, 0);
	glTexCoord2f(0, 1); glVertex3f(-10, 20, 10);
	glTexCoord2f(1, 1); glVertex3f(10, 20, 10);
	glTexCoord2f(1, 0); glVertex3f(10, 20, -10);
	glTexCoord2f(0, 0); glVertex3f(-10, 20, -10);
	glEnd();


	glFlush();
	glutSwapBuffers();
}
