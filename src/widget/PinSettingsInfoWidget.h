/*!
 * \brief Widget for PIN settings information.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class PinSettingsInfoWidget;
}

namespace governikus
{

class PinSettingsInfoWidget
	: public QWidget
{
	Q_OBJECT

	public:
		PinSettingsInfoWidget(QWidget* pParent = nullptr);
		virtual ~PinSettingsInfoWidget() override;

		void setInfoTitle(const QString& pTitle);
		void setInfoDescription(const QString& pDescription);

	protected:
		virtual void changeEvent(QEvent* pEvent) override;

	private:
		QScopedPointer<Ui::PinSettingsInfoWidget> mUi;

		virtual void paintEvent(QPaintEvent*) override;
};

} /* namespace governikus */
