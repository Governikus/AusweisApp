/*!
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderScanEnabler.h"

#include "Env.h"
#include "ReaderManager.h"

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


ReaderScanEnabler::ReaderScanEnabler(QQuickItem* pParent)
	: QQuickItem(pParent)
	, mObligedToStopScan(false)
{
	if (isVisible())
	{
		enableScan(true);
	}
}


ReaderScanEnabler::~ReaderScanEnabler()
{
	enableScan(false);
}


void ReaderScanEnabler::enableScan(const bool pEnable)
{
	const auto manager = Env::getSingleton<ReaderManager>();
	if (pEnable && !manager->isScanRunning())
	{
		qCDebug(qml) << "Starting scan.";
		mObligedToStopScan = true;
		manager->startScanAll(false);
	}
	else if (mObligedToStopScan)
	{
		qCDebug(qml) << "Stopping scan.";
		mObligedToStopScan = false;
		manager->stopScanAll();
	}
}


void ReaderScanEnabler::itemChange(QQuickItem::ItemChange pChange, const QQuickItem::ItemChangeData& pValue)
{
	if (pChange == QQuickItem::ItemVisibleHasChanged)
	{
		enableScan(pValue.boolValue);
	}

	QQuickItem::itemChange(pChange, pValue);
}
