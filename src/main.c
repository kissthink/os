#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include <stdio.h>
#include <assert.h>
#include "Graphics.h"
#include "Mouse.h"
#include "OS.h"
#include "Utilities.h"

FILE* logfd = NULL;

void initLog() {
	logfd = fopen(LOG_PATH, "w");
	assert(logfd != NULL);

	LOG("initLog", "logging successfully initialized");
}

int main(int argc, char **argv) {
	char TAG[30] = "main";

	initLog();
	LOG(TAG, "starting program");

	srand(time(NULL));
	sef_startup();

	/*
	 * -------------------
	 * VESA graphics modes
	 *      16-bit (5:6:5)
	 * -------------------
	 *   320×200 - 0x10E
	 *   640×480 - 0x111
	 *   800×600 - 0x114
	 *  1024×768 - 0x117
	 * 1280×1024 - 0x11A
	 * -------------------
	 */
	initGraphics(0x114);

	OS* os = (OS*) startOS();
	while (!os->done) {
		updateOS(os);

		if (!os->done) {
			if (os->draw)
				drawOS(os);

			if (getMouse()->draw)
				drawMouse(getMouse());
		}
	}
	stopOS(os);

	exitGraphics();

	LOG(TAG, "terminating program");

	return 0;
}
