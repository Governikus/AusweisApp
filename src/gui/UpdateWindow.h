/*!
 * \brief Window for application updates
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppUpdateData.h"

#include "GlobalStatus.h"

#include <QDialog>
#include <QMessageBox>

namespace Ui
{
class UpdateWindow;
}

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

		void showBox(QMessageBox::Icon pIcon, const QString& pMsg);
		void fillData();

	public:
		UpdateWindow(bool pSilent = false, QWidget* pParent = nullptr);
		virtual ~UpdateWindow();

	private Q_SLOTS:
		void onAppUpdateCheckFinished(bool pUpdateAvailable, const GlobalStatus& pError);
		void onUpdateClicked();
		void onSkipVersionClicked();
};

} /* namespace governikus */
