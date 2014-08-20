#include "WindowContent.h"

#include "Graphics.h"

WindowContent* newWindowContent() {
	WindowContent* content = (WindowContent*) malloc(sizeof(WindowContent));

	content->x = 0, content->y = 0;
	content->w = 0, content->h = 0;

	return content;
}

void updateWindowContent(WindowContent* content) {

}

void drawWindowContent(WindowContent* content) {
	int x = content->x, y = content->y;
	int w = content->w, h = content->h;

	drawFilledRectangle(x, y, x + w, y + h, WHITE);
}

void deleteWindowContent(WindowContent* content) {
	free(content);
}
