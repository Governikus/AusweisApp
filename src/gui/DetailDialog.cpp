/*!
 * DetailDialog.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */
#include "DetailDialog.h"
#include "DetailWidget.h"
#include "generic/HelpAction.h"
#include "ui_DetailDialog.h"
#include <QKeyEvent>

using namespace governikus;

DetailDialog::DetailDialog(QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::DetailDialog)
{
	mUi->setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	setWindowTitle(QCoreApplication::applicationName() + " - " + tr("Service provider data"));

	installEventFilter(this);

	connect(mUi->buttonBox, &QDialogButtonBox::rejected, this, &DetailDialog::close);
}


DetailDialog::~DetailDialog()
{
	delete mUi;
}


void DetailDialog::setDetails(const QString& pDetails)
{
	mUi->detailWidget->setDetails(pDetails);
	adjustSize();
}


bool DetailDialog::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}
	}
	return QDialog::eventFilter(pObject, pEvent);
}
