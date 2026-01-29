/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
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


ConnectRequest::ConnectRequest(const Discovery& pDiscovery,
		const QByteArray& pPsk,
		int pTimeoutMs)
	: mDiscovery(pDiscovery)
	, mPsk(pPsk)
	, mSockets()
	, mTimer()
	, mRemoteHostRefusedConnection(false)
{
	if (mDiscovery.isLocalIfd())
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
		if (!remoteServiceSettings.checkAndGenerateKey(Env::getSingleton<SecureStorage>()->getIfdCreateSize()))
		{
			qCCritical(ifd) << "Cannot get required key/certificate for tls";
			return;
		}
	}

	mTimer.setSingleShot(true);
	mTimer.setInterval(pTimeoutMs);
	connect(&mTimer, &QTimer::timeout, this, &ConnectRequest::onTimeout);
}


QSslConfiguration ConnectRequest::getTlsConfiguration() const
{
	QSslConfiguration config;

	if (mDiscovery.isLocalIfd())
	{
		config = Env::getSingleton<SecureStorage>()->getTlsConfigLocalIfd().getConfiguration();
	}
	else
	{
		auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		if (mPsk.isEmpty())
		{
			config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd().getConfiguration();
			config.setCaCertificates(remoteServiceSettings.getTrustedCertificates());
		}
		else
		{
			config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getConfiguration();
		}

		config.setPrivateKey(remoteServiceSettings.getKey());
		config.setLocalCertificateChain(remoteServiceSettings.getCertificates());
		config.setPeerVerifyMode(QSslSocket::VerifyPeer);
	}

	return config;
}


QAbstractSocket::SocketState ConnectRequest::getState(const QSharedPointer<QWebSocket>& pSocket) const
{
	if (!pSocket)
	{
		return QAbstractSocket::UnconnectedState;
	}

	return pSocket->state();
}


void ConnectRequest::processResult(const QSharedPointer<QWebSocket>& pSocket)
{
	const auto state = getState(pSocket);
	if (pSocket)
	{
		qCDebug(ifd) << "    Connection to" << pSocket->requestUrl() << "finished with" << state;
		if (!mSockets.removeOne(pSocket))
		{
			qCWarning(ifd) << "Ignoring result from unexpected socket. ConnectRequest has probably already been completed";
			return;
		}

		if (state != QAbstractSocket::ConnectedState && !mSockets.isEmpty())
		{
			qCDebug(ifd) << "    Connection failed. Waiting for pending connections";
			return;
		}
	}

	mTimer.stop();
	for (const auto& socket : std::as_const(mSockets))
	{
		socket->abort();
	}
	mSockets.clear();

	if (pSocket && state == QAbstractSocket::ConnectedState)
	{
		qCDebug(ifd) << "    Connection succeeded";
		Q_EMIT fireConnectionCreated(this, pSocket);
		return;
	}

	if (pSocket)
	{
		qCDebug(ifd) << "    Connection failed. No more pending connections left";
	}
	else
	{
		qCWarning(ifd) << "Connection could not be established after" << mTimer.interval() << "ms";
	}
	if (mRemoteHostRefusedConnection)
	{
		Q_EMIT fireConnectionError(this, IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
		return;
	}

	Q_EMIT fireConnectionError(this, IfdErrorCode::CONNECTION_ERROR);
}


void ConnectRequest::onConnected(const QSharedPointer<QWebSocket>& pSocket)
{
	const auto& cfg = pSocket->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(ifd));

	bool isRemotePairing = false;
	bool abortConnection = false;

	const auto& minimalKeySizes = [](QSsl::KeyAlgorithm pKeyAlgorithm){
				return Env::getSingleton<SecureStorage>()->getMinimumIfdKeySize(pKeyAlgorithm);
			};

	if (mDiscovery.isLocalIfd())
	{
		abortConnection |= !TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey(), minimalKeySizes);
	}
	else
	{
		const auto* secureStorage = Env::getSingleton<SecureStorage>();
		const auto& pairingTlsConfig = secureStorage->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK);
		isRemotePairing = pairingTlsConfig.getCiphers().contains(cfg.sessionCipher());

		abortConnection |= !TlsChecker::hasValidCertificateKeyLength(cfg.peerCertificate(), minimalKeySizes);
		abortConnection |= (!isRemotePairing && !TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey(), minimalKeySizes));
	}

	const auto rootCert = TlsChecker::getRootCertificate(cfg.peerCertificateChain());
	if (rootCert.isNull())
	{
		qCCritical(ifd) << "    No root certificate found!";
		abortConnection = true;
	}

	if (abortConnection)
	{
		qCCritical(ifd) << "    Server denied... abort connection!";
		pSocket->abort();
		mRemoteHostRefusedConnection = true;
		processResult(pSocket);
		return;
	}

	if (mDiscovery.isLocalIfd())
	{
		qCDebug(ifd) << "    Connected to localhost";
	}
	else
	{
		qCDebug(ifd) << "    Connected to remote device";

		auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		if (isRemotePairing)
		{
			qCDebug(ifd) << "    Pairing completed | Add certificate:" << rootCert;
			settings.addTrustedCertificate(rootCert);
		}
		else
		{
			auto info = settings.getRemoteInfo(rootCert);
			info.setLastConnected(QDateTime::currentDateTime());
			settings.updateRemoteInfo(info);
		}
	}

	processResult(pSocket);
}


void ConnectRequest::onError(const QSharedPointer<QWebSocket>& pSocket, QAbstractSocket::SocketError pError)
{
	if (pSocket)
	{
		qCWarning(ifd) << "    Connection error:" << pError << pSocket->errorString();
	}
	else
	{
		qCWarning(ifd) << "    Connection error:" << pError;
	}

	if (pError == QAbstractSocket::SocketError::RemoteHostClosedError
			|| pError == QAbstractSocket::SocketError::SslHandshakeFailedError)
	{
		mRemoteHostRefusedConnection = true;
	}

	processResult(pSocket);
}


void ConnectRequest::onTimeout()
{
	processResult();
}


void ConnectRequest::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator) const
{
	qCDebug(ifd) << "    Request pairing...";
	pAuthenticator->setPreSharedKey(mPsk);
}


void ConnectRequest::onSslErrors(const QSharedPointer<QWebSocket>& pSocket, const QList<QSslError>& pErrors) const
{
	QList allowedErrors = {
		QSslError::HostNameMismatch
	};

	if (mDiscovery.isLocalIfd())
	{
		allowedErrors << QSslError::NoPeerCertificate;
	}

	const auto& config = pSocket->sslConfiguration();
	const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getCiphers();
	if (pairingCiphers.contains(config.sessionCipher()))
	{
		allowedErrors << QSslError::SelfSignedCertificate;
		allowedErrors << QSslError::SelfSignedCertificateInChain;
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
		pSocket->ignoreSslErrors(pErrors);
		return;
	}

	qCDebug(ifd) << "    Server is untrusted | cipher:" << config.sessionCipher() << "| certificate:" << config.peerCertificate() << "| error:" << pErrors;
}


const Discovery& ConnectRequest::getDiscovery() const
{
	return mDiscovery;
}


void ConnectRequest::start()
{
	const auto& addresses = mDiscovery.getAddresses();
	if (addresses.isEmpty())
	{
		Q_EMIT fireConnectionError(this, IfdErrorCode::INVALID_REQUEST);
		return;
	}

	if (mDiscovery.isLocalIfd())
	{
		qCInfo(ifd) << "Start local connection. Addresses:" << addresses;
	}
	else
	{
		if (mPsk.isEmpty())
		{
			qCInfo(ifd) << "Start reconnect to server. Addresses:" << addresses;
		}
		else
		{
			qCInfo(ifd) << "Start pairing to server. Addresses:" << addresses;
		}
	}

	const auto& tlsConfig = getTlsConfiguration();
	for (const auto& address : addresses)
	{
		const auto& socket = QSharedPointer<QWebSocket>(new QWebSocket(), &QObject::deleteLater);
		socket->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
		socket->setSslConfiguration(tlsConfig);

		connect(socket.data(), &QWebSocket::preSharedKeyAuthenticationRequired, this, &ConnectRequest::onPreSharedKeyAuthenticationRequired);
		connect(socket.data(), &QWebSocket::sslErrors, this, [socket, this](const QList<QSslError>& pErrors){onSslErrors(socket, pErrors);});
		connect(socket.data(), &QWebSocket::connected, this, [socket, this](){onConnected(socket);});
		connect(socket.data(), &QWebSocket::errorOccurred, this, [socket, this](QAbstractSocket::SocketError pError){onError(socket, pError);});

		mSockets << socket;
		QMetaObject::invokeMethod(this, [socket, address] {
					socket->open(address);
				}, Qt::QueuedConnection);
	}

	mTimer.start();
}
