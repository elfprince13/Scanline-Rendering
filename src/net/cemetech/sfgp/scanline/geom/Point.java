package net.cemetech.sfgp.scanline.geom;

import java.util.Collection;
import java.util.Iterator;
import java.util.NoSuchElementException;

public class Point implements Collection<Float>{
	public enum CoordName{
		X, Y, Z;
		public int toNum() {
			switch(this){
				case X: return 0;
				case Y: return 1;
				case Z: return 2;
				default: throw new IllegalArgumentException("bad enum value: " + this);
			}
		}
	}
	
	private float[] components;
	public Point(float x, float y, float z){
		components = new float[3];
		components[CoordName.X.toNum()] = x;
		components[CoordName.Y.toNum()] = y;
		components[CoordName.Z.toNum()] = z;
	}
	
	public Point(float[] initComponents) {
		if(initComponents.length != 3) throw new IllegalArgumentException("A point only has three components");
		components = initComponents;
	}
	
	public static Point origin(){ return new Point(0,0,0); }
	
	public float getComponent(CoordName e){ 
		return components[e.toNum()];
	}

	public void setEndPoint(CoordName e, float newComponent){
		components[e.toNum()] = newComponent;
	}
	
	public void negate(){
		for(int i = 0; i < components.length; i++) components[i] = -components[i];
	}
	
	public static Point negated(Point e){
		return new Point(
				-e.getComponent(CoordName.X), 
				-e.getComponent(CoordName.Y), 
				-e.getComponent(CoordName.Z));
	}

	@Override
	public Iterator<Float> iterator() {
		return new Iterator<Float>(){
			int i = 0;

			@Override
			public boolean hasNext() {
				return (i != components.length);
			}

			@Override
			public Float next() {
				if(i == components.length){
					throw new NoSuchElementException();
				} else {
					return components[i++];
				}
			}

			@Override
			public void remove() {
				throw new UnsupportedOperationException("can't modify points like this");
			}
			
		};
	}

	@Override
	public int size() {
		return components.length;
	}

	@Override
	public boolean isEmpty() {
		return false;
	}

	@Override
	public boolean contains(Object o) {
		boolean ret = false;
		if(o instanceof Float){
			for(int i = 0; i < components.length; i++) ret |= ((Float)o).floatValue() == components[i];
		}
		return ret;
	}

	@Override
	public Object[] toArray() {
		return toArray(new Float[components.length]);
	}

	@Override
	public <T> T[] toArray(T[] a) {
		if(a.length < components.length) a = (T[])(new Float[components.length]);
		if(a.length > components.length) a[components.length] = null;
		for(int i = 0; i < components.length; i++) a[i] = (T)(new Float(components[i]));
		return a;
	}

	@Override
	public boolean add(Float e) {
		throw new UnsupportedOperationException("can't modify points like this");
	}

	@Override
	public boolean remove(Object o) {
		throw new UnsupportedOperationException("can't modify points like this");
	}

	@Override
	public boolean containsAll(Collection<?> c) {
		boolean ret = true;
		for(Object o : c){
			ret &= contains(o);
			if(!ret) break;
		}
		return ret;
	}

	@Override
	public boolean addAll(Collection<? extends Float> c) {
		throw new UnsupportedOperationException("can't modify points like this");
	}

	@Override
	public boolean removeAll(Collection<?> c) {
		throw new UnsupportedOperationException("can't modify points like this");
	}

	@Override
	public boolean retainAll(Collection<?> c) {
		throw new UnsupportedOperationException("can't modify points like this");
	}

	@Override
	public void clear() {
		throw new UnsupportedOperationException("can't modify points like this");
	}
	
	public boolean equals(Object o){
		boolean ret = false;
		if(o instanceof Point){
			ret = true;
			for(int i = 0; i < components.length; i++) ret &= ((Point)o).components[i] == components[i];
		}
		return ret;
	}
	
	public int hashCode(){
		int a = 0;
		for(float i : components) a ^= Float.floatToIntBits(i);
		return a;
	}

}
