/*!
 * \brief Custom implementation of QLineEdit that uses the password echo mode.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QLineEdit>
#include <QRegularExpression>

namespace governikus
{

class PasswordLineEdit
	: public QLineEdit
{
	Q_OBJECT

	public:
		PasswordLineEdit(QWidget* pParent = nullptr);
		virtual ~PasswordLineEdit();

		/*!
		 * The field width is set to display pWidth characters
		 */
		void setFieldWidth(int pWidth);

		/*!
		 * Configures a regular expression for input validation.
		 * If the invalid value tool tip is set, it is displayed on invalid values.
		 */
		void configureValidation(const QRegularExpression& pExpression, const QString& pInvalidValueToolTip);
};

}
