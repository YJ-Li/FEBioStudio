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

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <MeshTools/FEProject.h>
#include "HelpDialog.h"
#include "WebDefines.h"

#ifdef WEBHELP
	#include <QWebEngineView>
#endif

class Ui::CHelpDialog
{
public:
#ifdef WEBHELP
	QPushButton* helpButton;
	QWebEngineView* helpView;
#endif

	QHBoxLayout* helpLayout;

public:
	void setupUi(QWidget* parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout;
		helpLayout = new QHBoxLayout;

#ifdef WEBHELP
		helpLayout->addWidget(helpView = new QWebEngineView, 2);
		helpView->setMinimumSize(600,400);
		helpView->setVisible(false);
#endif

		mainLayout->addLayout(helpLayout);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

#ifdef WEBHELP
		helpButton = new QPushButton("Help");
		helpButton->setCheckable(true);

		bb->addButton(helpButton, QDialogButtonBox::HelpRole);
#endif

		mainLayout->addWidget(bb);

		QObject::connect(bb, SIGNAL(accepted()), parent, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), parent, SLOT(reject()));
		QObject::connect(bb, SIGNAL(helpRequested()), parent, SLOT(on_help_clicked()));

		parent->setLayout(mainLayout);
	}
};


CHelpDialog::CHelpDialog(FEProject& prj, QWidget* parent) : QDialog(parent), ui(new Ui::CHelpDialog)
{
	ui->setupUi(this);

	m_module = prj.GetModule();
}

CHelpDialog::~CHelpDialog() { delete ui; }

void CHelpDialog::on_help_clicked()
{
#ifdef WEBHELP
	if(ui->helpButton->isChecked())
	{
		m_withoutHelp = size();
		// reset min size
		setMinimumSize(0,0);
		ui->helpView->setVisible(true);
		LoadPage();
		resize(m_withHelp);
	}
	else
	{
		m_withHelp = size();
		ui->helpView->setVisible(false);
		// reset min size
		setMinimumSize(0,0);
		resize(m_withoutHelp);
	}
#endif
}

void CHelpDialog::SetLeftSideLayout(QLayout* layout)
{
	ui->helpLayout->insertLayout(0, layout);
}

void CHelpDialog::LoadPage()
{
#ifdef WEBHELP
	// Make sure the help view is actually visible
	if (ui->helpView->isVisible() == false) return;

	QString oldURL = m_url;

	SetURL();

	if(!m_url.isEmpty())
	{
		if(m_url == UNSELECTED_HELP)
		{
			ui->helpView->setHtml(QString("<html><body><p><b>%1</b></p></body></html>").arg(m_unselectedHelp));
			return;
		}

		m_url.insert(0, currentManualURL);

		if(m_url != oldURL)
		{
			ui->helpView->load(m_url);
		}
	}
	else
	{
		ui->helpView->setHtml("<html><body><p><b>There is currently no help article available for this item.</b></p></body></html>");
	}
#endif
}
