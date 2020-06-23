/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "AppSettings.h"
#include "EstablishPaceChannelParser.h"
#include "NumberModel.h"
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
	, mCombinedDevices(this, true, true)
	, mConnectionInfo()
	, mConnectedServerDeviceNames()
	, mIsSaCPinChangeWorkflow()
	, mRememberedServerEntry()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::firePluginAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(&mWifiInfo, &WifiInfo::fireWifiEnabledChanged, this, &RemoteServiceModel::onEnvironmentChanged);

	auto* const remoteClient = Env::getSingleton<RemoteClient>();
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

	bool readerAvailable = !(Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderManagerPlugInType::NFC).isEmpty());
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


RemoteDeviceModel* RemoteServiceModel::getCombinedDevices()
{
	return &mCombinedDevices;
}


void RemoteServiceModel::setDetectRemoteDevices(bool pNewStatus)
{
	if (pNewStatus == Env::getSingleton<RemoteClient>()->isDetecting())
	{
		return;
	}

	if (pNewStatus)
	{
		mAvailableRemoteDevices.onUiShown();
		mKnownDevices.onUiShown();
	}
	else
	{
		mAvailableRemoteDevices.onUiHidden();
		mKnownDevices.onUiHidden();
	}
}


bool RemoteServiceModel::detectRemoteDevices()
{
	return Env::getSingleton<RemoteClient>()->isDetecting();
}


void RemoteServiceModel::connectToRememberedServer(const QString& pServerPsk)
{
	if (!pServerPsk.isEmpty() && !mRememberedServerEntry.isNull())
	{
		auto* const remoteClient = Env::getSingleton<RemoteClient>();
		connect(remoteClient, &RemoteClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);

		qDebug() << "Starting to pair.";
		remoteClient->establishConnection(mRememberedServerEntry, pServerPsk);
	}
}


bool RemoteServiceModel::rememberServer(const QString& pDeviceId)
{
	mRememberedServerEntry = mAvailableRemoteDevices.getRemoteDeviceListEntry(pDeviceId);
	return !mRememberedServerEntry.isNull();
}


void RemoteServiceModel::onEstablishConnectionDone(const QSharedPointer<RemoteDeviceListEntry>& pEntry, const GlobalStatus& pStatus)
{
	auto* const remoteClient = Env::getSingleton<RemoteClient>();
	disconnect(remoteClient, &RemoteClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);
	qDebug() << "Pairing finished:" << pStatus;
	const auto deviceName = RemoteServiceSettings::escapeDeviceName(pEntry->getRemoteDeviceDescriptor().getIfdName());
	if (pStatus.isError())
	{
		Q_EMIT firePairingFailed(deviceName, pStatus.toErrorDescription());
	}
	else
	{
		Q_EMIT firePairingSuccess(deviceName);
	}
}


void RemoteServiceModel::onConnectionInfoChanged(bool pConnected)
{
	if (mContext && pConnected)
	{
		const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		const QString peerName = settings.getRemoteInfo(mContext->getRemoteServer()->getCurrentCertificate()).getNameEscaped();
		//: INFO ANDROID IOS The smartphone is connected as card reader (SaK) and currently processing an authentication request. The user is asked to pay attention the its screen.
		mConnectionInfo = tr("Please pay attention to the display on your other device \"%1\".").arg(peerName);
		Q_EMIT fireConnectionInfoChanged();
	}
	Q_EMIT fireConnectedChanged();
}


void RemoteServiceModel::onCardConnectionEstablished(const QSharedPointer<CardConnection>& pConnection)
{
	pConnection->setProgressMessage(mConnectionInfo);
}


void RemoteServiceModel::resetContext(const QSharedPointer<RemoteServiceContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetContext(pContext);

	mPsk.clear();
	onEstablishPaceChannelMessageUpdated(QSharedPointer<const IfdEstablishPaceChannel>());

	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &RemoteServiceModel::fireIsRunningChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, [this](const QByteArray& pPsk){
					mPsk = pPsk;
				});
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, &RemoteServiceModel::firePskChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::fireConnectedChanged, this, &RemoteServiceModel::onConnectionInfoChanged);
		connect(mContext.data(), &RemoteServiceContext::fireCardConnectionEstablished, this, &RemoteServiceModel::onCardConnectionEstablished);
		connect(mContext.data(), &RemoteServiceContext::fireEstablishPaceChannelMessageUpdated, this, &RemoteServiceModel::onEstablishPaceChannelMessageUpdated);
	}

	Q_EMIT fireConnectedChanged();
}


void RemoteServiceModel::setPairing(bool pEnabled)
{
	if (mContext)
	{
		mContext->getRemoteServer()->setPairing(pEnabled);
	}
}


bool RemoteServiceModel::isConnectedToPairedDevice() const
{
	if (mContext)
	{
		return mContext->getRemoteServer()->isConnected() && !mContext->getRemoteServer()->isPairingConnection();
	}

	return false;
}


bool RemoteServiceModel::isSaCPinChangeWorkflow() const
{
	return mIsSaCPinChangeWorkflow;
}


bool RemoteServiceModel::isRunnable() const
{
	return mRunnable;
}


bool RemoteServiceModel::isCanEnableNfc() const
{
	return mCanEnableNfc;
}


QString RemoteServiceModel::getErrorMessage() const
{
	return mErrorMessage;
}


QByteArray RemoteServiceModel::getPsk() const
{
	return mPsk;
}


QString RemoteServiceModel::getConnectionInfo() const
{
	return mConnectionInfo;
}


QString RemoteServiceModel::getConnectedServerDeviceNames() const
{
	return mConnectedServerDeviceNames;
}


bool RemoteServiceModel::pinPadModeOn()
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
}


QString RemoteServiceModel::getPasswordType() const
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
		//: INFO ALL_PLATFORMS The device does not offer NFC.
		return tr("NFC is not available on your device.");
	}
	if (!pNfcPluginEnabled)
	{
		//: INFO ALL_PLATFORMS NFC is available but not active.
		return tr("Please enable NFC to use the remote service.");
	}
	if (!pWifiEnabled)
	{
		//: INFO ALL_PLATFORMS The WiFi feature is not enabled but required to use the smartphone as a card reader (SaK).
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
	auto* const remoteClient = Env::getSingleton<RemoteClient>();
	const auto deviceInfos = remoteClient->getConnectedDeviceInfos();
	QStringList deviceNames;
	for (const auto& info : deviceInfos)
	{
		deviceNames.append(QLatin1Char('"') + info.getNameEscaped() + QLatin1Char('"'));
	}
	mConnectedServerDeviceNames = deviceNames.join(QLatin1String(", "));
	Q_EMIT fireConnectedServerDeviceNamesChanged();
}


void RemoteServiceModel::onEstablishPaceChannelMessageUpdated(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	Env::getSingleton<NumberModel>()->setRequestTransportPin(false);

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
