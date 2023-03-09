/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "RemoteIfdClient.h"
#include "RemoteIfdServer.h"
#include "RemoteServiceSettings.h"
#include "controller/IfdServiceController.h"

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
	, mErrorMessage()
	, mPsk()
	, mAvailableRemoteDevices(this, false, true)
	, mKnownDevices(this, true, false)
	, mConnectionInfo()
	, mConnectedServerDeviceNames()
	, mRememberedServerEntry()
#ifdef Q_OS_IOS
	// iOS 14 introduced a local network permission, so we need to handle it.
	, mRequiresLocalNetworkPermission(QOperatingSystemVersion::current() >= QOperatingSystemVersion(QOperatingSystemVersion::IOS, 14))
	, mWasRunning(false)
	, mWasPairing(false)
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
	connect(applicationModel, &ApplicationModel::fireApplicationStateChanged, this, &RemoteServiceModel::onApplicationStateChanged);

	const auto* const ifdClient = Env::getSingleton<RemoteIfdClient>();
	connect(ifdClient, &IfdClient::fireDetectionChanged, this, &RemoteServiceModel::fireDetectionChanged);
	connect(ifdClient, &IfdClient::fireNewDispatcher, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(ifdClient, &IfdClient::fireDispatcherDestroyed, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(ifdClient, &IfdClient::fireDeviceAppeared, this, &RemoteServiceModel::fireRemoteReaderVisibleChanged);
	connect(ifdClient, &IfdClient::fireDeviceVanished, this, &RemoteServiceModel::fireRemoteReaderVisibleChanged);
	connect(ifdClient, &IfdClient::fireCertificateRemoved, this, &RemoteServiceModel::fireCertificateRemoved);

	QMetaObject::invokeMethod(this, &RemoteServiceModel::onEnvironmentChanged, Qt::QueuedConnection);
}


void RemoteServiceModel::onEnvironmentChanged()
{
	const auto& nfcInfo = Env::getSingleton<ReaderManager>()->getPlugInInfo(ReaderManagerPlugInType::NFC);
	const bool nfcPluginAvailable = nfcInfo.isAvailable();
	const bool nfcPluginEnabled = nfcInfo.isEnabled();

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


void RemoteServiceModel::onApplicationStateChanged(bool pIsAppInForeground)
{
#if defined(Q_OS_IOS)
	if (pIsAppInForeground)
	{
		setRunning(mWasRunning, mWasPairing);

		mWasRunning = false;
		mWasPairing = false;
	}
	else
	{
		mWasRunning = isRunning();
		mWasPairing = isPairing();

		setRunning(false);
	}
#else
	Q_UNUSED(pIsAppInForeground)
#endif
}


void RemoteServiceModel::onTranslationChanged()
{
	onEnvironmentChanged();
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
		const auto ifdServer = QSharedPointer<IfdServer>(new RemoteIfdServer());
		const auto request = WorkflowRequest::createWorkflowRequest<IfdServiceController, IfdServiceContext>(ifdServer);
		Q_EMIT fireStartWorkflow(request);
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


void RemoteServiceModel::setDetectRemoteDevices(bool pNewStatus)
{
	mAvailableRemoteDevices.setDetectRemoteDevices(pNewStatus);
	mKnownDevices.setDetectRemoteDevices(pNewStatus);
}


bool RemoteServiceModel::detectRemoteDevices() const
{
	return Env::getSingleton<RemoteIfdClient>()->isDetecting();
}


void RemoteServiceModel::connectToRememberedServer(const QString& pServerPsk)
{
	if (!pServerPsk.isEmpty() && !mRememberedServerEntry.isNull())
	{
		auto* const ifdClient = Env::getSingleton<RemoteIfdClient>();
		connect(ifdClient, &IfdClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);

		qDebug() << "Starting to pair.";
		ifdClient->establishConnection(mRememberedServerEntry, pServerPsk);
	}
}


bool RemoteServiceModel::rememberServer(const QString& pDeviceId)
{
	mRememberedServerEntry = mAvailableRemoteDevices.getRemoteDeviceListEntry(pDeviceId);
	return !mRememberedServerEntry.isNull();
}


void RemoteServiceModel::onEstablishConnectionDone(const QSharedPointer<IfdListEntry>& pEntry, const GlobalStatus& pStatus)
{
	const auto* const ifdClient = Env::getSingleton<RemoteIfdClient>();
	disconnect(ifdClient, &IfdClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);
	qDebug() << "Pairing finished:" << pStatus;
	const auto deviceName = RemoteServiceSettings::escapeDeviceName(pEntry->getIfdDescriptor().getIfdName());
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
		const QString peerName = settings.getRemoteInfo(mContext->getIfdServer()->getCurrentCertificate()).getNameEscaped();
		//: INFO ANDROID IOS The smartphone is connected as card reader (SaK) and currently processing an authentication request. The user is asked to pay attention the its screen.
		mConnectionInfo = tr("Please pay attention to the display on your other device \"%1\".").arg(peerName);
		Q_EMIT fireConnectionInfoChanged();
	}
	Q_EMIT fireConnectedChanged();
}


void RemoteServiceModel::onCardConnected(const QSharedPointer<CardConnection>& pConnection)
{
	pConnection->setProgressMessage(mConnectionInfo);
}


void RemoteServiceModel::onCardDisconnected(const QSharedPointer<CardConnection>& pConnection)
{
	pConnection->setProgressMessage(mConnectionInfo);
}


void RemoteServiceModel::resetRemoteServiceContext(const QSharedPointer<IfdServiceContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetWorkflowContext(pContext);

	mPsk.clear();

	if (mContext)
	{
		connect(mContext.data(), &IfdServiceContext::fireIsRunningChanged, this, [this](){
				setStarting(false);
			});
		connect(mContext.data(), &IfdServiceContext::fireIsRunningChanged, this, &RemoteServiceModel::fireIsRunningChanged);
		connect(mContext->getIfdServer().data(), &IfdServer::firePskChanged, this, [this](const QByteArray& pPsk){
				mPsk = pPsk;
			});
		connect(mContext->getIfdServer().data(), &IfdServer::firePskChanged, this, &RemoteServiceModel::firePskChanged);
		connect(mContext->getIfdServer().data(), &IfdServer::fireConnectedChanged, this, &RemoteServiceModel::onConnectionInfoChanged);
		connect(mContext->getIfdServer().data(), &IfdServer::firePairingCompleted, this, &RemoteServiceModel::firePairingCompleted);
		connect(mContext.data(), &IfdServiceContext::fireCardConnected, this, &RemoteServiceModel::onCardConnected);
		connect(mContext.data(), &IfdServiceContext::fireCardDisconnected, this, &RemoteServiceModel::onCardDisconnected);
		connect(mContext.data(), &IfdServiceContext::fireEstablishPaceChannelUpdated, this, &RemoteServiceModel::fireEstablishPaceChannelUpdated);

		setPairing(mPairingRequested);
	}
	else
	{
		setStarting(false);
	}

	mPairingRequested = false;

	Q_EMIT fireIsRunningChanged();
	Q_EMIT fireConnectedChanged();
	Q_EMIT fireEstablishPaceChannelUpdated();
}


void RemoteServiceModel::setPairing(bool pEnabled)
{
	if (mContext)
	{
		mContext->getIfdServer()->setPairing(pEnabled);
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
		return mContext->getIfdServer()->isConnected() && !mContext->getIfdServer()->isPairingConnection();
	}

	return false;
}


bool RemoteServiceModel::enableTransportPinLink() const
{
	return mContext && mContext->allowToChangePinLength();
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
	return Env::getSingleton<RemoteIfdClient>()->hasAnnouncingRemoteDevices();
}


bool RemoteServiceModel::pinPadModeOn() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
}


QString RemoteServiceModel::getErrorMessage(bool pNfcPluginAvailable, bool pNfcPluginEnabled, bool pWifiEnabled) const
{
	if (!pWifiEnabled)
	{
		//: INFO ALL_PLATFORMS The WiFi feature is not enabled but required to use the smartphone as a card reader (SaK).
		return tr("Please connect your WiFi to use your smartphone as a card reader (SaC).");
	}
	if (!pNfcPluginAvailable)
	{
		//: INFO ALL_PLATFORMS The device does not offer NFC.
		return tr("The NFC radio standard is required for communication with the ID card.\n\nUnfortunately NFC is not available on your device.");
	}
	if (!pNfcPluginEnabled)
	{
		//: INFO ALL_PLATFORMS NFC is available but not active.
		return tr("The NFC radio standard is required for communication with the ID card.\n\nPlease enable NFC to use your smartphone as a card reader (SaC).");
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
		mContext->cancelPasswordRequest();
	}
}


void RemoteServiceModel::changePinLength()
{
	if (mContext)
	{
		mContext->changePinLength();
	}
}


void RemoteServiceModel::onConnectedDevicesChanged()
{
	auto* const ifdClient = Env::getSingleton<RemoteIfdClient>();
	const auto deviceInfos = ifdClient->getConnectedDeviceInfos();
	QStringList deviceNames;
	for (const auto& info : deviceInfos)
	{
		deviceNames.append(QLatin1Char('"') + info.getNameEscaped() + QLatin1Char('"'));
	}
	mConnectedServerDeviceNames = deviceNames.join(QLatin1String(", "));
	Q_EMIT fireConnectedServerDeviceNamesChanged();
}
