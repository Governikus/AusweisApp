/*!
 * \brief Window for application updates
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppUpdateData.h"

#include <QDialog>
#include <QMessageBox>

namespace Ui
{
class UpdateWindow;
} // namespace Ui

namespace governikus
{

class UpdateWindow
	: public QDialog
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::UpdateWindow> mUi;
		bool mSilent;
		AppUpdateData mUpdateData;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		UpdateWindow(QWidget* pParent = nullptr);
		virtual ~UpdateWindow() override;
		void fillData();

	private Q_SLOTS:
		void onUpdateClicked();
		void onSkipVersionClicked();

	Q_SIGNALS:
		void fireShowUpdateDialog(QMessageBox::Icon pIcon, const QString& pMsg);
};

} // namespace governikus
