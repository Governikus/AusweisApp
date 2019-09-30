/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "RemotePinInputDialog.h"
#include "ui_RemotePinInputDialog.h"

#include "generic/PasswordEdit.h"

#include <QDialogButtonBox>
#include <QLoggingCategory>
#include <QPushButton>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(gui)


RemotePinInputDialog::RemotePinInputDialog(QWidget* pParent)
	: QDialog(pParent, Qt::WindowTitleHint)
	, mUi(new Ui::RemotePinInputDialog())
{
	mUi->setupUi(this);
	connect(mUi->buttonBox, &QDialogButtonBox::accepted, this, &RemotePinInputDialog::onOkClicked);
	connect(mUi->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	mUi->pinEntryLine->setMaxLength(4, false);
	mUi->pinEntryLine->setAlignment(Qt::AlignCenter);
	QRegularExpression onlyNumbersExpression(QStringLiteral("[0-9]*"));
	mUi->pinEntryLine->configureValidation(onlyNumbersExpression, tr("Only digits (0-9) are allowed."));
}


RemotePinInputDialog::~RemotePinInputDialog()
{
}


const QString RemotePinInputDialog::getPin(QWidget* pParent)
{
	RemotePinInputDialog dialog(pParent);
	if (dialog.exec() == QDialog::Rejected)
	{
		return QString();
	}

	if (isValidPin(dialog.getPinEntry()))
	{
		return dialog.getPinEntry();
	}

	qCCritical(gui) << "Pairing code entered was not valid (4 Digits)";
	return QString();
}


bool RemotePinInputDialog::isValidPin(const QString& pPin)
{
	return QRegularExpression(QStringLiteral("[0-9]{4}")).match(pPin).hasMatch();
}


QString RemotePinInputDialog::getPinEntry() const
{
	return mUi->pinEntryLine->text();
}


void RemotePinInputDialog::onOkClicked()
{
	if (isValidPin(getPinEntry()))
	{
		accept();
	}
	else
	{
		QToolTip::showText(mUi->pinEntryLine->mapToGlobal(QPoint(0, 0)), tr("A pairing code has to be 4 digits long."), mUi->pinEntryLine, QRect(), 3000);
	}
}
