/*!
 * \brief Dialog to display information about the application
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class AboutDialog;
}

namespace governikus
{

class AboutDialog
	: public QDialog
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::AboutDialog> mUi;

	private Q_SLOTS:
		void onCheckboxStateChanged();
		void onAccept();

	public:
		AboutDialog(QWidget* pParent = nullptr);
		virtual ~AboutDialog();
};

} /* namespace governikus */
