/*!
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#include "ConnectRequest.h"

#include "AppSettings.h"
#include "Env.h"
#include "LogHandler.h"
#include "SecureStorage.h"
#include "TlsChecker.h"

#include <QLoggingCategory>
#include <QSslPreSharedKeyAuthenticator>
#include <QWebSocket>

Q_DECLARE_LOGGING_CATEGORY(remote_device)

using namespace governikus;


ConnectRequest::ConnectRequest(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor,
		const QByteArray& pPsk,
		int pTimeoutMs)
	: mRemoteDeviceDescriptor(pRemoteDeviceDescriptor)
	, mPsk(pPsk)
	, mSocket(new QWebSocket(), &QObject::deleteLater)
	, mTimer()
{
	auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const bool invalidLength = !TlsChecker::hasValidCertificateKeyLength(remoteServiceSettings.getCertificate());
	if (!remoteServiceSettings.checkAndGenerateKey(invalidLength))
	{
		qCCritical(remote_device) << "Cannot get required key/certificate for tls";
		return;
	}

	QSslConfiguration config;
	if (mPsk.isEmpty())
	{
		config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote().getConfiguration();
		config.setCaCertificates(remoteServiceSettings.getTrustedCertificates());
		qCInfo(remote_device) << "Start reconnect to server";
	}
	else
	{
		config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
		qCInfo(remote_device) << "Start pairing to server";
	}
	config.setPrivateKey(remoteServiceSettings.getKey());
	config.setLocalCertificate(remoteServiceSettings.getCertificate());
	config.setPeerVerifyMode(QSslSocket::VerifyPeer);
	mSocket->setSslConfiguration(config);

	connect(mSocket.data(), &QWebSocket::preSharedKeyAuthenticationRequired, this, &ConnectRequest::onPreSharedKeyAuthenticationRequired);
	connect(mSocket.data(), &QWebSocket::sslErrors, this, &ConnectRequest::onSslErrors);

	mSocket->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
	connect(mSocket.data(), &QWebSocket::connected, this, &ConnectRequest::onConnected);
	connect(mSocket.data(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ConnectRequest::onError);

	mTimer.setSingleShot(true);
	mTimer.setInterval(pTimeoutMs);
	connect(&mTimer, &QTimer::timeout, this, &ConnectRequest::onTimeout);
}


void ConnectRequest::onConnected()
{
	mTimer.stop();

	const auto& cfg = mSocket->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(remote_device));

	if (!TlsChecker::hasValidCertificateKeyLength(cfg.peerCertificate()) ||
			(!cfg.ephemeralServerKey().isNull() && !TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey())))
	{
		qCCritical(remote_device) << "Server denied... abort connection!";
		mSocket->abort();
		Q_EMIT fireConnectionError(mRemoteDeviceDescriptor, RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
		return;
	}

	qCDebug(remote_device) << "Connected to remote device";

	auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getCiphers();
	if (pairingCiphers.contains(cfg.sessionCipher()))
	{
		qCDebug(remote_device) << "Pairing completed | Add certificate:" << cfg.peerCertificate();
		settings.addTrustedCertificate(cfg.peerCertificate());
		settings.save();
	}
	else
	{
		auto info = settings.getRemoteInfo(cfg.peerCertificate());
		info.setLastConnected(QDateTime::currentDateTime());
		settings.updateRemoteInfo(info);
	}

	Q_EMIT fireConnectionCreated(mRemoteDeviceDescriptor, mSocket);
}


void ConnectRequest::onError(QAbstractSocket::SocketError pError)
{
	qCWarning(remote_device) << "Connection error:" << pError;

	mTimer.stop();
	if (pError == QAbstractSocket::SocketError::RemoteHostClosedError)
	{
		Q_EMIT fireConnectionError(mRemoteDeviceDescriptor, RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
	}
	else
	{
		Q_EMIT fireConnectionError(mRemoteDeviceDescriptor, RemoteErrorCode::CONNECTION_ERROR);
	}
}


void ConnectRequest::onTimeout()
{
	Q_EMIT fireConnectionTimeout(mRemoteDeviceDescriptor);
}


void ConnectRequest::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
{
	qCDebug(remote_device) << "Request pairing...";
	pAuthenticator->setPreSharedKey(mPsk);
}


void ConnectRequest::onSslErrors(const QList<QSslError>& pErrors)
{
	QList<QSslError::SslError> allowedErrors = {
		QSslError::HostNameMismatch
	};

	const auto& config = mSocket->sslConfiguration();
	const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getCiphers();
	if (pairingCiphers.contains(config.sessionCipher()))
	{
		allowedErrors << QSslError::SelfSignedCertificate;
	}

	bool ignoreErrors = true;
	for (const auto& error : pErrors)
	{
		if (!allowedErrors.contains(error.error()))
		{
			ignoreErrors = false;
			break;
		}
	}

	if (ignoreErrors)
	{
		mSocket->ignoreSslErrors();
		return;
	}

	qCDebug(remote_device) << "Server is untrusted | cipher:" << config.sessionCipher() << "| certificate:" << config.peerCertificate() << "| error:" << pErrors;
}


const RemoteDeviceDescriptor& ConnectRequest::getRemoteDeviceDescriptor() const
{
	return mRemoteDeviceDescriptor;
}


void ConnectRequest::start()
{
	mSocket->open(mRemoteDeviceDescriptor.getUrl());
	mTimer.start();
}
