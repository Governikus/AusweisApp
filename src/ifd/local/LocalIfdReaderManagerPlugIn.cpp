/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "LocalIfdReaderManagerPlugIn.h"

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

LocalIfdReaderManagerPlugIn::LocalIfdReaderManagerPlugIn()
	: IfdReaderManagerPlugIn(ReaderManagerPlugInType::LOCAL_IFD)
	, mServiceConnected(false)
{
	updateState();
	if (!getInfo().isAvailable())
	{
		qCWarning(ifd) << "Smart-eID not available, could not find correct AusweisApp version";
	}
}


void LocalIfdReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	updateState();
	if (!getInfo().isAvailable())
	{
		qCWarning(ifd) << "Smart-eID not available, could not find correct AusweisApp version";
		return;
	}

	const auto ifdClient = LocalIfdReaderManagerPlugIn::getIfdClient();
	connect(ifdClient, &IfdClient::fireDeviceAppeared, this, &LocalIfdReaderManagerPlugIn::connectToReader, Qt::UniqueConnection);
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

	IfdReaderManagerPlugIn::startScan(pAutoConnect);
}


void LocalIfdReaderManagerPlugIn::stopScan(const QString& pError)
{
	if (!getInfo().isAvailable())
	{
		return;
	}

	// Make sure to disconnect the signal, so we don't stop the scan again once the service closes the connection
	for (auto& dispatcher : getDispatchers())
	{
		disconnect(dispatcher.data(), &IfdDispatcherClient::fireClosed, this, &LocalIfdReaderManagerPlugIn::onLocalIfdConnectionClosed);
	}

	const auto ifdClient = LocalIfdReaderManagerPlugIn::getIfdClient();
	disconnect(ifdClient, &IfdClient::fireDeviceAppeared, this, &LocalIfdReaderManagerPlugIn::connectToReader);
	IfdReaderManagerPlugIn::stopScan(pError);
}


LocalIfdClient* LocalIfdReaderManagerPlugIn::getIfdClient() const
{
	return Env::getSingleton<LocalIfdClient>();
}


void LocalIfdReaderManagerPlugIn::addDispatcher(const QSharedPointer<IfdDispatcherClient>& pDispatcher)
{
	connect(pDispatcher.data(), &IfdDispatcherClient::fireClosed, this, &LocalIfdReaderManagerPlugIn::onLocalIfdConnectionClosed);
	IfdReaderManagerPlugIn::addDispatcher(pDispatcher);
}


void LocalIfdReaderManagerPlugIn::onLocalIfdConnectionClosed(GlobalStatus::Code pCloseCode, const QString& pId)
{
	Q_UNUSED(pId)

	qCDebug(ifd) << "Stopping scan, because service closed connection with status:" << getEnumName(pCloseCode);
	stopScan(tr("Connection closed by remote with status: %1.").arg(getEnumName(pCloseCode)));
}


bool LocalIfdReaderManagerPlugIn::isAusweisAppInstalled()
{
	if (mServiceConnected)
	{
		return true;
	}

#if defined(Q_OS_ANDROID)
	const auto& secureStorage = Env::getSingleton<SecureStorage>();
	const auto& aa2PackageName = secureStorage->getLocalIfdPackageName();
	const auto& aa2MinVersion = VersionNumber(secureStorage->getLocalIfdMinVersion());
	const auto& aa2CertificateHashes = secureStorage->getLocalIfdAllowedCertificateHashes();

	const auto& packageInfo = BuildHelper::getPackageInfo(aa2PackageName);
	if (!packageInfo.isValid())
	{
		qCWarning(ifd) << "Could not find installed AusweisApp";
		setState(LocalIfdState::NOT_INSTALLED);
		return false;
	}

	const auto& versionName = packageInfo.getObjectField<jstring>("versionName").toString();
	const auto& versionNumber = VersionNumber(versionName);
	if (versionNumber < aa2MinVersion)
	{
		qCWarning(ifd) << "Invalid AusweisApp:" << versionNumber << ", required version >=" << aa2MinVersion.getVersionNumber().toString();
		setState(LocalIfdState::INCOMPATIBLE_VERSION);
		return false;
	}

	const auto& certificates = BuildHelper::getAppCertificates(aa2PackageName);
	bool hasValidCertificate = std::any_of(certificates.begin(), certificates.end(), [aa2CertificateHashes](const auto& certificate){
			return aa2CertificateHashes.contains(QCryptographicHash::hash(certificate, QCryptographicHash::Sha256));
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


void LocalIfdReaderManagerPlugIn::setState(LocalIfdState pState)
{
	setPlugInValue(ReaderManagerPlugInInfo::Key::LOCAL_IFD_STATE, getEnumName(pState));
}


void LocalIfdReaderManagerPlugIn::updateState()
{
	const bool ausweisAppInstalled = isAusweisAppInstalled();
	if (getInfo().isAvailable() != ausweisAppInstalled)
	{
		setPlugInAvailable(ausweisAppInstalled);
	}
}


void LocalIfdReaderManagerPlugIn::connectToReader(const QSharedPointer<IfdListEntry> pIfdDevice) const
{
	const auto ifdClient = getIfdClient();
	ifdClient->establishConnection(pIfdDevice, ifdClient->getPsk());
}
