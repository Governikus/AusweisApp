/*!
 * ReaderDriverDialog.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "generic/HelpAction.h"
#include "ReaderDriverDialog.h"
#include "ui_ReaderDriverDialog.h"


#include <QKeyEvent>


using namespace governikus;

ReaderDriverDialog::ReaderDriverDialog(QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::ReaderDriverDialog)
	, mReaderDriverWidget(new ReaderDriverWidget(pParent))
{
	mUi->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
	setWindowTitle(QCoreApplication::applicationName() + " - " + tr("Reader Driver Integration"));

	installEventFilter(this);

	mUi->readerDriverLayout->addWidget(mReaderDriverWidget);

	connect(mUi->closeButton, &QAbstractButton::clicked, this, &ReaderDriverDialog::close);
	connect(this, &ReaderDriverDialog::fireUpdateContent, mReaderDriverWidget, &ReaderDriverWidget::onUpdateContent);
}


ReaderDriverDialog::~ReaderDriverDialog()
{
}


bool ReaderDriverDialog::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent* const keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}
	}
	return QDialog::eventFilter(pObject, pEvent);
}
