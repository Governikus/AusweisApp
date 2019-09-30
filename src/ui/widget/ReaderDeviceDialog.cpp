/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderDeviceDialog.h"
#include "ui_ReaderDeviceDialog.h"

#include "HelpAction.h"

#include <QKeyEvent>


using namespace governikus;

ReaderDeviceDialog::ReaderDeviceDialog(QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::ReaderDeviceDialog)
	, mReaderDeviceWidget(new ReaderDeviceWidget(pParent))
{
	mUi->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
	setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Reader Driver Integration"));

	installEventFilter(this);

	mUi->readerDriverLayout->addWidget(mReaderDeviceWidget);

	connect(mUi->closeButton, &QAbstractButton::clicked, this, &ReaderDeviceDialog::close);
}


ReaderDeviceDialog::~ReaderDeviceDialog()
{
}


bool ReaderDeviceDialog::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		auto* const keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}
	}
	return QDialog::eventFilter(pObject, pEvent);
}


void ReaderDeviceDialog::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
