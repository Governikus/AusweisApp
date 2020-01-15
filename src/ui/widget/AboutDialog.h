/*!
 * \brief Dialog to display information about the application
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class AboutDialog;
} // namespace Ui

namespace governikus
{

class AboutDialog
	: public QDialog
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::AboutDialog> mUi;

	private Q_SLOTS:
		void onDeveloperOptionsChanged();
		void onAccept();

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		AboutDialog(QWidget* pParent = nullptr);
		virtual ~AboutDialog() override;
};

} // namespace governikus
