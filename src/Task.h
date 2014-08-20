#pragma once

#include "Window.h"

typedef struct {
	char name[50];
	Window* window;
} Task;

Task* newTask();
void updateTask(Task* task);
void drawTask(Task* task);
void deleteTask(Task* task);
