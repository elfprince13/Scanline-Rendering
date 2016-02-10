/*
 //  Transformation.c
 //  ScanLineRender
 //
 //  Created by Thomas Dickerson on 1/25/16.
 //  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
 */

#include "Transformation.h"
#include <math.h>

void identProj(const Point *p, Point *o, const void * discard){
	*o = *p;
}

void orthoProj(const Point *p, Point *o, const Edge * viewportSpan){
	const Point * min = (*viewportSpan)[START];
	const Point * max = (*viewportSpan)[END];
	const float l = min->x,
	r = max->x,
	b = min->y,
	t = max->y,
	n = min->z,
	f = max->z,
	x = (2*p->x - (l + r))/(r-l),
	y = (2*p->y - (t + b))/(t-b),
	z = (-2*p->z + (f + n))/(f-n);
	Point tmp;
	INIT_POINT(tmp, x,y,z);
	*o = tmp;
}


void composeProj(const Point *p, Point *o, const ComposeProj * composition){
	Point tmp;
	composition->r.f(p, &tmp, composition->r.state);
	composition->l.f(&tmp, o, composition->l.state);
}


void ontoProj(const Point *p, Point *o, const OntoProj * state){
	Point tmp;
	const float x = state->zeroCoord == offsetof(Point, x) ? state->planeValue : p->x,
	y = state->zeroCoord == offsetof(Point, y) ? state->planeValue : p->y,
	z = state->zeroCoord == offsetof(Point, z) ? state->planeValue : p->z;
	INIT_POINT(tmp,x,y,z);
	*o = tmp;
}


void scaleProj(const Point *p, Point *o, const float * scale){
	Point tmp;
	INIT_POINT(tmp,
			   *scale * p->x,
			   *scale * p->y,
			   *scale * p->z);
	*o = tmp;
}

void yprToTrans(const YPRTrans* ypr, RotMat o){
	/*
	 * In our coordinates, yaw = y, pitch = x, roll = z
	 * We will compose our rotations as Y_1 X_2 Z_3
	 */
	const float yaw = ypr->yaw,
	pitch = ypr->pitch,
	roll = ypr->roll,
	c1 = cos(yaw),	s1 = sin(yaw),
	c2 = cos(pitch),s2 = sin(pitch),
	c3 = cos(roll),	s3 = sin(roll);
	
	INIT_POINT(o[0], c1*c3 + s1*s2*s3, c3*s1*s2 - c1*s3, c2*s1);
	INIT_POINT(o[1], c2*s3, c2*c3, -s2);
	INIT_POINT(o[2], c1*s2*s3 - c3 * s1, c1*c3*s2 + s1*s3, c1*c2);
}

void rotateTrans(const Point *p, Point *o, const RotMat rot) {
	Point tmp;
	INIT_POINT(tmp,
			   dot(p,rot + 0),
			   dot(p,rot + 1),
			   dot(p,rot + 2));
	*o = tmp;
}

const TransformationF identity = &identProj;
const TransformationF orthographic = (TransformationF)(&orthoProj);
const TransformationF compose = (TransformationF)(&composeProj);
const TransformationF onto = (TransformationF)(&ontoProj);
const TransformationF scale = (TransformationF)(&scaleProj);
const TransformationF rotate = (TransformationF)(&rotateTrans);