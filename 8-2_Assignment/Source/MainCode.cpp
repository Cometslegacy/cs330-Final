#include <GLFW\glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <windows.h>
#include <time.h>

using namespace std;

const float DEG2RAD = 3.14159 / 180;

void processInput(GLFWwindow* window);

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE };
enum ONOFF { ON, OFF };



class Brick
{
public:
	float red, green, blue;
	float x, y, width;
	BRICKTYPE brick_type;
	ONOFF onoff;

	Brick(BRICKTYPE bt, float xx, float yy, float ww, float rr, float gg, float bb)
	{
		brick_type = bt; x = xx; y = yy, width = ww; red = rr, green = gg, blue = bb;
		onoff = ON;
	};

	void drawBrick()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);

			glVertex2d(x + halfside, y + halfside);
			glVertex2d(x + halfside, y - halfside);
			glVertex2d(x - halfside, y - halfside);
			glVertex2d(x - halfside, y + halfside);

			glEnd();
		}
	}
	
	void drawPaddle()
	{
		if (onoff == ON)
		{
			double halfside = width / 2;

			glColor3d(red, green, blue);
			glBegin(GL_POLYGON);

			glVertex2d(x + halfside, y + halfside *0.25);
			glVertex2d(x + halfside, y - halfside *0.25);
			glVertex2d(x - halfside, y - halfside *0.25);
			glVertex2d(x - halfside, y + halfside *0.25);

			glEnd();
		}
	}
};


class Circle
{
public:
	float red, green, blue;
	float radius;
	float x;
	float y;
	float speed = 0.03;
	int direction; // 1=up 2=right 3=down 4=left 5 = up right   6 = up left  7 = down right  8= down left
	//ONOFF onoff;

	Circle(double xx, double yy, double rr, int dir, float rad, float r, float g, float b)
	{
		x = xx;
		y = yy;
		radius = rr;
		red = r;
		green = g;
		blue = b;
		radius = rad;
		direction = dir;
	}

	void CheckCollision(Brick* brk)
	{
		if (brk->brick_type == REFLECTIVE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				direction = GetBounceDirection();
				x = x + 0.03;
				y = y + 0.04;
			}
		}
		else if (brk->brick_type == DESTRUCTABLE)
		{
			if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width && y <= brk->y + brk->width))
			{
				brk->onoff = OFF;
				direction = GetBounceDirection();
				speed = speed * 0.5;
			}
		}
	}

	void checkCollisionPaddle(Brick* brk)
	{
		if ((x > brk->x - brk->width && x <= brk->x + brk->width) && (y > brk->y - brk->width*0.25 && y <= brk->y + brk->width*0.25))
		{
			direction = GetBounceDirection();
			y += 0.001;
		}

	}

	void CheckCollisionCircles(Circle* circ)
	{
		float dx = circ->x - x; //difference in x coords
		float dy = circ->y - y; //difference in y coords
		float distance = sqrt(dx * dx + dy * dy);


		if ( distance <= 0.1 )
		{
			//cout << "Collision detected!" << " This Object: " << x << " " << y  << "    dx: " << circ->x << "  dy: " << circ->y << "    Distance: " << distance << "    |    Object: " << circ << endl;
			direction = GetRandomDirection();
			//x = x + 0.005; 
			//y = y + 0.005;
			if (speed < 0.03)
			{
				speed = speed * 1.2;
			}

		}
	}


	int GetRandomDirection()
	{
		return (rand() % 8) + 1;
	}

	int GetBounceDirection()
	{
		return direction = (direction % 8) + 1;
	}

	void MoveOneStep()
	{
		if (direction == 1 || direction == 5 || direction == 6)  // up
		{
			if (y > -1 + radius)
			{
				y -= speed;
			}
			else
			{
				direction = 2;
				speed += speed * 0.1;
				if (speed >= 0.05)
				{
					speed = 0.05;
				}
			}
		}

		if (direction == 2 || direction == 5 || direction == 7)  // right
		{
			if (x < 1 - radius)
			{
				x += speed;
			}
			else
			{
				direction = 3;
				speed += speed * 0.1;
				if (speed >= 0.05)
				{
					speed = 0.05;
				}
			}
		}

		if (direction == 3 || direction == 7 || direction == 8)  // down
		{
			if (y < 1 - radius) {
				y += speed;
			}
			else
			{
				direction = 1;
				speed += speed * 0.1;
				if (speed >= 0.05)
				{
					speed = 0.05;
				}
			}
		}

		if (direction == 4 || direction == 6 || direction == 8)  // left
		{
			if (x > -1 + radius) {
				x -= speed;
			}
			else
			{
				direction = 2;
				speed += speed * 0.1;
				if (speed >= 0.05)
				{
					speed = 0.05;
				}
			}
		}
	}

	void DrawCircle()
	{
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 360; i++) {
			float degInRad = i * DEG2RAD;
			glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
		}
		glEnd();
	}
};


vector<Circle> world;
vector<Brick> bricks;
Brick paddle(REFLECTIVE, 0.0, -1, 0.5, 1, 1, 1);

int main(void) {
	srand(time(NULL));

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow* window = glfwCreateWindow(480, 480, "8-2 Assignment", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//Brick brick(REFLECTIVE, 0.5, -0.33, 0.2, 1, 0, 0);
	//Brick brick2(DESTRUCTABLE, -0.5, 0.33, 0.2, 0, 1, 0);
	//Brick brick3(DESTRUCTABLE, -0.5, -0.33, 0.2, 0, 1, 1);
	//Brick brick4(REFLECTIVE, 0, 0, 0.2, 1, 0.5, 0.5);

	//creating the vector with all the brick placements
	for (int j = 0; j < 4; j++) // j < (number of rows)
	{
		for (int i = 0; i < 10; i++) // i < (number of bricks per row)
		{
			Brick brick(DESTRUCTABLE, (i * 0.25 - 1 + (j % 2) *0.125), 1.0 - (j*0.25), 0.2, 1, j*0.25, 0.1);

			bricks.push_back(brick);

		}

	}


	while (!glfwWindowShouldClose(window)) {
		//Setup View
		float ratio;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		processInput(window);

		//Movement
		for (int i = 0; i < world.size(); i++)
		{
			//world[i].CheckCollision(&brick);
			//world[i].CheckCollision(&brick2);
			//world[i].CheckCollision(&brick3);
			//world[i].CheckCollision(&brick4);
			world[i].checkCollisionPaddle(&paddle);


			for (int k = 0; k < bricks.size(); k++)
			{
				world[i].CheckCollision(&bricks[k]);
			}

			world[i].MoveOneStep();

			for (int j = 0; j < world.size(); j++) {
				if (i == j) continue;
				world[i].CheckCollisionCircles(&world[j]);
			}

			for (int i = bricks.size() - 1; i >= 0; i--)
			{
				if (bricks[i].onoff == OFF) 
				{
					bricks.erase(bricks.begin() + i); 
				}
			}

			world[i].DrawCircle();

		}

		//brick.drawBrick();
		//brick2.drawBrick();
		//brick3.drawBrick();
		//brick4.drawBrick();
		for (int i = 0; i < bricks.size(); i++)
		{
			bricks[i].drawBrick();
		}

		paddle.drawPaddle();
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		double r, g, b;
		r = rand() / 10000;
		g = rand() / 10000;
		b = rand() / 10000;
		Circle B(0, 0, 0.2, 2, 0.05, r, g, b);
		B.direction = 1;
		world.push_back(B);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		paddle.x -= 0.01;
		if (paddle.x < -1)
		{
			paddle.x = -1;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		paddle.x += 0.01;
		if (paddle.x > 1)
		{
			paddle.x = 1;
		}
	}
}