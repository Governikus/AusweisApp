/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "DiagnosisGui.h"
#include "DiagnosisWidget.h"

using namespace governikus;

DiagnosisGui::DiagnosisGui(QWidget* pParentWidget)
	: QObject(pParentWidget)
	, mDialog()
{
}


DiagnosisGui::~DiagnosisGui()
{
}


void DiagnosisGui::activate()
{
	if (mDialog)
	{
		if (mDialog->isMinimized())
		{
			mDialog->showNormal();
		}
		if (!mDialog->isVisible())
		{
			mDialog->show();
		}
		mDialog->activateWindow();
		mDialog->raise();
		return;
	}

	QWidget* dialogParent = qobject_cast<QWidget*>(parent());
	if (!dialogParent)
	{
		return;
	}

	auto context = new DiagnosisContext();
	mDialog = new DiagnosisDialog(context, dialogParent);
	connect(mDialog, &QDialog::finished, this, &DiagnosisGui::fireFinished);
	mDialog->show();

	auto controller = new DiagnosisController(context, mDialog);
	controller->run();
}


void DiagnosisGui::deactivate()
{
	if (mDialog)
	{
		mDialog->close();
	}
}
