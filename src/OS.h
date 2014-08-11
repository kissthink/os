#pragma once

#include "Timer.h"

typedef struct {
	int IRQ_SET_KB;
	int IRQ_SET_MOUSE;
	int IRQ_SET_TIMER;

	int done, draw;
	Timer* timer;
	unsigned long scancode;

	unsigned int taskbarHeight;
} OS;

OS* initOS();
void updateOS(OS* os);
void drawOS(OS* os);
void stopOS(OS* os);
