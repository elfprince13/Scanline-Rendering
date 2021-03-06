/*
//  ScanlineRenderer.c
//  ScanLineRender
//
//  Created by Thomas Dickerson on 1/27/16.
//  Copyright © 2016 StickFigure Graphic Productions. All rights reserved.
*/

#include "AEL.h"
#include "ScanlineRenderer.h"
#include "link_list/linklist.h"
#include "debugConfig.h"
#include <math.h>

int roundOwn(double x);

typedef int(*StatelessCompF)(const void*,const void*) ;

/* We really ought to re-type this if possible */
static int topToBottom(const Primitive *, const Primitive *);
static int pointerDiff(const Primitive*, const Primitive*);
static StatelessCompF topToBottomF = (StatelessCompF)(&topToBottom);
static StatelessCompF pointerDiffF = (StatelessCompF)(&pointerDiff);
static float topMostPoint(const Primitive *);
static float bottomMostPoint(const Primitive *);
#ifndef NDEBUG
static float rightMostPoint(const Primitive *);
static float leftMostPoint(const Primitive *);
#endif

void transformData(const Transformation *txForm, const Point *srcGeometry, Point *dstGeometry, size_t pointCount){
	const TransformationF f = txForm->f;
	const void* state = txForm->state;
	size_t i;
	
	for(i = 0; i < pointCount; ++i){
		f(srcGeometry + i, dstGeometry + i, state);
		dPrintf(("(%f, %f, %f) -> (%f, %f, %f))\n",
				 srcGeometry[i].x,
				 srcGeometry[i].y,
				 srcGeometry[i].z,
				 dstGeometry[i].x,
				 dstGeometry[i].y,
				 dstGeometry[i].z));
	}
}

rb_red_blk_tree* bucketPrims(rb_red_blk_tree* buckets, int numLines, Primitive *geometry, size_t geomCount){
	size_t i;
	if(!buckets){
		buckets = SafeMalloc(numLines * sizeof(rb_red_blk_tree));
		for(i = 0; i < numLines; RBTreeInit(buckets + (i++), pointerDiffF, NULL, &RBNodeAlloc));
	} else {
		for(i = 0; i < numLines; RBTreeClear(buckets + (i++)));
	}
	
	qsort(geometry, geomCount, sizeof(Primitive), topToBottomF);
	for(i = 0; i < geomCount; ++i){
		Primitive *prim = geometry + i;
		const int pScanLine = roundOwn(bottomMostPoint(prim));
		if(pScanLine < numLines && (pScanLine >= 0 || topMostPoint(prim) >= 0)){
			rb_red_blk_tree *const dstBucket = buckets + max(0, pScanLine);
			RBSetAdd(dstBucket, prim);
			dPrintf(("dstBucket " SZF " gets geometry with arity " SZF " begins on %f and ends on %f and now has size " SZF "\n", dstBucket - buckets, prim->arity, bottomMostPoint(prim), topMostPoint(prim), dstBucket->size));
		}
	}
	
	return buckets;
}

rb_red_blk_tree* teardownBuckets(rb_red_blk_tree* buckets, int numLines){
	size_t i;
	for (i = 0; i < numLines; RBTreeDestroy(buckets + (i++), false));
	free(buckets);
	return NULL;
}

void render(PaletteRef *raster, int lineWidth, int numLines, const rb_red_blk_tree *scanLinePrimBuckets){
	static OntoProj screenPlaneData = {offsetof(Point, z), 0};
	static const Transformation screenPlane = {(TransformationF)(&snapOntoProj), &screenPlaneData};
	{
		int line;
		rb_red_blk_tree activePrimSet;
		ActiveEdgeList ael = freshAEL();
		rb_red_blk_map_tree inFlags;
		rb_red_blk_tree deFlags;
		/* This ensures that both trees are initialized and in a cleared state */
		RBTreeMapInit(&inFlags, pointerDiffF, NULL, &RBMapNodeAlloc, NULL);
		RBTreeInit(&deFlags, pointerDiffF, NULL, &RBNodeAlloc);
		RBTreeInit(&activePrimSet, pointerDiffF, NULL, &RBNodeAlloc);
		dPrintf(("Scanning line: 0\n"));
		for(line = 0; line < numLines; (++line), (raster += lineWidth)) {
			rb_red_blk_node *primIt, *p = NULL, *nextP;
			dPrintf(("\tUpdating activePrimSet\n"));
			for (primIt = activePrimSet.first; primIt != activePrimSet.sentinel; (p = primIt), (primIt = nextP)) {
				const Primitive* prim = primIt->key;
				const int top = roundOwn(topMostPoint(prim));
				nextP = TreeSuccessor(&activePrimSet, primIt);
				if(top < line){
#ifndef NDEBUG
					{
						const int bottom = roundOwn(bottomMostPoint(prim));
						dPrintf(("\t\t%d -> %d ( %s ) is not valid here: %d\n",top,bottom,fmtColor(prim->color), line));
					}
#endif
					RBDelete(&activePrimSet, primIt);
					primIt = p; /* We don't want to advance p into garbage data */
				}
			}
			{
				const rb_red_blk_tree *bucket = scanLinePrimBuckets + line;
				const rb_red_blk_node *node;
				for(node = bucket->first; node != bucket->sentinel; node = TreeSuccessor(bucket, node)) {
					Primitive * prim = node->key;
#ifndef NDEBUG
					{
						const int top = roundOwn(topMostPoint(prim)),
						bottom = roundOwn(bottomMostPoint(prim));
						dPrintf(("\t\t%d -> %d ( %s ) is added here: %d\n",top,bottom,fmtColor(prim->color), line));
					}
#endif
					RBTreeInsert(&activePrimSet, prim);
				}
			}
			stepEdges(&ael, &activePrimSet);
			{
				int curPixel = 0;
				const Primitive *curDraw = NULL;
				EdgeListEntry *nextEdge;
				LinkN* i = ael.activeEdges;
				if(i){
					nextEdge = i->data;
					while(nextEdge && curPixel < lineWidth){
						EdgeListEntry *const startEdge = nextEdge;
						Primitive *const startOwner = startEdge->owner;
						int startX = roundOwn(getSmartXForLine(startEdge, line)), nextX;
						rb_red_blk_map_node *inFlag = (rb_red_blk_map_node *)RBExactQuery((rb_red_blk_tree*)(&inFlags), startOwner);
						
						if(inFlag){
							static Point localPoints[6]; /* We don't recurse, so this is fine */
							static Edge flatHere = {localPoints, localPoints + 1},
							flatIn = {localPoints + 2, localPoints + 3},
							vert = {localPoints + 4, localPoints + 5};
							const EdgeListEntry *const edgeInEntry = inFlag->info;
							Point **const edgeHere = startEdge->edge, **edgeIn = edgeInEntry->edge;
							const Point *const s = edgeHere[START],
							*const e = edgeHere[END];
							Point here;
							bool sV, eV, v;
							float dotH, dotIn;
							transformEdge(&screenPlane, edgeHere, flatHere);
							transformEdge(&screenPlane, edgeIn, flatIn);
							INIT_POINT(here, startX, line, 0);
							sV = contains(edgeIn, s);
							eV = contains(edgeIn, e);
							v = (sV || eV) && contains(flatIn, &here) && contains(flatHere, &here) && (startOwner->arity != 1);
							vert[START] = &here;
							INIT_POINT(*(vert[END]), startX, line+1, 0);
							dotH = v ? dotEdge(vert, flatHere) : 0;
							dotIn = v ? dotEdge(vert, flatIn) : 0;
							if(!v || dotH * dotIn > 0){
								dPrintf(("\tNot *in* old %s at %f\n", fmtColor(startEdge->owner->color), getSmartXForLine(startEdge, line)));
								RBSetAdd(&deFlags, startOwner);
							} else {
								dPrintf(("\tFound horizontal vertex %s at %f. Don't delete it yet\n",fmtColor(startEdge->owner->color), getSmartXForLine(startEdge, line)));
							}
						} else {
							dPrintf(("\tNow *in* new %s at %f\n",fmtColor(startEdge->owner->color), getSmartXForLine(startEdge, line)));
							/* This might happen if a polygon is parallel to the x-axis */
							RBMapPut(&inFlags, startOwner, startEdge);
						}
						
						if(curPixel < startX){
							dPrintf(("\tcurPixel has fallen behind, dragging from %d to %d\n",curPixel, startX));
							curPixel = startX;
						}
						
						i = i->tail;
						if(i){
							nextEdge = i->data;
							nextX = roundOwn(getSmartXForLine(nextEdge, line));
							dPrintf(("\tNext edges @ x = %d from %s\n",nextX, fmtColor(nextEdge->owner->color)));
						} else {
							dPrintf(("\tNo more edges\n"));
							nextEdge = NULL;
							nextX = 0;
						}
						
						nextX = min(nextX, lineWidth);
						while ((!nextEdge && curPixel < lineWidth) || (curPixel < nextX)) {
							bool zFight = false, solitary = false;
							float bestZ = HUGE_VAL;
							const rb_red_blk_node *node;
							curDraw = NULL;
							dPrintf(("\tTesting depth:\n"));
							for(node = inFlags.tree.first; node != inFlags.tree.sentinel; node = TreeSuccessor((rb_red_blk_tree*)(&inFlags), node)) {
								const Primitive *prim = node->key;
								/* We need sub-pixel accuracy */
								const float testZ = getZForXY(prim, curPixel, line);
								if(testZ <= bestZ + PT_EPS){
									dPrintf(("\t\tHit: %f <= %f for %s\n",testZ, bestZ, fmtColor(prim->color)));
									if (CLOSE_ENOUGH(testZ, bestZ)) {
										if (prim->arity == 1) {
											zFight = curDraw && curDraw->arity == 1;
											curDraw = prim;
											solitary = RBSetContains(&deFlags, prim);
										} else {
											zFight = curDraw && curDraw->arity != 1;
										}
									} else {
										zFight = false;
										bestZ = testZ;
										curDraw = prim;
										solitary = RBSetContains(&deFlags, prim);
									}
								} else {
									dPrintf(("\t\tMiss: %f > %f for %s\n",testZ, bestZ, fmtColor(prim->color)));
								}
							}
							
							if(curDraw){
#ifndef NDEBUG
								if(nextEdge || solitary){
#endif
									const int drawWidth =  (zFight || solitary) ? 1 : ((nextEdge ? nextX : lineWidth) - curPixel),
									stopPixel = curPixel + min(lineWidth - curPixel,
															   max(0, drawWidth));
									const PaletteRef drawColor = /*(uint16_t)roundOwn(63 * bestZ / 100) << 5;*/decodeColor(curDraw->color);
									dPrintf(("Drawing %d @ (%d, %d)\n",drawWidth,curPixel,line));
									dPrintf(("Drawing %d @ (%d, %d)\n",stopPixel - curPixel,curPixel,line));
									while(curPixel < stopPixel){
										raster[curPixel++] = drawColor;
									}
#ifndef NDEBUG
								} else {
									dPrintf(("Warning: we probably shouldn't have to draw if there are no more edges to turn us off. Look for parity errors\n");
											RBTreeClear((rb_red_blk_tree*)&inFlags));
								}
#endif
							} else if(!inFlags.tree.size && nextEdge){
								/* fast forward, we aren't in any polys */
								dPrintf(("Not in any polys at the moment, fast-forwarding(1) to %d\n", nextX));
								curPixel = nextX;
							} else {
								/* Nothing left */
								dPrintf(("Nothing to draw at end of line\n"));
								curPixel = lineWidth;
							}
							
							for(node = deFlags.first; node != deFlags.sentinel; node = TreeSuccessor(&deFlags, node)){
								RBMapRemove(&inFlags, node->key);
							}
							RBTreeClear(&deFlags);
						}
						if (!inFlags.tree.size && nextEdge) {
							dPrintf(("Not in any polys at the moment, fast-forwarding(2) to %d\n", nextX));
							curPixel = nextX;
						}
					}
				}
			}
#ifndef NDEBUG
			{
				dPrintf(("Scanning line: %d\n", line+1));
				if(inFlags.tree.size){
					rb_red_blk_node *node;
					dPrintf(("\tGarbage left in inFlags:\n"));
					for (node = inFlags.tree.first; node != inFlags.tree.sentinel; node = TreeSuccessor((rb_red_blk_tree*)&inFlags, node)) {
						dPrintf(("\t\t%s\n",fmtColor(((const Primitive*)node->key)->color)));
					}
				}
			}
#endif
			RBTreeClear(&deFlags);
			RBTreeClear((rb_red_blk_tree*)(&inFlags));
		}
		RBTreeDestroy(&activePrimSet, false);
		RBTreeDestroy(&deFlags, false);
		RBTreeDestroy((rb_red_blk_tree*)(&inFlags), false);
	}
}

int roundOwn(double x){
	double intpart,
	fractpart = modf(x, &intpart),
	delta = ((fabs(fractpart) < 0.5) ? 0 : 1) * ((x < 0) ? -1 : 1);
	return intpart + delta;
}

int pointerDiff(const Primitive *p1, const Primitive *p2){
	const ptrdiff_t delta = p1 - p2;
	return delta ? (delta < 0 ? -1 : 1) : 0;
}

int topToBottom(const Primitive *p1, const Primitive *p2){
	float delta = topMostPoint(p1) - topMostPoint(p2);
#ifndef NDEBUG
	if(!delta) delta = bottomMostPoint(p1) - bottomMostPoint(p2);
	if(!delta) delta = leftMostPoint(p1) - leftMostPoint(p2);
	if(!delta) delta = rightMostPoint(p1) - rightMostPoint(p2);
	if(!delta) delta = p1->arity - p2->arity;
	if(!delta) delta = p1->color - p2->color;
#endif
	return delta ? (delta > 0 ? 1 : -1) : 0;
}

float topMostPoint(const Primitive *prim){
	Point **const boundary = prim->boundary;
	const size_t arity = prim->arity;
	size_t i; float top = -HUGE_VAL;
	for(i = 0; i < arity; ++i){
		const float candidate = boundary[i]->y;
		if(candidate > top) top = candidate;
	}
	return top;
}

float bottomMostPoint(const Primitive *prim){
	Point **const boundary = prim->boundary;
	const size_t arity = prim->arity;
	size_t i; float bottom = HUGE_VAL;
	for(i = 0; i < arity; ++i){
		const float candidate = boundary[i]->y;
		if(candidate < bottom) bottom = candidate;
	}
	return bottom;
}

#ifndef NDEBUG
float rightMostPoint(const Primitive *prim){
	Point **const boundary = prim->boundary;
	const size_t arity = prim->arity;
	size_t i; float right = -HUGE_VAL;
	for(i = 0; i < arity; ++i){
		const float candidate = boundary[i]->x;
		if(candidate > right) right = candidate;
	}
	return right;
}

float leftMostPoint(const Primitive *prim){
	Point **const boundary = prim->boundary;
	const size_t arity = prim->arity;
	size_t i; float left = HUGE_VAL;
	for(i = 0; i < arity; ++i){
		const float candidate = boundary[i]->x;
		if(candidate < left) left = candidate;
	}
	return left;
}
#endif
