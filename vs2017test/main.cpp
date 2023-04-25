#include "glut.h"
#include <math.h>

#define VERTICAL 1 
#define HORIZONTAL 0
#define LIGHTOFF_BASE 0.175
#define LIGHTOFF_OFFSET 0.25
#define TRAFIC_LIGHT_BODY 0.15
#define TRAFIC_LIGHT_PROTECTOR 0.10

const int W = 600;
const int H = 600;
const double PI = 3.14;

double pitch = 0;
bool pitch_is_moving = false;
double pitch_state = 0;
double eyeXPosAngle = 45;
int last = HORIZONTAL;

const int TW = 256; // must be a power of 2
const int TH = 256; // must be a power of 2
unsigned char tx0[TH][TW][3];

// lighting
float l0amb[4] = { 0.2, 0.2, 0.2,0 };
float l0dif[4] = { 0.9, 0.9, 0.9,0 };
float l0sp[4] = { 0.5, 0.5, 0.5,0 };
float l0pos[4] = { 0.5, 1, 0.5,0 };// if the 4-th parameter is 0 then the light is directional
// if the 4-th parameter is 1 then the light is positional

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (x > 60 && x < 90 &&
			(H - y)>15 + pitch - 10 && (H - y) < 135 + pitch + 10) // start pitch drag
			pitch_is_moving = true;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (x > 60 && x < 90 &&
			(H - y)>15 + pitch - 10 && (H - y) < 135 + pitch + 10) // end pitch drag
			pitch_is_moving = false;
	}
}

void mouse_drag(int x, int y)
{
	if (pitch_is_moving && (H - y) < 135 && (H - y) > 15)
	{
		pitch = (H - y) - 75;
		if ((H - y) <= 100 && (H - y) > 50)
			pitch_state = 0; // yellow
		else if ((H - y) <= 50 && (H - y) > 0)
		{
			pitch_state = 1;//green
			last = HORIZONTAL;
		}
		else
		{
			pitch_state = 2;//red
			last = VERTICAL;
		}
	}
}

void setTexture(int num_texture)
{
	int i, j, k;
	int delta;
	switch (num_texture)
	{
	case 0: // common space
		for (i = 0; i < TH; i++)
			for (j = 0; j < TW; j++)
				for (k = 0; k < 3; k++)
					tx0[i][j][k] = 96;
		break;
	case 1: // road
		for (i = 0; i < TH; i++)
			for (j = 0; j < TW; j++)
			{
				delta = -15 + rand() % 30;
				if (i<10 || i>TH - 10 || (j<TW / 2 && i>TH / 2 - 10 && i < TH / 2 + 10))
				{
					tx0[i][j][0] = 220 + delta; // red
					tx0[i][j][1] = 220 + delta; // green
					tx0[i][j][2] = 220 + delta;// blue
				}
				else // road
				{
					tx0[i][j][0] = 96 + delta; // red
					tx0[i][j][1] = 96 + delta; // green
					tx0[i][j][2] = 96 + delta; // blue
				}
			}
		break;
	case 2: // cross road
		for (i = 0; i < TH; i++)
			for (j = 0; j < TW; j++)
			{
				delta = -15 + rand() % 30;
				if ((i >= 20 && i <= TH) && ((i - 20) % 60 <= 30))
				{
					tx0[i][j][0] = 200 + delta; // red
					tx0[i][j][1] = 200 + delta; // green
					tx0[i][j][2] = 200 + delta;// blue
				}
				else // road
				{
					tx0[i][j][0] = 96 + delta; // red
					tx0[i][j][1] = 96 + delta; // green
					tx0[i][j][2] = 96 + delta; // blue
				}
			}
		break;
	}
}

void init()
{
	glClearColor(0.04, 0.35, 0.49, 0);// color of window background
	glEnable(GL_DEPTH_TEST); // uses Z-buffer

	setTexture(0); // common space
	glBindTexture(GL_TEXTURE_2D, 0); // 0 is the "name" of a texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx0);

	setTexture(1); // road
	glBindTexture(GL_TEXTURE_2D, 1); // 1 is the "name" of a texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx0);

	setTexture(2); // cross road
	glBindTexture(GL_TEXTURE_2D, 2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TW, TH, 0, GL_RGB, GL_UNSIGNED_BYTE, tx0);
}

void drawRoad()
{
	glPushMatrix();
	glTranslated(-1.75, 0, -1.75);
	glScaled(2.5, 1, 2.5);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);//common road
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
	glPushMatrix();
	glTranslated(2, 0, 2);
	glBegin(GL_POLYGON);
	glTexCoord2d(1, 0);	 glVertex3d(1, 0.1, 1);
	glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
	glTexCoord2d(0, 1);	 glVertex3d(-1, 0.1, -1);
	glTexCoord2d(1, 1);  glVertex3d(-1, 0.1, 1);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	for (int k = -2; k >= -10; k -= 2) 
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 1);// road
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
		glPushMatrix();
		glTranslated(2, 0, k);
		glBegin(GL_POLYGON);
		glTexCoord2d(2, 0);	 glVertex3d(1, 0.1, 1);
		glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
		glTexCoord2d(0, 2);	 glVertex3d(-1, 0.1, -1);
		glTexCoord2d(2, 2);  glVertex3d(-1, 0.1, 1);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 2);//cross road
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
	glPushMatrix();
	glTranslated(2, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2d(1, 0);	 glVertex3d(1, 0.1, 1);
	glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
	glTexCoord2d(0, 1);	 glVertex3d(-1, 0.1, -1);
	glTexCoord2d(1, 1);  glVertex3d(-1, 0.1, 1);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	for (int k = -2; k >= -10; k -= 2)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 1);// road
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
		glPushMatrix();
		glTranslated(k, 0, 2);
		glBegin(GL_POLYGON);
		glTexCoord2d(0, 2);	 glVertex3d(1, 0.1, 1);
		glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
		glTexCoord2d(2, 0);	 glVertex3d(-1, 0.1, -1);
		glTexCoord2d(2, 2);  glVertex3d(-1, 0.1, 1);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 2);//cross road
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
	glPushMatrix();
	glTranslated(0, 0, 2);
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 1);	 glVertex3d(1, 0.1, 1);
	glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
	glTexCoord2d(1, 0);	 glVertex3d(-1, 0.1, -1);
	glTexCoord2d(1, 1);  glVertex3d(-1, 0.1, 1);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	for (int k = 6; k <= 10; k += 2)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 1);// road
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
		glPushMatrix();
		glTranslated(k, 0, 2);
		glBegin(GL_POLYGON);
		glTexCoord2d(0, 2);	 glVertex3d(1, 0.1, 1);
		glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
		glTexCoord2d(2, 0);	 glVertex3d(-1, 0.1, -1);
		glTexCoord2d(2, 2);  glVertex3d(-1, 0.1, 1);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 2);//cross road
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
	glPushMatrix();
	glTranslated(4, 0, 2);
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 1);	 glVertex3d(1, 0.1, 1);
	glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
	glTexCoord2d(1, 0);	 glVertex3d(-1, 0.1, -1);
	glTexCoord2d(1, 1);  glVertex3d(-1, 0.1, 1);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	for (int k = 6; k <= 10; k += 2)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 1);// road
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
		glPushMatrix();
		glTranslated(2, 0, k);
		glBegin(GL_POLYGON);
		glTexCoord2d(2,0);	 glVertex3d(1, 0.1, 1);
		glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
		glTexCoord2d(0,2);	 glVertex3d(-1, 0.1, -1);
		glTexCoord2d(2, 2);  glVertex3d(-1, 0.1, 1);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 2);//cross road
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // can be GL_MODULATE
	glPushMatrix();
	glTranslated(2, 0, 4);
	glBegin(GL_POLYGON);
	glTexCoord2d(1, 0);	 glVertex3d(1, 0.1, 1);
	glTexCoord2d(0, 0);	 glVertex3d(1, 0.1, -1);
	glTexCoord2d(0, 1);	 glVertex3d(-1, 0.1, -1);
	glTexCoord2d(1, 1);  glVertex3d(-1, 0.1, 1);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}

void DrawCilynder(int n, double topr, double bottomr,int times,float red , float green , float blue)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha < times*PI; alpha += teta)
	{
		glColor3d(red, green, blue);
		glBegin(GL_POLYGON);
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // 1
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // 2 
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // 3 
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // 4
		glEnd();
	}
}

void DrawSphere(int n, int slices, float red, float green, float blue)
{
	double beta, delta = PI / slices;
	double topr, bottomr;
	int i;
	for (beta = -PI / 2; beta <= PI / 2; beta += delta)
	{
		topr = cos(beta + delta);
		bottomr = cos(beta);
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCilynder(n, topr, bottomr,2,red,green,blue);
		glPopMatrix();
	}
}

void drawTraficLight(int side)//side == 1 to vertical || side == 0 to horizontal
{
	glPushMatrix();
	{
		glPushMatrix(); {// outer budy
			glScaled(1,6,1);
			DrawCilynder(30, 1, 1,1, TRAFIC_LIGHT_BODY, TRAFIC_LIGHT_BODY, TRAFIC_LIGHT_BODY);
		}glPopMatrix();

		glPushMatrix(); {//face
			glTranslated(-1,0,-1);
			glBegin(GL_POLYGON);
			glColor3d(TRAFIC_LIGHT_BODY, TRAFIC_LIGHT_BODY, TRAFIC_LIGHT_BODY);
			glVertex3d(1,0,0);
			glVertex3d(1,0,2);
			glVertex3d(1,6,2);
			glVertex3d(1,6,0);
			glEnd();
		}	glPopMatrix();

		glPushMatrix(); {// red light protector 
			glTranslated(0,5,0);
			glRotated(90,0,0,1);
			glScaled(1,1.5,1);
			DrawCilynder(30, 1, 1, 1, TRAFIC_LIGHT_PROTECTOR, TRAFIC_LIGHT_PROTECTOR, TRAFIC_LIGHT_PROTECTOR);
		}glPopMatrix();
		glPushMatrix(); {// red light 
			glTranslated(0, 4.75, 0);
			glRotated(90, 0, 0, 1);
			glScaled(0.75, 0.75, 0.75);
			if(((pitch_state==2 && side == VERTICAL) || (pitch_state == 1 && side == HORIZONTAL)) || (side==last && pitch_state==0))
				DrawSphere(30, 20, 1, 0, 0); 
			else 
				DrawSphere(30, 20, LIGHTOFF_BASE+ LIGHTOFF_OFFSET, LIGHTOFF_BASE, LIGHTOFF_BASE);
		}glPopMatrix();

		glPushMatrix(); {// light protector yellow
			glTranslated(0, 3, 0);
			glRotated(90, 0, 0, 1);
			glScaled(1, 1.5, 1);
			DrawCilynder(30, 1, 1, 1, TRAFIC_LIGHT_PROTECTOR, TRAFIC_LIGHT_PROTECTOR, TRAFIC_LIGHT_PROTECTOR);
		}glPopMatrix();
		glPushMatrix(); {// yellow light 
			glTranslated(0, 2.75, 0);
			glRotated(90, 0, 0, 1);
			glScaled(0.75, 0.75, 0.75);
			(pitch_state ==0 )? DrawSphere(30, 20, 1, 1, 0):DrawSphere(30, 20, LIGHTOFF_BASE + LIGHTOFF_OFFSET, LIGHTOFF_BASE + LIGHTOFF_OFFSET, LIGHTOFF_BASE);
		}glPopMatrix();

		glPushMatrix(); {// light protector green
			glTranslated(0, 1, 0);
			glRotated(90, 0, 0, 1);
			glScaled(1, 1.5, 1);
			DrawCilynder(30, 1, 1, 1, TRAFIC_LIGHT_PROTECTOR, TRAFIC_LIGHT_PROTECTOR, TRAFIC_LIGHT_PROTECTOR);
		}glPopMatrix();
		glPushMatrix(); {// green light 
			glTranslated(0, 0.75, 0);
			glRotated(90, 0, 0, 1);
			glScaled(0.75, 0.75, 0.75);
			if ((pitch_state == 1 && side == VERTICAL) || (pitch_state == 2 && side == HORIZONTAL))
				DrawSphere(30, 20, 0, 1, 0);
			else
				DrawSphere(30, 20, LIGHTOFF_BASE, LIGHTOFF_BASE + LIGHTOFF_OFFSET, LIGHTOFF_BASE);
		}glPopMatrix();
	}
	glPopMatrix();
}
void DrawPitchControl()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.29, 0.29, 0.29, 0.5); // the 4-th parameter is opacity	
	glBegin(GL_POLYGON);
	glVertex2d(0, 0);
	glVertex2d(0, 150);
	glVertex2d(150, 150);
	glVertex2d(150, 0);
	glEnd();

	glColor4d(1, 0, 0, 1); // the 4-th parameter is opacity	
	glBegin(GL_POLYGON);
	glVertex2d(50, 150);
	glVertex2d(50, 100);
	glVertex2d(100, 100);
	glVertex2d(100,150);
	glEnd();

	glColor4d(1, 1, 0, 1); // the 4-th parameter is opacity	
	glBegin(GL_POLYGON);
	glVertex2d(50, 100);
	glVertex2d(50, 50);
	glVertex2d(100, 50);
	glVertex2d(100, 100);
	glEnd();

	glColor4d(0, 1, 0, 1); // the 4-th parameter is opacity	
	glBegin(GL_POLYGON);
	glVertex2d(50, 50);
	glVertex2d(50, 0);
	glVertex2d(100, 0);
	glVertex2d(100, 50);
	glEnd();

	glDisable(GL_BLEND);
	glLineWidth(2);
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(75, 135);
	glVertex2d(75, 15);
	glEnd();
	glLineWidth(1);

	glColor3d(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glVertex2d(60, 75 + pitch - 10);
	glVertex2d(60, 75 + pitch + 10);
	glVertex2d(90, 75 + pitch + 10);
	glVertex2d(90, 75 + pitch - 10);
	glEnd();
}

void display()
{
	double dx, dy, beta;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and z-buffer

	glViewport(0, 0, W, H); // full window
	glMatrixMode(GL_PROJECTION); // here we define working matrices of PROJECTIONS

	glLoadIdentity(); // start transformations here
	glFrustum(-1, 1, -1, 1, 0.7, 350);

	// look to variable direction
	gluLookAt(sqrt(2)*10*sin(eyeXPosAngle), 8, sqrt(2) * 10 * cos(eyeXPosAngle),
		0, 0, 0,  // this is the direction
		0, 1, 0);

	// Add lighting
	glEnable(GL_LIGHTING); // general lighting
	glEnable(GL_LIGHT0); // light source 0
	glEnable(GL_NORMALIZE);
	// definitions of light source 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0sp);
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);
	glDisable(GL_LIGHTING);

	//////////////////////////////////////    Adding  Texture   /////////////////////////////////////

	glPushMatrix();
	glTranslated(-4, 0, -4);
		glPushMatrix();
		glTranslated(-5, 0, -5);
		glScaled(25, 1, 25);
			glBegin(GL_POLYGON);
			glColor3d(0.08, 0.56, 0.04);
			glVertex3d(1.25, 0, 1.25);
			glVertex3d(1.25, 0, -1);
			glVertex3d(-1, 0, -1);
			glVertex3d(-1, 0, 1.25);
			glEnd();
		glPopMatrix();
	drawRoad();
	glPopMatrix();


	//west
	glPushMatrix();
	glTranslated(-3,2,2);
	drawTraficLight(VERTICAL);
	glPopMatrix();
	//north
	glPushMatrix();
	glRotated(-90,0,1,0);
	glTranslated(-4, 2, 5);
	drawTraficLight(HORIZONTAL);
	glPopMatrix();
	//east
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glTranslated(-2,2,3);
	drawTraficLight(HORIZONTAL);
	glPopMatrix();
	// south
	glPushMatrix();
	glRotated(180, 0, 1, 0);
	glTranslated(-5, 2, 4);
	drawTraficLight(VERTICAL);
	glPopMatrix();


	// add pitch control
	glViewport(0, 0, 150, 150);
	glMatrixMode(GL_PROJECTION); // turn back to PROJECTION matrices
	glLoadIdentity();
	glOrtho(0, 150, 0, 150, -1, 1);
	glDisable(GL_DEPTH_TEST); // 2d
	glMatrixMode(GL_MODELVIEW);// here we switch to working matrices of objects
	DrawPitchControl();
	glEnable(GL_DEPTH_TEST);
	glutSwapBuffers(); // show all
}

void idle()
{
	glutPostRedisplay(); // sends a message to the window to run display function
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == 'a')
		eyeXPosAngle -= 0.01;
	if (key == 'd')
		eyeXPosAngle += 0.01;
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Simple Transformations Example");

	glutDisplayFunc(display); // display is a name of refresh function
	glutIdleFunc(idle);// idle is a function that runs in background
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_drag);
	init();
	glutMainLoop();
}