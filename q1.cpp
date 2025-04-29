#include <glut.h>
#include <math.h>
#include <iostream>
using namespace std;

int xi, yi, xf, yf;
int xmax = 500, ymax = 500, xmin = 100, ymin = 100;
int ww = 600;
int wh = 600;
//int r = sqrt(pow((double)(xi - xf), 2) + pow(double(yi - yf), 2));

const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

struct Color {
	float r, g, b;
};

struct Color red = { 1.0,0.0,0.0 };
struct Color blue = { 0.0,0.0,1.0 };
bool useColor = false;

void putPixelCircle(int x, int y,struct Color color) {

	if (useColor) {
		glColor3f(0.0, 1.0, 0.0);
	}
	else {
		glColor3f(color.r, color.g, color.b);
	}
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	glFlush();
}

void plotCircle(int h, int k, int x, int y, struct Color color) {
	putPixelCircle(x+h, y+k,color);
	putPixelCircle(-x+h,-y+k, color);

	putPixelCircle(-x+h, y+k, color);
	putPixelCircle(x+h, -y+k, color);

	putPixelCircle(y+h,x+k, color);
	putPixelCircle(-y+h,-x+k, color);

	putPixelCircle(-y+h,x+k,color);
	putPixelCircle(y+h,-x+k,color);
}

void midpoint(int h, int k, int r,struct Color color) {
	int x = 0, y = r, d = 1 - r;

	while (x <= y) {
		plotCircle(h, k, x, y,color);

		if (d < 0) {
			d = d + 2*x + 3;

		}
		else {
			d = d + 2*(x - y) + 5;
			y--;
		}
		x++;

	}
}

void bresenhamCircle(int h,int k,int r,struct Color color) {

	int x = 0, y = r, d = 3 - 2 * r;
	while (x <= y) {
		plotCircle(h,k,x, y,color);

		if (d < 0) {
			d = d + 4*x + 6;
		}
		else {
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}

}

void putPixelLine(int x,int y) {

	int circle1 = (pow(double(x - 300), 2) + pow(double(y - 300), 2)) - (50*50);
	int circle2 = (pow(double(x - 300), 2) + pow(double(y - 300), 2)) - (100*100);

	if (circle2 <= 0) {

		if (circle1 <= 0) {
			glColor3f(1.0, 0.0, 0.0);
		}
		else {
			glColor3f(0.0, 0.0, 1.0);
		}
	}
	else {
		glColor3f(0.0, 0.0, 0.0);
	}

	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
	glFlush();


}

void dda(int xa, int ya, int xb, int yb) {
	float dx = xb - xa;
	float dy = yb - ya;

	float xinc, yinc, x, y;

	int steps , k;

	x = xa;
	y = ya;

	if (abs(dx) > abs(dy)) {
		steps = abs(dx);
	}
	else {
		steps = abs(dy);
	}

	xinc = dx / steps;
	yinc = dy / steps;

	putPixelLine(x, y);

	for (k = 0;k < steps;k++) {
		x = x + xinc;
		y = y + yinc;
		putPixelLine(x, y);
	}
}


void brehensam(int x0,int y0,int x1,int y1) {
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	if (dx >= dy) {
		int d = 2 * dy - dx;
		int dt = 2 * (dy - dx);
		int ds = 2 * dy;

		int x, y;

		if (x0 < x1) {
			x = x0;
			y = y0;
		}
		else {
			x = x1;
			y = y1;
			x1 = x0;
			y1 = y0;
		}
		putPixelLine(x, y);

		while (x <= x1) {
			if (d < 0) {
				d = d + ds;
			}
			else {

				if (y < y1) {
					d = d + dt;
					y++;
				}
				else {
					d = d + dt;
					y--;
				}
				
			}
			x++;
			putPixelLine(x, y);
		}

	}
	else {
		int d = 2 * dx - dy;
		int dt = 2 * (dx - dy);
		int ds = 2 * dx;
		int x, y;

		if (y0 < y1) {
			x = x0;
			y = y0;

		}
		else {
			x = x1;
			y = y1;
			x1 = x0;
			y1 = y0;
		}
		putPixelLine(x,y);

		while (y <= y1) {
			if (d < 0) {
				d = d + ds;
			}
			else {
				if (x < x1) {
					d = d + dt;
					x++;
				}
				else {
					d = d + dt;
					x--;
				}
			}
			y++;
			putPixelLine(x, y);
		}

	}
}
int computeOutCode(int x, int y) {
	int code = INSIDE;

	if (x < xmin)
		code |= LEFT;
	else if (x > xmax)
		code |= RIGHT;
	if (y < ymin)
		code |= BOTTOM;
	else if (y > ymax)
		code |= TOP;

	return code;
}

void cohenSutherland(int x0, int y0, int x1, int y1) {
	int outcode0, outcode1, outcode;
	bool accept = false, done = false;

	outcode0 = computeOutCode(x0, y0);
	outcode1 = computeOutCode(x1, y1);

	do {
		if (!(outcode0 | outcode1)) { 
			accept = true;
			done = true;
		}
		else if (outcode0 & outcode1) { 
			done = true;
		}
		else {
			float x, y;
			outcode = outcode0 ? outcode0 : outcode1;

			if (outcode & TOP) {
				x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
				y = ymax;
			}
			else if (outcode & BOTTOM) {
				x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
				y = ymin;
			}
			else if (outcode & RIGHT) {
				y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
				x = xmax;
			}
			else if (outcode & LEFT) {
				y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
				x = xmin;
			}

			if (outcode == outcode0) {
				x0 = x;
				y0 = y;
				outcode0 = computeOutCode(x0, y0);
			}
			else {
				x1 = x;
				y1 = y;
				outcode1 = computeOutCode(x1, y1);
			}
		}
	} while (!done);

	if (accept) {
		brehensam(x0, y0, x1, y1);
	}
}

void lianBasky(int x0, int y0, int x1, int y1) {

	int dx = x1 - x0;
	int dy = y1 - y0;

	float p, q, r;

	float t0 = 0.0, t1 = 1.0;

	for (int edge = 0; edge < 4; edge++) {
		if (edge == 0) {
			p = -dx;
			q = x0 - xmin;
		}
		else if (edge == 1) {
			p = dx;
			q = xmax - x0;
		}
		else if (edge == 2) {
			p = -dy;
			q = y0 - ymin;
		}
		else {
			p = dy;
			q = ymax - y0;
		}

		r = q / p;

		if (p == 0 && q < 0) {
			return;
		}

		if (p > 0) {
			if (r < t0) {
				return;
			}
			else if (r < t1) {
				t1 = r;
			}
		}

		if (p < 0) {
			if (r > t0) {
				t0 = r;
			}
			else if (r > t1) {
				return;
			}
		}
	}
	float x_clip = x0 + t0 * dx;
	float y_clip = y0 + t0 * dy;
	float x_clip_end = x0 + t1 * dx;
	float y_clip_end = y0 + t1 * dy;

	dda(x_clip, y_clip, x_clip_end, y_clip_end);
}


void drawWindow() {
	dda(xmin,ymin,xmin,ymax);
	dda(xmin,ymax,xmax,ymax);
	dda(xmax,ymax,xmax,ymin);
	dda(xmax,ymin,xmin,ymin);
}

void keyboard(unsigned char key,int x,int y) {
	if (key == 'r') {
		float red = 1.0,green = 0.0,blue = 0.0;
	}
	glutPostRedisplay();
}


void mouse(int btn,int state,int x,int y) {
	if (btn == GLUT_LEFT_BUTTON) {
		useColor = false;
		if (state == GLUT_DOWN) {
			xi = x;
			yi = wh - y;

		}
		else if (state == GLUT_UP) {
			xf = x;
			yf = wh - y;
			lianBasky(xi,yi,xf,yf);
		}
	}
	else if (btn == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			glClear(GL_COLOR_BUFFER_BIT);
			useColor = true;
			glutPostRedisplay();
			glFlush();
		}
		
	}
}

void display() {
	glClearColor(1.0,1.0,1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//dda(0, 0, 600, 600);
	midpoint(300, 300, 100,blue);
	bresenhamCircle(300, 300,50,red);
	drawWindow();
	glFlush();
}

void init() {
	glViewport(0, 0, ww, wh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)ww, 0.0, (GLdouble)wh);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(ww, wh);
	glutCreateWindow("2022 end");
	glutDisplayFunc(display);
	init();
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}