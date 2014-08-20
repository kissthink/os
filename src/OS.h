#pragma once

#include "Bitmap.h"
#include "MouseOptionsMenu.h"
#include "Task.h"
#include "Timer.h"

typedef struct {
	int IRQ_SET_KB;
	int IRQ_SET_MOUSE;
	int IRQ_SET_TIMER;

	int done, draw;
	Timer* timer;
	unsigned long scancode;

	int useBackgroundImage, backgroundColor;
	Bitmap* backgroundImage;

	unsigned int taskbarHeight;
	MouseOptionsMenu* mouseOptionsMenu;
	Task* picViewer;
} OS;

OS* initOS();
void updateOS(OS* os);
void drawOS(OS* os);
void stopOS(OS* os);
