/*!
 * \brief An overlay to show a busy indicator.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QStackedWidget>

namespace governikus
{

class BusyOverlay;

class BusyOverlayContainer
	: public QStackedWidget
{
	Q_OBJECT

	public:
		BusyOverlayContainer(QWidget* pWidgetToOverlay, bool pStart = true, QWidget* pParent = nullptr);
		virtual ~BusyOverlayContainer();

		void startAnimation();
		void stopAnimation();

	private:
		BusyOverlay* mOverlay;
};

} // namespace governikus
