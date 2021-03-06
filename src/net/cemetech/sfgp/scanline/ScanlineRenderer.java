package net.cemetech.sfgp.scanline;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Iterator;
import java.util.LinkedList;

import net.cemetech.sfgp.scanline.geom.Edge;
import net.cemetech.sfgp.scanline.geom.Point;
import net.cemetech.sfgp.scanline.geom.Primitive;
import net.cemetech.sfgp.scanline.geom.Edge.EndPoint;
import net.cemetech.sfgp.scanline.geom.Point.CoordName;
import net.cemetech.sfgp.scanline.projections.CoordinatePlane;
import net.cemetech.sfgp.scanline.projections.Projection;

public class ScanlineRenderer<T extends Projection> {
	static Projection id = Projection.identity();
	static Projection screenPlane = CoordinatePlane.onto(CoordName.Z, 0);
	public void render(BufferedImage raster, T p, Primitive[] geometry){
		Primitive[] projectedGeometry = new Primitive[geometry.length];
		for(int i = 0; i < geometry.length; i++){
			projectedGeometry[i] = p.projectPrimitive(geometry[i]);
			System.out.println("Source geometry with arity " + geometry[i].getArity() + " begins on " + bottomMostPoint(geometry[i]) + " and ends on " + topMostPoint(geometry[i]));
			System.out.println("Projected geometry with arity " + projectedGeometry[i].getArity() + " begins on " + bottomMostPoint(projectedGeometry[i]) + " and ends on " + topMostPoint(projectedGeometry[i]));
		}
		
		int numLines = raster.getHeight();
		int lineWidth = raster.getWidth();
		ArrayList<TreeSet<Primitive>> scanLinePrimBuckets = new ArrayList<TreeSet<Primitive>>(numLines);
		for(int i = 0; i < numLines; i++) scanLinePrimBuckets.add(new TreeSet<Primitive>());
		Arrays.sort(projectedGeometry,new TopToBottom());
		for(Primitive prim : projectedGeometry){
			prim.unique = prim.uniqueV++;
			int pScanline = Math.round(bottomMostPoint(prim));
			if(pScanline < numLines && (pScanline >= 0 || topMostPoint(prim) >= 0)){
				int dstBucket = Math.max(0,pScanline);
				scanLinePrimBuckets.get(dstBucket).add(prim);
				System.out.println("dstBucket " + dstBucket + " gets geometry with arity " + prim.getArity() + " begins on " + bottomMostPoint(prim) + " and ends on " + topMostPoint(prim) + " and now has size " + scanLinePrimBuckets.get(dstBucket).size());
			}
		}
		
		TreeSet<Primitive> activePrimSet = new TreeSet<Primitive>();
		TreeMap<Primitive,Edge> inFlags = new TreeMap<Primitive,Edge>();
		TreeSet<Primitive> deFlags = new TreeSet<Primitive>();
		ActiveEdgeList ael = new ActiveEdgeList();
		for(int line = 0; line < numLines; line++){
			System.out.println("Scanning line: " + line);
			if(!inFlags.isEmpty()){
				System.out.println("\tGarbage left in inFlags:");
				for(Primitive pp : inFlags.keySet()){
					System.out.println("\t\t" + pp.color.toString());
				}
			}
			inFlags.clear();
			deFlags.clear();
			System.out.println("\tUpdating activePrimSet");
			for(Iterator<Primitive> i = activePrimSet.iterator(); i.hasNext();){
				Primitive prim = i.next();
				int top = Math.round(topMostPoint(prim));
				int bottom = Math.round(bottomMostPoint(prim));
				if(top < line){
					System.out.println("\t\t" + top + " -> " + bottom + " ( " + prim.color + " ) is not valid here: " + line);
					i.remove();	
				}
			}
			for(Primitive prim : scanLinePrimBuckets.get(line)){
				int top = Math.round(topMostPoint(prim));
				int bottom = Math.round(bottomMostPoint(prim));
				System.out.println("\t\t" + top + " -> " + bottom + " ( " + prim.color + " ) is added here: " + line);
				
				activePrimSet.add(prim);
			}
			ael.stepEdges(activePrimSet);
			int curPixel = 0;
			Primitive curDraw = null;
			ActiveEdgeList.EdgeListDatum startEdge;
			ActiveEdgeList.EdgeListDatum nextEdge;
			Iterator<ActiveEdgeList.EdgeListDatum> i = ael.activeEdges.iterator();
			if(i.hasNext()){
				nextEdge = i.next();
				while((nextEdge != null) && curPixel < lineWidth){
					startEdge = nextEdge;
					int startX = Math.round(startEdge.smartXForLine(id, line));
					int nextX;
					nextEdge = null; // Safety valve. We want a NullPointerException if we use it before the update.
										
					if(inFlags.containsKey(startEdge.owner) ){
						Edge edgeHere = startEdge.edge;
						Point s = edgeHere.getEndPoint(EndPoint.START);
						Point e = edgeHere.getEndPoint(EndPoint.END);
						Edge edgeIn = inFlags.get(startEdge.owner);
						Edge flatHere = screenPlane.projectEdge(edgeHere);
						Edge flatIn = screenPlane.projectEdge(edgeIn);
						Point here = new Point(startX,line,0);
						boolean sV = edgeIn.contains(s);
						boolean eV = edgeIn.contains(e);
						boolean v = (sV || eV) && flatIn.contains(here) && flatHere.contains(here) && !(startEdge.owner.getArity() == 1);
						Edge vert = new Edge(here, new Point(startX,line+1,0));
						float dotH = v ? vert.dot(flatHere) : 0;
						float dotIn = v ? vert.dot(flatIn) : 0;
						if(v && (dotH * dotIn <= 0)){
							System.out.println("\tFound horizontal vertex " + startEdge.owner.color.toString() + " at " + startEdge.smartXForLine(id, line) + ". Don't delete it yet");
						} else {
							System.out.println("\tNot *in* old " + startEdge.owner.color.toString() + " at " + startEdge.smartXForLine(id, line));
							deFlags.add(startEdge.owner);
							//inFlags.remove(startEdge.owner);
						}
					} else if(!inFlags.containsKey(startEdge.owner)) {
						System.out.println("\tNow *in* new " + startEdge.owner.color.toString() + " at " + startEdge.smartXForLine(id, line));
						inFlags.put(startEdge.owner,startEdge.edge);
					}
					
					if(curPixel < startX){
						System.out.println("\tcurPixel has fallen behind, dragging from " + curPixel + " to " + startX);
						curPixel = startX;
					}
					
					if(i.hasNext()){
						nextEdge = i.next();
						nextX = Math.round(nextEdge.smartXForLine(id, line));
						System.out.println("\tNext edges @ x = " + nextX + " from " + nextEdge.owner.color.toString());
					} else {
						System.out.println("\tNo more edges");
						nextEdge = null;
						nextX = 0;
					}
					
					nextX = Math.min(lineWidth, nextX);
					while((nextEdge == null && curPixel < lineWidth) || (curPixel < nextX)){
						boolean zFight = false;
						boolean solitary = false;
						System.out.println("\tTesting depth:");
						curDraw = null; float bestZ = Float.POSITIVE_INFINITY;
						for(Primitive prim : inFlags.keySet()){
							float testZ = prim.getZForXY(curPixel, line);
							// -Z is out of the screen, so ... 
							if(testZ <= bestZ){
								System.out.println("\t\tHit: " + testZ + " <= " + bestZ + " for " + prim.color);
								if(testZ == bestZ){
									if(prim.getArity() == 1){
										zFight = curDraw != null && curDraw.getArity() == 1;
										curDraw = prim;
										solitary = deFlags.contains(prim);
									} else {
										zFight = curDraw != null && curDraw.getArity() != 1;
									}
								} else {
									zFight = false;
									bestZ = testZ;
									curDraw = prim;
									solitary = deFlags.contains(prim);
								}
							}
						}
	
						if (curDraw != null) {
							/*
							if(nextEdge == null && !solitary){
								System.out.println("Warning: we probably shouldn't have to draw if there are no edges to turn us off. Look for parity errors");
								inFlags.clear();
							} else {
							//*/
								int drawWidth = (zFight || solitary) ? 1 : (((nextEdge == null) ? lineWidth : nextX) - curPixel);
								System.out.println("Drawing " + drawWidth + " @ " + "(" + curPixel + ", " + line + ")");
								drawWidth = Math.min(lineWidth - curPixel, Math.max(0, drawWidth));
								System.out.println("Drawing " + drawWidth + " @ " + "(" + curPixel + ", " + line + ")");
								int[] rgbs = new int[drawWidth];
								Arrays.fill(rgbs, curDraw.color.getRGB());
								raster.setRGB(curPixel, line, drawWidth, 1, rgbs, 0, 1);
								curPixel +=  drawWidth;
							//}
						} else if (inFlags.size() == 0 && nextEdge != null) {
							System.out.println("Not in any polys at the moment, fast-forwarding(1) to " + nextX);
							curPixel = nextX;
						} else {
							System.out.println("Nothing to draw at end of line");
							curPixel = lineWidth;
						}
						for(Primitive prim : deFlags){
							inFlags.remove(prim);
						}
						deFlags.clear();
					}
					if (inFlags.size() == 0 && nextEdge != null) {
						System.out.println("Not in any polys at the moment, fast-forwarding(2) to " + nextX);
						curPixel = nextX;
					}

				}
			}
		}
	}
	
	
	
	private class TopToBottom implements Comparator<Primitive> {	
		@Override
		public int compare(Primitive o1, Primitive o2) {
			float delta = topMostPoint(o1) - topMostPoint(o2);
			if(delta == 0){
				delta = bottomMostPoint(o1) - bottomMostPoint(o2);
			}
			if(delta == 0){
				delta = leftMostPoint(o1) - leftMostPoint(o2);
			}
			if(delta == 0){
				delta = rightMostPoint(o1) - rightMostPoint(o2);
			}
			if(delta == 0){
				delta = o1.getArity() - o2.getArity();
			}
			if(delta == 0){
				delta = o1.color.hashCode() - o2.color.hashCode();
			}
			return delta == 0 ? 0 : (delta < 0 ? -1 : 1);
		}
	}
	
	private float topMostPoint(Primitive prim){
		float top = 0; int i = 0;
		for(Edge e : prim.boundary) {
			float candidate = topMostPoint(e);
			if(i++ == 0 || candidate > top) top = candidate;
		}
		return top;
	}
	
	private float bottomMostPoint(Primitive prim){
		float bottom = 0; int i = 0;
		for(Edge e : prim.boundary) {
			float candidate = bottomMostPoint(e);
			if(i++ == 0 || candidate < bottom) bottom = candidate;
		}
		return bottom;
	}
	
	private float topMostPoint(Edge e){
		return Math.max(
				e.getEndPoint(EndPoint.START).getComponent(CoordName.Y),
				e.getEndPoint(EndPoint.END).getComponent(CoordName.Y));
	}
	
	private float bottomMostPoint(Edge e){
		return Math.min(
				e.getEndPoint(EndPoint.START).getComponent(CoordName.Y),
				e.getEndPoint(EndPoint.END).getComponent(CoordName.Y));
	}
	
	private float rightMostPoint(Primitive prim){
		float top = 0; int i = 0;
		for(Edge e : prim.boundary) {
			float candidate = rightMostPoint(e);
			if(i++ == 0 || candidate > top) top = candidate;
		}
		return top;
	}
	
	private float leftMostPoint(Primitive prim){
		float bottom = 0; int i = 0;
		for(Edge e : prim.boundary) {
			float candidate = leftMostPoint(e);
			if(i++ == 0 || candidate < bottom) bottom = candidate;
		}
		return bottom;
	}
	
	private float rightMostPoint(Edge e){
		return Math.max(
				e.getEndPoint(EndPoint.START).getComponent(CoordName.X),
				e.getEndPoint(EndPoint.END).getComponent(CoordName.X));
	}
	
	private float leftMostPoint(Edge e){
		return Math.min(
				e.getEndPoint(EndPoint.START).getComponent(CoordName.X),
				e.getEndPoint(EndPoint.END).getComponent(CoordName.X));
	}
}
