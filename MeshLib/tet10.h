/*This file is part of the FEBio Studio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio-Studio.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#pragma once

namespace TET10 
{
	// shape functions
	inline void shape(double* H, double r, double s, double t)
	{
		double r1 = 1.0 - r - s - t;
		double r2 = r;
		double r3 = s;
		double r4 = t;

		H[0] = r1*(2.0*r1 - 1.0);
		H[1] = r2*(2.0*r2 - 1.0);
		H[2] = r3*(2.0*r3 - 1.0);
		H[3] = r4*(2.0*r4 - 1.0);
		H[4] = 4.0*r1*r2;
		H[5] = 4.0*r2*r3;
		H[6] = 4.0*r3*r1;
		H[7] = 4.0*r1*r4;
		H[8] = 4.0*r2*r4;
		H[9] = 4.0*r3*r4;
	}

	// shape function derivatives
	inline void shape_deriv(double* Hr, double* Hs, double* Ht, double r, double s, double t)
	{
		Hr[0] = -3.0 + 4.0*r + 4.0*(s + t);
		Hr[1] =  4.0*r - 1.0;
		Hr[2] =  0.0;
		Hr[3] =  0.0;
		Hr[4] =  4.0 - 8.0*r - 4.0*(s + t);
		Hr[5] =  4.0*s;
		Hr[6] = -4.0*s;
		Hr[7] = -4.0*t;
		Hr[8] =  4.0*t;
		Hr[9] =  0.0;

		Hs[0] = -3.0 + 4.0*s + 4.0*(r + t);
		Hs[1] =  0.0;
		Hs[2] =  4.0*s - 1.0;
		Hs[3] =  0.0;
		Hs[4] = -4.0*r;
		Hs[5] =  4.0*r;
		Hs[6] =  4.0 - 8.0*s - 4.0*(r + t);
		Hs[7] = -4.0*t;
		Hs[8] =  0.0;
		Hs[9] =  4.0*t;

		Ht[0] = -3.0 + 4.0*t + 4.0*(r + s);
		Ht[1] =  0.0;
		Ht[2] =  0.0;
		Ht[3] =  4.0*t - 1.0;
		Ht[4] = -4.0*r;
		Ht[5] =  0.0;
		Ht[6] = -4.0*s;
		Ht[7] =  4.0 - 8.0*t - 4.0*(r + s);
		Ht[8] =  4.0*r;
		Ht[9] =  4.0*s;
	}

	// iso-parametric coordinates of nodes (for n = -1 return center coordinates)
	inline void iso_coord(int n, double q[3])
	{
		switch (n)
		{
		case -1: q[0] = 0.25; q[1] = 0.25; q[2] = 0.25; break;
		case 0: q[0] = 0; q[1] = 0; q[2] = 0; break;
		case 1: q[0] = 1; q[1] = 0; q[2] = 0; break;
		case 2: q[0] = 0; q[1] = 1; q[2] = 0; break;
		case 3: q[0] = 0; q[1] = 0; q[2] = 1; break;
		case 4: q[0] = 0.5; q[1] = 0.0; q[2] = 0.0; break;
		case 5: q[0] = 0.5; q[1] = 0.5; q[2] = 0.0; break;
		case 6: q[0] = 0.0; q[1] = 0.5; q[2] = 0.0; break;
		case 7: q[0] = 0.0; q[1] = 0.0; q[2] = 0.5; break;
		case 8: q[0] = 0.5; q[1] = 0.0; q[2] = 0.5; break;
		case 9: q[0] = 0.0; q[1] = 0.5; q[2] = 0.5; break;
		}
	}
}
