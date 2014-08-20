#pragma once

typedef struct {
	int x, y;
	int w, h;
} WindowContent;

WindowContent* newWindowContent();
void updateWindowContent(WindowContent* content);
void drawWindowContent(WindowContent* content);
void deleteWindowContent(WindowContent* content);
