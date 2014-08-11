#include "Utilities.h"

#include <math.h>

int fileExists(const char* filename) {
	FILE* file = fopen(filename, "r");

	if (file) {
		fclose(file);
		return 1;
	}

	return 0;
}

int distanceBetweenPoints(int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;

	return sqrt(dx * dx + dy * dy);
}
