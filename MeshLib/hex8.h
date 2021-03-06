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

namespace HEX8
{
	// shape functions
	inline void shape(double* H, double r, double s, double t) 
	{ 
		H[0] = 0.125*(1 - r)*(1 - s)*(1 - t);
		H[1] = 0.125*(1 + r)*(1 - s)*(1 - t);
		H[2] = 0.125*(1 + r)*(1 + s)*(1 - t);
		H[3] = 0.125*(1 - r)*(1 + s)*(1 - t);
		H[4] = 0.125*(1 - r)*(1 - s)*(1 + t);
		H[5] = 0.125*(1 + r)*(1 - s)*(1 + t);
		H[6] = 0.125*(1 + r)*(1 + s)*(1 + t);
		H[7] = 0.125*(1 - r)*(1 + s)*(1 + t);
	}

	// shape function derivatives
	inline void shape_deriv(double* Hr, double* Hs, double* Ht, double r, double s, double t)
	{
		Hr[0] = -0.125*(1 - s)*(1 - t); Hs[0] = -0.125*(1 - r)*(1 - t); Ht[0] = -0.125*(1 - r)*(1 - s);
		Hr[1] =  0.125*(1 - s)*(1 - t);	Hs[1] = -0.125*(1 + r)*(1 - t);	Ht[1] = -0.125*(1 + r)*(1 - s);
		Hr[2] =  0.125*(1 + s)*(1 - t);	Hs[2] =  0.125*(1 + r)*(1 - t);	Ht[2] = -0.125*(1 + r)*(1 + s);
		Hr[3] = -0.125*(1 + s)*(1 - t);	Hs[3] =  0.125*(1 - r)*(1 - t);	Ht[3] = -0.125*(1 - r)*(1 + s);
		Hr[4] = -0.125*(1 - s)*(1 + t);	Hs[4] = -0.125*(1 - r)*(1 + t);	Ht[4] =  0.125*(1 - r)*(1 - s);
		Hr[5] =  0.125*(1 - s)*(1 + t);	Hs[5] = -0.125*(1 + r)*(1 + t);	Ht[5] =  0.125*(1 + r)*(1 - s);
		Hr[6] =  0.125*(1 + s)*(1 + t);	Hs[6] =  0.125*(1 + r)*(1 + t);	Ht[6] =  0.125*(1 + r)*(1 + s);
		Hr[7] = -0.125*(1 + s)*(1 + t);	Hs[7] =  0.125*(1 - r)*(1 + t);	Ht[7] =  0.125*(1 - r)*(1 + s);
	}

	// iso-parametric coordinates of nodes (for n = -1 return center coordinates)
	inline void iso_coord(int n, double q[3])
	{
		switch (n)
		{
		case -1: q[0] = 0; q[1] = 0; q[2] = 0; break;
		case 0: q[0] = -1; q[1] = -1; q[2] = -1; break;
		case 1: q[0] = 1; q[1] = -1; q[2] = -1; break;
		case 2: q[0] = 1; q[1] = 1; q[2] = -1; break;
		case 3: q[0] = -1; q[1] = 1; q[2] = -1; break;
		case 4: q[0] = -1; q[1] = -1; q[2] = 1; break;
		case 5: q[0] = 1; q[1] = -1; q[2] = 1; break;
		case 6: q[0] = 1; q[1] = 1; q[2] = 1; break;
		case 7: q[0] = -1; q[1] = 1; q[2] = 1; break;
		}
	}
}
