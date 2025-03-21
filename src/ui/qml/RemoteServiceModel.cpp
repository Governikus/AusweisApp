/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteServiceModel.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "ReaderManager.h"
#include "RemoteIfdClient.h"
#include "RemoteIfdServer.h"
#include "RemoteServiceSettings.h"
#include "controller/IfdServiceController.h"

#include <QOperatingSystemVersion>


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
	, mAllDevices(this)
	, mAvailableDevicesInPairingMode(&mAllDevices, RemoteDeviceFilterModel::showActivePairingMode)
	, mAvailablePairedDevices(&mAllDevices, RemoteDeviceFilterModel::showAvailableAndPaired)
	, mUnavailablePairedDevices(&mAllDevices, RemoteDeviceFilterModel::showUnavailableAndPaired)
	, mConnectionInfo()
	, mConnectedServerDeviceNames()
	, mRememberedServerEntry()
	, mRequiresLocalNetworkPermission(QOperatingSystemVersion::currentType() == QOperatingSystemVersion::IOS)
#ifdef Q_OS_IOS
	, mWasRunning(false)
	, mWasPairing(false)
#endif
{
	QQmlEngine::setObjectOwnership(&mAllDevices, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&mAvailableDevicesInPairingMode, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&mAvailablePairedDevices, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&mUnavailablePairedDevices, QQmlEngine::CppOwnership);

	const auto* readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::firePluginAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderAdded, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(readerManager, &ReaderManager::fireReaderRemoved, this, &RemoteServiceModel::onEnvironmentChanged);
	const auto* applicationModel = Env::getSingleton<ApplicationModel>();
	connect(applicationModel, &ApplicationModel::fireWifiEnabledChanged, this, &RemoteServiceModel::onEnvironmentChanged);
	connect(applicationModel, &ApplicationModel::fireApplicationStateChanged, this, &RemoteServiceModel::onApplicationStateChanged);

	const auto* const ifdClient = Env::getSingleton<RemoteIfdClient>();
	connect(ifdClient, &IfdClient::fireNewDispatcher, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(ifdClient, &IfdClient::fireDispatcherChanged, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(ifdClient, &IfdClient::fireDispatcherDestroyed, this, &RemoteServiceModel::onConnectedDevicesChanged);
	connect(ifdClient, &IfdClient::fireDispatcherDestroyed, this, &RemoteServiceModel::fireConnectedServerDisconnected);
	connect(ifdClient, &IfdClient::fireDeviceAppeared, this, &RemoteServiceModel::fireRemoteReaderVisibleChanged);
	connect(ifdClient, &IfdClient::fireDeviceVanished, this, &RemoteServiceModel::fireRemoteReaderVisibleChanged);
	connect(ifdClient, &IfdClient::fireCertificateRemoved, this, &RemoteServiceModel::fireCertificateRemoved);

	connect(this, &WorkflowModel::fireReaderPluginTypeChanged, this, &RemoteServiceModel::onReaderPluginTypesChanged);

	QMetaObject::invokeMethod(this, &RemoteServiceModel::onEnvironmentChanged, Qt::QueuedConnection);
}


void RemoteServiceModel::onEnvironmentChanged()
{
	const auto& nfcInfo = Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::NFC);
	const bool nfcPluginAvailable = nfcInfo.isAvailable();
	const bool nfcPluginEnabled = nfcInfo.isEnabled();

	bool readerAvailable = !(Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({ReaderManagerPluginType::NFC})).isEmpty());
	const bool wifiEnabled = Env::getSingleton<ApplicationModel>()->isWifiEnabled();

	const bool runnable = readerAvailable && wifiEnabled;
	const bool canEnableNfc = nfcPluginAvailable && !nfcPluginEnabled;
	const QString errorMessage = getErrorMessage(nfcPluginEnabled, wifiEnabled);
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
#ifndef Q_OS_IOS
const
#endif
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


void RemoteServiceModel::onPairingCompleted(const QSslCertificate& pCertificate)
{
	mAllDevices.setLastPairedReader(pCertificate);
	Q_EMIT firePairingCompleted();
}


void RemoteServiceModel::onTranslationChanged()
{
	onEnvironmentChanged();
}


void RemoteServiceModel::onReaderPluginTypesChanged(bool pExplicitStart)
{
	if (!mContext)
	{
		return;
	}

	const auto& pluginType = getReaderPluginType();
	if (mContext->getIfdServer() && mContext->getIfdServer()->getMessageHandler())
	{
		mContext->getIfdServer()->getMessageHandler()->setAllowedCardTypes({pluginType});
	}

	auto* readerManager = Env::getSingleton<ReaderManager>();
	const auto& readerPluginTypes = Enum<ReaderManagerPluginType>::getList();
	for (const auto t : readerPluginTypes)
	{
		if (t != pluginType)
		{
			readerManager->stopScan(t);
			continue;
		}

#ifdef Q_OS_IOS
		if (pluginType != ReaderManagerPluginType::NFC || pExplicitStart)
#else
		Q_UNUSED(pExplicitStart)
#endif
		{
			readerManager->startScan(pluginType);
		}
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
		const auto ifdServer = QSharedPointer<IfdServer>(new RemoteIfdServer());
		const auto request = WorkflowRequest::create<IfdServiceController, IfdServiceContext>(ifdServer);
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


RemoteDeviceModel* RemoteServiceModel::getAllDevices()
{
	return &mAllDevices;
}


RemoteDeviceFilterModel* RemoteServiceModel::getAvailablePairedDevices()
{
	return &mAvailablePairedDevices;
}


RemoteDeviceFilterModel* RemoteServiceModel::getAvailableDevicesInPairingMode()
{
	return &mAvailableDevicesInPairingMode;
}


RemoteDeviceFilterModel* RemoteServiceModel::getUnavailablePairedDevices()
{
	return &mUnavailablePairedDevices;
}


void RemoteServiceModel::startDetection()
{
	mAllDevices.startDetection();
}


void RemoteServiceModel::stopDetection(bool pStopScan)
{
	mAllDevices.stopDetection(pStopScan);
}


void RemoteServiceModel::connectToRememberedServer(const QByteArray& pServerPsk)
{
	if (!pServerPsk.isEmpty() && !mRememberedServerEntry.isNull())
	{
		auto* const ifdClient = Env::getSingleton<RemoteIfdClient>();
		connect(ifdClient, &IfdClient::fireEstablishConnectionDone, this, &RemoteServiceModel::onEstablishConnectionDone);

		qDebug() << "Starting to pair.";
		ifdClient->establishConnection(mRememberedServerEntry, pServerPsk);
	}
}


QList<ReaderManagerPluginType> RemoteServiceModel::getSupportedReaderPluginTypes() const
{
#if __has_include("SmartManager.h")
	return {ReaderManagerPluginType::NFC, ReaderManagerPluginType::SMART};

#else
	return {ReaderManagerPluginType::NFC};

#endif
}


bool RemoteServiceModel::rememberServer(const QVariant& pDeviceId)
{
	mRememberedServerEntry = mAllDevices.getRemoteDeviceListEntry(pDeviceId.toByteArray());
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
		//: INFO ANDROID IOS The smartphone is connected as card reader (SaK) and currently processing an authentication request. The user is asked to pay attention the its screen.
		mConnectionInfo = tr("Please pay attention to the display on your other device \"%1\".").arg(getConnectedClientName());
		Q_EMIT fireConnectionInfoChanged();
	}
	else if (mContext)
	{
		mContext->setReaderPluginTypes({});
	}
	Q_EMIT fireConnectedChanged();
}


void RemoteServiceModel::onCardConnected(const QSharedPointer<CardConnection>& pConnection) const
{
	pConnection->setProgressMessage(mConnectionInfo);
}


void RemoteServiceModel::onCardDisconnected(const QSharedPointer<CardConnection>& pConnection) const
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
		connect(mContext->getIfdServer().data(), &IfdServer::fireNameChanged, this, &RemoteServiceModel::onNameChanged);
		connect(mContext->getIfdServer().data(), &IfdServer::firePskChanged, this, &RemoteServiceModel::firePskChanged);
		connect(mContext.data(), &IfdServiceContext::fireDisplayTextChanged, this, &RemoteServiceModel::fireDisplayTextChanged);
		connect(mContext->getIfdServer().data(), &IfdServer::fireConnectedChanged, this, &RemoteServiceModel::onConnectionInfoChanged);
		connect(mContext->getIfdServer().data(), &IfdServer::firePairingCompleted, this, &RemoteServiceModel::onPairingCompleted);
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
	Q_EMIT fireDisplayTextChanged();
}


void RemoteServiceModel::setPairing(bool pEnabled) const
{
	if (mContext)
	{
		mContext->getIfdServer()->setPairing(pEnabled);
	}
}


bool RemoteServiceModel::isPairing() const
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


QString RemoteServiceModel::getDisplayText() const
{
	QString displayText = mContext ? mContext->getDisplayText() : QString();
	return displayText.remove(getPercentMatcher());
}


int RemoteServiceModel::getPercentage() const
{
	QString displayText = mContext ? mContext->getDisplayText() : QString();
	return getPercentMatcher().match(displayText).captured(2).toInt();
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


QString RemoteServiceModel::getTransactionInfo() const
{
	return QString();
}


QString RemoteServiceModel::getConnectedClientName() const
{
	if (mContext)
	{
		const auto& sslCertificate = mContext->getIfdServer()->getCurrentCertificate();
		const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		return settings.getRemoteInfo(sslCertificate).getNameEscaped();
	}

	return QString();
}


bool RemoteServiceModel::pinPadModeOn() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
}


QString RemoteServiceModel::getErrorMessage(bool pNfcPluginEnabled, bool pWifiEnabled) const
{
	if (!pWifiEnabled)
	{
		//: INFO ALL_PLATFORMS The WiFi feature is not enabled but required to use the smartphone as a card reader (SaK).
		return tr("Please connect your WiFi to use your smartphone as a card reader (SaC).");
	}
	if (!pNfcPluginEnabled)
	{
		//: INFO ALL_PLATFORMS NFC is available but not active.
		return tr("The NFC radio standard is required for communication with the ID card.\n\nPlease enable NFC to use your smartphone as a card reader (SaC).");
	}

	return QString();
}


QRegularExpression RemoteServiceModel::getPercentMatcher() const
{
	return QRegularExpression(QStringLiteral("(\\s|^)(100|\\d{1,2}) ?%"));
}


void RemoteServiceModel::forgetDevice(const QVariant& pId)
{
	mAllDevices.forgetDevice(pId.toByteArray());
}


void RemoteServiceModel::cancelPasswordRequest()
{
	if (mContext)
	{
		mContext->userError(StatusCode::INPUT_CANCELLED);
	}
}


void RemoteServiceModel::passwordsDiffer()
{
	if (mContext)
	{
		mContext->userError(StatusCode::PASSWORDS_DIFFER);
	}
}


void RemoteServiceModel::changePinLength()
{
	if (mContext)
	{
		mContext->changePinLength();
	}
}


bool RemoteServiceModel::isPinAuthentication() const
{
	return mContext && mContext->isPinAuthentication();
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


void RemoteServiceModel::onNameChanged()
{
	onConnectionInfoChanged(true);
}
