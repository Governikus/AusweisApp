/*!
 * SettingsWidget.h
 *
 * \brief Widget for the settings.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QScopedPointer>

#include "generic/GuiModule.h"
#include "generic/Page.h"

namespace Ui
{
class SettingsWidget;
}

namespace governikus
{

class SettingsWidget
	: public Page
{
	Q_OBJECT

	public:
		SettingsWidget(QWidget* pParent = nullptr);
		virtual ~SettingsWidget();

		void workflowStarted();
		void workflowFinished();

		void switchToGuiModule(GuiModule pModule);

		QString getActiveTabObjectName();

		bool isSettingsChanged();
		void showSettingsChangedMessage();

	Q_SIGNALS:
		void changePinRequested();
		void diagnosisRequested();
		void driverRequested();
		void settingsDone();

	protected:
		virtual void paintEvent(QPaintEvent*) override;
		virtual void hideEvent(QHideEvent* pEvent) override;

	private Q_SLOTS:
		void onTabChanged(int pIndex);
		void onApplyButtonClicked();
		void onCancelButtonClicked();

		void onSetChangePinButtonEnabled(bool pEnabled);
		void onSettingsChanged();

	private:
		void resetSettings();
		void setSettingsChanged(bool pChanged);
		void applyAppSettings();

	private:
		QScopedPointer<Ui::SettingsWidget> mUi;
		QString mApplyButtonText;
		bool mSettingsChanged;
};

} /* namespace governikus */
