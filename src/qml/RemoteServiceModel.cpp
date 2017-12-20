/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "EstablishPACEChannelParser.h"
#include "RemoteClientImpl.h"
#include "RemoteServiceSettings.h"

using namespace governikus;

RemoteServiceModel::RemoteServiceModel()
	: mContext()
	, mWifiInfo()
	, mRunnable(false)
	, mCanEnableNfc(false)
	, mErrorMessage()
	, mPsk()
	, mAvailableRemoteDevices(this, false, true)
	, mKnownDevices(this, true, false)
{
	connect(&ReaderManager::getInstance(), &ReaderManager::firePluginAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireStatusChanged, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &RemoteServiceModel::onEnvironmentChanged);

	const QSharedPointer<RemoteClient>& remoteClient = ReaderManager::getInstance().getRemoteClient();
	connect(remoteClient.data(), &RemoteClient::fireDetectionChanged, this, &RemoteServiceModel::fireDetectionChanged);

	onEnvironmentChanged();
}


void RemoteServiceModel::onEnvironmentChanged()
{
	bool nfcPluginAvailable = false;
	bool nfcPluginEnabled = false;
	const auto& allPlugins = ReaderManager::getInstance().getPlugInInfos();
	for (const auto& pluginInfo : allPlugins)
	{
		if (pluginInfo.getPlugInType() != ReaderManagerPlugInType::NFC)
		{
			// At this time no bluetooth basic reader available so we can skip
			continue;
		}

		nfcPluginAvailable |= pluginInfo.isAvailable();
		nfcPluginEnabled |= pluginInfo.isEnabled();
	}

	bool readerAvailable = false;
	const auto& allReader = ReaderManager::getInstance().getReaderInfos();
	for (const auto& readerInfo : allReader)
	{
		readerAvailable |= readerInfo.isBasicReader();
	}

	const bool wifiEnabled = mWifiInfo.isWifiEnabled();

	const bool runnable = readerAvailable && wifiEnabled;
	const bool canEnableNfc = nfcPluginAvailable && !nfcPluginEnabled;
	const QString errorMessage = getErrorMessage(nfcPluginAvailable, nfcPluginEnabled, wifiEnabled);
	if (mRunnable != runnable || mCanEnableNfc != canEnableNfc || mErrorMessage != errorMessage)
	{
		mRunnable = runnable;
		mCanEnableNfc = canEnableNfc;
		mErrorMessage = errorMessage;

		Q_EMIT fireEnvironmentChanged();
	}

	if (!runnable && isRunning())
	{
		setRunning(false);
	}
}


bool RemoteServiceModel::isRunning() const
{
	return mContext ? mContext->isRunning() : false;
}


QString RemoteServiceModel::getCurrentState() const
{
	return mContext ? mContext->getCurrentState() : QString();
}


void RemoteServiceModel::setRunning(bool pState)
{
	if (isRunning() == pState)
	{
		return;
	}

	if (isRunning() && mContext)
	{
		Q_EMIT mContext->fireCancelWorkflow();
	}
	else
	{
		Q_EMIT fireStartWorkflow();
	}

	Q_EMIT fireIsRunningChanged();
}


QString RemoteServiceModel::getReaderPlugInType() const
{
	if (mContext)
	{
		return getEnumName(mContext->getReaderPlugInTypes().at(0));
	}

	return QString();
}


void RemoteServiceModel::setReaderPlugInType(const QString& pReaderPlugInType)
{
	if (mContext)
	{
		mContext->setReaderPlugInTypes({Enum<ReaderManagerPlugInType>::fromString(pReaderPlugInType, ReaderManagerPlugInType::UNKNOWN)});
	}
}


RemoteDeviceModel* RemoteServiceModel::getAvailableRemoteDevices()
{
	return &mAvailableRemoteDevices;
}


RemoteDeviceModel* RemoteServiceModel::getKnownDevices()
{
	return &mKnownDevices;
}


void RemoteServiceModel::setDetectRemoteDevices(bool pNewStatus)
{
	if (pNewStatus)
	{
		mAvailableRemoteDevices.onWidgetShown();
		mKnownDevices.onWidgetShown();
	}
	else
	{
		mAvailableRemoteDevices.onWidgetHidden();
		mKnownDevices.onWidgetHidden();
	}
}


bool RemoteServiceModel::detectRemoteDevices()
{
	const QSharedPointer<RemoteClient>& remoteClient = ReaderManager::getInstance().getRemoteClient();
	return remoteClient->isDetecting();
}


void RemoteServiceModel::connectToServer(const QString& pDeviceId, const QString& pServerPsk)
{
	if (!pServerPsk.isEmpty())
	{
		const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();
		connect(remoteClient.data(), &RemoteClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);
		remoteClient->establishConnection(mAvailableRemoteDevices.getRemoteDeviceListEntry(pDeviceId), pServerPsk);
	}
}


void RemoteServiceModel::onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus)
{
	Q_UNUSED(pEntry);
	const QSharedPointer<RemoteClient>& remoteClient = Env::getSingleton<ReaderManager>()->getRemoteClient();
	disconnect(remoteClient.data(), &RemoteClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);
	if (pStatus.isError())
	{
		Q_EMIT firePairingFailed();
	}
}


void RemoteServiceModel::resetContext(const QSharedPointer<RemoteServiceContext>& pContext)
{
	mPsk.clear();

	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &RemoteServiceModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &RemoteServiceModel::fireIsRunningChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, [this](const QByteArray& pPsk){
					mPsk = pPsk;
				});
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, &RemoteServiceModel::firePskChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::fireConnectedChanged, this, &RemoteServiceModel::fireConnectedChanged);
	}

	Q_EMIT fireConnectedChanged(isConnected());
}


void RemoteServiceModel::setPairing(bool pEnabled)
{
	if (mContext)
	{
		mContext->getRemoteServer()->setPairing(pEnabled);
	}
}


QString RemoteServiceModel::getCurrentFingerprint() const
{
	if (mContext && mContext->getRemoteServer()->isConnected())
	{
		return RemoteServiceSettings::generateFingerprint(mContext->getRemoteServer()->getCurrentCertificate());
	}

	return QString();
}


bool RemoteServiceModel::isConnected() const
{
	if (mContext)
	{
		return mContext->getRemoteServer()->isConnected();
	}

	return false;
}


bool RemoteServiceModel::pinPadModeOn()
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
}


QString RemoteServiceModel::getPacePasswordId() const
{
	if (mContext.isNull())
	{
		return QString();
	}

	const QSharedPointer<const IfdEstablishPaceChannel> establishPaceChannelMessage = mContext->getEstablishPaceChannelMessage();
	if (establishPaceChannelMessage.isNull())
	{
		return QString();
	}

	const EstablishPACEChannelParser parser = EstablishPACEChannelParser::fromCcid(establishPaceChannelMessage->getInputData());
	switch (parser.getPasswordId())
	{
		case PACE_PASSWORD_ID::PACE_CAN:
			return QStringLiteral("CAN");

		case PACE_PASSWORD_ID::PACE_PIN:
			return QStringLiteral("PIN");

		case PACE_PASSWORD_ID::PACE_PUK:
			return QStringLiteral("PUK");

		default:
			return QString();
	}
}


QString RemoteServiceModel::getErrorMessage(bool pNfcPluginAvailable, bool pNfcPluginEnabled, bool pWifiEnabled) const
{
	if (!pNfcPluginAvailable)
	{
		return tr("NFC is not available on your device.");
	}
	if (!pNfcPluginEnabled)
	{
		return tr("Please enable NFC to use the remote service.");
	}
	if (!pWifiEnabled)
	{
		return tr("Please connect your WiFi to use the remote service.");
	}

	return QString();
}


void RemoteServiceModel::forgetDevice(const QString& pId)
{
	mKnownDevices.forgetDevice(pId);
}


void RemoteServiceModel::cancelPasswordRequest()
{
	if (mContext)
	{
		Q_EMIT mContext->fireCancelEstablishPaceChannel();
	}
}
