/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
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

Q_DECLARE_LOGGING_CATEGORY(ifd)

using namespace governikus;


ConnectRequest::ConnectRequest(const IfdDescriptor& pIfdDescriptor,
		const QByteArray& pPsk,
		int pTimeoutMs)
	: mIfdDescriptor(pIfdDescriptor)
	, mPsk(pPsk)
	, mSocket(new QWebSocket(), &QObject::deleteLater)
	, mTimer()
{
	if (mIfdDescriptor.isLocalIfd())
	{
		if (mPsk.isEmpty())
		{
			Q_ASSERT(false);
			qCCritical(ifd) << "Missing pre-shared key for a local ifd connection";
		}
	}
	else
	{
		auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		const bool invalidLength = !TlsChecker::hasValidCertificateKeyLength(remoteServiceSettings.getCertificate());
		if (!remoteServiceSettings.checkAndGenerateKey(invalidLength))
		{
			qCCritical(ifd) << "Cannot get required key/certificate for tls";
			return;
		}
	}

	setTlsConfiguration();

	connect(mSocket.data(), &QWebSocket::preSharedKeyAuthenticationRequired, this, &ConnectRequest::onPreSharedKeyAuthenticationRequired);
	connect(mSocket.data(), &QWebSocket::sslErrors, this, &ConnectRequest::onSslErrors);

	mSocket->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
	connect(mSocket.data(), &QWebSocket::connected, this, &ConnectRequest::onConnected);
	connect(mSocket.data(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &ConnectRequest::onError);

	mTimer.setSingleShot(true);
	mTimer.setInterval(pTimeoutMs);
	connect(&mTimer, &QTimer::timeout, this, &ConnectRequest::onTimeout);
}


void ConnectRequest::setTlsConfiguration() const
{
	QSslConfiguration config;

	if (mIfdDescriptor.isLocalIfd())
	{
		config = Env::getSingleton<SecureStorage>()->getTlsConfigLocalIfd().getConfiguration();
		qCInfo(ifd) << "Start local connection";
	}
	else
	{
		auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		if (mPsk.isEmpty())
		{
			config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd().getConfiguration();
			config.setCaCertificates(remoteServiceSettings.getTrustedCertificates());
			qCInfo(ifd) << "Start reconnect to server";
		}
		else
		{
			config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getConfiguration();
			qCInfo(ifd) << "Start pairing to server";
		}

		config.setPrivateKey(remoteServiceSettings.getKey());
		config.setLocalCertificate(remoteServiceSettings.getCertificate());
		config.setPeerVerifyMode(QSslSocket::VerifyPeer);
	}

	mSocket->setSslConfiguration(config);
}


void ConnectRequest::onConnected()
{
	mTimer.stop();

	const auto& cfg = mSocket->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(ifd));

	bool isRemotePairing = false;
	bool abortConnection = false;
	if (mIfdDescriptor.isLocalIfd())
	{
		abortConnection |= !TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey());
	}
	else
	{
		const auto& secureStorage = Env::getSingleton<SecureStorage>();
		const auto& pairingTlsConfig = secureStorage->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK);
		isRemotePairing = pairingTlsConfig.getCiphers().contains(cfg.sessionCipher());

		abortConnection |= !TlsChecker::hasValidCertificateKeyLength(cfg.peerCertificate());
		abortConnection |= (!isRemotePairing && !TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey()));
	}

	if (abortConnection)
	{
		qCCritical(ifd) << "Server denied... abort connection!";
		mSocket->abort();
		Q_EMIT fireConnectionError(mIfdDescriptor, IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
		return;
	}

	if (mIfdDescriptor.isLocalIfd())
	{
		qCDebug(ifd) << "Connected to localhost";
		Q_EMIT fireConnectionCreated(mIfdDescriptor, mSocket);
		return;
	}

	qCDebug(ifd) << "Connected to remote device";

	auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	if (isRemotePairing)
	{
		qCDebug(ifd) << "Pairing completed | Add certificate:" << cfg.peerCertificate();
		settings.addTrustedCertificate(cfg.peerCertificate());
	}
	else
	{
		auto info = settings.getRemoteInfo(cfg.peerCertificate());
		info.setLastConnected(QDateTime::currentDateTime());
		settings.updateRemoteInfo(info);
	}

	Q_EMIT fireConnectionCreated(mIfdDescriptor, mSocket);
}


void ConnectRequest::onError(QAbstractSocket::SocketError pError)
{
	qCWarning(ifd) << "Connection error:" << pError;

	mTimer.stop();
	if (pError == QAbstractSocket::SocketError::RemoteHostClosedError)
	{
		Q_EMIT fireConnectionError(mIfdDescriptor, IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
	}
	else
	{
		Q_EMIT fireConnectionError(mIfdDescriptor, IfdErrorCode::CONNECTION_ERROR);
	}
}


void ConnectRequest::onTimeout()
{
	Q_EMIT fireConnectionTimeout(mIfdDescriptor);
}


void ConnectRequest::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
{
	qCDebug(ifd) << "Request pairing...";
	pAuthenticator->setPreSharedKey(mPsk);
}


void ConnectRequest::onSslErrors(const QList<QSslError>& pErrors)
{
	QList<QSslError::SslError> allowedErrors = {
		QSslError::HostNameMismatch
	};

	if (mIfdDescriptor.isLocalIfd())
	{
		allowedErrors << QSslError::NoPeerCertificate;
	}

	const auto& config = mSocket->sslConfiguration();
	const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getCiphers();
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
#if defined(GOVERNIKUS_QT) || (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
		mSocket->ignoreSslErrors(pErrors);
#else
		mSocket->ignoreSslErrors();
#endif
		return;
	}

	qCDebug(ifd) << "Server is untrusted | cipher:" << config.sessionCipher() << "| certificate:" << config.peerCertificate() << "| error:" << pErrors;
}


const IfdDescriptor& ConnectRequest::getIfdDescriptor() const
{
	return mIfdDescriptor;
}


void ConnectRequest::start()
{
	mSocket->open(mIfdDescriptor.getUrl());
	mTimer.start();
}
