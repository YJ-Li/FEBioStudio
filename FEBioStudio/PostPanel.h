#pragma once
#include "CommandPanel.h"

namespace Ui {
	class CPostPanel;
}

namespace Post {
	class CGLObject;
}

class CPostPanel : public CCommandPanel
{
	Q_OBJECT

public:
	CPostPanel(CMainWindow* wnd, QWidget* parent = 0);

	void Update(bool breset = true) override;

	void Reset();

	void Apply();

	void SelectObject(Post::CGLObject* po);

	void OnViewChanged();

public slots:
	void on_postTab_currentChanged(int index);

private:
	Ui::CPostPanel*	ui;
};
