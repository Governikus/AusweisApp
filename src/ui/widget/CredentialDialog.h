/*!
 * \brief Show a dialog to fill in proxy credentials.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

namespace Ui
{
class CredentialDialog;
} // namespace Ui

#include <QDialog>

namespace governikus
{

class CredentialDialog
	: public QDialog
{
	Q_OBJECT

	private:
		Ui::CredentialDialog* mUi;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		explicit CredentialDialog(QWidget* pParent = nullptr);
		virtual ~CredentialDialog() override;

		void setUser(const QString& pUser);

		QString getUser() const;

		QString getPassword() const;


};

} // namespace governikus
