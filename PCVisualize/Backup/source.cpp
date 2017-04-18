/*
Program Title
=============
Point Cloud Data Visualizer

Author
====== 
Shishir Pagad
Graduate Student
Department of Computer Science and Enginering
University of Minnesota, Twin Cities

Date: 7th March, 2017
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glut.h>
#include <cmath>
#include <vector>

#include "imageloader.h"

#define MENU_A    1                  /* menu option identifiers */
#define MENU_B    2
#define MENU_C    3
#define MENU_QUIT 9

using namespace std;

const float BOX_SIZE = 7.0f;
float _angle = 0;   
float Incx=0,Incz=0;  
float Incx1=0,Incz1=0;  
float Incx2=0,Incz2=0; 
float Incx3=0,Incz3=0;
float cx=0.4,cy=-0.4,cz=0.3;
float Incx4=0,Incz4=0,Incy4=0;
void *currentfont;
int starta=0,data=0,i=0,ofl=0,count=0;
void sendpacket();

// PointCloud data read from file
vector< vector<float> > pointCloud;

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGB,
				 image->width, image->height,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 image->pixels);
	return textureId;
}

void setFont(void *font)
{
	currentfont=font;
}



void drawstring(float x,float y,float z,char *string)
{
	    char *c;
	glRasterPos3f(x,y,z);
 
	for(c=string;*c!='\0';c++)
	{	glColor3f(0.0,0.0,0.0);
		glutBitmapCharacter(currentfont,*c);
	}
}


// Colors
GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat GREEN[] = {0, 1, 0};
GLfloat MAGENTA[] = {1, 0, 1};
GLfloat BROWN[] = {0.647059, 0.164706, 0.164706};


// A camera.  It moves horizontally in a circle centered at the origin of
// radius 10.  It moves vertically straight up and down.
class Camera {
  double theta;      // determines the x and z positions
  double y;          // the current y position
  double z;
  double dTheta;     // increment in theta for swinging the camera around
  double dy;         // increment in y for moving the camera up/down
  double dz;
public:
  Camera(): theta(0), y(3.5), dTheta(0.04), dy(0.2) , z(1.35), dz(0.02) {}
  double getX() {return 10 * z*cos(theta);}
  double getY() {return y;}
  double getZ() {return 10 * z*sin(theta);}
  void moveRight() {theta += dTheta;}
  void moveLeft() {theta -= dTheta;}
  void moveUp() {y += dy;}
  //void moveDown() {if (y > dy) y -= dy;}
  void moveDown() {y -= dy;}
  void zoomOut() {z += dz; }
  void zoomIn() {z -=dz; }
};

Camera camera;


// Application-specific initialization: Set up global lighting parameters
// and create display lists.
void init() 
{
	glEnable(GL_DEPTH_TEST);
	glMaterialfv(GL_FRONT, GL_AMBIENT, WHITE);
	//glMaterialf(GL_FRONT, GL_SHININESS, 30);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable( GL_LINE_SMOOTH);
	glPointSize(2.0f);
}



void drawPoint(float red, float green, float blue, float xCoord, float yCoord, float zCoord)
{
	red = red/255;
	green = green/255;
	blue = blue/255;
	
	glBegin(GL_POINTS);
		glColor3f(red, green, blue);
		glVertex3f(xCoord, -yCoord, zCoord);
	glEnd();
}


void drawAxis()
{
	// X-Axis
	glLineWidth(2);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.25, 0.0, 0.0);
	glEnd();
	
	// Y-Axis
	glLineWidth(2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.25, 0.0);
	glEnd();
	
	// Z-Axis
	glLineWidth(2);
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.25);
	glEnd();


}
		



void update(int value)
{	
	glutPostRedisplay();
}


void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(camera.getX(), 
				camera.getY(), 
				camera.getZ(),
				0,
				0,
				0,
				0,
				1,
				0
				);	
				
	GLfloat whiteEmissiveMaterial[] = {1.0, 1.0, 1.0};


	GLfloat ambientColor[] = {1.0f, 1.0f, 1.0f, 1.0f}; //Color(0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	//glMaterialfv(GL_FRONT, GL_EMISSION, whiteEmissiveMaterial);


	// Display axis
	drawAxis();
	
	// Display Point Cloud
	for(int i=0; i<pointCloud.size(); i++)
	{			
		drawPoint(pointCloud[i][0], pointCloud[i][1], pointCloud[i][2], pointCloud[i][3], pointCloud[i][4], pointCloud[i][5]);
	}
		
		
	//glutTimerFunc(100,update,0);
	glFlush();
	glutSwapBuffers();
}



void readPointCloud(void)
{
	ifstream file("structure.txt");
	string line;
	
	// Ignore first line
	getline(file, line);
	
	while(getline(file, line))
	{
		istringstream iss(line);
		
		float val;
		// Ignore first value
		iss >> val;
		
		vector<float> points;
		
		for(int i=0; i<6; i++)
		{
			iss >> val;
			points.push_back(val);
		}
		pointCloud.push_back(points);
	}
}




// On reshape, constructs a camera that perfectly fits the window.
void reshape(GLint w, GLint h) {
  	glViewport(0, 0, w, h);
  	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
  	gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 150.0);
  	glMatrixMode(GL_MODELVIEW);
}

// Requests to draw the next frame.

void timer(int v) {
  	glutPostRedisplay();
  	glutTimerFunc(1000/60, timer, 0);
}


//this method is called by glutidlefunc. This method is required to animate the packets
void idle()
{
	glutPostRedisplay();
}




// Moves the camera according to the key pressed, then ask to refresh the
// display.

void special(int key, int, int) {
  	switch (key) {
		case GLUT_KEY_LEFT: camera.moveRight(); break;
		case GLUT_KEY_RIGHT: camera.moveLeft(); break;
		case GLUT_KEY_UP: camera.moveUp(); break;
		case GLUT_KEY_DOWN: camera.moveDown(); break;
		case GLUT_KEY_PAGE_UP: camera.zoomIn(); break;
		case GLUT_KEY_PAGE_DOWN: camera.zoomOut(); break;
  	}
  	glutPostRedisplay();
}

void menuCB(int item)
{
	switch (item) 
	{   // process the popup menu commands
		case MENU_A:
			starta=1;
			data=1;
			break;

		case MENU_B:
			starta=2;
			break;
	 
		case MENU_C:
			starta=1;
			ofl=1;
			break;

		case MENU_QUIT:
			exit(0);		// quit the program
			break;
	}
}



// Initializes GLUT and enters the main loop.
int main(int argc, char** argv) 
{
 	glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  	glutInitWindowPosition(80, 80);
  	glutInitWindowSize(1280, 720);
  	glutCreateWindow("Point Cloud Visualizer");
  	glClearColor(1.0,1.0,1.0,1);
  	
  	// Read Point Cloud data
  	readPointCloud();
  	
  	glutDisplayFunc(display);


  	glutCreateMenu(menuCB);                    	// main popup menu
	glutAddMenuEntry("Send Data", MENU_A);  
	glutAddMenuEntry("Receive Data", MENU_B);	// menu option A
	glutAddMenuEntry("Overflow Data", MENU_C);
	glutAddMenuEntry("Quit",     MENU_QUIT);   	// quit option
    glutAttachMenu(GLUT_RIGHT_BUTTON);


  	glutReshapeFunc(reshape);
  	glutSpecialFunc(special);
  	glutIdleFunc(idle);
  	//glutTimerFunc(100, timer, 0);
  	init();
  	glutMainLoop();
}
