/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "ApplicationModel.h"
#include "AppSettings.h"
#include "EstablishPaceChannel.h"
#include "NumberModel.h"
#include "RemoteClientImpl.h"
#include "RemoteServiceSettings.h"

#ifdef Q_OS_IOS
#include <QOperatingSystemVersion>
#endif

using namespace governikus;

RemoteServiceModel::RemoteServiceModel()
	: WorkflowModel()
	, mContext()
	, mRunnable(false)
	, mIsStarting(false)
	, mCanEnableNfc(false)
	, mPairingRequested(false)
	, mRequestTransportPin(false)
	, mErrorMessage()
	, mPsk()
	, mAvailableRemoteDevices(this, false, true)
	, mKnownDevices(this, true, false)
	, mCombinedDevices(this, true, true)
	, mConnectionInfo()
	, mConnectedServerDeviceNames()
	, mRememberedServerEntry()
#ifdef Q_OS_IOS
	// iOS 14 introduced a local network permission, so we need to handle it.
	, mRequiresLocalNetworkPermission(QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::IOS, 14))
#else
	, mRequiresLocalNetworkPermission(false)
#endif
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::firePluginAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &RemoteServiceModel::onEnvironmentChanged);
	const auto applicationModel = Env::getSingleton<ApplicationModel>();
	connect(applicationModel, &ApplicationModel::fireWifiEnabledChanged, this, &RemoteServiceModel::onEnvironmentChanged);

	const auto* const remoteClient = Env::getSingleton<RemoteClient>();
	connect(remoteClient, &RemoteClient::fireDetectionChanged, this, &RemoteServiceModel::fireDetectionChanged);
	connect(remoteClient, &RemoteClient::fireNewRemoteDispatcher, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(remoteClient, &RemoteClient::fireDispatcherDestroyed, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(remoteClient, &RemoteClient::fireDeviceAppeared, this, &RemoteServiceModel::fireRemoteReaderVisibleChanged);
	connect(remoteClient, &RemoteClient::fireDeviceVanished, this, &RemoteServiceModel::fireRemoteReaderVisibleChanged);

	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&settings, &GeneralSettings::fireLanguageChanged, this, &RemoteServiceModel::onEnvironmentChanged);

	QMetaObject::invokeMethod(this, &RemoteServiceModel::onEnvironmentChanged, Qt::QueuedConnection);
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
			// At this time no basic reader available so we can skip
			continue;
		}

		nfcPluginAvailable |= pluginInfo.isAvailable();
		nfcPluginEnabled |= pluginInfo.isEnabled();
	}

	bool readerAvailable = !(Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({ReaderManagerPlugInType::NFC})).isEmpty());
	const bool wifiEnabled = Env::getSingleton<ApplicationModel>()->isWifiEnabled();

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


void RemoteServiceModel::setRunning(bool pState, bool pEnablePairing)
{
	bool enablePairing = pEnablePairing && pState;
	if (mContext)
	{
		setPairing(enablePairing);
	}
	else
	{
		mPairingRequested = enablePairing;
	}

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
		setStarting(true);
		Q_EMIT fireStartWorkflow();
	}
}


void RemoteServiceModel::setStarting(bool pStarting)
{
	mIsStarting = pStarting;
	Q_EMIT fireIsStartingChanged();
}


bool RemoteServiceModel::isStarting() const
{
	return mIsStarting;
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


bool RemoteServiceModel::detectRemoteDevices() const
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
	const auto* const remoteClient = Env::getSingleton<RemoteClient>();
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


void RemoteServiceModel::resetRemoteServiceContext(const QSharedPointer<RemoteServiceContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetWorkflowContext(pContext);

	mPsk.clear();
	onEstablishPaceChannelUpdated();

	if (mContext)
	{
		connect(mContext.data(), &RemoteServiceContext::fireIsRunningChanged, this, [this](){
					setStarting(false);
				});
		connect(mContext.data(), &RemoteServiceContext::fireIsRunningChanged, this, &RemoteServiceModel::fireIsRunningChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, [this](const QByteArray& pPsk){
					mPsk = pPsk;
				});
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePskChanged, this, &RemoteServiceModel::firePskChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::fireConnectedChanged, this, &RemoteServiceModel::onConnectionInfoChanged);
		connect(mContext->getRemoteServer().data(), &RemoteServer::firePairingCompleted, this, &RemoteServiceModel::firePairingCompleted);
		connect(mContext.data(), &RemoteServiceContext::fireCardConnectionEstablished, this, &RemoteServiceModel::onCardConnectionEstablished);
		connect(mContext.data(), &RemoteServiceContext::fireEstablishPaceChannelUpdated, this, &RemoteServiceModel::onEstablishPaceChannelUpdated);

		setPairing(mPairingRequested);
	}
	else
	{
		setStarting(false);
	}

	mPairingRequested = false;

	Q_EMIT fireIsRunningChanged();
	Q_EMIT fireConnectedChanged();
}


void RemoteServiceModel::setPairing(bool pEnabled)
{
	if (mContext)
	{
		mContext->getRemoteServer()->setPairing(pEnabled);
	}
}


bool RemoteServiceModel::isPairing()
{
	if (!mContext)
	{
		return false;
	}

	return !getPsk().isEmpty();
}


bool RemoteServiceModel::isConnectedToPairedDevice() const
{
	if (mContext)
	{
		return mContext->getRemoteServer()->isConnected() && !mContext->getRemoteServer()->isPairingConnection();
	}

	return false;
}


bool RemoteServiceModel::enableTransportPinLink() const
{
	return mContext && mContext->isPinChangeWorkflow() && mContext->getPreferredPinLength() == 0;
}


bool RemoteServiceModel::isRequestTransportPin() const
{
	return mRequestTransportPin;
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


bool RemoteServiceModel::getRemoteReaderVisible() const
{
	return Env::getSingleton<RemoteClient>()->hasAnnouncingRemoteDevices();
}


bool RemoteServiceModel::pinPadModeOn() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
}


QString RemoteServiceModel::getPasswordType() const
{
	if (!mContext)
	{
		return QString();
	}

	switch (mContext->getEstablishPaceChannel().getPasswordId())
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


void RemoteServiceModel::changePinLength()
{
	mRequestTransportPin = !mRequestTransportPin;
	Q_EMIT fireEstablishPaceChannelUpdated();
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


void RemoteServiceModel::onEstablishPaceChannelUpdated()
{
	mRequestTransportPin = (mContext && mContext->isPinChangeWorkflow() && mContext->getPreferredPinLength() == 5);
	Q_EMIT fireEstablishPaceChannelUpdated();
}
