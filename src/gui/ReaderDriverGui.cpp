/*!
 * ReaderDriverGui.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ReaderDriverGui.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(gui)

ReaderDriverGui::ReaderDriverGui(QWidget* pParentWidget)
	: QObject(pParentWidget)
	, mDialog(nullptr)
{
}


ReaderDriverGui::~ReaderDriverGui()
{
}


void ReaderDriverGui::activate()
{
	if (!mDialog)
	{
		QWidget* dialogParent = qobject_cast<QWidget*>(parent());
		if (!dialogParent)
		{
			return;
		}

		mDialog = new ReaderDriverDialog(dialogParent);
		connect(mDialog, &ReaderDriverDialog::finished, this, &ReaderDriverGui::onFinished);
		connect(mDialog, &QDialog::finished, this, &ReaderDriverGui::fireFinished);
	}
	reactivate();
}


void ReaderDriverGui::deactivate()
{
	if (mDialog)
	{
		mDialog->close();
	}
}


void ReaderDriverGui::reactivate()
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
	Q_EMIT mDialog->fireUpdateContent();
}


void ReaderDriverGui::onFinished(int result)
{
	Q_UNUSED(result);

	mDialog = nullptr;
}
