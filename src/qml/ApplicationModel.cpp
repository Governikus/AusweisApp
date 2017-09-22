/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "ApplicationModel.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/SelfAuthenticationContext.h"
#include "ReaderInfo.h"
#include "ReaderManager.h"

using namespace governikus;


void ApplicationModel::onStatusChanged(const ReaderManagerPlugInInfo& pInfo)
{
	if (pInfo.getPlugInType() == ReaderManagerPlugInType::BLUETOOTH)
	{
		Q_EMIT fireBluetoothEnabledChanged();
	}
	if (pInfo.getPlugInType() == ReaderManagerPlugInType::NFC)
	{
		Q_EMIT fireNfcEnabledChanged();
	}
}


ApplicationModel::ApplicationModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
{
	connect(&ReaderManager::getInstance(), &ReaderManager::fireStatusChanged, this, &ApplicationModel::onStatusChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderPropertiesUpdated, this, &ApplicationModel::fireReaderPropertiesUpdated);
}


ApplicationModel::~ApplicationModel()
{
}


void ApplicationModel::resetContext(QSharedPointer<WorkflowContext> pContext)
{
	mContext = pContext;
	Q_EMIT fireCurrentWorkflowChanged();
}


bool ApplicationModel::isEnabled(ReaderManagerPlugInType pType) const
{
	const auto pluginInfos = ReaderManager::getInstance().getPlugInInfos();
	for (const auto& info : pluginInfos)
	{
		if (info.getPlugInType() == pType)
		{
			return info.isEnabled();
		}
	}
	return false;
}


bool ApplicationModel::isNfcEnabled() const
{
	return isEnabled(ReaderManagerPlugInType::NFC);
}


bool ApplicationModel::isExtendedLengthApdusUnsupported() const
{
	for (const ReaderInfo& readerInfo : ReaderManager::getInstance().getReaderInfos(ReaderManagerPlugInType::NFC))
	{
		if (readerInfo.getExtendedLengthApduSupportCode() == ExtendedLengthApduSupportCode::NOT_SUPPORTED)
		{
			return true;
		}
	}

	return false;
}


bool ApplicationModel::isBluetoothEnabled() const
{
	return isEnabled(ReaderManagerPlugInType::BLUETOOTH);
}


QString ApplicationModel::getCurrentWorkflow() const
{
	if (mContext.dynamicCast<ChangePinContext>())
	{
		return QStringLiteral("changepin");
	}
	if (mContext.dynamicCast<SelfAuthenticationContext>())
	{
		return QStringLiteral("selfauthentication");
	}
	if (mContext.dynamicCast<AuthContext>())
	{
		return QStringLiteral("authentication");
	}
	return QString();
}
