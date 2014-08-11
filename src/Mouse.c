#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "Mouse.h"

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

	mouse->byteBeingRead = 0;

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

void drawMouse(Mouse* mouse) {
	int x = mouse->x, y = mouse->y;
	int size = 3;

	// black borders
	drawLine(x - size, y - 1, x + size, y - 1, BLACK);
	drawLine(x - size, y + 1, x + size, y + 1, BLACK);
	drawLine(x - 1, y - size, x - 1, y + size, BLACK);
	drawLine(x + 1, y - size, x + 1, y + size, BLACK);

	// white inside
	drawLine(x - size, y, x + size, y, WHITE);
	drawLine(x, y - size, x, y + size, WHITE);
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
