/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "LocalTlsServer.h"

#include "LogHandler.h"
#include "SecureStorage.h"
#include "TlsChecker.h"

#include <QHostAddress>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ifd)

using namespace governikus;


bool LocalTlsServer::startListening(quint16 pPort)
{
	if (isListening())
	{
		return false;
	}

	const QList<QHostAddress> localHosts = {QHostAddress::LocalHostIPv6, QHostAddress::LocalHost};
	for (const auto& localHost : localHosts)
	{
		if (listen(localHost, pPort))
		{
			return true;
		}

		qCWarning(ifd) << "Listen to" << localHost << "failed:" << serverError() << ", " << errorString();
	}

	return false;
}


QSslConfiguration LocalTlsServer::sslConfiguration() const
{
	return Env::getSingleton<SecureStorage>()->getTlsConfigLocalIfd().getConfiguration();
}


void LocalTlsServer::onSslErrors(const QList<QSslError>& pErrors)
{
	const auto& socket = getSslSocket();
	qCDebug(ifd) << "Client is not allowed | cipher:" << socket->sessionCipher() << "| certificate:" << socket->peerCertificate() << "| error:" << pErrors;
}


void LocalTlsServer::onEncrypted()
{
	const auto& socket = getSslSocket();
	TlsChecker::logSslConfig(socket->sslConfiguration(), spawnMessageLogger(ifd));

	qCDebug(ifd) << "Client connected";

	socket->disconnect(this);
	Q_EMIT fireNewConnection(socket.data());
}
