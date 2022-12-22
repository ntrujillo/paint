#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib> 
#include <ctime>


#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define BRUSH_MAX 128
#define BRUSH_MIN 1


float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH];
GLFWwindow *window;

enum brush_shape {
	square,
	circle
};

// Color structure. Can be used to define the brush and background color.
struct color { float r, g, b; };

// global configurations
int brushSize = 1;
brush_shape brushShape = square;
bool brushSprayPaint = false;
double brushSprayPaintPercentage = 75;
color brushColor = {
	255, // red
	255, // blue
	255  // green
};
color backgroundColor = {
	0, // red
	0, // blue
	0  // green
};

// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

// Set a particular pixel of the frameBuffer to the provided color
void SetFrameBufferPixel(int x, int y, struct color lc)
{
	// Origin is upper left corner. 
	// Changes the origin from the lower-left corner to the upper-left corner
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	frameBuffer[y][x][0] = lc.r;
	frameBuffer[y][x][1] = lc.g;
	frameBuffer[y][x][2] = lc.b;

}

void ClearFrameBuffer()
{
	memset(frameBuffer, 0.0f, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);
}

// Display frameBuffer on screen
void Display()
{	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
}

/* 
* Given mouse position, brush size, and brush shape, 
* this function will update frame buffer with the new colors of brush.
*/
void DrawBrush(int xpos, int ypos)
{
	// iterate through each pixel in brush region
	if (brushShape == square) { // square brush shape
		
		for (int y = (ypos - brushSize); y < (ypos + brushSize); y++) {
			for (int x = (xpos - brushSize); x < (xpos + brushSize); x++) {
				if (!brushSprayPaint) {
					// update color in frame buffer
					SetFrameBufferPixel(x, y, brushColor);
					// update mask
					mask[y][x] = true;

				} else { // spray paint enabled
					double numberRoll = rand() % 100 + 1;
					if (numberRoll < brushSprayPaintPercentage) {
						// update color in frame buffer
						SetFrameBufferPixel(x, y, brushColor);
						// update mask
						mask[y][x] = true;
					}
				}
			}
		}
	} else { // cirlce brush shape
		// iterate through square
		for (int y = (ypos - brushSize); y < (ypos + brushSize); y++) {
			for (int x = (xpos - brushSize); x < (xpos + brushSize); x++) {
				if (!brushSprayPaint) {
					// if pixel is within radius (using distance formula)
					if ((sqrt(pow((x-xpos), 2) + pow((y-ypos), 2))) <= brushSize) {
						// update color in frame buffer
						SetFrameBufferPixel(x, y, brushColor);
						// update mask
						mask[y][x] = true;
					}
				} else { // spray paint enabled
					double numberRoll = rand() % 100 + 1; 
					if (numberRoll < brushSprayPaintPercentage) {
						if ((sqrt(pow((x-xpos), 2) + pow((y-ypos), 2))) <= brushSize) {
							// update color in frame buffer
							SetFrameBufferPixel(x, y, brushColor);
							// update mask
							mask[y][x] = true;
						}
					}
				}
			}
		}
	}
}

void InitBrushSprayPaint() {
	brushSprayPaint = !brushSprayPaint;
	srand(time(NULL));
	brushSprayPaintPercentage = (rand() % 45 + 15); 
}

/*
* Given an RGB value
* change the color of background
* and preserve existing drawings
*/
void ChangeBackgroundColor() {

	// iterate through mask
	for (int y = 0; y < WINDOW_HEIGHT; y++) {
		for (int x = 0; x < WINDOW_WIDTH; x++) {

			// if pixel has not been updated
			if (mask[y][x] == false) {
				// update pixel color in frame buffer
				SetFrameBufferPixel(x, y, backgroundColor);
			}

		}
	}
}

void ClearDrawing() {
	for (int y = 0; y < WINDOW_HEIGHT; y++) {
		for (int x = 0; x < WINDOW_WIDTH; x++) {
			// update frame buffer
			SetFrameBufferPixel(x, y, backgroundColor);
			// update mask
			mask[y][x] = false;
		}
	}
}

// Mouse position callback function
void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
	int leftMouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (leftMouse_state == GLFW_PRESS)
	{
		DrawBrush(xpos, ypos);
	}
}

// Mouse callback function
void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		ClearDrawing();
		std::cout << "Drawing cleared." << std::endl;
	}
}

// You can use "switch" or "if" to compare key to a specific character.
// for example,
// if (key == '0')
//     DO SOMETHING

// Keyboard callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	int temp = 0;
	switch (key) {

	// brush shape options (toggle between square and cirlce)
	case 'b':
		if (brushShape == square) {
			brushShape = circle;
		} else {
			brushShape = square;
		}
		std::cout << "Brush shape toggled." << std::endl;
		break;

	// spray paint options
	case 's':
		InitBrushSprayPaint();
		std::cout << "Spray paint brush toggled." << std::endl;
		break;

	// brush size options
	case '+':
		temp = brushSize * 2;
		brushSize = std::min(BRUSH_MAX, temp);
		std::cout << "Brush size increased to " << brushSize << "." << std::endl;
		break;
	case '-':
		temp = brushSize / 2;
		brushSize = std::max(BRUSH_MIN, temp);
		std::cout << "Brush size decreased to " << brushSize << "." << std::endl;
		break;

	// brush color options
	case '0': // black
		brushColor = {0, 0, 0};
		std::cout << "Brush color changed to black." << std::endl;
		break;
	case '1': // blue
		brushColor = {0, 0, 255};
		std::cout << "Brush color changed to blue." << std::endl;
		break;
	case '2': // green
		brushColor = {0, 255, 0};
		std::cout << "Brush color changed to green." << std::endl;
		break;
	case '3': // cyan
		brushColor = {0, 255, 255};
		std::cout << "Brush color changed to cyan." << std::endl;
		break;
	case '4': // red
		brushColor = {255, 0, 0};
		std::cout << "Brush color changed to red." << std::endl;
		break;
	case '5': // magenta
		brushColor = {255, 0, 255};
		std::cout << "Brush color changed to magenta." << std::endl;
		break;
	case '6': // yellow
		brushColor = {255, 255, 0};
		std::cout << "Brush color changed to yellow." << std::endl;
		break;
	case '7': // white
		brushColor = {255, 255, 255};
		std::cout << "Brush color changed to white." << std::endl;
		break;

	// background color options
	case ')': // black
		backgroundColor = {0, 0, 0};
		ChangeBackgroundColor();
		std::cout << "Background color changed to black." << std::endl;
		break;
	case '!': // blue
		backgroundColor = {0, 0, 255};
		ChangeBackgroundColor();
		std::cout << "Background color changed to blue." << std::endl;
		break;
	case '@': // green
		backgroundColor = {0, 255, 0};
		ChangeBackgroundColor();
		std::cout << "Background color changed to green." << std::endl;
		break;
	case '#': // cyan
		backgroundColor = {0, 255, 255};
		ChangeBackgroundColor();
		std::cout << "Background color changed to cyan." << std::endl;
		break;
	case '$': // red
		backgroundColor = {255, 0, 0};
		ChangeBackgroundColor();
		std::cout << "Background color changed to red." << std::endl;
		break;
	case '%': // magenta
		backgroundColor = {255, 0, 255};
		ChangeBackgroundColor();
		std::cout << "Background color changed to magenta." << std::endl;
		break;
	case '^': // yellow
		backgroundColor = {255, 255, 0};
		ChangeBackgroundColor();
		std::cout << "Background color changed to yellow." << std::endl;
		break;
	case '&': // white
		backgroundColor = {255, 255, 255};
		ChangeBackgroundColor();
		std::cout << "Background color changed to white." << std::endl;
		break;
	}
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Paint Tool - Nathaniel Trujillo", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glewExperimental = GL_TRUE;
	glewInit();
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffer();
}


int main()
{	
	Init();
	while (glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}