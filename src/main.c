#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include <stdio.h>
#include <assert.h>
#include "Graphics.h"
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
	initGraphics(0x117);

	OS* os = (OS*) startOS();
	while (!os->done) {
		updateOS(os);

		if (os->draw && !os->done)
			drawOS(os);
	}
	stopOS(os);

	exitGraphics();

	LOG(TAG, "terminating program");

	return 0;
}
