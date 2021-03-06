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

#include "stdafx.h"
#include "ScalarFieldTool.h"
#include "ModelDocument.h"
#include <MeshTools/LaplaceSolver.h>
#include <GeomLib/GObject.h>
#include <MeshTools/GGroup.h>
#include <MeshTools/FENodeData.h>
#include <MeshTools/FEElementData.h>
#include <QLineEdit>
#include <QBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QValidator>
#include <QMessageBox>
#include <QLabel>
#include <QHeaderView>
#include "MainWindow.h"

class UIScalarFieldTool : public QWidget
{
public:
	QLineEdit*		m_name;
	QPushButton*	m_add;
	QPushButton*	m_apply;
	QTableWidget*	m_table;
	QLineEdit*		m_val;

public:
	UIScalarFieldTool(CScalarFieldTool* w)
	{
		QVBoxLayout* l = new QVBoxLayout;

		m_name = new QLineEdit;
		m_add = new QPushButton("Add");
		m_val = new QLineEdit; m_val->setValidator(new QDoubleValidator);
		m_val->setText("0");

		QHBoxLayout* h1 = new QHBoxLayout;
		h1->addWidget(new QLabel("Name"));
		h1->addWidget(m_name);

		QHBoxLayout* h2 = new QHBoxLayout;
		h2->addWidget(new QLabel("Value"));
		h2->addWidget(m_val);
		h2->addWidget(m_add);

		l->addLayout(h1);
		l->addLayout(h2);
		l->addWidget(m_table = new QTableWidget);
		m_table->setColumnCount(2);
		m_table->horizontalHeader()->setStretchLastSection(true);
		m_table->setHorizontalHeaderLabels(QStringList() << "selection" << "value");

		l->addWidget(m_apply = new QPushButton("Create"));

		l->addStretch();

		setLayout(l);

		QObject::connect(m_add, SIGNAL(clicked()), w, SLOT(OnAddClicked()));
		QObject::connect(m_apply, SIGNAL(clicked()), w, SLOT(OnApply()));
	}
};

CScalarFieldTool::CScalarFieldTool(CMainWindow* wnd) : CAbstractTool(wnd, "Scalar Field")
{
	m_po = nullptr;

	m_ntype = 0;

	ui = nullptr;
}

QWidget* CScalarFieldTool::createUi()
{
	if (ui == nullptr) ui = new UIScalarFieldTool(this);
	return ui;
}

void CScalarFieldTool::OnAddClicked()
{
	double v = ui->m_val->text().toDouble();

	GObject* po = GetMainWindow()->GetActiveObject();
	if (m_po == nullptr) m_po = po;
	if (m_po == po)
	{
		CModelDocument* doc = GetMainWindow()->GetModelDocument();
		FESelection* sel = doc->GetCurrentSelection();
		if (sel)
		{
			FEItemListBuilder* items = sel->CreateItemList();
			if (items)
			{
				m_data.push_back(items);

				int n = ui->m_table->rowCount();
				ui->m_table->insertRow(n);

				QTableWidgetItem* it0 = new QTableWidgetItem;
				it0->setText(QString("selection%1").arg(n));
				it0->setFlags(Qt::ItemIsSelectable);
				ui->m_table->setItem(n, 0, it0);

				QTableWidgetItem* it1 = new QTableWidgetItem;
				it1->setText(QString::number(v));
				ui->m_table->setItem(n, 1, it1);
			}
		}		
	}
}

void CScalarFieldTool::Activate()
{
	Clear();
	CAbstractTool::Activate();
}

void CScalarFieldTool::Clear()
{
	m_po = nullptr;
	for (int i = 0; i < m_data.size(); ++i) delete m_data[i];
	m_data.clear();
	ui->m_table->clear();
	ui->m_table->setRowCount(0);
	ui->m_name->clear();
}

void CScalarFieldTool::OnApply()
{
	// get the document
	CModelDocument* pdoc = dynamic_cast<CModelDocument*>(GetDocument());

	// get the currently selected object
	GObject* po = pdoc->GetActiveObject();
	if (po == 0)
	{
		QMessageBox::critical(GetMainWindow(), "Tool", "You must first select an object.");
		return;
	}

	// make sure there is a mesh
	FEMesh* pm = po->GetFEMesh();
	if (pm == 0)
	{
		QMessageBox::critical(GetMainWindow(), "Tool", "The object needs to be meshed before you can apply this tool.");
		return;
	}

	QString name = ui->m_name->text();
	if (name.isEmpty())
	{
		QMessageBox::critical(GetMainWindow(), "Tool", "You must enter a valid name.");
		return;
	}

	//get the model and nodeset
	FEModel* ps = pdoc->GetFEModel();
	GModel& model = ps->GetModel();

	int NN = pm->Nodes();
	vector<int> bn(NN, 0);
	vector<double> val(NN, 0.0);

	for (int i = 0; i < m_data.size(); ++i)
	{
		FEItemListBuilder* item = m_data[i];
		double v = ui->m_table->item(i, 1)->text().toDouble();

		FENodeList* node = item->BuildNodeList(); assert(node);
		if (node)
		{
			for (int i = 0; i < NN; ++i) pm->Node(i).m_ntag = i;

			FENodeList::Iterator it = node->First();
			int nn = node->Size();
			for (int j=0; j<nn; ++j, it++)
			{ 
				assert(it->m_pm == pm);
				int nid = it->m_pi->m_ntag;
				assert((nid >= 0) && (nid < NN));
				val[nid] = v;
				bn[nid] = 1;
			}
		}
	}

	// solve Laplace equation
	LaplaceSolver L;
	L.Solve(pm, val, bn);

	if (m_ntype == 0)
	{
		// create node data
		FENodeData* pdata = pm->AddNodeDataField(name.toStdString());
		for (int i = 0; i<NN; i++) pdata->set(i, val[i]);
	}
	else
	{
		// create element data
		int parts = po->Parts();
		vector<int> partList(parts);
		for (int i = 0; i < parts; ++i) partList[i] = i;

		FEPartData* pdata = new FEPartData(po->GetFEMesh());
		pdata->SetName(name.toStdString());
		pdata->Create(partList);
		pm->AddMeshDataField(pdata);

		FEElemList* elemList = pdata->BuildElemList();
		int NE = elemList->Size();
		auto it = elemList->First();
		for (int i = 0; i < NE; ++i, ++it)
		{
			FEElement_& el = *it->m_pi;
			int ne = el.Nodes();
			double v = 0.0;
			for (int j = 0; j < ne; ++j) v += val[el.m_node[j]];
			v /= (double)ne;

			pdata->set(i, v);
		}
		delete elemList;
	}
	Clear();
	GetMainWindow()->UpdateModel();
	QMessageBox::information(GetMainWindow(), "Tool", "Datafield successfully added.");
}
