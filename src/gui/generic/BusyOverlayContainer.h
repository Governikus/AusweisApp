/*!
 * BusyOverlayContainer.h
 *
 * \brief A page in a mobile GUI.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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

} /* namespace governikus */
