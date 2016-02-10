package net.cemetech.sfgp.scanline.geom;

import java.awt.Color;
import java.util.LinkedList;

import net.cemetech.sfgp.scanline.geom.Edge.EndPoint;
import net.cemetech.sfgp.scanline.geom.Point.CoordName;

public class Primitive implements Comparable<Primitive> {
	public LinkedList<Edge> boundary;
	public Color color;
	public int unique;
	
	public static int uniqueV = 0;
	
	public Primitive(LinkedList<Edge> b){
		boundary = b;
		color = Color.black;
		unique = uniqueV++;
	}
	
	public static Primitive makeLine(Edge e){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e);
		return new Primitive(le);
	}
	
	public static Primitive makeTri(Edge e1, Edge e2, Edge e3){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e1);
		le.add(e2);
		le.add(e3);
		return new Primitive(le);
	}
	
	public static Primitive makeQuad(Edge e1, Edge e2, Edge e3, Edge e4){
		LinkedList<Edge> le = new LinkedList<Edge>();
		le.add(e1);
		le.add(e2);
		le.add(e3);
		le.add(e4);
		return new Primitive(le);
	}
	
	public int getArity(){ return boundary.size(); }
	
	public float getZForXY(int x, int y){
		assert(getArity() > 0);
		float ret;
		if(getArity() == 1){
			// Alpha blend! wut wut.
			final Edge v = boundary.getFirst();
			final Point vs = v.getEndPoint(EndPoint.START),
					ve = v.getEndPoint(EndPoint.END);
			
			final float sx = vs.getComponent(CoordName.X),
					sy = vs.getComponent(CoordName.Y),
					sz = vs.getComponent(CoordName.Z);
			
			final float dx = ve.getComponent(CoordName.X) - sx,
					dy = ve.getComponent(CoordName.Y) - sy,
					dz = ve.getComponent(CoordName.Z) - sz;
			
			assert((dx == 0 && dy == 0) || (dx != 0 && dy != 0));
			
			// Theoretically these should be the same, but
			// we'll average for robustness
			final float xNumer = dz * (x - sx),
					yNumer = dz * (y - sy),
					xEst = (dx == 0) ? Math.min(0,  dz) : (xNumer / dx),
					yEst = (dy == 0) ? Math.min(0,  dz) : (yNumer / dy);
			
			ret = sz + (xEst + yEst) / 2; 
		} else {
			final Edge e1 = boundary.getFirst(),
					e2 = boundary.get(1);
			
			final Point us = e1.getEndPoint(EndPoint.START),
					ue = e1.getEndPoint(EndPoint.END),
					u = new Point(
					us.getComponent(CoordName.X) - ue.getComponent(CoordName.X),
					us.getComponent(CoordName.Y) - ue.getComponent(CoordName.Y),
					us.getComponent(CoordName.Z) - ue.getComponent(CoordName.Z));
			final float ux = u.getComponent(CoordName.X),
					uy = u.getComponent(CoordName.Y),
					uz = u.getComponent(CoordName.Z);
			
			final Point vs = e2.getEndPoint(EndPoint.START),
					ve = e2.getEndPoint(EndPoint.END),
					v = new Point(
					vs.getComponent(CoordName.X) - ve.getComponent(CoordName.X),
					vs.getComponent(CoordName.Y) - ve.getComponent(CoordName.Y),
					vs.getComponent(CoordName.Z) - ve.getComponent(CoordName.Z));
			final float vx = v.getComponent(CoordName.X),
					vy = v.getComponent(CoordName.Y),
					vz = v.getComponent(CoordName.Z),
					nx = uy * vz - uz * vy,
					ny = uz * vx - ux * vz,
					nz = ux * vy - uy * vx,
					d = -nx * us.getComponent(CoordName.X)
					-ny * us.getComponent(CoordName.Y)
					-nz * us.getComponent(CoordName.Z),
					numer = (-d - nx * x - ny * y);
			
			ret = (nz == 0) ? ((numer > 0) ? Integer.MAX_VALUE : Integer.MIN_VALUE) : (numer / nz) ;
		}
		return ret;
	}
	//*
	public int hashCode() {
		int ret = 0;
		for(Edge e : boundary){
			ret ^= e.hashCode();
		}
		ret ^= color.hashCode();
		return ret;
	}
	//*/
	public int compareTo(Primitive o){
		return this.unique - o.unique;
	}

}
