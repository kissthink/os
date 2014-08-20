#include <minix/drivers.h>
#include "OS.h"

#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Utilities.h"

const int FPS = 25;
const int mouseFPSmult = 3;

OS* startOS() {
	LOG("OS", "starting");
	OS* os = (OS*) malloc(sizeof(OS));

	// subscribing devices
	os->IRQ_SET_KB = kb_subscribe_int();
	os->IRQ_SET_MOUSE = subscribeMouse();
	os->IRQ_SET_TIMER = subscribeTimer();

	// resetting timer frequency
	timerSetSquare(0, mouseFPSmult * FPS);

	// initialize other stuff
	os->useBackgroundImage = 1;
	os->backgroundColor = TEAL;
	os->backgroundImage = loadBitmap("/home/os/res/pictures/03-800x600.bmp");

	os->taskbarHeight = 0.05 * getVerResolution();
	os->mouseOptionsMenu = newMouseOptionsMenu();
	os->picViewer = newTask();

	// finish os initialization
	os->done = 0, os->draw = 1;
	os->timer = newTimer();
	os->scancode = 0;

	return os;
}

void updateOS(OS* os) {
	int ipc_status, r = 0;
	message msg;

	resetTimerTickedFlag(os->timer);
	resetMouseFlags();

	if (driver_receive(ANY, &msg, &ipc_status) != 0) {
		LOG("udateOS", "driver_receive failed");
		return;
	}

	if (is_ipc_notify(ipc_status)) {
		switch (_ENDPOINT_P(msg.m_source)) {
		case HARDWARE:
			// TIMER interruption
			if (msg.NOTIFY_ARG & os->IRQ_SET_TIMER)
				timerHandler(os->timer);

			// KEYBOARD interruption
			if (msg.NOTIFY_ARG & os->IRQ_SET_KB)
				os->scancode = readKBCState();

			// MOUSE interruption
			if (msg.NOTIFY_ARG & os->IRQ_SET_MOUSE)
				updateMouse(getMouse());

			break;
		default:
			break;
		}
	}

	if (os->scancode != 0 || getMouse()->hasBeenUpdated) {
		if (os->scancode == KEY_ESC)
			os->done = 1;

		if (getMouse()->rightButtonReleased)
			; //openCloseMouseOptionsMenu(os->mouseOptionsMenu);
	}

	if (os->timer->ticked) {
		getMouse()->draw = 1;

		if (os->timer->counter % mouseFPSmult == 0) {
			if (os->useBackgroundImage && !os->backgroundImage)
				os->useBackgroundImage = 0;

			updateTask(os->picViewer);

			os->draw = 1;
		}
	}
}

void drawBackground(OS* os) {
	if (os->useBackgroundImage)
		drawBitmap(os->backgroundImage, 0, 0, ALIGN_LEFT);
	else
		fillDisplay(os->backgroundColor);
}

void drawOS(OS* os) {
	drawBackground(os);

	// tasks
	drawTask(os->picViewer);

	// taskbar
	int yi = getVerResolution() - os->taskbarHeight;
	int yf = getVerResolution();
	drawFilledRectangle(0, yi, getHorResolution(), yf, SILVER);
	drawLine(0, yi + 1, getHorResolution(), yi + 1, LIGHT_GRAY);

	// mouse options menu
	if (os->mouseOptionsMenu->isOpen)
		drawMouseOptionsMenu(os->mouseOptionsMenu);

	flipDisplay();
	os->draw = 0;
}

void stopOS(OS* os) {
	// unsubscribe devices
	kb_unsubscribe_int();
	unsubscribeMouse();
	unsubscribeTimer();

	deleteTimer(os->timer);
	deleteMouse(getMouse());

	free(os);
}
