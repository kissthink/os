#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "Mouse.h"

#include <math.h>
#include "Graphics.h"
#include "KBC.h"
#include "Utilities.h"

static int hook;

// mouse global variable
Mouse* mouse = NULL;
Mouse mousePreviousState;

Mouse* getMouse() {
	if (!mouse) {
		enableMouse();
		mouse = newMouse();
	}

	return mouse;
}

Mouse* newMouse() {
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->x = 0;
	mouse->y = 0;
	mouse->speedMultiplier = 1.4;

	mouse->hasBeenUpdated = 0;
	mouse->leftButtonReleased = 0;

	mouse->byteBeingRead = 0;

	mouse->draw = 1;

	return mouse;
}

void updateMouse(Mouse* mouse) {
	// read package byte
	readMouse(&mouse->packet[mouse->byteBeingRead]);

	// if this byte is not the package beginning, return and wait for the next byte
	if (mouse->byteBeingRead == 0
			&& !(mouse->packet[mouse->byteBeingRead] & BIT(3)))
		return;

	// incrementing counter of package bytes
	mouse->byteBeingRead++;

	// if the full package has been received
	if (mouse->byteBeingRead > 2) {
		// reset package bytes counter
		mouse->byteBeingRead = 0;

		// process package content
		mouse->xSign = mouse->packet[0] & BIT(4);
		mouse->ySign = mouse->packet[0] & BIT(5);
		mouse->deltaX = mouse->packet[1];
		mouse->deltaY = mouse->packet[2];
		if (mouse->xSign)
			mouse->deltaX |= (-1 << 8);
		if (mouse->ySign)
			mouse->deltaY |= (-1 << 8);

		mouse->leftButtonDown = mouse->packet[0] & BIT(0);
		mouse->rightButtonDown = mouse->packet[0] & BIT(1);
		mouse->middleButtonDown = mouse->packet[0] & BIT(2);
		if (mousePreviousState.leftButtonDown != 0
				&& mouse->leftButtonDown == 0)
			mouse->leftButtonReleased = 1;

		// updating x-coord
		if (mouse->deltaX != 0) {
			mouse->x += mouse->speedMultiplier * mouse->deltaX;

			if (mouse->x < 0)
				mouse->x = 0;
			else if (mouse->x >= getHorResolution())
				mouse->x = getHorResolution() - 1;
		}

		// updating y-coord
		if (mouse->deltaY != 0) {
			mouse->y -= mouse->speedMultiplier * mouse->deltaY;

			if (mouse->y < 0)
				mouse->y = 0;
			else if (mouse->y >= getVerResolution())
				mouse->y = getVerResolution() - 1;
		}

		/*
		 // DEBUGGING BLOCK
		 printf("Mouse status:\n");
		 printf("x: %d\ty: %d\n", mouse->x, mouse->y);
		 printf("LB: %d\tRB: %d\n", mouse->leftButtonDown,
		 mouse->rightButtonDown);
		 printf("LB-released: %d\n", mouse->leftButtonReleased);
		 printf("\n");
		 */

		// saving state
		mousePreviousState = *mouse;

		mouse->hasBeenUpdated = 1;
	}
}

int setDirectPixel(long x, long y, int color) {
	if (x < 0 || y < 0 || x >= getHorResolution() || y >= getVerResolution())
		return -1;

	int id = (x + y * getHorResolution()) * getBytesPerPixel();
	getGraphicsMBuffer()[id] = color & 0xFF;
	getGraphicsMBuffer()[id + 1] = (color >> 8) & 0xFF;

	return 0;
}

int drawDirectLine(long xi, long yi, long xf, long yf, int color) {
	int a = abs(xf - xi);
	int b = abs(yf - yi);

	int invertXY = 0;
	if (b > a) {
		swap(int, xi, yi);
		swap(int, xf, yf);
		swap(int, a, b);
		invertXY = 1;
	}

	if (xf < xi) {
		swap(int, xi, xf);
		swap(int, yi, yf);
	}

	// do not touch this
	int inc2 = 2 * b;
	int d = inc2 - a;	// d = 2*b – a;
	int inc1 = d - a;	// inc1 = 2*(b-a);

	int i, x = xi, y = yi;
	for (i = 0; i <= a; i++, x++) {
		invertXY ? setDirectPixel(y, x, color) : setDirectPixel(x, y, color);

		if (d >= 0)
			yi > yf ? y-- : y++, d += inc1;
		else
			d += inc2;
	}

	return 0;
}

void drawCrosshair(Mouse* mouse) {
	int x = mouse->x, y = mouse->y;
	int size = 3;

	// black borders
	drawDirectLine(x - size, y - 1, x + size, y - 1, BLACK);
	drawDirectLine(x - size, y + 1, x + size, y + 1, BLACK);
	drawDirectLine(x - 1, y - size, x - 1, y + size, BLACK);
	drawDirectLine(x + 1, y - size, x + 1, y + size, BLACK);

	// white inside
	drawDirectLine(x - size, y, x + size, y, WHITE);
	drawDirectLine(x, y - size, x, y + size, WHITE);
}

/*
 void drawArrow(Mouse* mouse) {
 int x = mouse->x, y = mouse->y;
 int size = 8;

 int tx = x + size;
 int ty = y + sqrt(4 * size * size - size * size);

 // black borders
 drawLine(x, y, x, y + 2 * size, BLACK);
 drawLine(x, y, tx, ty, BLACK);
 drawLine(x, y + 2 * size, tx, ty, BLACK);

 // white inside
 }
 */

void drawMouse(Mouse* mouse) {
	flipMBuffer();

	drawCrosshair(mouse);
	//drawArrow(mouse);

	flipDisplay();
	mouse->draw = 0;
}

void deleteMouse(Mouse* mouse) {
	free(mouse);
}

int mouseInside(int x1, int y1, int x2, int y2) {
	return x1 <= getMouse()->x && getMouse()->x <= x2 && y1 <= getMouse()->y
			&& getMouse()->y <= y2;
}

int subscribeMouse() {
	hook = MOUSE_IRQ;
	int bitmask = BIT(hook);

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook) != OK
			|| sys_irqenable(&hook) != OK)
		return -1;

	return bitmask;
}

int unsubscribeMouse() {
	if (sys_irqdisable(&hook) != OK || sys_irqrmpolicy(&hook) != OK)
		return -1;

	return 0;
}

int enableMouse() {
	unsigned long result;

	do {
		LOG("mouse", "trying to enable mouse packets");
		writeToMouse(0xF4);
		readMouse(&result);
	} while (result != ACK);

	return 0;
}

int readMouse(unsigned long* reg) {
	return (sys_inb(DATA_REG, reg) != OK);
}

int writeToMouse(unsigned char byte) {
	if (writeToKBC(STAT_REG, 0xD4) != OK)
		return 1;

	if (writeToKBC(DATA_REG, byte) != OK)
		return 1;

	return 0;
}
