/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "AppSettings.h"
#include "Env.h"
#include "EstablishPaceChannelParser.h"
#include "RemoteClientImpl.h"
#include "RemoteServiceSettings.h"
#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(RemoteServiceModel)


RemoteServiceModel::RemoteServiceModel()
	: WorkflowModel()
	, mContext()
	, mWifiInfo()
	, mRunnable(false)
	, mCanEnableNfc(false)
	, mErrorMessage()
	, mPsk()
	, mAvailableRemoteDevices(this, false, true)
	, mKnownDevices(this, true, false)
	, mConnectedClientDeviceName()
	, mConnectedServerDeviceNames()
	, mIsSaCPinChangeWorkflow()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::firePluginAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &RemoteServiceModel::onEnvironmentChanged);

	RemoteClient* const remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireDetectionChanged, this, &RemoteServiceModel::fireDetectionChanged);
	connect(remoteClient, &RemoteClient::fireNewRemoteDispatcher, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(remoteClient, &RemoteClient::fireDispatcherDestroyed, this, &RemoteServiceModel::onConnectedDevicesChanged);
	onEnvironmentChanged();
}


void RemoteServiceModel::onEnvironmentChanged()
{
	bool nfcPluginAvailable = false;
	bool nfcPluginEnabled = false;
	const auto& allPlugins = Env::getSingleton<ReaderManager>()->getPlugInInfos();
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

	bool readerAvailable = (Env::getSingleton<ReaderManager>()->getReaderInfos().length() > 0);
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
	return Env::getSingleton<RemoteClient>()->isDetecting();
}


void RemoteServiceModel::connectToServer(const QString& pDeviceId, const QString& pServerPsk)
{
	if (!pServerPsk.isEmpty())
	{
		RemoteClient* const remoteClient = Env::getSingleton<RemoteClient>();
		connect(remoteClient, &RemoteClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);

		remoteClient->establishConnection(mAvailableRemoteDevices.getRemoteDeviceListEntry(pDeviceId), pServerPsk);
	}
}


void RemoteServiceModel::onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus)
{
	Q_UNUSED(pEntry);
	RemoteClient* const remoteClient = Env::getSingleton<RemoteClient>();
	disconnect(remoteClient, &RemoteClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);
	if (pStatus.isError())
	{
		Q_EMIT firePairingFailed();
	}
}


void RemoteServiceModel::onClientConnectedChanged(bool pConnected)
{
	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const QString peerName = settings.getRemoteInfo(getCurrentFingerprint()).getName();
	mConnectedClientDeviceName = peerName;
	Q_EMIT fireConnectedClientDeviceNameChanged();
	Q_EMIT fireConnectedChanged(pConnected);
}


void RemoteServiceModel::resetContext(const QSharedPointer<RemoteServiceContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetContext(pContext);

	mPsk.clear();
	onEstablishPaceChannelMessageUpdated(QSharedPointer<const IfdEstablishPaceChannel>());

	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &RemoteServiceModel::fireIsRunningChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, [this](const QByteArray& pPsk){
					mPsk = pPsk;
				});
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, &RemoteServiceModel::firePskChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::fireConnectedChanged, this, &RemoteServiceModel::onClientConnectedChanged);
		connect(mContext.data(), &RemoteServiceContext::fireEstablishPaceChannelMessageUpdated, this, &RemoteServiceModel::onEstablishPaceChannelMessageUpdated);
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


bool RemoteServiceModel::isSaCPinChangeWorkflow() const
{
	return mIsSaCPinChangeWorkflow;
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
	if (establishPaceChannelMessage->isIncomplete())
	{
		return QString();
	}

	const EstablishPaceChannelParser parser = EstablishPaceChannelParser::fromCcid(establishPaceChannelMessage->getInputData());
	switch (parser.getPasswordId())
	{
		case PacePasswordId::PACE_CAN:
			return QStringLiteral("CAN");

		case PacePasswordId::PACE_PIN:
			return QStringLiteral("PIN");

		case PacePasswordId::PACE_PUK:
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
		Q_EMIT mContext->fireCancelPasswordRequest();
	}
}


RemoteServiceModel& RemoteServiceModel::getInstance()
{
	return *Instance;
}


void RemoteServiceModel::onConnectedDevicesChanged()
{
	RemoteClient* const remoteClient = Env::getSingleton<RemoteClient>();
	const auto deviceInfos = remoteClient->getConnectedDeviceInfos();
	QStringList deviceNames;
	for (const auto& info : deviceInfos)
	{
		deviceNames.append(QLatin1Char('"') + info.getName() + QLatin1Char('"'));
	}
	mConnectedServerDeviceNames = deviceNames.join(QLatin1String(", "));
	Q_EMIT fireConnectedServerDeviceNamesChanged();
}


void RemoteServiceModel::onEstablishPaceChannelMessageUpdated(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	if (pMessage.isNull() || pMessage->isIncomplete())
	{
		mIsSaCPinChangeWorkflow = false;
	}
	else
	{
		const EstablishPaceChannelParser parser = EstablishPaceChannelParser::fromCcid(pMessage->getInputData());
		mIsSaCPinChangeWorkflow = parser.getCertificateDescription().isEmpty();
	}

	Q_EMIT fireEstablishPaceChannelMessageUpdated();
}
