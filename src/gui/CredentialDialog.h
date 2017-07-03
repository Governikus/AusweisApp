/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

namespace Ui
{
class CredentialDialog;
}

#include <QDialog>

namespace governikus
{

class CredentialDialog
	: public QDialog
{
	Q_OBJECT

	private:
		Ui::CredentialDialog* mUi;

	public:
		explicit CredentialDialog(QWidget* pParent = nullptr);
		~CredentialDialog();

		void setUser(const QString& pUser);

		QString getUser() const;

		QString getPassword() const;


};

} /* namespace governikus */
