/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "LocalTlsServer.h"

#include "SecureStorage.h"

#include <QHostAddress>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


QSslConfiguration LocalTlsServer::sslConfiguration() const
{
	return Env::getSingleton<SecureStorage>()->getTlsConfigLocalIfd().getConfiguration();
}


bool LocalTlsServer::acceptSslErrors(const QPointer<QSslSocket>&, const QList<QSslError>&) const
{
	return false;
}


bool LocalTlsServer::checkSslConfiguration(const QSslConfiguration&) const
{
	return true;
}


void LocalTlsServer::updateClientInfo(const QSslConfiguration&)
{
}


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
