/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ProxyCredentials.h"


using namespace governikus;


QString ProxyCredentials::createUrl(const QNetworkProxy& pProxy) const
{
	QUrl url;
	switch (pProxy.type())
	{
		case QNetworkProxy::Socks5Proxy:
			url.setScheme(QStringLiteral("socks5"));
			break;

		case QNetworkProxy::FtpCachingProxy:
			url.setScheme(QStringLiteral("ftp"));
			break;

		default:
			url.setScheme(QStringLiteral("http"));
	}
	url.setHost(pProxy.hostName());
	url.setPort(pProxy.port());

	return url.toString();
}


ProxyCredentials::ProxyCredentials(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
	: QObject()
	, mLoop()
	, mAuthenticator(pAuthenticator)
	, mProposedUser(pProxy.user())
	, mUrl(createUrl(pProxy))
{
}


QString ProxyCredentials::getProposedUser() const
{
	return mProposedUser;
}


QString ProxyCredentials::getUrl() const
{
	return mUrl;
}


QString ProxyCredentials::getUser() const
{
	if (mAuthenticator)
	{
		return mAuthenticator->user();
	}

	return QString();
}


void ProxyCredentials::setUser(const QString& pUser)
{
	if (mAuthenticator && mAuthenticator->user() != pUser)
	{
		mAuthenticator->setUser(pUser);
		Q_EMIT fireProxyCredentialsChanged();
	}
}


QString ProxyCredentials::getPassword() const
{
	if (mAuthenticator)
	{
		return mAuthenticator->password();
	}

	return QString();
}


void ProxyCredentials::setPassword(const QString& pPassword)
{
	if (mAuthenticator && mAuthenticator->password() != pPassword)
	{
		mAuthenticator->setPassword(pPassword);
		Q_EMIT fireProxyCredentialsChanged();
	}
}


void ProxyCredentials::confirmInput()
{
	mLoop.exit();
}


void ProxyCredentials::waitForConfirmation()
{
	mLoop.exec();
}
