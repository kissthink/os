#include "Point.h"

#include <stdio.h>

Point* newPoint(int x, int y) {
	Point* p = (Point*) malloc(sizeof(Point));

	p->x = x;
	p->y = y;

	return p;
}

int equalPoints(Point* p1, Point* p2) {
	return p1->x == p2->x && p1->y == p2->y;
}

void setPointLocation(Point* p, int x, int y) {
	p->x = x;
	p->y = y;
}

void deletePoint(Point* p) {
	free(p);
}
