/*!
 * \brief Widget for the general settings.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "GlobalStatus.h"

#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class GeneralSettingsWidget;
}

namespace governikus
{

class GeneralSettingsWidget
	: public QWidget
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::GeneralSettingsWidget> mUi;

	private Q_SLOTS:
		void onCheckBoxStateChanged(int pState);
		void onUpdateCheckButtonClicked();
		virtual void showEvent(QShowEvent*) override;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		GeneralSettingsWidget(QWidget* pParent = nullptr);
		virtual ~GeneralSettingsWidget() override;

		void apply();
		void reset();

	Q_SIGNALS:
		void settingsChanged();
};

} /* namespace governikus */
