/*!
 * \brief Widget for the settings.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DeveloperSettingsWidget.h"
#include "generic/GuiModule.h"

#include <QScopedPointer>


namespace Ui
{
class SettingsWidget;
}

namespace governikus
{

class SettingsWidget
	: public QWidget
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::SettingsWidget> mUi;
		bool mScanRunning;
		bool mWorkflowRunning;
		bool mSettingsChanged;

#ifndef QT_NO_DEBUG
		QScopedPointer<DeveloperSettingsWidget> mDeveloperTab;
#endif

		void resetSettings();
		void setSettingsChanged(bool pChanged);
		void applyAppSettings();
		void setDeveloperTabName();

	public Q_SLOTS:
		void onTabChanged(int pIndex);

	private Q_SLOTS:
		void onApplyButtonClicked();
		void onCancelButtonClicked();

		void onUpdateButtonState();
		void onUpdateApplyButtonText();
		void onSettingsChanged();

	protected:
		virtual void paintEvent(QPaintEvent*) override;
		virtual void hideEvent(QHideEvent* pEvent) override;
		virtual void showEvent(QShowEvent* pEvent) override;
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		SettingsWidget(QWidget* pParent = nullptr);
		virtual ~SettingsWidget() override;

		void keyPressEvent(QKeyEvent* pEvent) override;

		bool remoteScanRunning() const
		{
			return mScanRunning;
		}


		void workflowStarted();
		void workflowFinished();

		void switchToGuiModule(GuiModule pModule);

		QString getActiveTabObjectName();

		bool isSettingsChanged();
		void showSettingsChangedMessage();

	Q_SIGNALS:
		void changePinRequested();
		void diagnosisRequested();
		void settingsDone();
		void fireBackspacePressedOnApply();
};

} /* namespace governikus */
