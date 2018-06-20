/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "GuiUtils.h"

#include <QAbstractButton>
#include <QApplication>
#include <QFrame>
#include <QIcon>
#include <QMessageBox>

using namespace governikus;


void GuiUtils::showPinCanPukErrorDialog(CardReturnCode pReturnCode, int pRetryCounter, bool pCanAllowedMode, QWidget* pParent)
{
	QMessageBox messageBox(pParent);
	QString title;
	QString text;
	switch (pReturnCode)
	{
		case CardReturnCode::INVALID_CAN:
			title = tr("Wrong card access number (CAN)");
			if (pCanAllowedMode)
			{
				text = tr("The given card access number (CAN) is not correct.");
			}
			else
			{
				text = tr("The given card access number (CAN) is not correct. You have one more try to enter the correct PIN."
						  " Please mind that you have to acknowledge this last try with your card access"
						  " number (CAN).");
			}
			break;

		case CardReturnCode::INVALID_PUK:
			title = tr("Wrong PUK");
			text = tr("Please enter your PUK again.");
			break;

		case CardReturnCode::PUK_INOPERATIVE:
			title = tr("PUK is inoperative");
			text = tr("You have correctly entered the PUK ten times and have thus reached the maximum count."
					  " The PUK is now inoperative and can no longer be used for unblocking the PIN. Please address your"
					  " competent authority that has issued your ID card for unblocking your PIN.");
			break;

		case CardReturnCode::INVALID_PIN:
		default:
			title = tr("Wrong PIN");
			break;
	}

	if (text.isEmpty())
	{
		switch (pRetryCounter)
		{
			case 0:
				text = tr("After three wrong entries your PIN is blocked. Using the online identification"
						  " function is no longer possible. </p><p>You can unblock your PIN in the"
						  " following dialog. The program supports you with the steps now required.");
				break;

			case 1:
				text = tr("The given PIN is not correct. You have one more try to enter the correct PIN."
						  " Please mind that you have to acknowledge this last try with your card access"
						  " number (CAN).");
				break;

			default:
				text = tr("The given PIN is not correct. You have %1 tries to enter the correct PIN.").arg(pRetryCounter);
				break;
		}
	}

	messageBox.setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + title);
	messageBox.setWindowModality(Qt::WindowModal);
	messageBox.setWindowFlags(messageBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
	messageBox.setText(QStringLiteral("<h4>%1</h4><p>%2</p>").arg(title, text));
	messageBox.setIconPixmap(QIcon(QStringLiteral(":/images/npa.svg")).pixmap(32, 32));
	messageBox.setStandardButtons(QMessageBox::Ok);
	messageBox.button(QMessageBox::Ok)->setFocus();

	messageBox.exec();
}


bool GuiUtils::showWrongPinBlockedDialog(QWidget* pParent)
{
	QMessageBox messageBox(pParent);

	QString title = tr("PIN blocked");
	QString text = tr("After three wrong entries your PIN is blocked. Using the online identification"
					  " function is no longer possible. <br/>You can unblock the PIN as"
					  " follows:<ol><li> Select the \"Settings\" function.</li><li>Select the \"PIN"
					  " Management\" tab. </li><li>Follow the instructions on the"
					  " screen.</li></ol>Note: You will find the PUK in the letter you received during"
					  " the application for the ID card in the \"Unblocking key PUK\" section. Further"
					  " information is available on the site <a href=\"http://www.personalausweisportal"
					  ".de\">http://www.personalausweisportal.de</a>.<br>"
					  "Do you want to unblock the PIN now?");
	messageBox.setWindowTitle(QCoreApplication::applicationName() + QStringLiteral(" - ") + title);
	messageBox.setWindowModality(Qt::WindowModal);
	messageBox.setText(QStringLiteral("<h4>%1</h4><p>%2</p>").arg(title, text));
	messageBox.setIcon(QMessageBox::Warning);
	messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	messageBox.button(QMessageBox::Yes)->setFocus();

	return messageBox.exec() == QMessageBox::Yes;
}
