/*
//  Point.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Point_h
#define Point_h

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct {
	float x; float y; float z;
} Point;

#define INIT_POINT(p, xv, yv, zv)  (((p).x = (xv)), ((p).y = (yv)), ((p).z = (zv)))

#define FLOATBITS(f) (*(int32_t*)&(f))
#define HASH_POINT(p) (FLOATBITS((p).x) ^ FLOATBITS((p).y) ^ FLOATBITS((p).z))

/* Experimentally, this is about the right tolerance */
#define PT_EPS 0.00001
#define CLOSE_ENOUGH(a, b) (fabs((a) - (b)) < PT_EPS)

const Point origin(void);

void negate(Point *);
void negated(const Point *p, Point *o);

bool pointsEqual(const Point *p, const Point *q);

#define DOT(p, q) (((p).x * (q).x) + ((p).y * (q).y) + ((p).z * (q).z));
float dot(const Point *p, const Point *q);

#endif /* Point_h */
