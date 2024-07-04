/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderScanEnabler.h"

#include "ApplicationModel.h"
#include "Env.h"
#include "ReaderManager.h"

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


ReaderScanEnabler::ReaderScanEnabler(QQuickItem* pParent)
	: QQuickItem(pParent)
	, mPluginType(ReaderManagerPluginType::UNKNOWN)
	, mObligedToStopScan(false)
{
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireStatusChanged, this, &ReaderScanEnabler::onStatusChanged);
}


ReaderScanEnabler::~ReaderScanEnabler()
{
	enableScan(false);
}


void ReaderScanEnabler::enableScan(const bool pEnable)
{
	if (pEnable)
	{
		if (!isScanRunning())
		{
			qCDebug(qml) << "Starting scan on" << mPluginType;
			mObligedToStopScan = Env::getSingleton<ApplicationModel>()->getCurrentWorkflow() == ApplicationModel::Workflow::NONE;
			Env::getSingleton<ReaderManager>()->startScan(mPluginType, false);
		}
		return;
	}

	if (mObligedToStopScan)
	{
		qCDebug(qml) << "Stopping scan on" << mPluginType;
		mObligedToStopScan = false;
		Env::getSingleton<ReaderManager>()->stopScan(mPluginType);
	}
}


void ReaderScanEnabler::enableScanIfVisible()
{
	enableScan(isVisible());
}


bool ReaderScanEnabler::isScanRunning() const
{
	return Env::getSingleton<ReaderManager>()->getPluginInfo(mPluginType).isScanRunning();
}


void ReaderScanEnabler::onStatusChanged(const ReaderManagerPluginInfo& pInfo)
{
	if (pInfo.getPluginType() == mPluginType)
	{
		Q_EMIT fireScanRunningChanged();
	}
}


ReaderManagerPluginType ReaderScanEnabler::getPluginType() const
{
	return mPluginType;
}


void ReaderScanEnabler::setPluginType(ReaderManagerPluginType pPluginType)
{
	if (mPluginType == pPluginType)
	{
		return;
	}

	enableScan(false);
	mPluginType = pPluginType;
	QMetaObject::invokeMethod(this, &ReaderScanEnabler::enableScanIfVisible, Qt::QueuedConnection);

	Q_EMIT firePluginTypeChanged();
	Q_EMIT fireScanRunningChanged();
}


void ReaderScanEnabler::itemChange(QQuickItem::ItemChange pChange, const QQuickItem::ItemChangeData& pValue)
{
	if (pChange == QQuickItem::ItemVisibleHasChanged)
	{
		QMetaObject::invokeMethod(this, &ReaderScanEnabler::enableScanIfVisible, Qt::QueuedConnection);
	}

	QQuickItem::itemChange(pChange, pValue);
}


void ReaderScanEnabler::restartScan()
{
	enableScan(false);
	enableScan(true);
}
