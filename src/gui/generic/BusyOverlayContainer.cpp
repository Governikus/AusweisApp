/*!
 * BusyOverlayContainer.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "BusyOverlayContainer.h"

#include <QBoxLayout>
#include <QStackedLayout>

#include "BusyOverlay.h"

using namespace governikus;

BusyOverlayContainer::BusyOverlayContainer(QWidget* pWidgetToOverlay, bool pStart, QWidget* pParent)
	: QStackedWidget(pParent)
	, mOverlay(new BusyOverlay(pStart))
{
	QWidget* busyOverlayContainer = new QWidget;
	QBoxLayout* overlayContainerLayout = new QVBoxLayout(busyOverlayContainer);
	overlayContainerLayout->addWidget(mOverlay, 0, Qt::AlignHCenter | Qt::AlignVCenter);

	QStackedLayout* stackLayout = qobject_cast<QStackedLayout*>(layout());
	stackLayout->setStackingMode(QStackedLayout::StackAll);

	stackLayout->addWidget(pWidgetToOverlay);
	stackLayout->addWidget(busyOverlayContainer);
	stackLayout->setCurrentIndex(1);
}


BusyOverlayContainer::~BusyOverlayContainer()
{
}


void BusyOverlayContainer::startAnimation()
{
	mOverlay->startAnimation();
}


void BusyOverlayContainer::stopAnimation()
{
	mOverlay->stopAnimation();
}
