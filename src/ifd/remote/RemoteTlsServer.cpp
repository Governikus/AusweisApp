/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteTlsServer.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "Randomizer.h"
#include "SecureStorage.h"
#include "TlsChecker.h"

#include <QHostAddress>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ifd)

using namespace governikus;


QSslConfiguration RemoteTlsServer::sslConfiguration() const
{
	const auto& psk = getPsk();
	const auto cipherCfg = psk.isEmpty() ? SecureStorage::TlsSuite::DEFAULT : SecureStorage::TlsSuite::PSK;
	QSslConfiguration config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(cipherCfg).getConfiguration();
	const auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	config.setPrivateKey(settings.getKey());
	config.setLocalCertificateChain(settings.getCertificates());
	config.setPeerVerifyMode(QSslSocket::VerifyPeer);

	if (psk.isEmpty())
	{
		config.setCaCertificates(settings.getTrustedCertificates());
	}

	return config;
}


RemoteTlsServer::RemoteTlsServer()
	: TlsServer()
{
}


bool RemoteTlsServer::startListening(quint16 pPort)
{
	Q_UNUSED(pPort)
	if (QTcpServer::isListening())
	{
		return false;
	}

	auto& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	if (!remoteServiceSettings.checkAndGenerateKey(Env::getSingleton<SecureStorage>()->getIfdCreateSize()))
	{
		qCCritical(ifd) << "Cannot get required key/certificate for tls";
		return false;
	}

	static quint16 usedServerPort = 0;
	if (listen(QHostAddress::Any, usedServerPort))
	{
		if (usedServerPort == 0)
		{
			usedServerPort = serverPort();
		}
		return true;
	}

	qCCritical(ifd) << "Listen failed:" << serverError() << ", " << errorString();

	return false;
}


void RemoteTlsServer::onSslErrors(const QList<QSslError>& pErrors)
{
	const auto& socket = getSslSocket();
	if (pErrors.size() == 1 &&
			(pErrors.first().error() == QSslError::SelfSignedCertificate || pErrors.first().error() == QSslError::SelfSignedCertificateInChain))
	{
		const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getCiphers();
		if (pairingCiphers.contains(socket->sessionCipher()))
		{
			qCDebug(ifd) << "Client requests pairing | cipher:" << socket->sessionCipher() << "| certificate:" << socket->peerCertificate();
			socket->ignoreSslErrors(pErrors);
			return;
		}
	}

	qCDebug(ifd) << "Client is not allowed | cipher:" << socket->sessionCipher() << "| certificate:" << socket->peerCertificate() << "| error:" << pErrors;
}


void RemoteTlsServer::onEncrypted()
{
	const auto& socket = getSslSocket();
	const auto& cfg = socket->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(ifd));

	QLatin1String error;
	const auto& minimalKeySizes = [](QSsl::KeyAlgorithm pKeyAlgorithm){
				return Env::getSingleton<SecureStorage>()->getMinimumIfdKeySize(pKeyAlgorithm);
			};
	if (!TlsChecker::hasValidCertificateKeyLength(cfg.peerCertificate(), minimalKeySizes))
	{
		error = QLatin1String("Client denied... abort connection!");
	}

	const auto rootCert = TlsChecker::getRootCertificate(cfg.peerCertificateChain());
	if (rootCert.isNull())
	{
		error = QLatin1String("Client denied... no root certificate found!");
	}

	if (!error.isEmpty())
	{
		qCCritical(ifd) << error;
		socket->abort();
		socket->deleteLater();
		return;
	}

	qCDebug(ifd) << "Client connected";
	auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getCiphers();
	if (pairingCiphers.contains(cfg.sessionCipher()))
	{
		qCDebug(ifd) << "Pairing completed | Add certificate:" << rootCert;
		settings.addTrustedCertificate(rootCert);
		setPairing(false);
		Q_EMIT firePairingCompleted(rootCert);
	}
	else
	{
		auto info = settings.getRemoteInfo(rootCert);
		info.setLastConnected(QDateTime::currentDateTime());
		settings.updateRemoteInfo(info);
	}

	socket->disconnect(this);
	Q_EMIT fireNewConnection(socket.data());
}


void RemoteTlsServer::setPairing(bool pEnable)
{
	if (pEnable)
	{
		std::uniform_int_distribution<int> uni(0, 9999);
		QByteArray pin = QByteArray::number(uni(Randomizer::getInstance().getGenerator()));
		pin.prepend(4 - pin.size(), '0');
		setPsk(pin);
	}
	else
	{
		setPsk(QByteArray());
	}
}


QSslCertificate RemoteTlsServer::getCurrentCertificate() const
{
	const auto& socket = getSslSocket();
	return socket ? socket->sslConfiguration().peerCertificate() : QSslCertificate();
}
