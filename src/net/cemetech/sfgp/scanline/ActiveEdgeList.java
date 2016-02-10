package net.cemetech.sfgp.scanline;

import java.util.TreeSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Comparator;
import java.util.Collections;

import net.cemetech.sfgp.scanline.geom.Edge;
import net.cemetech.sfgp.scanline.geom.Point;
import net.cemetech.sfgp.scanline.geom.Primitive;
import net.cemetech.sfgp.scanline.projections.Projection;

public class ActiveEdgeList {
	
	LinkedList<EdgeListDatum> activeEdges;
	int scanLine;
	
	public ActiveEdgeList(){ scanLine = -1; activeEdges = new LinkedList<EdgeListDatum>(); }
	public void stepEdges(TreeSet<Primitive> activePrims){
		scanLine++;
		for(Iterator<EdgeListDatum> i = activeEdges.iterator(); i.hasNext();){
			EdgeListDatum edge = i.next();
			final float edgeEnd = Math.max( 
					edge.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y),
					edge.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y)),
					lowEnd = Math.min( 
					edge.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y),
					edge.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y));
			if(edgeEnd < scanLine){
				i.remove();
				System.out.println("Deactivating " + edge.owner.color.toString() + 
						" with y-span: " + lowEnd + " -> " + edgeEnd + 
						" with x-span: " + edge.getMinXForLine(Projection.identity(), scanLine) + 
						" -> " + edge.getMaxXForLine(Projection.identity(), scanLine) + 
						"(true: " + edge.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.X) + 
						" -> " + edge.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.X) + ")");
			}
		}
		for(Primitive prim : activePrims){
			for(Edge e : prim.boundary){
				final float sy = e.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y),
						ey = e.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y),
						mnY = Math.min(sy, ey),
						mxY = Math.max(sy, ey);
				EdgeListDatum newEdge = new EdgeListDatum(e, prim);
				if ((mnY == scanLine && (sy != ey || newEdge.isSingleton())) || (scanLine == 0 && mnY < 0 && mxY > 0)){
					System.out.println("Activating " + prim.color.toString() + 
							" with y-span: " + mnY + " -> " + mxY + 
							" with x-span: " + newEdge.getMinXForLine(Projection.identity(), scanLine) + 
							" -> " + newEdge.getMaxXForLine(Projection.identity(), scanLine) + 
							"(true: " + e.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.X) + 
							" -> " + e.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.X) + ")");
					
					activeEdges.addFirst(newEdge);
					if(newEdge.isSingleton()){
						System.out.println("\t->Activating dummy end");
						activeEdges.addFirst(new EdgeListDatum(e, prim, true));
					}
				}
			}
		}
		
		Collections.sort(activeEdges,new LeftToRight(scanLine));
		
	}

	public class EdgeListDatum{
		Edge edge;
		Primitive owner;
		boolean placeHolder;
		public EdgeListDatum(Edge e, Primitive p){this(e, p, false);}
		public EdgeListDatum(Edge e, Primitive p, boolean pH){
			edge = e; owner = p; placeHolder = pH;
		}
		
		public float getMinXForLine(Projection p, int scanLine){
			final Edge projEdge = p.projectEdge(edge);
			final Point start = projEdge.getEndPoint(Edge.EndPoint.START),
					end = projEdge.getEndPoint(Edge.EndPoint.END);
			final float sx = start.getComponent(Point.CoordName.X),
					ex = end.getComponent(Point.CoordName.X),
					sy = start.getComponent(Point.CoordName.Y),
					ey = end.getComponent(Point.CoordName.Y),
					run = ex - sx,
					rise = ey - sy,
					ret = ((rise == 0) ? Math.min(ex, sx) : (((scanLine - sy) * run) / rise) + sx);
			return  ret;
		}
		
		public float getMaxXForLine(Projection p, int scanLine){
			final Edge projEdge = p.projectEdge(edge);
			final Point start = projEdge.getEndPoint(Edge.EndPoint.START),
					end = projEdge.getEndPoint(Edge.EndPoint.END);
			final float sx = start.getComponent(Point.CoordName.X),
					ex = end.getComponent(Point.CoordName.X),
					sy = start.getComponent(Point.CoordName.Y),
					ey = end.getComponent(Point.CoordName.Y),
					rise = sy - ey,
					run = sx - ex,
					minBelow = getMinXForLine(p, scanLine - 1),
					minAbove = getMinXForLine(p, scanLine + 1);
			return ((rise == 0) ? Math.max(sx, ex)
					: ((run == 0) ? sx : (Math.max(minBelow,minAbove) - 1)));
		}
		
		public float smartXForLine(Projection p, int scanLine){
			return placeHolder ? getMaxXForLine(p, scanLine) : getMinXForLine(p, scanLine);
		}
		
		public boolean isSingleton(){ return owner.getArity() == 1; }
	}
	
	public static class LeftToRight implements Comparator<EdgeListDatum>{
		
		int scanLine;
		private static Projection id = Projection.identity();
		public LeftToRight(int sL){scanLine = sL;}

		@Override
		public int compare(EdgeListDatum o1, EdgeListDatum o2) {
			float delta = o1.smartXForLine(id, scanLine) - o2.smartXForLine(id, scanLine);
			if(delta == 0){
				delta = o1.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.X) - o2.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.X);
			}
			if(delta == 0){
				delta = o1.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.X) - o2.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.X);
			}
			if(delta == 0){
				delta = o1.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y) - o2.edge.getEndPoint(Edge.EndPoint.START).getComponent(Point.CoordName.Y);
			}
			if(delta == 0){
				delta = o1.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y) - o2.edge.getEndPoint(Edge.EndPoint.END).getComponent(Point.CoordName.Y);
			}
			if(delta == 0){
				delta = o1.owner.getArity() - o2.owner.getArity();
			}
			if(delta == 0){
				delta = o1.owner.color.hashCode() - o2.owner.color.hashCode();
			}
			if(delta == 0){
				delta = (o1.placeHolder ? 1 : 0) - (o2.placeHolder ? 1 : 0);
			}
			return (delta == 0) ? 0 : ((delta < 0) ? -1 : 1);
		}
		
	}
	
}


