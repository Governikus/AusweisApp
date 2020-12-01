/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
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
	, mPlugInType(ReaderManagerPlugInType::UNKNOWN)
	, mObligedToStopScan(false)
{
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
		qCDebug(qml) << "Starting scan on" << mPlugInType;
		mObligedToStopScan = true;
		manager->startScan(mPlugInType, false);
	}
	else if (mObligedToStopScan)
	{
		qCDebug(qml) << "Stopping scan on" << mPlugInType;
		mObligedToStopScan = false;
		manager->stopScan(mPlugInType);
	}
}


ReaderManagerPlugInType ReaderScanEnabler::getPlugInType() const
{
	return mPlugInType;
}


void ReaderScanEnabler::setPlugInType(ReaderManagerPlugInType pPlugInType)
{
	if (mPlugInType == pPlugInType)
	{
		return;
	}

	enableScan(false);
	mPlugInType = pPlugInType;
	if (isVisible())
	{
		enableScan(true);
	}

	Q_EMIT firePlugInTypeChanged();
}


void ReaderScanEnabler::itemChange(QQuickItem::ItemChange pChange, const QQuickItem::ItemChangeData& pValue)
{
	if (pChange == QQuickItem::ItemVisibleHasChanged)
	{
		enableScan(pValue.boolValue);
	}

	QQuickItem::itemChange(pChange, pValue);
}
