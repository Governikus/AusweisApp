/*!
 * BusyOverlay.h
 *
 * \brief Widget for the settings.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>

namespace Ui
{
class BusyOverlay;
}

class QMovie;

namespace governikus
{

class BusyOverlay
	: public QWidget
{
	Q_OBJECT

	public:
		BusyOverlay(bool pStart = true, QWidget* pParent = nullptr);
		virtual ~BusyOverlay();

		void startAnimation();
		void stopAnimation();

		virtual QSize sizeHint() const override;

	protected:
		void paintEvent(QPaintEvent*) override;

	private:
		QScopedPointer<Ui::BusyOverlay> mUi;
		QScopedPointer<QMovie> mMovie;
};

} /* namespace governikus */
