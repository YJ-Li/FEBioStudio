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
#include "GLDataMap.h"
#include <GLWLib/GLWidget.h>
#include <GLLib/GLTexture1D.h>
#include <PostLib/ColorMap.h>

namespace Post {

class CGLModel;

//-----------------------------------------------------------------------------

class CGLColorMap : public CGLDataMap
{
	enum { DATA_FIELD, DATA_SMOOTH, COLOR_MAP, NODAL_VALS, RANGE_TYPE, RANGE_DIVS, SHOW_LEGEND, USER_MAX, USER_MIN };

public:
	CGLColorMap(CGLModel* po);
	~CGLColorMap();

	Post::CColorTexture* GetColorMap() { return &m_Col; }

	void Update(int ntime, float dt, bool breset) override;

	int GetEvalField() const { return m_nfield; }
	void SetEvalField(int n);

	void GetRange(float* pd) { pd[0] = m_range.min; pd[1] = m_range.max; }
	int GetRangeType() { return m_range.ntype; }

	void SetRange(float* pd) { m_range.min = pd[0]; m_range.max = pd[1]; }
	void SetRangeMax(float f) { m_range.max = f; }
	void SetRangeMin(float f) { m_range.min = f; }
	void SetRangeType(int n) { m_range.ntype = n; m_breset = true; }

	void DisplayNodalValues(bool b) { m_bDispNodeVals = b; }
	bool DisplayNodalValues() { return m_bDispNodeVals; }

	bool ShowLegend() { return m_pbar->visible(); }
	void ShowLegend(bool b) { if (b) m_pbar->show(); else m_pbar->hide(); }

	bool GetColorSmooth();
	void SetColorSmooth(bool b);

	void Activate(bool b) override { CGLObject::Activate(b); ShowLegend(b); }

private:
	void UpdateState(int ntime, bool breset);

	bool UpdateData(bool bsave = true) override;

	void Update() override;

protected:
	int		m_nfield;
	bool	m_breset;	// reset the range when the field has changed
	DATA_RANGE	m_range;	// range for legend

public:
	bool	m_bDispNodeVals;	// render nodal values

	Post::CColorTexture	m_Col;	// colormap used for rendering

	GLLegendBar*	m_pbar;	// the legend bar
};
}
