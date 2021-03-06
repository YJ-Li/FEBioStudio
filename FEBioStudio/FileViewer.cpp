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
#include "FileViewer.h"
#include "MainWindow.h"
#include "DocManager.h"
#include "Document.h"
#include "ModelDocument.h"
#include "PostDocument.h"
#include "FEBioStudioProject.h"
#include <QTreeWidget>
#include <QFileSystemModel>
#include <QBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QSignalMapper>
#include <QInputDialog>
#include <QMessageBox>

enum FileItemType {
	OPEN_FILES,
	OPEN_FILE,
	PROJECT,
	PROJECT_GROUP,
	PROJECT_FILE,
	EXTERNAL_FILE
};

class Ui::CFileViewer
{
public:
	::CMainWindow*	m_wnd;
	QTreeWidget*	m_tree;

public:
	void setupUi(QWidget* parent)
	{
		QVBoxLayout* l = new QVBoxLayout(parent);
		l->setMargin(0);

		m_tree = new QTreeWidget;
		m_tree->setColumnCount(1);
		m_tree->header()->hide();
		m_tree->setObjectName("fileList");
		m_tree->setUniformRowHeights(true);
		
		l->addWidget(m_tree);

		parent->setLayout(l);
	}
};

CFileViewer::CFileViewer(CMainWindow* pwnd, QWidget* parent) : QWidget(parent), ui(new Ui::CFileViewer)
{
	ui->m_wnd = pwnd;
	// build Ui
	ui->setupUi(this);

	QMetaObject::connectSlotsByName(this);
}

void CFileViewer::on_fileList_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	int ntype = item->data(0, Qt::UserRole).toInt();

	switch (ntype)
	{
	case FileItemType::OPEN_FILE:
	{
		int nview = item->data(0, Qt::UserRole + 1).toInt();
		ui->m_wnd->SetActiveView(nview);
	}
	break;
	case FileItemType::PROJECT_FILE:
	{
		int itemId = item->data(0, Qt::UserRole + 1).toInt();
		const FEBioStudioProject* prj = ui->m_wnd->GetProject();
		const FEBioStudioProject::ProjectItem* file = prj->FindFile(itemId); assert(file);
		QString filePath = file->Name();
		CDocument* doc = ui->m_wnd->FindDocument(filePath.toStdString());
		if (doc)
			ui->m_wnd->SetActiveDocument(doc);
		else
			ui->m_wnd->OpenFile(filePath, false);
	}
	break;
	case FileItemType::EXTERNAL_FILE:
	{
		QString filePath = item->data(0, Qt::UserRole + 1).toString();
		CDocument* doc = ui->m_wnd->FindDocument(filePath.toStdString());
		if (doc)
			ui->m_wnd->SetActiveDocument(doc);
		else
			ui->m_wnd->OpenFile(filePath, false);
	}
	break;
	}
}

void CFileViewer::contextMenuEvent(QContextMenuEvent* ev)
{
	QList<QTreeWidgetItem*> sel = ui->m_tree->selectedItems();
	if (sel.size() != 1) return;

	const FEBioStudioProject* prj = ui->m_wnd->GetProject();

	int ntype = sel[0]->data(0, Qt::UserRole).toInt();
	if (ntype == FileItemType::OPEN_FILE)
	{
		CDocManager* dm = ui->m_wnd->GetDocManager();
		int n = sel[0]->data(0, Qt::UserRole + 1).toInt();
		QString file = QString::fromStdString(dm->GetDocument(n)->GetDocFilePath());

		QMenu menu(this);

		QSignalMapper* closeMap = new QSignalMapper(&menu);
		QAction* ac = menu.addAction("Close", closeMap, SLOT(map())); closeMap->setMapping(ac, file);
		connect(closeMap, SIGNAL(mapped(QString)), ui->m_wnd, SLOT(on_closeFile(const QString&)));

		if (prj->ContainsFile(file) == false)
		{
			QSignalMapper* addMap = new QSignalMapper(&menu);
			QAction* ac = menu.addAction("Add to project", addMap, SLOT(map())); addMap->setMapping(ac, file);
			connect(addMap, SIGNAL(mapped(QString)), ui->m_wnd, SLOT(on_addToProject(const QString&)));
		}

#ifdef _WIN32
		menu.addAction("Show in Explorer", this, SLOT(onShowInExplorer()));
#endif

		menu.exec(ev->globalPos());
	}
	if (ntype == FileItemType::PROJECT)
	{
		QMenu menu(this);
		menu.addAction("Save Project As ...", ui->m_wnd, SLOT(on_actionSaveProject_triggered()));
		menu.addAction("Create Group ...", this, SLOT(onCreateGroup()));
		menu.addAction("Close project", ui->m_wnd, SLOT(on_closeProject()));
		menu.addAction("Clear project", ui->m_wnd, SLOT(on_clearProject()));

#ifdef _WIN32
		const FEBioStudioProject* prj = ui->m_wnd->GetProject();
		if (prj->GetProjectFileName().isEmpty() == false)
		{
			menu.addAction("Show in Explorer", this, SLOT(onShowInExplorer()));
		}
#endif

		menu.exec(ev->globalPos());
	}
	else if (ntype == FileItemType::PROJECT_GROUP)
	{
		QMenu menu(this);
		menu.addAction("Remove Group", this, SLOT(onRemoveGroup()));
		menu.addAction("Rename Group ...", this, SLOT(onRenameGroup()));
		menu.addAction("Add Group ...", this, SLOT(onCreateGroup()));
		menu.exec(ev->globalPos());
	}
	else if (ntype == FileItemType::PROJECT_FILE)
	{
		QMenu menu(this);
		int fileId = sel[0]->data(0, Qt::UserRole + 1).toInt();


		QAction* ac = menu.addAction("Remove from project", this, SLOT(onRemoveFromProject()));

		const FEBioStudioProject::ProjectItem* prjItem = prj->FindFile(fileId);
		if (prjItem && (prjItem->Parent()))
		{
			QSignalMapper* groupmap = new QSignalMapper;
			QMenu* groupMenu = new QMenu("Move to group");
			std::vector<int> groups = prjItem->Parent()->AllGroups();
			for (int i = 0; i < groups.size(); ++i)
			{
				const FEBioStudioProject::ProjectItem* group_i = prj->FindGroup(groups[i]);
				QAction* aci = groupMenu->addAction(group_i->Name(), groupmap, SLOT(map())); groupmap->setMapping(aci, group_i->Id());
			}
			QAction* aci = groupMenu->addAction("(none)", groupmap, SLOT(map())); groupmap->setMapping(aci, -1);
			menu.addAction(groupMenu->menuAction());
			connect(groupmap, SIGNAL(mapped(int)), this, SLOT(onMoveToGroup(int)));
		}


#ifdef _WIN32
		menu.addAction("Show in Explorer", this, SLOT(onShowInExplorer()));
#endif
		menu.exec(ev->globalPos());
	}
}

void CFileViewer::onShowInExplorer()
{
#ifdef WIN32
	QList<QTreeWidgetItem*> itemList = ui->m_tree->selectedItems();
	if (itemList.size() != 1) return;

	const FEBioStudioProject* prj = ui->m_wnd->GetProject();
	QString file;
	int ntype = itemList[0]->data(0, Qt::UserRole).toInt();
	if (ntype == FileItemType::OPEN_FILE)
	{
		CDocManager* dm = ui->m_wnd->GetDocManager();
		int n = itemList[0]->data(0, Qt::UserRole + 1).toInt();
		file = QString::fromStdString(dm->GetDocument(n)->GetDocFilePath());
	}
	else if (ntype == FileItemType::PROJECT_FILE)
	{
		int id = itemList[0]->data(0, Qt::UserRole + 1).toInt();
		file = prj->FindFile(id)->Name();
	}
	else if (ntype == FileItemType::EXTERNAL_FILE)
	{
		file = itemList[0]->data(0, Qt::UserRole + 1).toString();
	}
	else if (ntype == FileItemType::PROJECT)
	{
		file = prj->GetProjectFileName();
	}

	if (file.isEmpty() == false)
	{
		QProcess::startDetached("explorer.exe", { "/select,", QDir::toNativeSeparators(file) });
	}
#endif
}

void addProjectGroup(const FEBioStudioProject::ProjectItem& parent, QTreeWidgetItem* treeItem, CMainWindow* wnd)
{
	// add groups first
	for (int n = 0; n < parent.Items(); ++n)
	{
		const FEBioStudioProject::ProjectItem& item = parent.Item(n);

		if (item.IsGroup())
		{
			QTreeWidgetItem* t2 = new QTreeWidgetItem(treeItem);
			t2->setText(0, item.Name());
			t2->setData(0, Qt::UserRole, FileItemType::PROJECT_GROUP);
			t2->setData(0, Qt::UserRole + 1, item.Id());
			QFont f = t2->font(0);
			f.setBold(true);
			t2->setFont(0, f);
			addProjectGroup(item, t2, wnd);
		}
	}

	// add files next
	for (int n = 0; n < parent.Items(); ++n)
	{
		const FEBioStudioProject::ProjectItem& item = parent.Item(n);

		if (item.IsFile())
		{
			QString filename_i = item.Name();

			CDocument* doc = wnd->FindDocument(filename_i.toStdString());

			if (doc && (doc->GetDocFilePath().empty() == false))
			{
				QString docFile = QString::fromStdString(doc->GetDocFileName());
				QString docPath = QString::fromStdString(doc->GetDocFilePath());

				QTreeWidgetItem* t2 = new QTreeWidgetItem(treeItem);
				t2->setText(0, docFile);
				t2->setToolTip(0, docPath);
				t2->setData(0, Qt::UserRole, FileItemType::PROJECT_FILE);
				t2->setData(0, Qt::UserRole + 1, item.Id());

				CModelDocument* modelDoc = dynamic_cast<CModelDocument*>(doc);
				if (modelDoc && modelDoc->FEBioJobs())
				{
					t2->setExpanded(true);
					for (int n = 0; n < modelDoc->FEBioJobs(); ++n)
					{
						CFEBioJob* job = modelDoc->GetFEBioJob(n);

						std::string plotFile = job->GetPlotFileName();
						QString xpltPath(doc->ToAbsolutePath(plotFile));

						QFileInfo xpltFile(xpltPath);

						CPostDocument* postDoc = dynamic_cast<CPostDocument*>(wnd->FindDocument(xpltPath.toStdString()));

						QTreeWidgetItem* t3 = new QTreeWidgetItem(t2);
						t3->setText(0, xpltFile.fileName());
						t3->setToolTip(0, xpltPath);
						t3->setData(0, Qt::UserRole, FileItemType::EXTERNAL_FILE);
						t3->setData(0, Qt::UserRole + 1, xpltPath);

						if (postDoc == nullptr)
						{
							QFont f = t3->font(0);
							f.setItalic(true);
							t3->setFont(0, f);
							t3->setForeground(0, Qt::gray);
						}
					}
				}
			}
			else
			{
				QFileInfo fi(filename_i);
				QString fileName = fi.fileName();

				QTreeWidgetItem* t2 = new QTreeWidgetItem(treeItem);
				t2->setText(0, fileName);

				QFont f = t2->font(0);
				f.setItalic(true);
				t2->setFont(0, f);
				t2->setForeground(0, Qt::gray);

				t2->setSizeHint(0, QSize(100, 50));
				t2->setToolTip(0, filename_i);
				t2->setData(0, Qt::UserRole, FileItemType::PROJECT_FILE);
				t2->setData(0, Qt::UserRole + 1, item.Id());
			}
		}
	}
}

void CFileViewer::Update()
{
	ui->m_tree->clear();

	CDocManager* dm = ui->m_wnd->GetDocManager();

	// Open files list
	QTreeWidgetItem* it = new QTreeWidgetItem(QStringList("OPEN FILES"));
	it->setData(0, Qt::UserRole, FileItemType::OPEN_FILES);
	QFont f = it->font(0);
	f.setBold(true);
	it->setFont(0, f);
	ui->m_tree->addTopLevelItem(it);
	it->setExpanded(true);

	QFontInfo fi(f);

	int px = fi.pixelSize();
	px = 5 * px / 3;
	it->setSizeHint(0, QSize(100, px));
	
	for (int i = 0; i < dm->Documents(); ++i)
	{
		CDocument* doc = dm->GetDocument(i);
		QString docPath = QString::fromStdString(doc->GetDocFilePath());

		QTreeWidgetItem* t2 = new QTreeWidgetItem(it);
		t2->setText(0, QString::fromStdString(doc->GetDocTitle()));
		t2->setData(0, Qt::UserRole  , FileItemType::OPEN_FILE);
		t2->setData(0, Qt::UserRole+1, i);
		if (docPath.isEmpty() == false)
		{
			t2->setToolTip(0, docPath);
		}
		t2->setSizeHint(0, QSize(100, px));
	}

	// Project list
	const FEBioStudioProject* prj = ui->m_wnd->GetProject();
	QString prjFile = prj->GetProjectFileName();

	QString prjName = "unsaved";
	if (prjFile.isEmpty() == false)
	{
		QFileInfo fi(prjFile);
		prjName = fi.fileName();
	}

	it = new QTreeWidgetItem(ui->m_tree);
	it->setText(0, QString("PROJECT (%1)").arg(prjName));
	it->setFont(0, f);
	ui->m_tree->addTopLevelItem(it);
	it->setExpanded(true);
	it->setSizeHint(0, QSize(0, px));
	it->setData(0, Qt::UserRole, FileItemType::PROJECT);
	if (prjFile.isEmpty() == false) it->setToolTip(0, prjFile);

	const FEBioStudioProject::ProjectItem& root = prj->RootItem();
	addProjectGroup(root, it, ui->m_wnd);

	ui->m_tree->expandAll();
}

QTreeWidgetItem* CFileViewer::currentItem()
{
	QList<QTreeWidgetItem*> sel = ui->m_tree->selectedItems();
	if (sel.size() != 1) return nullptr;
	return sel[0];
}

void CFileViewer::onCreateGroup()
{
	QTreeWidgetItem* item = CFileViewer::currentItem();
	if (item == nullptr) return;

	int ntype = item->data(0, Qt::UserRole).toInt();
	if ((ntype == FileItemType::PROJECT) || ( ntype == FileItemType::PROJECT_GROUP))
	{
		QString groupName = QInputDialog::getText(this, "Create Group", "Group name:");
		if (groupName.isEmpty() == false)
		{
			FEBioStudioProject* prj = ui->m_wnd->GetProject();
			int groupId = item->data(0, Qt::UserRole + 1).toInt();
			prj->AddGroup(groupName, groupId);

			Update();
			QList<QTreeWidgetItem*> itemList = ui->m_tree->findItems(groupName, Qt::MatchExactly | Qt::MatchRecursive);
			if (itemList.empty() == false)
			{
				QTreeWidgetItem* it = itemList[0];
				it->setSelected(true);
				it = it->parent();
				while (it) { it->setExpanded(true); it = it->parent(); }
			}
		}
	}
}

void CFileViewer::onMoveToGroup(int i)
{
	QTreeWidgetItem* item = CFileViewer::currentItem();
	int ntype = item->data(0, Qt::UserRole).toInt();
	if (ntype == PROJECT_FILE)
	{
		int itemId = item->data(0, Qt::UserRole + 1).toInt();
		FEBioStudioProject* prj = ui->m_wnd->GetProject();
		prj->MoveToGroup(itemId, i);

		QString txt = item->text(0);
		Update();
		QList<QTreeWidgetItem*> itemList = ui->m_tree->findItems(txt, Qt::MatchExactly | Qt::MatchRecursive);
		if (itemList.empty() == false)
		{
			QTreeWidgetItem* it = itemList[0];
			it->setSelected(true);
			it = it->parent();
			while (it) { it->setExpanded(true); it = it->parent(); }
		}
	}
}

void CFileViewer::onRemoveGroup()
{
	QTreeWidgetItem* item = CFileViewer::currentItem();
	int ntype = item->data(0, Qt::UserRole).toInt();
	if (ntype == PROJECT_GROUP)
	{
		if (QMessageBox::question(this, "Remove Group", "Are you sure you want to remove this group?\nThis cannot be undone!") == QMessageBox::Yes)
		{
			int itemId = item->data(0, Qt::UserRole + 1).toInt();
			FEBioStudioProject* prj = ui->m_wnd->GetProject();
			prj->RemoveGroup(itemId);
			Update();
		}
	}
}

void CFileViewer::onRenameGroup()
{
	QTreeWidgetItem* item = CFileViewer::currentItem();
	int ntype = item->data(0, Qt::UserRole).toInt();
	if (ntype == PROJECT_GROUP)
	{
		QString newName = QInputDialog::getText(this, "Rename Group", "New name:");
		if (newName.isEmpty() == false)
		{
			int itemId = item->data(0, Qt::UserRole + 1).toInt();
			FEBioStudioProject* prj = ui->m_wnd->GetProject();
			prj->RenameGroup(itemId, newName);
			item->setText(0, newName);
		}
	}
}

void CFileViewer::onRemoveFromProject()
{
	QTreeWidgetItem* item = CFileViewer::currentItem();
	int ntype = item->data(0, Qt::UserRole).toInt();
	if (ntype == PROJECT_FILE)
	{
		if (QMessageBox::question(this, "Remove", "Are you sure you want to remove this file from the project?\nThis cannot be undone.") == QMessageBox::Yes)
		{
			FEBioStudioProject* prj = ui->m_wnd->GetProject();
			int itemId = item->data(0, Qt::UserRole + 1).toInt();
			prj->RemoveFile(itemId);
			Update();
		}
	}
}
