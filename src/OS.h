#pragma once

typedef enum {
	MainMenuState, PlayState
} State;

typedef struct {
	int IRQ_SET_KB;
	int IRQ_SET_MOUSE;
	int IRQ_SET_TIMER;

	int done, draw;

	State currentState;
	void* state;
} OS;

OS* initOS();
void updateOS(OS* os);
void drawOS(OS* os);
void stopOS(OS* os);
