package net.cemetech.sfgp.scanline.projections;

import net.cemetech.sfgp.scanline.geom.Point;
import net.cemetech.sfgp.scanline.geom.Point.CoordName;

public class Orthographic extends Projection {

	Point min, max;
	
	public Orthographic(Point min, Point max) {
		setViewPort(min, max);
	}
	
	public void setViewPort(Point min, Point max) {
		this.min = min;
		this.max = max;
	}

	@Override
	public Point projectPoint(Point p) {
		final float x = p.getComponent(Point.CoordName.X),
				y = p.getComponent(Point.CoordName.Y),
				z = p.getComponent(Point.CoordName.Z),
				
				l = min.getComponent(Point.CoordName.X),
				r = max.getComponent(Point.CoordName.X),
				b = min.getComponent(Point.CoordName.Y),
				t = max.getComponent(Point.CoordName.Y),
				n = min.getComponent(Point.CoordName.Z),
				f = max.getComponent(Point.CoordName.Z);
		
		return new Point((2*x - (l + r))/(r-l),(2*y - (t + b))/(t-b),(-2*z + (f + n))/(f-n));
	}

}
