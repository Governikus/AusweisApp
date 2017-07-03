/*!
 * \brief Widget for the general settings.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "generic/Page.h"

#include <QScopedPointer>
#include <QSettings>

namespace Ui
{
class GeneralSettingsWidget;
}

namespace governikus
{

class GeneralSettingsWidget
	: public Page
{
	Q_OBJECT

	public:
		GeneralSettingsWidget(QWidget* pParent = nullptr);
		virtual ~GeneralSettingsWidget();

		void apply();
		void reset();

	Q_SIGNALS:
		void settingsChanged();

	private Q_SLOTS:
		void onCheckBoxStateChanged(int pState);
		void onUpdateCheckButtonClicked();
		void showEvent(QShowEvent*);

	private:
		QScopedPointer<Ui::GeneralSettingsWidget> mUi;
};

} /* namespace governikus */
