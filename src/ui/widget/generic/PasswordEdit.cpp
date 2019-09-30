/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */


#include "PasswordEdit.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include <QScopeGuard>
#else
#include "ScopeGuard.h"
#endif

#include <QKeyEvent>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(gui)

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


		virtual QValidator::State validate(QString& pInput, int& pPos) const override
		{
			QValidator::State state = QRegularExpressionValidator::validate(pInput, pPos);

			if (state == State::Invalid && !mInvalidValueToolTip.isNull())
			{
				auto* parentWidget = static_cast<QWidget*>(parent());
				QToolTip::showText(parentWidget->mapToGlobal(QPoint(0, 0)), mInvalidValueToolTip, parentWidget, QRect(), 3000);
			}

			return state;
		}


};

} // namespace governikus


PasswordEdit::PasswordEdit(QWidget* pParent)
	: QWidget(pParent)
	, mUi(new Ui::PasswordEdit())
{
	mUi->setupUi(this);
	mUi->lineEdit->installEventFilter(this);

	connect(mUi->lineEdit, &QLineEdit::textEdited, this, &PasswordEdit::textEdited);
	connect(mUi->lineEdit, &QLineEdit::selectionChanged, this, &PasswordEdit::selectionChanged);
}


int PasswordEdit::determindeWidth(int pNumChars)
{
	QLineEdit* const lineEdit = mUi->lineEdit;

	const QString currentText = lineEdit->text();
	const auto resetText = qScopeGuard([lineEdit, currentText] {
				lineEdit->setText(currentText);
			});

	// get the display text for a password of length pWidth
	lineEdit->setText(QString(pNumChars, QLatin1Char('6')));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
	const int displayTextWidth = lineEdit->fontMetrics().horizontalAdvance(lineEdit->displayText());
#else
	const int displayTextWidth = lineEdit->fontMetrics().width(lineEdit->displayText());
#endif

	// in  QLineEdit::sizeHint() the width is calculated as
	// 17th times the size of 'x' plus some magic margins.
	// So we calculate this margin by subtraction to set the content size correctly.
	const int widthHint = lineEdit->sizeHint().width();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
	const int margin = widthHint - 17 * lineEdit->fontMetrics().horizontalAdvance(QLatin1Char('x'));
#else
	const int margin = widthHint - 17 * lineEdit->fontMetrics().width(QLatin1Char('x'));
#endif
	return margin + displayTextWidth;
}


bool PasswordEdit::eventFilter(QObject* pObj, QEvent* pEvent)
{
	if (pEvent->type() == QEvent::KeyPress)
	{
		const QKeyEvent* const keyEvent = static_cast<QKeyEvent*>(pEvent);
		if (keyEvent->key() == Qt::Key_Backspace && text().isEmpty())
		{
			Q_EMIT fireBackspacePressedAndEmpty();
		}
	}
	return QObject::eventFilter(pObj, pEvent);
}


void PasswordEdit::setMaxLength(int pLength, bool pShrink)
{
	if (pShrink)
	{
		mUi->lineEdit->setFixedWidth(determindeWidth(pLength));
	}
	mUi->lineEdit->setMaxLength(pLength);
}


void PasswordEdit::configureValidation(const QRegularExpression& pExpression, const QString& pInvalidValueToolTip)
{
	mUi->lineEdit->setValidator(new RegExValidator(pExpression, pInvalidValueToolTip, this));
}


void PasswordEdit::removeLastCharacter()
{
	const QString current = mUi->lineEdit->text();
	if (current.isEmpty())
	{
		return;
	}
	mUi->lineEdit->setText(current.left(current.length() - 1));
}


QString PasswordEdit::text() const
{
	return mUi->lineEdit->text();
}


void PasswordEdit::setDigitFieldInvalid(bool pMakeInvalid, const QString& pInvalidMessage)
{
	if (pMakeInvalid)
	{
		mUi->lineEdit->setStyleSheet(QStringLiteral("background-color: red;"));
		QToolTip::showText(mapToGlobal(QPoint(0, 32)), pInvalidMessage, mUi->lineEdit, QRect(), 3000);
	}
	else
	{
		mUi->lineEdit->setStyleSheet(QString());
	}
}


void PasswordEdit::clear()
{
	mUi->lineEdit->clear();
}


void PasswordEdit::setText(const QString& pText)
{
	mUi->lineEdit->setText(pText);
}


void PasswordEdit::setCursorPosition(int pPosition)
{
	mUi->lineEdit->setCursorPosition(pPosition);
}


void PasswordEdit::setAccessibleName(const QString& pName)
{
	mUi->lineEdit->setAccessibleName(pName);
}


void PasswordEdit::setFocus()
{
	mUi->lineEdit->setFocus();
}


void PasswordEdit::setAlignment(Qt::Alignment pAlignment)
{
	mUi->lineEdit->setAlignment(pAlignment);
}


#include "PasswordEdit.moc"
