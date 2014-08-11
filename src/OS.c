#include <minix/drivers.h>
#include "OS.h"

#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Utilities.h"

OS* startOS() {
	LOG("OS", "starting");

	OS* os = (OS*) malloc(sizeof(OS));

	// subscribing devices
	os->IRQ_SET_KB = kb_subscribe_int();
	os->IRQ_SET_MOUSE = subscribeMouse();
	// game->IRQ_SET_TIMER = timer_subscribe_int();

	// resetting frequency
	//timer_set_square(0, FPS);

	os->done = 0;
	os->draw = 1;

	//os->currentState = MainMenuState;
	//game->state = newMainMenuState();

	//game->timer = newTimer();

	return os;
}

void updateOS(OS* os) {
	int ipc_status, r = 0;
	message msg;

	unsigned long scancode = 0;
	//os->timer->ticked = 0;

	if (driver_receive(ANY, &msg, &ipc_status) != 0) {
		LOG("udateOS", "driver_receive failed");
		return;
	}

	if (is_ipc_notify(ipc_status)) {
		switch (_ENDPOINT_P(msg.m_source)) {
		case HARDWARE:
			// TIMER interruption
			//if (msg.NOTIFY_ARG & game->IRQ_SET_TIMER)
			//timerHandler(game->timer);

			// KEYBOARD interruption
			if (msg.NOTIFY_ARG & os->IRQ_SET_KB)
				scancode = readKBCState();

			// MOUSE interruption
			if (msg.NOTIFY_ARG & os->IRQ_SET_MOUSE)
				updateMouse(getMouse());

			break;
		default:
			break;
		}
	}

	if (scancode != 0 || getMouse()->hasBeenUpdated) {
		if (scancode == KEY_DOWN(KEY_ESC))
			os->done = 1;

		os->draw = 1;
	}
}

void drawOS(OS* os) {
	fillDisplay(WHITE);
	//fillDisplay(BLUE);

	int r = distanceBetweenPoints(50, 50, getMouse()->x, getMouse()->y);
	drawFilledCircle(50, 50, r, BLUE);
	drawMouse(getMouse());

	flipDisplay();
	os->draw = 0;
}

void stopOS(OS* os) {
	//deleteCurrentState(os);

	// unsubscribing devices
	kb_unsubscribe_int();
	unsubscribeMouse();
	//timer_unsubscribe_int();

	deleteMouse(getMouse());
	//deleteTimer(game->timer);
	//deleteFont(getDefaultFont());

	free(os);
}
