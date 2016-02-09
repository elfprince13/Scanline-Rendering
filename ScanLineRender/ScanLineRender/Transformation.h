/*
//  Transformation.h
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#ifndef Transformation_h
#define Transformation_h

#include "Point.h"
#include "Edge.h"

typedef void(*TransformationF)(const Point *p, Point *o, const void * state);
typedef struct _Transformation {
	TransformationF f;
	void * state;
} Transformation;

typedef struct {
	Transformation l;
	Transformation r;
} ComposeProj;

typedef struct {
	size_t zeroCoord;
	float planeValue;
} OntoProj;

typedef struct {
	float yaw;
	float pitch;
	float roll;
} YPRTrans;

typedef Point RotMat[3];

void identProj(const Point *p, Point *o, const void *);
void orthoProj(const Point *p, Point *o, const Edge * viewportSpan);
void composeProj(const Point *p, Point *o, const ComposeProj * composition);
void ontoProj(const Point *p, Point *o, const OntoProj * zOff);
void scaleProj(const Point *p, Point *o, const float * scale);

void yprToTrans(const YPRTrans*, RotMat);
void rotateTrans(const Point *p, Point *o, const RotMat rot);

extern const TransformationF identity;
extern const TransformationF orthographic;
extern const TransformationF compose;
extern const TransformationF onto;
extern const TransformationF scale;
extern const TransformationF rotate;


#endif /* Transformation_h */
