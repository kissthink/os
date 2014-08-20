#pragma once

typedef struct {
	int x, y;
	int w, h;
	int color;

	int isOpen;
	int draw;
} MouseOptionsMenu;

MouseOptionsMenu* newMouseOptionsMenu();
void openCloseMouseOptionsMenu(MouseOptionsMenu* menu);
void updateMouseOptionsMenu(MouseOptionsMenu* menu);
void drawMouseOptionsMenu(MouseOptionsMenu* menu);
void deleteMouseOptionsMenu(MouseOptionsMenu* menu);
