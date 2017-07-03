/*!
 * PasswordEdit.h
 *
 * \brief Widget for entering a password; can be toggled between line edit and keyless edit.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "generic/PasswordLineEdit.h"
#include <QWidget>

class QRegularExpression;
class QStackedLayout;
class QToolButton;

namespace governikus
{

class KeylessPasswordEdit;


class PasswordEdit
	: public QWidget
{
	Q_OBJECT

	public:
		PasswordEdit(QWidget* pParent = nullptr);
		virtual ~PasswordEdit();

		void setMaxLength(int pLength);

		/*!
		 * The field width is set to display pWidth characters
		 */
		void setFieldWidth(int pWidth);

		/*!
		 * Configures a regular expression for input validation.
		 * If the invalid value tool tip is set, it is displayed on invalid values.
		 */
		void configureValidation(const QRegularExpression& pExpression, const QString& pInvalidValueToolTip);

		PasswordLineEdit* getLineEdit();

		void clear();
		QString text() const;

		void setCursorPosition(int pIndex);
		void setKeylessPasswordMode(bool pKeylessPassword);
		bool isLineEditActive() const;
		void setDigitFieldInvalid(bool pMakeInvalid, const QString& pInvalidMessage);

	Q_SIGNALS:
		void textEdited(const QString& pText);
		void selectionChanged();

	private:
		static QWidget* createWrapperWidget(QWidget* pWidget);

	private:
		KeylessPasswordEdit* mKeylessEdit;
		PasswordLineEdit* mLineEdit;
		QStackedLayout* mStackedLayout;
};

}
