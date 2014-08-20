#include "MouseOptionsMenu.h"

#include "Graphics.h"
#include "Mouse.h"

MouseOptionsMenu* newMouseOptionsMenu() {
	MouseOptionsMenu* menu = (MouseOptionsMenu*) malloc(
			sizeof(MouseOptionsMenu));

	menu->x = 0, menu->y = 0;
	menu->w = 50, menu->h = 20;
	menu->color = WHITE;

	menu->isOpen = 0;

	return menu;
}

void openCloseMouseOptionsMenu(MouseOptionsMenu* menu) {
	if (menu->isOpen)
		menu->isOpen = 0;
	else {
		menu->x = getMouse()->x;
		menu->y = getMouse()->y;

		menu->isOpen = 1;
	}
}

void updateMouseOptionsMenu(MouseOptionsMenu* menu) {

}

void drawMouseOptionsMenu(MouseOptionsMenu* menu) {
	int x = menu->x, y = menu->y;

	drawFilledRectangle(x, y, x + menu->w, y + menu->h, menu->color);
}

void deleteMouseOptionsMenu(MouseOptionsMenu* menu) {
	free(menu);
}
