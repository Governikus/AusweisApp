/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DetailDialog.h"

#include "ui_DetailDialog.h"

#include "DetailWidget.h"
#include "HelpAction.h"

#include <QKeyEvent>
#include <QPushButton>

using namespace governikus;

DetailDialog::DetailDialog(QWidget* pParent)
	: QDialog(pParent)
	, mUi(new Ui::DetailDialog)
{
	mUi->setupUi(this);

	setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + tr("Service provider data"));

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
	mUi->buttonBox->button(QDialogButtonBox::Close)->setFocus();
	adjustSize();
}


bool DetailDialog::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		auto* keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_F1)
		{
			HelpAction::openContextHelp();
			return true;
		}
	}
	return QDialog::eventFilter(pObject, pEvent);
}


void DetailDialog::changeEvent(QEvent* pEvent)
{
	if (pEvent->type() == QEvent::LanguageChange)
	{
		mUi->retranslateUi(this);
	}
	QWidget::changeEvent(pEvent);
}
