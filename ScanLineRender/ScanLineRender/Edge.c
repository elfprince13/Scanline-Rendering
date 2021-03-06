/*
//  Edge.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Edge.h"
#include "Transformation.h"

#include "debugConfig.h"


void flip(Edge e){
	Point *tmp = e[START];
	e[START] = e[END];
	e[END] = tmp;
}

void flipped(const Edge e, Edge o){
	o[START] = e[END];
	o[END] = e[START];
}

float dotEdge(const Edge u, const Edge v){
	const Point
	*us = u[START],
	*ue = u[END],
	*vs = v[START],
	*ve = v[END], *tmp;
	Point u0, v0;
	
	bool tailTouch = pointsEqual(ue, ve);
	if (pointsEqual(us, ve) || tailTouch) {
		tmp = ve;
		ve = vs;
		vs = tmp;
	}
	if (pointsEqual(ue, vs) || tailTouch) {
		tmp = ue;
		ue = us;
		us = tmp;
	}
	
	assert(pointsEqual(us, vs));
 
	INIT_POINT(u0,
			   ue->x - us->x,
			   ue->y - us->y,
			   ue->z - us->z);
	
	INIT_POINT(v0,
			   ve->x - vs->x,
			   ve->y - vs->y,
			   ve->z - vs->z);
	
	return DOT(u0, v0);
}

void transformEdge(const Transformation * txForm, const Edge e, Edge o){
	const TransformationF f = txForm->f;
	const void * state = txForm->state;
	size_t j;
	for (j = START; j <= END; ++j) {
		f(e[j],o[j], state);
	}
}


bool contains(const Edge e, const Point *p){
	size_t i;
	bool ret = false;
	for(i = START; i <= END; ++i ){
		const Point* coord = e[i];
		if(CLOSE_ENOUGH(coord->x, p->x) && CLOSE_ENOUGH(coord->y, p->y) && CLOSE_ENOUGH(coord->z, p->z)){
			ret = true; break;
		}
	}
	return ret;
}