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
#include "PostDataPanel.h"
#include <QBoxLayout>
#include <QTableView>
#include <QLabel>
#include <QHeaderView>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QFileDialog>
#include <QFormLayout>
#include <QAction>
#include <QMenu>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QSplitter>
#include "MainWindow.h"
#include "Document.h"
#include <PostLib/FEPostModel.h>
#include <PostGL/GLModel.h>
#include <QtCore/QAbstractTableModel>
#include <PostLib/DataFilter.h>
#include "PropertyListView.h"
#include "PropertyListForm.h"
#include <PostLib/FEMeshData_T.h>
#include <PostLib/FEMathData.h>
#include "PostDocument.h"
#include <PostLib/FEDataField.h>
#include <PostLib/FEDistanceMap.h>
#include <PostLib/FEAreaCoverage.h>
#include "DlgAddEquation.h"

class CCurvatureProps : public CPropertyList
{
public:
	Post::FECurvatureField*	m_pf;

public:
	CCurvatureProps(Post::FECurvatureField* pf) : m_pf(pf)
	{
		addProperty("Smoothness", CProperty::Int);
		addProperty("Max iterations", CProperty::Int);
		addProperty("Use Extended Quadric", CProperty::Bool);
	}

	QVariant GetPropertyValue(int i) override
	{
		switch (i)
		{
		case 0: return m_pf->m_nlevels; break;
		case 1: return m_pf->m_nmax; break;
		case 2: return (m_pf->m_bext != 0); break;
		}

		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v) override
	{
		switch (i)
		{
		case 0: m_pf->m_nlevels = v.toInt(); break;
		case 1: m_pf->m_nmax = v.toInt(); break;
		case 2: m_pf->m_bext = (v.toBool() ? 1 : 0); break;
		}
	}
};

class CMathDataProps : public CPropertyList
{
public:
	Post::FEMathDataField*	m_pd;

	CMathDataProps(Post::FEMathDataField* pd) : m_pd(pd)
	{
		addProperty("Equation", CProperty::String);
	}

	QVariant GetPropertyValue(int i) override
	{
		return QString::fromStdString(m_pd->EquationString());
	}

	void SetPropertyValue(int i, const QVariant& v) override
	{
		m_pd->SetEquationString((v.toString()).toStdString());
	}
};

class CMathDataVec3Props : public CPropertyList
{
public:
	Post::FEMathVec3DataField*	m_pd;

	CMathDataVec3Props(Post::FEMathVec3DataField* pd) : m_pd(pd)
	{
		addProperty("x", CProperty::String);
		addProperty("y", CProperty::String);
		addProperty("z", CProperty::String);
	}

	QVariant GetPropertyValue(int i) override
	{
		switch (i)
		{
		case 0: return QString::fromStdString(m_pd->EquationString(0)); break;
		case 1: return QString::fromStdString(m_pd->EquationString(1)); break;
		case 2: return QString::fromStdString(m_pd->EquationString(2)); break;
		}

		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v) override
	{
		switch (i)
		{
		case 0: m_pd->SetEquationString(0, (v.toString()).toStdString()); break;
		case 1: m_pd->SetEquationString(1, (v.toString()).toStdString()); break;
		case 2: m_pd->SetEquationString(2, (v.toString()).toStdString()); break;
		}
	}
};

class CStrainProps : public CPropertyList
{
private:
	Post::FEStrainDataField*	m_ps;

public:
	CStrainProps(Post::FEStrainDataField* ps, int maxStates) : m_ps(ps)
	{
		addProperty("reference state", CProperty::Int)->setIntRange(0, maxStates);
	}

	QVariant GetPropertyValue(int i) override
	{
		switch (i)
		{
		case 0: return m_ps->m_nref + 1; break;
		}

		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v) override
	{
		switch (i)
		{
		case 0: m_ps->m_nref = v.toInt() - 1; break;
		}
	}
};

class CDistanceMapProps : public CPropertyList
{
public:
	CDistanceMapProps(Post::FEDistanceMap* map) : m_map(map)
	{
		addProperty("Assign to surface1", CProperty::Action, "");
		addProperty("Assign to surface2", CProperty::Action, "");
		addProperty("", CProperty::Action, "Apply");
	}

	QVariant GetPropertyValue(int i) override
	{
		if (i == 0)
		{
			int n = m_map->GetSurfaceSize(0);
			return QString("(%1 Faces)").arg(n);
		}
		if (i == 1)
		{
			int n = m_map->GetSurfaceSize(1);
			return QString("(%1 Faces)").arg(n);
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v) override
	{
		if (i == 0)
		{
			m_map->InitSurface(0);
			SetModified(true);
		}
		else if (i == 1)
		{
			m_map->InitSurface(1);
			SetModified(true);
		}
		else if (i == 2)
		{
			m_map->Apply();
		}
	}

private:
	Post::FEDistanceMap*	m_map;
};

class CAreaCoverageProps : public CPropertyList
{
public:
	CAreaCoverageProps(Post::FEAreaCoverage* map) : m_map(map)
	{
		addProperty("Assign to surface1", CProperty::Action, "");
		addProperty("Assign to surface2", CProperty::Action, "");
		addProperty("", CProperty::Action, "Apply");
	}

	QVariant GetPropertyValue(int i) override
	{
		if (i == 0)
		{
			int n = m_map->GetSurfaceSize(0);
			return QString("(%1 Faces)").arg(n);
		}
		if (i == 1)
		{
			int n = m_map->GetSurfaceSize(1);
			return QString("(%1 Faces)").arg(n);
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v) override
	{
		if (i == 0)
		{
			m_map->InitSurface(0);
			SetModified(true);
		}
		else if (i == 1)
		{
			m_map->InitSurface(1);
			SetModified(true);
		}
		else if (i == 2)
		{
			m_map->Apply();
		}
	}

private:
	Post::FEAreaCoverage*	m_map;
};


class CDataModel : public QAbstractTableModel
{
public:
	CDataModel(QWidget* pw) : QAbstractTableModel(pw), m_fem(0) {}

	void SetFEModel(Post::FEPostModel* pfem)
	{
		beginResetModel();
		m_fem = pfem;
		endResetModel();
	}

	Post::FEPostModel* GetFEModel() { return m_fem; }

	int rowCount(const QModelIndex& index) const
	{
		if (m_fem == 0) return 0;
		Post::FEDataManager& dm = *m_fem->GetDataManager();
		return dm.DataFields();
	}

	int columnCount(const QModelIndex& index) const { return 4; }

	QVariant headerData(int section, Qt::Orientation orient, int role) const
	{
		if ((orient == Qt::Horizontal) && (role == Qt::DisplayRole))
		{
			switch (section)
			{
			case 0: return QVariant(QString("Data field")); break;
			case 1: return QVariant(QString("Type")); break;
			case 2: return QVariant(QString("Class")); break;
			case 3: return QVariant(QString("Format")); break;
			}
		}
		return QAbstractTableModel::headerData(section, orient, role);
	}

	QVariant data(const QModelIndex& index, int role) const
	{
		const char* szclass[] = { "NODE", "FACE", "ELEM" };
		if (m_fem == 0) return QVariant();

		if (!index.isValid()) return QVariant();
		if (role == Qt::DisplayRole)
		{
			int nrow = index.row();
			int ncol = index.column();
			Post::FEDataManager& dm = *m_fem->GetDataManager();
			Post::FEDataField* pd = *dm.DataField(nrow);

			if (ncol == 0) return QString::fromStdString(pd->GetName());
			else if (ncol == 1) return QString(pd->TypeStr());
			else if (ncol == 2)
			{
				switch (pd->DataClass())
				{
				case Post::CLASS_NODE: return QString("NODE"); break;
				case Post::CLASS_FACE: return QString("FACE"); break;
				case Post::CLASS_ELEM: return QString("ELEM"); break;
				default:
					assert(false);
					return QString("(unknown");
				}
			}
			else if (ncol == 3)
			{
				switch (pd->Format())
				{
				case Post::DATA_NODE: return QString("NODE"); break;
				case Post::DATA_ITEM: return QString("ITEM"); break;
				case Post::DATA_COMP: return QString("MIXED"); break;
				case Post::DATA_REGION: return QString("REGION"); break;
				default:
					assert(false);
					return QString("(unknown)");
				}
			}
		}
		return QVariant();
	}

public:
	Post::FEPostModel*	m_fem;
};

class Ui::CPostDataPanel
{
public:
	CDataModel*	data;
	QTableView*	list;
	::CPropertyListForm*	m_prop;
	QLineEdit*	name;

	Post::FEDataField*	m_activeField;

public:
	void setupUi(::CPostDataPanel* parent)
	{
		m_activeField = 0;

		QVBoxLayout* pg = new QVBoxLayout(parent);
		pg->setMargin(0);

		const int BW = 60;
		const int BH = 23;

		QAction* addActionStd = new QAction("Standard ...", parent); addActionStd->setObjectName("AddStandard");
		QAction* addActionFile = new QAction("From file ...", parent); addActionFile->setObjectName("AddFromFile");
		QAction* addEquation = new QAction("Equation ...", parent); addEquation->setObjectName("AddEquation");
		QAction* addFilter = new QAction("Filter ...", parent); addFilter->setObjectName("AddFilter");

		QPushButton* pbAdd = new QPushButton(); //pbAdd->setFixedSize(BW, BH);
		pbAdd->setText("Add");

		QMenu* menu = new QMenu(parent);
		menu->addAction(addActionStd);
		menu->addAction(addActionFile);
		menu->addAction(addEquation);
		menu->addAction(addFilter);
		pbAdd->setMenu(menu);

		QPushButton* pbCpy = new QPushButton("Copy"); pbCpy->setObjectName("CopyButton"); //pbCpy->setFixedSize(BW, BH); 
		QPushButton* pbDel = new QPushButton("Delete"); pbDel->setObjectName("DeleteButton"); //pbDel->setFixedSize(BW, BH); 
		QPushButton* pbExp = new QPushButton("Export..."); pbExp->setObjectName("ExportButton"); //pbExp->setFixedSize(BW, BH); 

		QHBoxLayout* ph = new QHBoxLayout;
		ph->setSpacing(0);
		ph->addWidget(pbAdd);
		ph->addWidget(pbCpy);
		ph->addWidget(pbDel);
		ph->addWidget(pbExp);
		ph->addStretch();

		pg->addLayout(ph);

		QSplitter* psplitter = new QSplitter;
		psplitter->setOrientation(Qt::Vertical);
		pg->addWidget(psplitter);


		list = new QTableView;
		list->setObjectName(QStringLiteral("dataList"));
		list->setSelectionBehavior(QAbstractItemView::SelectRows);
		list->setSelectionMode(QAbstractItemView::SingleSelection);
		list->horizontalHeader()->setStretchLastSection(true);
		list->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		//		list->verticalHeader()->setDefaultSectionSize(24);
		list->verticalHeader()->hide();

		data = new CDataModel(list);
		list->setModel(data);

		psplitter->addWidget(list);

		m_prop = new ::CPropertyListForm;
		m_prop->setObjectName("props");

		QWidget* w = new QWidget;
		QVBoxLayout* l = new QVBoxLayout;
		l->setMargin(0);
		w->setLayout(l);

		l->addWidget(name = new QLineEdit); name->setObjectName("fieldName");
		l->addWidget(m_prop);

		psplitter->addWidget(w);

		QMetaObject::connectSlotsByName(parent);
	}
};

class Ui::CDlgAddDataFile
{
public:
	QLineEdit*	pfile;
	QLineEdit*	pname;
	QComboBox*	pclass;
	QComboBox*	ptype;

public:
	void setupUi(QDialog* parent)
	{
		QPushButton* buttonBrowse = new QPushButton("Browse...");
		buttonBrowse->setFixedWidth(75);
		QHBoxLayout* phb = new QHBoxLayout;
		phb->addWidget(pfile = new QLineEdit);
		phb->addWidget(buttonBrowse);

		QVBoxLayout* pv = new QVBoxLayout;
		QFormLayout* pl = new QFormLayout;
		pl->addRow("File:", phb);
		pl->addRow("Name:", pname = new QLineEdit);
		pname->setText(QString("DataField"));

		pclass = new QComboBox;
		pclass->addItem("Node");
		pclass->addItem("Face");
		pclass->addItem("Elem");

		ptype = new QComboBox;
		ptype->addItem("float (1 fl)");
		ptype->addItem("vec3f (3 fl)");
		ptype->addItem("mat3fs (6 fl)");

		pl->addRow("Class:", pclass);
		pl->addRow("Type:", ptype);

		pv->addLayout(pl);

		QDialogButtonBox* b = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		pv->addWidget(b);

		parent->setLayout(pv);

		QObject::connect(b, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(b, SIGNAL(rejected()), parent, SLOT(reject()));
		QObject::connect(buttonBrowse, SIGNAL(clicked()), parent, SLOT(onBrowse()));
	}
};

CDlgAddDataFile::CDlgAddDataFile(QWidget* parent) : QDialog(parent), ui(new Ui::CDlgAddDataFile)
{
	ui->setupUi(this);
}

void CDlgAddDataFile::accept()
{
	m_file = ui->pfile->text().toStdString();
	m_name = ui->pname->text().toStdString();
	m_nclass = ui->pclass->currentIndex();
	m_ntype = ui->ptype->currentIndex();

	QDialog::accept();
}

void CDlgAddDataFile::onBrowse()
{
	QString file = QFileDialog::getOpenFileName(this, "Open File");
	if (file.isEmpty() == false)
	{
		ui->pfile->setText(file);
	}
}

//=================================================================================================
class Ui::CDlgFilter
{
public:
	QLineEdit*	name;

	QComboBox* pselect;

	// scale filter
	QLineEdit* pscale;

	// smooth filter
	QLineEdit* ptheta;
	QLineEdit* piters;

	// math page
	QComboBox* poperation;
	QComboBox* poperand;

	QComboBox*	comp;

	QComboBox*	conv;

public:
	void setupUi(QDialog* parent)
	{
		// new name field
		QHBoxLayout* lname = new QHBoxLayout;
		lname->addWidget(new QLabel("Name:"));
		lname->addWidget(name = new QLineEdit);

		// filter choice
		pselect = new QComboBox;
		pselect->addItem("Scale");
		pselect->addItem("Smooth");
		pselect->addItem("Arithmetic");
		pselect->addItem("Gradient");
		pselect->addItem("Component");
		pselect->addItem("Fraction Anisotropy");
		pselect->addItem("Convert Format");

		QLabel* label;
		label = new QLabel("Filter:");
		label->setBuddy(pselect);

		QHBoxLayout* ph = new QHBoxLayout;
		ph->addWidget(label);
		ph->addWidget(pselect);

		QVBoxLayout* pvl = new QVBoxLayout;
		pvl->addLayout(lname);
		pvl->addLayout(ph);

		// scale filter
		QWidget* scalePage = new QWidget;
		QFormLayout* pform = new QFormLayout;
		pform->addRow("scale:", pscale = new QLineEdit); pscale->setValidator(new QDoubleValidator(-1e99, 1e99, 6));
		scalePage->setLayout(pform);

		// smooth filter
		QWidget* smoothPage = new QWidget;
		pform = new QFormLayout;
		pform->addRow("theta:", ptheta = new QLineEdit); ptheta->setValidator(new QDoubleValidator(0.0, 1.0, 6));
		pform->addRow("iterations:", piters = new QLineEdit); piters->setValidator(new QIntValidator(1, 1000));
		smoothPage->setLayout(pform);

		// math filter
		QWidget* mathPage = new QWidget;
		pform = new QFormLayout;
		pform->addRow("Operation:", poperation = new QComboBox);
		pform->addRow("Operand:", poperand = new QComboBox);
		mathPage->setLayout(pform);

		poperation->addItem("add");
		poperation->addItem("subtract");
		poperation->addItem("multiply");
		poperation->addItem("divide");
		poperation->addItem("least-square difference");

		// gradient page (doesn't need options)
		QWidget* gradPage = new QLabel("");

		// fractional anisotropy (doesn't need options)
		QWidget* faPage = new QLabel("");

		// array component
		QWidget* compPage = new QWidget;
		pform = new QFormLayout;
		pform->addRow("Component:", comp = new QComboBox);
		compPage->setLayout(pform);

		// format conversion
		QWidget* convPage = new QWidget;
		pform = new QFormLayout;
		pform->addRow("Format:", conv = new QComboBox);
		convPage->setLayout(pform);

		QStackedWidget* stack = new QStackedWidget;
		stack->addWidget(scalePage);
		stack->addWidget(smoothPage);
		stack->addWidget(mathPage);
		stack->addWidget(gradPage);
		stack->addWidget(compPage);
		stack->addWidget(faPage);
		stack->addWidget(convPage);

		pvl->addWidget(stack);

		QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		pvl->addWidget(buttonBox);

		parent->setLayout(pvl);

		QObject::connect(pselect, SIGNAL(currentIndexChanged(int)), stack, SLOT(setCurrentIndex(int)));
		QObject::connect(buttonBox, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), parent, SLOT(reject()));
	}
};

CDlgFilter::CDlgFilter(QWidget* parent) : QDialog(parent), ui(new Ui::CDlgFilter)
{
	ui->setupUi(this);

	ui->pscale->setText(QString::number(1.0));
	ui->ptheta->setText(QString::number(1.0));
	ui->piters->setText(QString::number(1));
}

void CDlgFilter::setDataOperands(const std::vector<QString>& opNames)
{
	for (int i = 0; i<(int)opNames.size(); ++i)
	{
		ui->poperand->addItem(opNames[i]);
	}
}

void CDlgFilter::setDataField(Post::FEDataField* pdf)
{
	ui->comp->clear();
	int n = pdf->components(Post::DATA_SCALAR);
	for (int i = 0; i<n; ++i)
	{
		std::string cname = pdf->componentName(i, Post::DATA_SCALAR);
		ui->comp->addItem(QString::fromStdString(cname));
	}

	Post::Data_Format frm = pdf->Format();
	ui->conv->clear();
	if (frm != Post::DATA_ITEM) ui->conv->addItem("ITEM", (int)Post::DATA_ITEM);
	if (frm != Post::DATA_NODE) ui->conv->addItem("NODE", (int)Post::DATA_NODE);
}

int CDlgFilter::getArrayComponent()
{
	return ui->comp->currentIndex();
}

void CDlgFilter::setDefaultName(const QString& name)
{
	ui->name->setText(name);
}

QString CDlgFilter::getNewName()
{
	return ui->name->text();
}

int CDlgFilter::getNewFormat()
{
	return ui->conv->currentData().toInt();
}

void CDlgFilter::accept()
{
	m_nflt = ui->pselect->currentIndex();

	m_scale = ui->pscale->text().toDouble();

	m_theta = ui->ptheta->text().toDouble();
	m_iters = ui->piters->text().toInt();

	m_nop = ui->poperation->currentIndex();
	m_ndata = ui->poperand->currentIndex();

	if ((m_nflt == 2) && (m_ndata < 0))
	{
		QMessageBox::critical(this, "Data Filter", "Invalid operand selection");
	}
	else QDialog::accept();
}

//=================================================================================================
CPostDataPanel::CPostDataPanel(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CPostDataPanel)
{
	ui->setupUi(this);
}

CPostDocument* CPostDataPanel::GetActiveDocument()
{
	return GetMainWindow()->GetPostDocument();
}

void CPostDataPanel::Update(bool breset)
{
	CPostDocument* pdoc = GetActiveDocument();
	if (pdoc)
	{
		Post::FEPostModel* oldFem = ui->data->GetFEModel();
		Post::FEPostModel* newFem = pdoc->GetFEModel();

		if ((oldFem != newFem) || breset)
		{
			ui->m_prop->setPropertyList(nullptr);
			ui->data->SetFEModel(pdoc->GetFEModel());
		}
	}
	else
	{
		ui->m_prop->setPropertyList(nullptr);
		ui->data->SetFEModel(nullptr);
	}
}

void CPostDataPanel::on_AddStandard_triggered()
{
	// NOTE: Make sure this list matches the list in Post::AddStandardDataField (FEDataField.cpp)
	QStringList items;
	items.push_back("Position");
	items.push_back("Initial position");
	items.push_back("Deformation gradient");
	items.push_back("Infinitesimal strain");
	items.push_back("Lagrange strain");
	items.push_back("Right Cauchy-Green");
	items.push_back("Right stretch");
	items.push_back("Biot strain");
	items.push_back("Right Hencky");
	items.push_back("Left Cauchy-Green");
	items.push_back("Left stretch");
	items.push_back("Left Hencky");
	items.push_back("Almansi strain");
	items.push_back("Volume");
	items.push_back("Volume ratio");
	items.push_back("Volume strain");
	items.push_back("Aspect ratio");
	items.push_back("1-Princ Curvature");
	items.push_back("2-Princ Curvature");
	items.push_back("Gaussian Curvature");
	items.push_back("Mean Curvature");
	items.push_back("RMS Curvature");
	items.push_back("Princ Curvature difference");
	items.push_back("Congruency");
	items.push_back("1-Princ Curvature vector");
	items.push_back("2-Princ Curvature vector");
	items.push_back("distance map");
	items.push_back("area coverage");

	bool ok = false;
	QString item = QInputDialog::getItem(this, "Select new data field", "data:", items, 0, false, &ok);
	if (ok)
	{
		if (Post::AddStandardDataField(*GetActiveDocument()->GetGLModel(), item.toStdString(), true) == false)
		{
			QMessageBox::critical(this, "Add Data Field", "Failed adding data");
		}

		// update the data list
		GetMainWindow()->UpdatePostToolbar();
		Update(true);
	}
}

void CPostDataPanel::on_AddFromFile_triggered()
{
	CPostDocument* doc = GetActiveDocument();
	if (doc->IsValid() == false)
	{
		QMessageBox::critical(this, "FEBio Studio", "No model data loaded");
		return;
	}

	CDlgAddDataFile dlg(this);
	if (dlg.exec())
	{
		bool bret = false;
		switch (dlg.m_nclass)
		{
		case 0: bret = Post::AddNodeDataFromFile(*doc->GetGLModel(), dlg.m_file.c_str(), dlg.m_name.c_str(), dlg.m_ntype); break;
		case 1: bret = Post::AddFaceDataFromFile(*doc->GetGLModel(), dlg.m_file.c_str(), dlg.m_name.c_str(), dlg.m_ntype); break;
		case 2: bret = Post::AddElemDataFromFile(*doc->GetGLModel(), dlg.m_file.c_str(), dlg.m_name.c_str(), dlg.m_ntype); break;
		default:
			assert(false);
		}

		if (bret == false)
		{
			QMessageBox::critical(this, "Add Data From File", "Failed reading data from file.");
		}

		// update the data list
		GetMainWindow()->UpdatePostToolbar();
		Update(true);
	}
}

void CPostDataPanel::on_AddEquation_triggered()
{
	CPostDocument& doc = *GetActiveDocument();
	if (doc.IsValid() == false) return;

	CDlgAddEquation dlg(this);
	if (dlg.exec())
	{
		QString name = dlg.GetDataName();

		int type = dlg.GetDataType();

		switch (type)
		{
		case 0:
		{
			QString eq = dlg.GetScalarEquation();
			if (eq.isEmpty()) eq = "";
			if (name.isEmpty()) name = eq;
			if (name.isEmpty()) name = "(empty)";

			// create new math data field
			Post::FEMathDataField* pd = new Post::FEMathDataField(name.toStdString());
			pd->SetEquationString(eq.toStdString());

			// add it to the model
			Post::FEPostModel& fem = *doc.GetFEModel();
			fem.AddDataField(pd);
		}
		break;
		case 1:
		{
			if (name.isEmpty()) name = "(empty)";

			QStringList s = dlg.GetVectorEquations();

			QString x = s.at(0);
			QString y = s.at(1);
			QString z = s.at(2);

			// create new math data field
			Post::FEMathVec3DataField* pd = new Post::FEMathVec3DataField(name.toStdString());
			pd->SetEquationStrings(x.toStdString(), y.toStdString(), z.toStdString());

			// add it to the model
			Post::FEPostModel& fem = *doc.GetFEModel();
			fem.AddDataField(pd);
		}
		break;
		case 2:
		{
			if (name.isEmpty()) name = "(empty)";
			QStringList s = dlg.GetMatrixEquations();

			// create new math data field
			Post::FEMathMat3DataField* pd = new Post::FEMathMat3DataField(name.toStdString());
			for (int i=0; i<9; ++i) pd->SetEquationString(i, s.at(i).toStdString());

			// add it to the model
			Post::FEPostModel& fem = *doc.GetFEModel();
			fem.AddDataField(pd);
		}
		};

		// update the data list
		GetMainWindow()->UpdatePostToolbar();
		Update(true);
	}
}

void CPostDataPanel::on_CopyButton_clicked()
{
	QItemSelectionModel* select = ui->list->selectionModel();
	QModelIndexList selRow = select->selectedRows();
	if (selRow.count() == 1)
	{
		int nsel = selRow.at(0).row();
		CPostDocument& doc = *GetActiveDocument();
		Post::FEPostModel& fem = *doc.GetFEModel();
		Post::FEDataManager& dm = *fem.GetDataManager();
		Post::FEDataField* pdf = *dm.DataField(nsel);
		if (pdf)
		{
			bool bret = false;
			QString name = QString::fromStdString(pdf->GetName());
			QString text = QInputDialog::getText(this, "Copy Data Field", "Name:", QLineEdit::Normal, QString("%1_copy").arg(name), &bret);
			if (bret)
			{
				std::string sname = text.toStdString();
				fem.CopyDataField(pdf, sname.c_str());
				Update(true);
			}
		}
	}
}

void CPostDataPanel::on_DeleteButton_clicked()
{
	QItemSelectionModel* select = ui->list->selectionModel();
	QModelIndexList selRow = select->selectedRows();
	if (selRow.count() == 1)
	{
		int nsel = selRow.at(0).row();
		CPostDocument& doc = *GetActiveDocument();
		Post::FEPostModel& fem = *doc.GetFEModel();
		Post::FEDataManager& dm = *fem.GetDataManager();
		Post::FEDataField* pdf = *dm.DataField(nsel);
		if (pdf)
		{
			QString name = QString::fromStdString(pdf->GetName());
			QString sz(QString("Are you sure you want to delete the \"%1\" data field?").arg(name));
			if (QMessageBox::question(this, "Delete Data Field", sz) == QMessageBox::Yes)
			{
				fem.DeleteDataField(pdf);
				Update(true);
			}
		}
	}
}

void CPostDataPanel::on_AddFilter_triggered()
{
	CMainWindow* wnd = GetMainWindow();
	QItemSelectionModel* select = ui->list->selectionModel();
	QModelIndexList selRow = select->selectedRows();
	if (selRow.count() == 1)
	{
		int nsel = selRow.at(0).row();
		CPostDocument& doc = *GetActiveDocument();
		Post::FEPostModel& fem = *doc.GetFEModel();
		Post::FEDataManager& dm = *fem.GetDataManager();
		Post::FEDataField* pdf = *dm.DataField(nsel);
		if (pdf)
		{
			// build a list of compatible data fields
			vector<QString> dataNames;
			vector<int> dataIds;
			for (int i = 0; i<dm.DataFields(); ++i)
			{
				Post::FEDataField* pdi = *dm.DataField(i);
				QString name = QString::fromStdString(pdi->GetName());
				if ((pdi != pdf) &&
					(pdi->DataClass() == pdf->DataClass()) &&
					(pdi->Format() == pdf->Format()) &&
					((pdi->Type() == pdf->Type()) || (pdi->Type() == Post::DATA_FLOAT)))
				{
					dataNames.push_back(name);
					dataIds.push_back(i);
				}
			}

			CDlgFilter dlg(this);
			dlg.setDataOperands(dataNames);
			dlg.setDataField(pdf);

			QString name = QString::fromStdString(pdf->GetName());
			QString newName = QString("%0_flt").arg(name);
			dlg.setDefaultName(newName);

			if (dlg.exec())
			{
				// get the name for the new field
				string sname = dlg.getNewName().toStdString();

				Post::FEDataField* newData = 0;
				bool bret = true;
				int nfield = pdf->GetFieldID();
				switch (dlg.m_nflt)
				{
				case 0:
				{
					newData = fem.CreateCachedCopy(pdf, sname.c_str());
					bret = DataScale(fem, newData->GetFieldID(), dlg.m_scale);
				}
				break;
				case 1:
				{
					newData = fem.CreateCachedCopy(pdf, sname.c_str());
					bret = DataSmooth(fem, newData->GetFieldID(), dlg.m_theta, dlg.m_iters);
				}
				break;
				case 2:
				{
					newData = fem.CreateCachedCopy(pdf, sname.c_str());
					Post::FEDataFieldPtr p = fem.GetDataManager()->DataField(dataIds[dlg.m_ndata]);
					bret = DataArithmetic(fem, newData->GetFieldID(), dlg.m_nop, (*p)->GetFieldID());
				}
				break;
				case 3:
				{
					// create new vector field for storing the gradient
					newData = new Post::FEDataField_T<Post::FENodeData<vec3f  > >(sname.c_str(), Post::EXPORT_DATA);
					fem.AddDataField(newData);

					// now, calculate gradient from scalar field
					bret = DataGradient(fem, newData->GetFieldID(), nfield);
				}
				break;
				case 4:
				{
					// create new field for storing the component
					newData = DataComponent(fem, pdf, dlg.getArrayComponent(), sname);
					bret = (newData != nullptr);
				}
				break;
				case 5:
				{
					newData = new Post::FEDataField_T<Post::FEElementData<float, Post::DATA_ITEM> >(sname.c_str(), Post::EXPORT_DATA);
					fem.AddDataField(newData);

					// calculate fractional anisotropy
					bret = DataFractionalAnsisotropy(fem, newData->GetFieldID(), nfield);
				}
				break;
				case 6:
				{
					int newformat = dlg.getNewFormat();
					newData = DataConvert(fem, pdf, newformat, sname);
					bret = (newData != nullptr);
				}
				break;
				default:
					QMessageBox::critical(this, "Data Filter", "Don't know this filter.");
				}

				if (bret == false)
				{
					if (newData) fem.DeleteDataField(newData);
					QMessageBox::critical(this, "Data Filter", "Cannot apply this filter.");
				}

				wnd->UpdatePostToolbar();
				Update(true);
				doc.UpdateFEModel(true);
				wnd->RedrawGL();
			}
		}
	}
}

void CPostDataPanel::on_ExportButton_clicked()
{
	QItemSelectionModel* select = ui->list->selectionModel();
	QModelIndexList selRow = select->selectedRows();
	if (selRow.count() == 1)
	{
		int nsel = selRow.at(0).row();
		CPostDocument& doc = *GetActiveDocument();
		Post::FEPostModel& fem = *doc.GetFEModel();
		Post::FEDataManager& dm = *fem.GetDataManager();
		Post::FEDataField* pdf = *dm.DataField(nsel);
		if (pdf)
		{
			QString file = QFileDialog::getSaveFileName(this, "Export Data");
			if (file.isEmpty() == false)
			{
				std::string sfile = file.toStdString();
				if (Post::ExportDataField(*doc.GetGLModel(), *pdf, sfile.c_str()) == false)
				{
					QMessageBox::critical(this, "Export Data", "Export Failed!");
				}
			}
		}
	}
	else QMessageBox::warning(this, "Export Data", "Please select a data field first.");
}

void CPostDataPanel::on_dataList_clicked(const QModelIndex& index)
{
	Post::FEPostModel* fem = ui->data->m_fem;
	Post::FEDataManager& dm = *fem->GetDataManager();
	int n = index.row();

	int nstates = fem->GetStates();

	Post::FEDataField* p = *dm.DataField(n);

	if ((dynamic_cast<Post::FECurvatureField*>(p)))
	{
		Post::FECurvatureField* pf = dynamic_cast<Post::FECurvatureField*>(p);
		ui->m_prop->setPropertyList(new CCurvatureProps(pf));
	}
	else if (dynamic_cast<Post::FEMathDataField*>(p))
	{
		Post::FEMathDataField* pm = dynamic_cast<Post::FEMathDataField*>(p);
		ui->m_prop->setPropertyList(new CMathDataProps(pm));
	}
	else if (dynamic_cast<Post::FEMathVec3DataField*>(p))
	{
		Post::FEMathVec3DataField* pm = dynamic_cast<Post::FEMathVec3DataField*>(p);
		ui->m_prop->setPropertyList(new CMathDataVec3Props(pm));
	}
	else if (dynamic_cast<Post::FEStrainDataField*>(p))
	{
		Post::FEStrainDataField* ps = dynamic_cast<Post::FEStrainDataField*>(p);
		ui->m_prop->setPropertyList(new CStrainProps(ps, nstates));
	}
	else if (dynamic_cast<Post::FEDistanceMap*>(p))
	{
		Post::FEDistanceMap* ps = dynamic_cast<Post::FEDistanceMap*>(p);
		ui->m_prop->setPropertyList(new CDistanceMapProps(ps));
	}
	else if (dynamic_cast<Post::FEAreaCoverage*>(p))
	{
		Post::FEAreaCoverage* ps = dynamic_cast<Post::FEAreaCoverage*>(p);
		ui->m_prop->setPropertyList(new CAreaCoverageProps(ps));
	}
	else ui->m_prop->setPropertyList(nullptr);

	ui->m_activeField = p;

	ui->name->setText(QString::fromStdString(p->GetName()));
}

void CPostDataPanel::on_fieldName_editingFinished()
{
	QString t = ui->name->text();
	if (ui->m_activeField && (t.isEmpty() == false))
	{
		ui->m_activeField->SetName(t.toStdString());
		Update(true);
		CPostDocument& doc = *GetActiveDocument();
//		doc.GetFEModel()->UpdateDependants();
	}
}

void CPostDataPanel::on_props_dataChanged(bool b)
{
	CPostDocument* doc = GetActiveDocument();
	doc->GetGLModel()->ResetAllStates();
	doc->UpdateFEModel(true);
	GetMainWindow()->RedrawGL();
}
