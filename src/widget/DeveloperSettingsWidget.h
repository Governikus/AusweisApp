/*!
 * \brief Widget for the developer settings.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>


namespace Ui
{
class DeveloperSettingsWidget;
}

namespace governikus
{

class DeveloperSettingsWidget
	: public QWidget
{
	Q_OBJECT

	private:
		QScopedPointer<Ui::DeveloperSettingsWidget> mUi;

	private Q_SLOTS:
		void onCheckBoxStateChanged();
		virtual void showEvent(QShowEvent*) override;

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	public:
		DeveloperSettingsWidget(QWidget* pParent = nullptr);
		virtual ~DeveloperSettingsWidget() override;

		void apply();
		void reset();

	Q_SIGNALS:
		void fireSettingsChanged();
};

} /* namespace governikus */
