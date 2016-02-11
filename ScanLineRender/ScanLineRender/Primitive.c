/*
//  Primitive.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/25/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "Primitive.h"
#include <math.h>
#include <string.h>
#include "util.h"

void makeLine(Primitive *o, Color c, const Edge e){
	Primitive tmp;
	Point **const b = SafeMalloc(sizeof(Edge));
	memcpy(b, e, sizeof(Edge));
	INIT_PRIM(tmp, c, 1, b);
	*o = tmp;
}
void makeTri(Primitive *o, Color c, const Edge e1, const Edge e2, const Edge e3){
	Primitive tmp;
	Point **const b = SafeMalloc(4*sizeof(Point*)),
	**bn = b;
	assert(e1[END] == e2[START]
		   && e2[END] == e3[START]
		   && e3[END] == e1[START]
		   && "Not a closed line-loop");
	memcpy(bn++, e1, sizeof(Point*));
	memcpy(bn++, e2, sizeof(Point*));
	memcpy(bn, e3, sizeof(Edge));
	INIT_PRIM(tmp, c, 3, b);
	*o = tmp;
}

void makeQuad(Primitive *o, Color c, const Edge e1, const Edge e2, const Edge e3, const Edge e4){
	Primitive tmp;
	Point **const b = SafeMalloc(5*sizeof(Point*)),
	**bn = b;
	assert(e1[END] == e2[START]
		   && e2[END] == e3[START]
		   && e3[END] == e4[START]
		   && e4[END] == e1[START]
		   && "Not a closed line-loop");
	memcpy(bn++, e1, sizeof(Point*));
	memcpy(bn++, e2, sizeof(Point*));
	memcpy(bn++, e3, sizeof(Point*));
	memcpy(bn, e4, sizeof(Edge));
	INIT_PRIM(tmp, c, 4, b);
	*o = tmp;
}

static float fastinvsqrt( float number );
float fastinvsqrt( float number )
{
	int32_t i;
	float x2, y;
	const float threehalfs = 1.5F;
	
	x2 = number * 0.5F;
	y  = number;
	i  = * ( int32_t * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );
	
	return y;
}

static void clampInBoundary(const Primitive *p, float *x, float *y);

float getZForXY(const Primitive *p, float x, float y){
	Point **const boundary = p->boundary;
	float ret;
	if(p->arity == 1){
		const Point *const vs = boundary[START],
		*const ve = boundary[END];
		
		const float sx = vs->x,
		sy = vs->y,
		sz = vs->z,
		
		dx = ve->x - sx,
		dy = ve->y - sy,
		dz = ve->z - sz;
		
		/* Theoretically these should be the same, but
		 // we'll average for robustness */
		const float xNumer = dz * (x - sx),
		yNumer = dz * (y - sy);
		const float xEst = CLOSE_ENOUGH(dx, 0) ? min(0,  dz) : (xNumer / dx),
		yEst = CLOSE_ENOUGH(dy, 0) ? min(0,  dz) : (yNumer / dy);
		
		ret = sz + (xEst + yEst) / 2;
	} else {
		const Point *const a = boundary[0],
		*const b = boundary[1],
		*const c = boundary[2];
		/* 2 vectors in the plane */
		const float ux = a->x - b->x,
		uy = a->y - b->y,
		uz = a->z - b->z,
		vx = c->x - b->x,
		vy = c->y - b->y,
		vz = c->z - b->z;
		
		/* the normal */
		float nx = uy * vz - uz * vy,
		ny = uz * vx - ux * vz,
		nz = ux * vy - uy * vx,
		ninvm = fastinvsqrt(nx*nx + ny*ny + nz*nz);
		nx /= ninvm;
		ny /= ninvm;
		nz /= ninvm;
		
		clampInBoundary(p, &x, &y);
		{
			/* d coefficient: nx * x + ny * y + nz * z + d = 0 */
			const float minusD = nx * a->x + ny * a->y + nz * a->z,
			numer = (minusD - nx * x - ny * y);
			ret = CLOSE_ENOUGH(nz, 0) ? ((numer > 0) ? HUGE_VAL : -HUGE_VAL) : (numer / nz) ;
		}

	}
	return ret;
}

void clampInBoundary(const Primitive *p, float *x, float *y){
	/*
	 * Points p1, p2, p3 will produce barycentric coordinates l1, l2, l3.
	 * Since the p1p3 edge is the interior edge, the l2 coordinate can be ignored
	 * But we'll swap these, to save arithmetic, to avoid ever computing l2 explicitly.
	 */
	/*
	 * Precondition: p has arity 3 or greater.
	 */
	Point **const boundary = p->boundary;
	float lx = *x, ly = *y;
	size_t i;
	const size_t maxI = p->arity - 2;
	for(i = 0; i <= maxI; i++){
		const Point *const p1 = boundary[0],
		*const p3 = boundary[1],
		*const p2 = boundary[2]; /* This swap is intentional */
		const float
		x1 = p1->x, y1 = p1->y,
		x3 = p3->x, y3 = p3->y,
		x2 = p2->x, y2 = p2->y,
		dY23 = y2 - y3,
		dX32 = x3 - x2,
		dX13 = x1 - x3,
		dXL3 = lx - x3,
		dYL3 = ly - y3,
		dY13 = y1 - y3,
		det = (dY23 * dX13 + dX32 * dY13);
		float l1 = (dY23 * dXL3 + dX32 * dYL3) / det,
		l2 = (dX13 * dYL3 - dY13 * dXL3) / det;
		bool upd = false;
		if(l1 < 0){
			l1 = 0.1; upd = true;
		}
		if(l2 < 0){
			l2 = 0.1; upd = true;
		}
		if(upd){
			float l3 = 1 - l1 - l2;
			lx = l1*x1 + l2*x2 + l3*x3;
			ly = l1*y1 + l2*y2 + l3*y3;
			break;
		}
	}
	*x = lx;
	*y = ly;
}


void transformPrimitive(const Transformation * txForm, const Primitive *p, Primitive *o){
	const TransformationF f = txForm->f;
	const void * state = txForm->state;
	Point **const pBoundary = p->boundary;
	Point **const oBoundary = o->boundary;
	size_t i, iMax = p->arity;
	for(i = 0; i <= iMax; ++i){
		f(pBoundary[i],oBoundary[i],state);
	}
}


int32_t hashPrim(const Primitive *p){
	size_t i, iMax = p->arity; int32_t ret = 0;
	for(i = 0; i <= iMax; ++i){
		ret ^= HASH_POINT(*(p->boundary[i]));
	}
	ret ^= (p->color << 8) | 0xFF;
	return ret;
}