#include<stdlib.h>
#include<string.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<random>
#include<windows.h>
#include<GLUT/glut.h>
using namespace std;

float posXP = 0, posYP = 10, posZP = 0;
float posXB = 0, posYB = 0, posZB = 0;
float dx = 0, dy = 1.0;

bool incx = false, incy = true;
bool ballRel = false;
bool pause = false;

GLfloat paddle_color_array[][3] = { { 1,0,0 },{ 0,0,1 },{ 0,1,0 },{ 1,0,1 },{ 1,1,0 },{ 0,1,1 },{ 1,1,1 },{ 1,0.5,0 } };
GLfloat brick_color_array[][3] = { { 1,0,0 },{ 0,0,1 },{ 0,1,0 },{ 1,0,1 },{ 1,1,0 },{ 0,1,1 },{ 1,1,1 },{ 1,0.5,0 } };
GLfloat ball_color_array[][3] = { { 1,0,0 },{ 0,0,1 },{ 0,1,0 },{ 1,0,1 },{ 1,1,0 },{ 0,1,1 },{ 1,1,1 },{ 1,0.5,0 } };
GLfloat paddle_size[] = { 50, 100, 200 };

const int column = 10;
const int row = 10;
int padsize = 2;
int brick_color = 6;
int paddle_color = 3;
int ball_color = 4;
int xmax, ymax;
int menux, menuy, scrolly;
int bricks[column][row];
int presentstate = 1;
int score = 0;
int windows[4];

void ball() {
	glColor3fv(ball_color_array[ball_color]);
	glScalef(5, 5, 0);
	glutSolidSphere(2.0, 100, 10);
}
void paddle() {
	glColor3fv(paddle_color_array[paddle_color]);
	glBegin(GL_POLYGON);
	glVertex3f(5.0, 0, 0);
	glVertex3f(paddle_size[padsize] - 10.0, 0, 0);
	for (float i = -90; i < 91; i = i + 0.01)
		glVertex3f((paddle_size[padsize] - 10.0) + 10 * cosf(i*M_PI), 10 + 10 * sinf(i*M_PI), 0.0);
	glVertex3f(paddle_size[padsize] - 10.0, 20, 0);
	glVertex3f(10.0, 20, 0);
	for (float i = 90; i < 270; i = i + 0.01)
		glVertex3f(10.0 + 10 * cosf(i*M_PI), 10 + 10 * sinf(i*M_PI), 0.0);
	glEnd();
}
void block(float xv, float yv)
{
	glColor3fv(brick_color[brick_color_array]);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0, 0);
	glVertex3f(xv, 0, 0);
	glVertex3f(xv, yv, 0);
	glVertex3f(0, yv, 0);
	glEnd();
}
void button()
{
	glColor3f(0.25, 0.22, 0.11);
	glBegin(GL_POLYGON);
	glVertex3f(0, 100, 0);
	for (float i = (M_PI / 2.0); i > 0; i = i - 0.01)
		glVertex3f(350.0 + 50.0 * cosf(i), 50.0 + 50.0 * sinf(i), 0.0);
	glVertex3f(400.0, 0.0, 0);
	for (float i = (3 * M_PI) / 2.0; i >= M_PI; i = i - 0.01)
		glVertex3f(50.0 + 50.0 * cosf(i), 50.0 + 50.0 * sinf(i), 0.0);
	glEnd();
	glLineWidth(5.0);
	glColor3f(0.99, 0.95, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 100, 0);
	for (float i = (M_PI / 2.0); i > 0; i = i - 0.01)
		glVertex3f(350.0 + 50.0 * cosf(i), 50.0 + 50.0 * sinf(i), 0.0);
	glVertex3f(400.0, 0.0, 0);
	for (float i = (3 * M_PI) / 2.0; i >= M_PI; i = i - 0.01)
		glVertex3f(50.0 + 50.0 * cosf(i), 50.0 + 50.0 * sinf(i), 0.0);
	glEnd();
}
void brick()
{
	float xf = xmax / column;
	float yf = (0.50 * ymax) / row;
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			if (bricks[i][j] == 1) {
				glPushMatrix();
				glLoadIdentity();
				glTranslated((i * xf) + (0.25 * xf), (0.40 * ymax) + (j * yf) + (0.25 * yf), 0);
				block(xf / 2, yf / 2);
				glPopMatrix();
			}
		}
	}
}
void writestring(void *font, char *s, float x, float y) {
	unsigned int i;
	glRasterPos2f(x, y);
	for (i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}
void writestring(void *font, const char *s, float x, float y) {
	unsigned int i;
	glRasterPos2f(x, y);
	for (i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}

void scoredisplay(); //To avoid error
void init();
void keyboardfunction(unsigned char key, int mousex, int mousey);

//Gameplay
void detectblock() { //To detect if ball hits the paddle
	float xf = xmax / column;
	float yf = (0.50 * ymax) / row;
	bool flag = false;
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			if (bricks[i][j] == 1)
				flag = true;
		}
	}
	if (!flag) {
		presentstate = 3;
		ballRel = !ballRel;
		windows[2] = glutCreateWindow("Score");
		glutFullScreen();
		glutSetWindow(windows[2]);
		glutDestroyWindow(windows[1]);
		init();
		glutKeyboardFunc(keyboardfunction);
		glutDisplayFunc(scoredisplay);
		glutMainLoop();
	}
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			float minx = (i * xf) + (0.25 * xf);
			float maxx = (i * xf) + (0.25 * xf) + (xf / 2);
			float miny = (0.40 * ymax) + (j * yf) + (0.25 * yf);
			float maxy = (0.40 * ymax) + (j * yf) + (0.25 * yf) + (yf / 2);
			if (bricks[i][j] == 1) {
				if ((posXB + 10.0) >= minx && (posXB - 10.0) <= maxx && (posYB + 10.0) >= miny && (posYB - 10.0) <= maxy) {
					bricks[i][j] = 0;
					score += 100;
					if (posYB < miny || posYB > maxy) {
						if (posYB < miny)
							incy = false;
						else if (posYB > maxy)
							incy = true;
						if (posXB <= ((maxx - minx) / 2)) {
							incx = false;
						}
						else {
							incx = true;
						}
					}
					else if (posXB < minx || posYB > maxx) {
						if (posXB < minx)
							incx = false;
						else if (posXB > maxx)
							incx = true;
						if ((posYB + 10.0) <= ((maxy - miny) / 2)) {
							incy = false;
						}
						else {
							incy = true;
						}
					}
				}
			}
		}
	}
}
void change_paddle_color(int action)
{
	paddle_color = action - 1;
}

void change_brick_color(int action)
{
	brick_color = action - 1;
}

void change_paddle_size(int action)
{
	padsize = action - 1;
}

void change_ball_color(int action)
{
	ball_color = action - 1;
}

void handle_menu(int action)
{
}

void menus()
{
	if (presentstate == 2) {
		int submenu1 = glutCreateMenu(change_ball_color);
		glutAddMenuEntry("Red", 1);
		glutAddMenuEntry("Blue", 2);
		glutAddMenuEntry("Green", 3);
		glutAddMenuEntry("Purple", 4);
		glutAddMenuEntry("Yellow", 5);
		glutAddMenuEntry("Cyan", 6);
		glutAddMenuEntry("White", 7);
		glutAddMenuEntry("Orange", 8);
		int submenu2 = glutCreateMenu(change_brick_color);
		glutAddMenuEntry("Red", 1);
		glutAddMenuEntry("Blue", 2);
		glutAddMenuEntry("Green", 3);
		glutAddMenuEntry("Purple", 4);
		glutAddMenuEntry("Yellow", 5);
		glutAddMenuEntry("Cyan", 6);
		glutAddMenuEntry("White", 7);
		glutAddMenuEntry("Orange", 8);
		int submenu3 = glutCreateMenu(change_paddle_color);
		glutAddMenuEntry("Red", 1);
		glutAddMenuEntry("Blue", 2);
		glutAddMenuEntry("Green", 3);
		glutAddMenuEntry("Purple", 4);
		glutAddMenuEntry("Yellow", 5);
		glutAddMenuEntry("Cyan", 6);
		glutAddMenuEntry("White", 7);
		glutAddMenuEntry("Orange", 8);
		int submenu4 = glutCreateMenu(change_paddle_size);
		glutAddMenuEntry("Small", 1);
		glutAddMenuEntry("Medium", 2);
		glutAddMenuEntry("Large", 3);
		glutCreateMenu(handle_menu);
		glutAddSubMenu("Ball Color", submenu1);
		glutAddSubMenu("Bricks Color", submenu2);
		glutAddSubMenu("Paddle Color", submenu3);
		glutAddSubMenu("Paddle Size", submenu4);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
}
void menudisplay() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_POLYGON);
	glColor3f(0.875, 0.43, 0.975);
	glVertex2f(-xmax, ymax);
	glColor3f(0.575, 0.33, 0.075);
	glVertex2f(-xmax, -ymax);
	glColor3f(0.056, 0.406, 0.99);
	glVertex2f(xmax, -ymax);
	glColor3f(0.875, 0.43, 0.75);
	glVertex2f(xmax, ymax);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.99, 0.95, 0.0);
	char displaystring[10] = "ARKANOID";
	if (menux < (xmax - 350))
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, menux + 50.0, menuy);
	else
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, (xmax - 300) - (menux - (xmax - 350)), menuy);
	menux = (menux + 1) % (2 * (xmax - 350));
	glutPostRedisplay();
	Sleep(5);
	glPushMatrix();
	glTranslatef(0.35 * xmax, 0.50 * ymax, 0.0);
	button();
	glPopMatrix();
	strcpy_s(displaystring, "New Game");
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, (0.35 * xmax) + 150.0, (0.50 * ymax) + 50.0);
	glPushMatrix();
	glTranslatef(0.35 * xmax, 0.25 * ymax, 0.0);
	button();
	glPopMatrix();
	strcpy_s(displaystring, "Credits");
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, displaystring, (0.35 * xmax) + 175.0, (0.25 * ymax) + 50.0);
	glFlush();
}
void gamedisplay() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	glColor3f(0.202, 0.45, 0.802);
	glVertex2f(-xmax, ymax);
	glVertex2f(-xmax, -ymax);
	glColor3f(0.37, 0.650, 0.920);
	glVertex2f(xmax, -ymax);
	glVertex2f(xmax, ymax);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(posXP, posYP, posZP);
	paddle();
	glPopMatrix();
	glPushMatrix();
	if (ballRel){
		glTranslatef(posXB, posYB, posZB);
	}else{
		glTranslatef(posXP + (paddle_size[padsize] / 2), posYP + 30.0, posZP);
	}
	ball();
	glPopMatrix();
	brick();
	char scorestring[10] = "Score : ";
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestring, xmax * 0.75, ymax * 0.95);
	_itoa_s(score, scorestring, 10);//To convert score to string
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestring, xmax * 0.85, ymax * 0.95);
	if (pause) {
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press P to continue ... ", xmax * 0.25, ymax * 0.95);
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " GAME PAUSED ", xmax * 0.45, ymax * 0.15);
	}
	else
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press P to pause ... ", xmax * 0.25, ymax * 0.95);
	glutPostRedisplay();
	glFlush();
}
void scoredisplay() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_QUADS);
	glColor3f(1.20, 1.0, 0.02);
	glVertex2f(-xmax, ymax);
	glVertex2f(-xmax, -ymax);
	glColor3f(1.0, 0.0, 0.20);
	glVertex2f(xmax, -ymax);
	glVertex2f(xmax, ymax);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, 0.80 * ymax);
	bool flag = false;
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			if (bricks[i][j] == 1)
				flag = true;
		}
	}
	if (flag)
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, "You missed the ball ... ", 0.40 * xmax, 0.75 * ymax);
	else
		writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Congratulations! You have won. ", 0.35 * xmax, 0.75 * ymax);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, 0.70 * ymax);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "YOU SCORE IS :", 0.35 * xmax, 0.50 * ymax);
	char scorestr[10];
	_itoa_s(score, scorestr, 10);//To convert score to string
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, scorestr, 0.55 * xmax, 0.50 * ymax);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, " Press Enter to continue ... ", xmax * 0.35, ymax * 0.30);
	glPopMatrix();
	glFlush();
}

void creditsdisplay() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.749, 1.0);
	glVertex2f(-xmax, ymax);
	glColor3f(0.780, 0.082, 0.522);
	glVertex2f(-xmax, -ymax);
	glColor3f(0.0, 0.545, 1.0);
	glVertex2f(xmax, -ymax);
	glColor3f(0.780, 0.082, 0.522);
	glVertex2f(xmax, ymax);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glLoadIdentity();
	glPushMatrix();
	glColor3f(1.0, 1.0, 0.0);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, (0.85 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Developed by", 0.40 * xmax, (0.80 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "----------------------------------------------------------------------------------------", 0.05 * xmax, (0.75 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Shreenivas Hathwar J", 0.35 * xmax, (0.70 * ymax) + scrolly);
	writestring(GLUT_BITMAP_TIMES_ROMAN_24, "Shrivishnu", 0.35 * xmax, (0.65 * ymax) + scrolly);
	glPopMatrix();
	glutPostRedisplay();
	Sleep(1);
	glFlush();
}

void idlefunction()
{
	if (presentstate == 2 && !pause) {
		if (ballRel)
		{
			if (posXB > 10.0 && posXB < (xmax - 10.0) && posYB >(posYP + 20.0) && posYB < (ymax - 10.0))
			{
				detectblock();
				if (incx)
					posXB += 10.0 * dx;
				else
					posXB -= 10.0 * dx;
				if (incy)
					posYB += 10.0 * dy;
				else
					posYB -= 10.0 * dy;
				Sleep(1);
			}
			if (posXB < 10) {
				incx = true;
				posXB += 10.0 * dx;
			}
			if (posXB >(xmax - 10.0)) {
				incx = false;
				posXB -= 10.0 * dx;
			}
			if (posYB > ((ymax * 0.95) - 10.0)) {
				incy = false;
				posYB -= 10.0 * dy;
			}
			if (posYB <= (posYP + 20.0)) {
				if ((posXB + 10.0) > posXP && (posXB - 10.0) < (posXP + paddle_size[padsize])) {
					incy = true;
					if (posXB == (posXP + (paddle_size[padsize] / 2))) {
						dx = 0;
					}
					else if (posXB < (posXP + (paddle_size[padsize] / 2))) {
						dx = (((posXP + (paddle_size[padsize] / 2)) - posXB) / (paddle_size[padsize] / 2)) + 0.25;
						incx = false;
						posXB -= 10.0 * dx;
					}
					else if (posXB >(posXP + (paddle_size[padsize] / 2))) {
						dx = ((posXB - (posXP + (paddle_size[padsize] / 2))) / (paddle_size[padsize] / 2)) + 0.25;
						incx = true;
						posXB += 10.0 * dx;
					}
					dy = abs(1 - dx);
					posYB += 10.0 * dy;
				}
				else {
					presentstate = 3;
					ballRel = !ballRel;
					windows[2] = glutCreateWindow("Score");
					glutFullScreen();
					glutSetWindow(windows[2]);
					glutDestroyWindow(windows[1]);
					init();
					glutKeyboardFunc(keyboardfunction);
					glutDisplayFunc(scoredisplay);
					glutMainLoop();
				}
			}
		}
		glutPostRedisplay();
	}
	if (presentstate == 4) {
		scrolly++;
		if (scrolly > (0.35 * ymax)) {
			presentstate = 1;
			glutSetWindow(windows[0]);
			glutDestroyWindow(windows[3]);
		}
	}
}

void passivefunction(int x, int y)
{
	if (presentstate == 2 && !pause) {
		if (x < (xmax - (paddle_size[padsize] / 2)) && x >(paddle_size[padsize] / 2))//Paddle Size and Curved Surface
			posXP = x - (paddle_size[padsize] / 2);
		else if (x >= (xmax - (paddle_size[padsize] / 2)))
			posXP = xmax - paddle_size[padsize];
		else
			posXP = 0.0;
		glutPostRedisplay();
	}
}
void mousefunction(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (presentstate == 1)
			if ((x > (0.35 * xmax) && x < ((0.35 * xmax) + 400.0)) && ((ymax - y) >(0.50 * ymax) && (ymax - y) < ((0.50 * ymax) + 100.0))) {
				for (int i = 0; i < column; i++)
					for (int j = 0; j < row; j++)
						bricks[i][j] = (rand() % 2);
				posXP = 0;
				posYP = 10;
				posZP = 0;
				posXB = 0;
				posYB = 0;
				posZB = 0;
				dx = 0;
				dy = 1.0;
				incx = false;
				incy = true;
				ballRel = false;
				score = 0;
				windows[1] = glutCreateWindow("Game");
				glutSetWindow(windows[1]);
				glutFullScreen();
				presentstate = 2;
				init();
				menus();
				glutDisplayFunc(gamedisplay);
				glutIdleFunc(idlefunction);
				glutPassiveMotionFunc(passivefunction);
				glutKeyboardFunc(keyboardfunction);
				glutMouseFunc(mousefunction);
				glutMainLoop();
			}
			else if ((x > (0.35 * xmax) && x < ((0.35 * xmax) + 400.0)) && ((ymax - y) >(0.25 * ymax) && (ymax - y) < ((0.25 * ymax) + 100.0))) {
				scrolly = -1 * (ymax * 0.85);
				windows[3] = glutCreateWindow("Credits");
				glutSetWindow(windows[3]);
				glutFullScreen();
				presentstate = 4;
				init();
				glutIdleFunc(idlefunction);
				glutDisplayFunc(creditsdisplay);
				glutKeyboardFunc(keyboardfunction);
				glutMainLoop();
			}
			if (presentstate == 2 && !pause) {
				if (!ballRel)
				{
					ballRel = true;
					posXB = posXP + (paddle_size[padsize] / 2);
					posYB = posYP + 35.0;
					posZB = posZP;
				}
			}
	}
	
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glColor3f(1.0, 1.0, 1.0);
	glOrtho(0.0, xmax, 0.0, ymax, 1.0, -1.0);
}

void keyboardfunction(unsigned char key, int mousex, int mousey) {
	switch (key) {
	case '\r':if (presentstate == 3) {
		presentstate = 1;
		score = 0;
		glutDestroyWindow(windows[2]);
		menux = 0;
		menuy = 0.9*ymax;
	}
			  break;
	case 'p':
	case 'P':pause = !pause;
		break;
	case 'q':
	case 'Q':
		exit(0);
	default:;
	}
}

int main(int argc, char** argv)
{
	xmax = 1366;
	ymax = 768;  // Resolution
	menux = 0;
	menuy = 0.9*ymax;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(xmax, ymax);
	glutInitWindowPosition(0, 0);
	windows[1] = glutCreateWindow("Menu");
	glutFullScreen();
	init();
	glutMouseFunc(mousefunction);
	glutKeyboardFunc(keyboardfunction);
	glutDisplayFunc(menudisplay);
	glutMainLoop();
}