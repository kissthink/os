#include "Window.h"

#include "Graphics.h"
#include "Mouse.h"

#define defaultWidth 0.7 * getHorResolution()
#define defaultHeight 0.6 * getVerResolution()
#define defaultTitlebarHeight 0.04 * getVerResolution()
#define defaultBorderThickness 0.2 * defaultTitlebarHeight

#define defaultColor SILVER

Window* newWindow() {
	Window* window = (Window*) malloc(sizeof(Window));

	window->x = 10, window->y = 10;
	window->w = defaultWidth, window->h = defaultHeight;
	window->titlebarHeight = defaultTitlebarHeight;
	window->borderThickness = defaultBorderThickness;

	window->state = WINDOWED;
	window->content = newWindowContent();

	return window;
}

void checkMouseHoveringWindowBorders(Window* window) {
	int x = window->x, y = window->y;
	int w = window->w, h = window->h;
	int hovMargin = 3;

	if (!window->beingResized)
		window->hovBorder = NIL;

	if (mouseInside(x, y - hovMargin, x + w, y + hovMargin))
		window->hovBorder = TOP;
	else if (mouseInside(x + w - hovMargin, y, x + w + hovMargin, y + h))
		window->hovBorder = RIGHT;
	else if (mouseInside(x, y + h - hovMargin, x + w, y + h + hovMargin))
		window->hovBorder = BOTTOM;
	else if (mouseInside(x - hovMargin, y, x + hovMargin, y + h))
		window->hovBorder = LEFT;

	// change cursor look
	switch (window->hovBorder) {
	case TOP:
	case BOTTOM:
		altMouseCursor(VERTICAL);
		break;
	case TOP_RIGHT:
	case BOTTOM_LEFT:
		altMouseCursor(DIAGONAL);
		break;
	case RIGHT:
	case LEFT:
		altMouseCursor(HORIZONTAL);
		break;
	case BOTTOM_RIGHT:
	case TOP_LEFT:
		altMouseCursor(ALT_DIAGONAL);
		break;
	default:
		resetMouseCursor();
		break;
	}

	// if mouse press and hold
	if (getMouse()->leftButtonDown && window->hovBorder != NIL
			&& !window->beingResized && !window->beingDragged)
		window->beingResized = 1;
}

int mousePressAndHoldOnTitlebar(Window* window) {
	int x = window->x, y = window->y;
	int xf = x + window->w, yf = y + window->titlebarHeight;

	return getMouse()->leftButtonDown && !window->beingResized
			&& !window->beingDragged && mouseInside(x, y, xf, yf);
}

int mouseReleasedAndWindowWasBeingDragged(Window* window) {
	return (window->beingResized || window->beingDragged)
			&& !getMouse()->leftButtonDown;
}

void updateContent(Window* window) {
	window->content->x = window->x + window->borderThickness;
	window->content->y = window->y + window->titlebarHeight;
	window->content->w = window->w - 2 * window->borderThickness;
	window->content->h = window->h - window->titlebarHeight
			- window->borderThickness;
}

void updateWindow(Window* window) {
	if (!window->beingResized)
		checkMouseHoveringWindowBorders(window);

	if (mousePressAndHoldOnTitlebar(window)) {
		window->clickX = getMouse()->x - window->x;
		window->clickY = getMouse()->y - window->y;

		window->beingDragged = 1;
	}

	if (mouseReleasedAndWindowWasBeingDragged(window)) {
		window->beingResized = 0;
		window->beingDragged = 0;
	}

	if (window->beingDragged) {
		window->x = getMouse()->x - window->clickX;
		window->y = getMouse()->y - window->clickY;
	} else if (window->beingResized) {
		int temp;

		switch (window->hovBorder) {
		case TOP:
			temp = window->y + window->h;
			window->y = getMouse()->y;
			window->h = temp - window->y;

			if (window->content->h < 0) {
				int t = window->titlebarHeight + window->borderThickness;
				window->h = t;
				window->y = temp - t;
			}

			break;
		case RIGHT:
			window->w = getMouse()->x - window->x;

			if (window->content->w < 0)
				window->content->w = 0;

			break;
		case BOTTOM:
			window->h = getMouse()->y - window->y;

			if (window->content->h < 0)
				window->content->h = 0;

			break;
		}
	}

	updateContent(window);
}

void drawWindow(Window* window) {
	int x = window->x, y = window->y;

	if (window->beingDragged)
		drawRectangle(x, y, x + window->w, y + window->h, defaultColor);
	else {
		// frame
		drawFilledRectangle(x, y, x + window->w, y + window->h, defaultColor);
		drawLine(x, y + 1, x + window->w, y + 1, LIGHT_GRAY);
		drawLine(x, y + window->h - 1, x + window->w, y + window->h - 1,
		DARK_GRAY);

		// content
		drawWindowContent(window->content);
	}
}

void deleteWindow(Window* window) {
	deleteWindowContent(window->content);
	free(window);
}
