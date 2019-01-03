/*!
 * \brief Widget for entering a password that uses the password echo mode.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ui_PasswordEdit.h"

#include <QLineEdit>
#include <QPointer>
#include <QRegularExpressionValidator>
#include <QScopedPointer>
#include <QToolTip>
#include <QWidget>

namespace governikus
{

class PasswordEdit
	: public QWidget
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::PasswordEdit> mUi;

		int determindeWidth(int pNumChars);

	protected:
		virtual bool eventFilter(QObject* pObj, QEvent* pEvent) override;

	public:
		PasswordEdit(QWidget* pParent = nullptr);

		void setMaxLength(int pLength, bool pShrink = true);
		void configureValidation(const QRegularExpression& pExpression, const QString& pInvalidValueToolTip);
		void removeLastCharacter();
		QString text() const;
		void setDigitFieldInvalid(bool pMakeInvalid, const QString& pInvalidMessage);
		void clear();
		void setText(const QString& pText);
		void setCursorPosition(int pPosition);
		void setAccessibleName(const QString& pName);
		void setFocus();
		void setAlignment(Qt::Alignment pAlignment);

	Q_SIGNALS:
		void textEdited(const QString& pText);
		void selectionChanged();
		void fireBackspacePressedAndEmpty();
};

} // namespace governikus
