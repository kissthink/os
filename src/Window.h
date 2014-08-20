#pragma once

#include "WindowContent.h"

typedef enum {
	MINIMIZED, WINDOWED, MAXIMIZED
} WindowState;

typedef enum {
	NIL,
	TOP,
	TOP_RIGHT,
	RIGHT,
	BOTTOM_RIGHT,
	BOTTOM,
	BOTTOM_LEFT,
	LEFT,
	TOP_LEFT
} BorderID;

typedef struct {
	int x, y;
	int w, h;
	int titlebarHeight;
	int borderThickness;

	int clickX, clickY;
	int beingDragged;
	int beingResized;

	BorderID hovBorder;
	WindowState state;
	WindowContent* content;
} Window;

Window* newWindow();
void updateWindow(Window* window);
void drawWindow(Window* window);
void deleteWindow(Window* window);
