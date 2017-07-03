/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "generic/PasswordLineEdit.h"

#include <QRegularExpressionValidator>
#include <QToolTip>
#include <QWidget>

using namespace governikus;

namespace governikus
{
class RegExValidator
	: public QRegularExpressionValidator
{
	Q_OBJECT

	private:
		const QString mInvalidValueToolTip;

	public:
		RegExValidator(const QRegularExpression& pExpression, const QString& pInvalidValueToolTip, QWidget* pParent)
			: QRegularExpressionValidator(pExpression, pParent)
			, mInvalidValueToolTip(pInvalidValueToolTip)
		{
		}


		virtual QValidator::State validate(QString& pInput, int& pPos) const
		{
			QValidator::State state = QRegularExpressionValidator::validate(pInput, pPos);

			if (state == State::Invalid && !mInvalidValueToolTip.isNull())
			{
				QWidget* parentWidget = static_cast<QWidget*>(this->parent());
				QToolTip::showText(parentWidget->mapToGlobal(QPoint(0, 0)), mInvalidValueToolTip, parentWidget, QRect(), 3000);
			}

			return state;
		}


};

}

PasswordLineEdit::PasswordLineEdit(QWidget* pParent)
	: QLineEdit(pParent)
{
	setEchoMode(QLineEdit::Password);
	setInputMethodHints(inputMethodHints() | Qt::ImhHiddenText | Qt::ImhSensitiveData | Qt::ImhPreferNumbers | Qt::ImhDigitsOnly | Qt::ImhFormattedNumbersOnly);
}


PasswordLineEdit::~PasswordLineEdit()
{
}


void PasswordLineEdit::configureValidation(const QRegularExpression& pExpression, const QString& pInvalidValueToolTip)
{
	setValidator(new RegExValidator(pExpression, pInvalidValueToolTip, this));
}


void PasswordLineEdit::setFieldWidth(int pWidth)
{
	// get the display text for a password of length pWidth
	setText(QString(pWidth, QLatin1Char('6')));
	int displayTextWidth = fontMetrics().width(displayText());
	clear();

	// in  QLineEdit::sizeHint() the width is calculated as
	// 17th times the size of 'x' plus some magic margins.
	// So we calculate this margin by subtraction to set the content size correctly.
	int widthHint = sizeHint().width();
	int margin = widthHint - 17 * fontMetrics().width(QLatin1Char('x'));

	setFixedWidth(margin + displayTextWidth);
}


#include "PasswordLineEdit.moc"
