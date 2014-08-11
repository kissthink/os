#pragma once

/** @defgroup mouse mouse
 * @{
 * Functions to manipulate mouse
 */

/// Represents a mouse
typedef struct {
	int x, y;
	int xSign, ySign;
	int deltaX, deltaY;
	double speedMultiplier;

	int leftButtonDown;
	int middleButtonDown;
	int rightButtonDown;

	int hasBeenUpdated;
	int leftButtonReleased;

	int byteBeingRead;
	unsigned long packet[3];

	int draw;
} Mouse;

/**
 * @brief Gets the mouse.
 * If it hasn't been defined yet, it is defined and then returned.
 */
Mouse* getMouse();

Mouse* newMouse();
void updateMouse(Mouse* mouse);
void drawMouse(Mouse* mouse);
void deleteMouse(Mouse* mouse);

int mouseInside(int x1, int y1, int x2, int y2);

int subscribeMouse();
int unsubscribeMouse();

int enableMouse();

int readMouse(unsigned long* reg);
int writeToMouse(unsigned char byte);

/**@}*/
