#include "Task.h"

Task* newTask() {
	Task* task = (Task*) malloc(sizeof(Task));

	task->window = newWindow();

	return task;
}

void updateTask(Task* task) {
	updateWindow(task->window);
}

void drawTask(Task* task) {
	drawWindow(task->window);
}

void deleteTask(Task* task) {
	deleteWindow(task->window);
	free(task);
}
