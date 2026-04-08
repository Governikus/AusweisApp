/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "LocalIfdReaderManagerPlugin.h"

#include "BuildHelper.h"
#include "SecureStorage.h"
#include "VersionNumber.h"

#include <QLoggingCategory>
#include <QThread>
#ifdef Q_OS_ANDROID
	#include <QCryptographicHash>
#endif

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(ifd)

LocalIfdReaderManagerPlugin::LocalIfdReaderManagerPlugin()
	: IfdReaderManagerPlugin(ReaderManagerPluginType::LOCAL_IFD)
	, mServiceConnected(false)
{
	updateState();
	if (!getInfo().isAvailable())
	{
		qCWarning(ifd) << "LocalIfd not available, could not find correct AusweisApp version";
	}
}


void LocalIfdReaderManagerPlugin::startScan(bool pAutoConnect)
{
	updateState();
	if (!getInfo().isAvailable())
	{
		qCWarning(ifd) << "LocalIfd not available, could not find correct AusweisApp version";
		return;
	}

	const auto ifdClient = LocalIfdReaderManagerPlugin::getIfdClient();
	connect(ifdClient, &IfdClient::fireDeviceAppeared, this, &LocalIfdReaderManagerPlugin::connectToReader);
	connect(ifdClient, &LocalIfdClient::fireDeviceAppeared, this, [this] {
				mServiceConnected = true;
				setState(LocalIfdState::CONNECTED);
				Q_EMIT fireStatusChanged(getInfo());
			});
	connect(ifdClient, &LocalIfdClient::fireDeviceVanished, this, [this] {
				mServiceConnected = false;
				setState(LocalIfdState::DISCONNECTED);
				Q_EMIT fireStatusChanged(getInfo());
			});

	IfdReaderManagerPlugin::startScan(pAutoConnect);
}


void LocalIfdReaderManagerPlugin::stopScan(const QString& pError)
{
	if (!getInfo().isAvailable())
	{
		return;
	}

	// Make sure to disconnect the signal, so we don't stop the scan again once the service closes the connection
	for (auto& dispatcher : getDispatchers())
	{
		disconnect(dispatcher.data(), &IfdDispatcherClient::fireClosed, this, &LocalIfdReaderManagerPlugin::onLocalIfdConnectionClosed);
	}

	const auto ifdClient = LocalIfdReaderManagerPlugin::getIfdClient();
	disconnect(ifdClient, &IfdClient::fireDeviceAppeared, this, &LocalIfdReaderManagerPlugin::connectToReader);
	IfdReaderManagerPlugin::stopScan(pError);
}


LocalIfdClient* LocalIfdReaderManagerPlugin::getIfdClient() const
{
	return Env::getSingleton<LocalIfdClient>();
}


void LocalIfdReaderManagerPlugin::addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher)
{
	connect(pDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &LocalIfdReaderManagerPlugin::onLocalIfdConnectionClosed);
	IfdReaderManagerPlugin::addDispatcher(pDispatcher);
}


void LocalIfdReaderManagerPlugin::onLocalIfdConnectionClosed(GlobalStatus::Code pCloseCode, const QByteArray& pId)
{
	Q_UNUSED(pId)

	qCDebug(ifd) << "Stopping scan, because service closed connection with status:" << getEnumName(pCloseCode);
	stopScan();
}


bool LocalIfdReaderManagerPlugin::isAusweisAppInstalled()
#ifndef Q_OS_ANDROID
const
#endif
{
	if (mServiceConnected)
	{
		return true;
	}

#if defined(Q_OS_ANDROID)
	const auto* secureStorage = Env::getSingleton<SecureStorage>();
	const auto& aaPackageName = secureStorage->getLocalIfdPackageName();
	const auto& aaMinVersion = VersionNumber(secureStorage->getLocalIfdMinVersion());
	const auto& aaCertificateHashes = secureStorage->getLocalIfdAllowedCertificateHashes();

	const auto& packageInfo = BuildHelper::getPackageInfo(aaPackageName);
	if (!packageInfo.isValid())
	{
		qCWarning(ifd) << "Could not find installed AusweisApp";
		setState(LocalIfdState::NOT_INSTALLED);
		return false;
	}

	const auto& versionName = packageInfo.getObjectField<jstring>("versionName").toString();
	const auto& versionNumber = VersionNumber(versionName);
	if (versionNumber < aaMinVersion)
	{
		qCWarning(ifd) << "Invalid AusweisApp:" << versionNumber << ", required version >=" << aaMinVersion.getVersionNumber().toString();
		setState(LocalIfdState::INCOMPATIBLE_VERSION);
		return false;
	}

	const auto& certificates = BuildHelper::getAppCertificates(aaPackageName);
	bool hasValidCertificate = std::any_of(certificates.begin(), certificates.end(), [aaCertificateHashes](const auto& certificate){
				return aaCertificateHashes.contains(QCryptographicHash::hash(certificate, QCryptographicHash::Sha256));
			});
	if (!hasValidCertificate)
	{
		qCWarning(ifd) << "Invalid AusweisApp certificate";
		setState(LocalIfdState::INVALID_CERTIFICATE);
		return false;
	}

	setState(LocalIfdState::DISCONNECTED);
	return true;

#else
	return false;

#endif
}


void LocalIfdReaderManagerPlugin::setState(LocalIfdState pState)
{
	setPluginValue(ReaderManagerPluginInfo::Key::LOCAL_IFD_STATE, getEnumName(pState));
}


void LocalIfdReaderManagerPlugin::updateState()
{
	const bool ausweisAppInstalled = isAusweisAppInstalled();
	if (getInfo().isAvailable() != ausweisAppInstalled)
	{
		setPluginAvailable(ausweisAppInstalled);
	}
}


void LocalIfdReaderManagerPlugin::connectToReader(const QSharedPointer<IfdListEntry> pIfdDevice) const
{
	const auto ifdClient = getIfdClient();
	ifdClient->establishConnection(pIfdDevice, ifdClient->getPsk());
}
