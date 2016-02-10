package net.cemetech.sfgp.scanline.projections;

import net.cemetech.sfgp.scanline.geom.Point;
import net.cemetech.sfgp.scanline.geom.Point.CoordName;

public class Scale extends Projection {
	float scale;
	public Scale(float s){scale = s;}

	@Override
	public Point projectPoint(Point p) {
		return new Point(
				scale*p.getComponent(Point.CoordName.X),
				scale*p.getComponent(Point.CoordName.Y),
				scale*p.getComponent(Point.CoordName.Z)
				);
	}

}
