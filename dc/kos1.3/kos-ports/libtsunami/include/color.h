/*      
   Tsunami for KallistiOS ##version##
        
   color.h

   Copyright (C)2003 Dan Potter

   $Id: color.h,v 1.1 2003/04/24 02:57:20 bardtx Exp $
*/

#ifndef __TSUNAMI_COLOR_H
#define __TSUNAMI_COLOR_H

#include <plx/color.h>

struct Color {
	/// Constructor
	Color() {
		a = r = g = b = 1.0f;
	}
	Color(float ir, float ig, float ib) {
		a = 1.0f;
		r = ir;
		g = ig;
		b = ib;
	}
	Color(float ia, float ir, float ig, float ib) {
		a = ia;
		r = ir;
		g = ig;
		b = ib;
	}

	Color operator * (const Color & other) const {
		return Color(a*other.a, r*other.r, g*other.g, b*other.b);
	}

	Color operator * (float factor) const {
		return Color(a*factor, r*factor, g*factor, b*factor);
	}

	Color operator + (const Color & other) const {
		return Color(a+other.a, r+other.r, g+other.g, b+other.b);
	}

	Color & operator *= (const Color & other) {
		*this = *this * other;
		return *this;
	}

	Color & operator += (const Color & other) {
		*this = *this + other;
		return *this;
	}

	operator uint32() const {
		float ta, tr, tg, tb;
		ta = (a < 0.0f) ? 0.0f : (a > 1.0f) ? 1.0f : a;
		tr = (r < 0.0f) ? 0.0f : (r > 1.0f) ? 1.0f : r;
		tg = (g < 0.0f) ? 0.0f : (g > 1.0f) ? 1.0f : g;
		tb = (b < 0.0f) ? 0.0f : (b > 1.0f) ? 1.0f : b;
		return plx_pack_color(ta, tr, tg, tb);
	}


	float	a, r, g, b;
};

#endif	/* __TSUNAMI_COLOR_H */

