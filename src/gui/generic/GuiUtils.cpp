/*!
 * GuiUtils.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "GuiUtils.h"

#include <QApplication>
#include <QFrame>
#include <QIcon>
#include <QMessageBox>

using namespace governikus;

void GuiUtils::setFontBold(QWidget* pWidget, bool pBold)
{
	QFont font = pWidget->font();
	if (pBold != font.bold())
	{
		font.setBold(pBold);
		pWidget->setFont(font);
	}
}


QFrame* GuiUtils::createLine(bool pHorizontal)
{
	QFrame* line = new QFrame;
	line->setFrameShape(pHorizontal ? QFrame::HLine : QFrame::VLine);
	return line;
}


void GuiUtils::showPinCanPukErrorDialog(CardReturnCode pReturnCode, int pRetryCounter, QWidget* pParent)
{
	QMessageBox messageBox(pParent);
	QString title;
	QString text;
	switch (pReturnCode)
	{
		case CardReturnCode::INVALID_CAN:
			title = tr("Wrong card access number (CAN)");
			text = tr("The given card access number (CAN) is not correct. You have one more try to enter the correct PIN."
					  " Please mind that you have to acknowledge this last try with your card access"
					  " number (CAN).");
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

	messageBox.setWindowTitle(QCoreApplication::applicationName() + " - " + title);
	messageBox.setWindowModality(Qt::WindowModal);
	messageBox.setText(QStringLiteral("<h4>%1</h4><p>%2</p>").arg(title, text));
	messageBox.setIconPixmap(QIcon(QStringLiteral(":/images/npa.svg")).pixmap(32, 32));
	messageBox.setStandardButtons(QMessageBox::StandardButton::Ok);

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
	messageBox.setWindowTitle(QCoreApplication::applicationName() + " - " + title);
	messageBox.setWindowModality(Qt::WindowModal);
	messageBox.setText(QStringLiteral("<h4>%1</h4><p>%2</p>").arg(title, text));
	messageBox.setIcon(QMessageBox::Warning);

	messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	return messageBox.exec() == QMessageBox::Yes;
}


void GuiUtils::updateFontSize(QWidget* pWidget)
{
#ifdef Q_OS_OSX
	return;

#endif

	const qreal factor = 1.2;

	QFont newFont = pWidget->font();

	if (newFont.pixelSize() > 0)
	{
		// resize by factor, but add at least one pixel
		int newPixelSize = qRound(newFont.pixelSize() * factor);
		if (newPixelSize == newFont.pixelSize())
		{
			newPixelSize += 1;
		}

		newFont.setPixelSize(newPixelSize);
	}
	else
	{
		newFont.setPointSizeF(newFont.pointSizeF() * factor);
	}

	pWidget->setFont(newFont);
}
