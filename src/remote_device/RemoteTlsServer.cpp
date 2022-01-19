/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteTlsServer.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "Randomizer.h"
#include "SecureStorage.h"
#include "TlsChecker.h"

#include <QHostAddress>
#include <QLoggingCategory>
#include <QNetworkProxy>
#include <QSharedPointer>

Q_DECLARE_LOGGING_CATEGORY(remote_device)

using namespace governikus;


RemoteTlsServer::RemoteTlsServer()
	: QTcpServer()
	, mSocket()
	, mPsk()
{
	//listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
	setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
}


RemoteTlsServer::~RemoteTlsServer()
{
	if (mSocket)
	{
		mSocket->deleteLater();
	}
}


bool RemoteTlsServer::listen()
{
	if (isListening())
	{
		return false;
	}

	auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const bool invalidLength = !TlsChecker::hasValidCertificateKeyLength(remoteServiceSettings.getCertificate());
	if (!remoteServiceSettings.checkAndGenerateKey(invalidLength))
	{
		qCCritical(remote_device) << "Cannot get required key/certificate for tls";
		return false;
	}

	static quint16 usedServerPort = 0;
	if (!QTcpServer::listen(QHostAddress::Any, usedServerPort))
	{
		qCCritical(remote_device) << "Listen failed:" << serverError() << ", " << errorString();
		return false;
	}

	if (usedServerPort == 0)
	{
		usedServerPort = serverPort();
	}
	return true;
}


void RemoteTlsServer::incomingConnection(qintptr pSocketDescriptor)
{
	if (mSocket.isNull())
	{
		mSocket = new QSslSocket();

		const auto cipherCfg = mPsk.isEmpty() ? SecureStorage::TlsSuite::DEFAULT : SecureStorage::TlsSuite::PSK;
		QSslConfiguration config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(cipherCfg).getConfiguration();
		const auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		config.setPrivateKey(settings.getKey());
		config.setLocalCertificate(settings.getCertificate());
		config.setPeerVerifyMode(QSslSocket::VerifyPeer);
		if (mPsk.isEmpty())
		{
			config.setCaCertificates(settings.getTrustedCertificates());
		}
		mSocket->setSslConfiguration(config);

		if (Q_LIKELY(mSocket->setSocketDescriptor(pSocketDescriptor)))
		{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
			connect(mSocket.data(), &QAbstractSocket::errorOccurred, this, &RemoteTlsServer::onError);
#else
			connect(mSocket.data(), QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
					this, &RemoteTlsServer::onError);
#endif

			connect(mSocket.data(), QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
					this, &RemoteTlsServer::onSslErrors);
			connect(mSocket.data(), &QSslSocket::preSharedKeyAuthenticationRequired,
					this, &RemoteTlsServer::onPreSharedKeyAuthenticationRequired);
			connect(mSocket.data(), &QSslSocket::encrypted, this, &RemoteTlsServer::onEncrypted);

			qCDebug(remote_device).noquote() << "Starting encryption for incoming connection from" << mSocket->peerAddress().toString();
			mSocket->startServerEncryption();
		}
		else
		{
			qCDebug(remote_device) << "Failed to set the socket descriptor";
			delete mSocket.data();
		}
	}
	else
	{
		QTcpSocket socket;
		socket.setSocketDescriptor(pSocketDescriptor);
		qCDebug(remote_device).noquote() << "Socket already connected. Incoming connection from" << socket.peerAddress().toString() << "refused";
		socket.abort();
	}
}


void RemoteTlsServer::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
{
	qCDebug(remote_device) << "Client requests pairing | identity:" << pAuthenticator->identity() << "| hint:" << pAuthenticator->identityHint();
	pAuthenticator->setPreSharedKey(mPsk);
}


void RemoteTlsServer::onError(QAbstractSocket::SocketError pSocketError)
{
	qCDebug(remote_device) << "Socket error:" << pSocketError << mSocket->errorString();
	mSocket->deleteLater();
}


void RemoteTlsServer::onSslErrors(const QList<QSslError>& pErrors)
{
	if (pErrors.size() == 1 && pErrors.first().error() == QSslError::SelfSignedCertificate)
	{
		const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getCiphers();
		if (pairingCiphers.contains(mSocket->sessionCipher()))
		{
			qCDebug(remote_device) << "Client requests pairing | cipher:" << mSocket->sessionCipher() << "| certificate:" << mSocket->peerCertificate();
			mSocket->ignoreSslErrors();
			return;
		}
	}

	qCDebug(remote_device) << "Client is not allowed | cipher:" << mSocket->sessionCipher() << "| certificate:" << mSocket->peerCertificate() << "| error:" << pErrors;
}


void RemoteTlsServer::onEncrypted()
{
	const auto& cfg = mSocket->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(remote_device));

	if (!TlsChecker::hasValidCertificateKeyLength(cfg.peerCertificate()))
	{
		qCCritical(remote_device) << "Client denied... abort connection!";
		mSocket->abort();
		mSocket->deleteLater();
		return;
	}

	qCDebug(remote_device) << "Client connected";

	auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getCiphers();
	if (pairingCiphers.contains(cfg.sessionCipher()))
	{
		qCDebug(remote_device) << "Pairing completed | Add certificate:" << cfg.peerCertificate();
		settings.addTrustedCertificate(cfg.peerCertificate());
		settings.save();
		setPairing(false);
		Q_EMIT firePairingCompleted();
	}
	else
	{
		auto info = settings.getRemoteInfo(cfg.peerCertificate());
		info.setLastConnected(QDateTime::currentDateTime());
		settings.updateRemoteInfo(info);
	}

	mSocket->disconnect(this);
	Q_EMIT newConnection(mSocket.data());
}


void RemoteTlsServer::setPairing(bool pEnable)
{
	if (pEnable)
	{
		std::uniform_int_distribution<int> uni(0, 9999);
		QByteArray pin = QByteArray::number(uni(Randomizer::getInstance().getGenerator()));
		pin.prepend(4 - pin.size(), '0');
		mPsk = pin;
		Q_EMIT firePskChanged(mPsk);
	}
	else if (!mPsk.isEmpty())
	{
		mPsk.clear();
		Q_EMIT firePskChanged(mPsk);
	}
}


QSslCertificate RemoteTlsServer::getCurrentCertificate() const
{
	return mSocket ? mSocket->sslConfiguration().peerCertificate() : QSslCertificate();
}
