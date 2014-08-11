#pragma once

/**
 * A point representing a location in (x,y) coordinate space, specified in integer precision.
 */
typedef struct {
	int x, y;
} Point;

/**
 * Constructs and initializes a point at the specified (x,y) location in the coordinate space.
 */
Point* newPoint(int x, int y);

/**
 * Determines whether or not two points are equal.
 */
int equalPoints(Point* p1, Point* p2);

/**
 * Moves this point to the specified location in the (x,y) coordinate plane.
 */
void setPointLocation(Point* p, int x, int y);

/**
 * Deletes the specified point.
 */
void deletePoint(Point* p);
