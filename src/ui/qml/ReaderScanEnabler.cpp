/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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
	, mPlugInType(ReaderManagerPlugInType::UNKNOWN)
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
			qCDebug(qml) << "Starting scan on" << mPlugInType;
			mObligedToStopScan = Env::getSingleton<ApplicationModel>()->getCurrentWorkflow() == ApplicationModel::Workflow::WORKFLOW_NONE;
			Env::getSingleton<ReaderManager>()->startScan(mPlugInType, false);
		}
		return;
	}

	if (mObligedToStopScan)
	{
		qCDebug(qml) << "Stopping scan on" << mPlugInType;
		mObligedToStopScan = false;
		Env::getSingleton<ReaderManager>()->stopScan(mPlugInType);
	}
}


void ReaderScanEnabler::enableScanIfVisible()
{
	enableScan(isVisible());
}


bool ReaderScanEnabler::isScanRunning() const
{
	return Env::getSingleton<ReaderManager>()->isScanRunning(mPlugInType);
}


void ReaderScanEnabler::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() == mPlugInType)
	{
		Q_EMIT fireScanRunningChanged();
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
	QMetaObject::invokeMethod(this, &ReaderScanEnabler::enableScanIfVisible, Qt::QueuedConnection);

	Q_EMIT firePlugInTypeChanged();
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
