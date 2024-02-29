/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisConnectionTest.h"

#include "NetworkManager.h"
#include "SecureStorage.h"
#include "TlsChecker.h"

using namespace governikus;

DiagnosisConnectionTest::DiagnosisConnectionTest()
	: QObject()
	, mIsProxySet(false)
	, mProxyHostName()
	, mProxyPort()
	, mProxyType()
	, mProxyCapabilities()
	, mErrorOfConnectionTestWithProxy()
	, mConnectionTestWithProxySuccessful(false)
	, mConnectionTestWithoutProxySuccessful(false)
	, mPingTestOnProxySuccessful(false)
	, mProxyPingDone(false)
	, mConnectionTestWithProxyDone(false)
	, mConnectionTestWithoutProxyDone(false)
	, mPingSocketToProxy()
	, mTcpSocketWithoutProxy()
	, mReplyWithProxy()
{
}


void DiagnosisConnectionTest::onProxyPingTestDone()
{
	mPingTestOnProxySuccessful = true;
	mPingSocketToProxy.disconnectFromHost();
	mProxyPingDone = true;
	checkIfAllProcessesDone();
}


void DiagnosisConnectionTest::onProxyPingTestError(QAbstractSocket::SocketError pSocketError)
{
	qDebug() << "Error occurred while trying to ping proxy:" << pSocketError;
	mPingTestOnProxySuccessful = false;
	mProxyPingDone = true;
	checkIfAllProcessesDone();
}


void DiagnosisConnectionTest::onSocketConnectionTestWithProxyDone()
{
	mConnectionTestWithProxySuccessful = false;
	if (mReplyWithProxy)
	{
		mConnectionTestWithProxySuccessful = (mReplyWithProxy->error() == QNetworkReply::NetworkError::NoError);
		mErrorOfConnectionTestWithProxy = mReplyWithProxy->errorString();
		mReplyWithProxy->close();
		mReplyWithProxy.reset();
	}

	mConnectionTestWithProxyDone = true;
	checkIfAllProcessesDone();
}


void DiagnosisConnectionTest::onSocketConnectionTestWithoutProxyDone()
{
	mConnectionTestWithoutProxySuccessful = true;
	mTcpSocketWithoutProxy.disconnectFromHost();
	mConnectionTestWithoutProxyDone = true;
	checkIfAllProcessesDone();
}


void DiagnosisConnectionTest::onSocketConnectionTestWithoutProxyError(QAbstractSocket::SocketError pSocketError)
{
	qDebug() << "Could not connect to test server without proxy:" << pSocketError;
	mConnectionTestWithoutProxySuccessful = false;
	mConnectionTestWithoutProxyDone = true;
	checkIfAllProcessesDone();
}


void DiagnosisConnectionTest::checkIfAllProcessesDone()
{
	if (mProxyPingDone && mConnectionTestWithProxyDone && mConnectionTestWithoutProxyDone)
	{
		Q_EMIT fireConnectionTestDone();
	}
}


QString DiagnosisConnectionTest::getProxyTypeAsQString(QNetworkProxy::ProxyType pType)
{
	switch (pType)
	{
		case QNetworkProxy::NoProxy:
			return QStringLiteral("NoProxy");

		case QNetworkProxy::DefaultProxy:
			return QStringLiteral("DefaultProxy");

		case QNetworkProxy::Socks5Proxy:
			return QStringLiteral("Socks5Proxy");

		case QNetworkProxy::HttpProxy:
			return QStringLiteral("HttpProxy");

		case QNetworkProxy::HttpCachingProxy:
			return QStringLiteral("HttpCachingProxy");

		case QNetworkProxy::FtpCachingProxy:
			return QStringLiteral("FtpCachingProxy");
	}

	Q_UNREACHABLE();
}


QString DiagnosisConnectionTest::getProxyCapabilitiesAsQString(QNetworkProxy::Capabilities pCaps)
{
	QStringList scaps;
	if (pCaps & QNetworkProxy::TunnelingCapability)
	{
		scaps << QStringLiteral("Tunnel");
	}
	if (pCaps & QNetworkProxy::ListeningCapability)
	{
		scaps << QStringLiteral("Listen");
	}
	if (pCaps & QNetworkProxy::UdpTunnelingCapability)
	{
		scaps << QStringLiteral("UDP");
	}
	if (pCaps & QNetworkProxy::CachingCapability)
	{
		scaps << QStringLiteral("Caching");
	}
	if (pCaps & QNetworkProxy::HostNameLookupCapability)
	{
		scaps << QStringLiteral("NameLookup");
	}
	if (pCaps & QNetworkProxy::SctpTunnelingCapability)
	{
		scaps << QStringLiteral("SctpTunnel");
	}
	if (pCaps & QNetworkProxy::SctpListeningCapability)
	{
		scaps << QStringLiteral("SctpListen");
	}
	return scaps.join(QLatin1String(", "));
}


void DiagnosisConnectionTest::startConnectionTest()
{
	mProxyPingDone = false;
	mConnectionTestWithProxyDone = false;
	mConnectionTestWithoutProxyDone = false;

	const QUrl& testUrl = QUrl(Env::getSingleton<SecureStorage>()->getAppcastUpdateUrl());
	const auto& proxies = QNetworkProxyFactory::proxyForQuery(QNetworkProxyQuery(testUrl));
	const auto& proxy = proxies.constFirst();
	if (proxies.size() > 1)
	{
		qDebug() << "More than one proxy found, using first proxy:" << proxy;
	}

	if (proxy.type() == QNetworkProxy::ProxyType::NoProxy)
	{
		mIsProxySet = false;
		mProxyPingDone = true;
		mConnectionTestWithProxyDone = true;
	}
	else
	{
		mIsProxySet = true;
		mProxyHostName = proxy.hostName();
		mProxyPort = QString::number(proxy.port());
		mProxyType = getProxyTypeAsQString(proxy.type());
		mProxyCapabilities = getProxyCapabilitiesAsQString(proxy.capabilities());
	}

	if (mIsProxySet)
	{
		mPingSocketToProxy.reset();
		mPingSocketToProxy.setProxy(QNetworkProxy::NoProxy);
		connect(&mPingSocketToProxy, &QAbstractSocket::connected, this, &DiagnosisConnectionTest::onProxyPingTestDone);
		connect(&mPingSocketToProxy, &QAbstractSocket::errorOccurred, this, &DiagnosisConnectionTest::onProxyPingTestError);
		mPingSocketToProxy.connectToHost(proxy.hostName(), proxy.port());

		QNetworkRequest request(testUrl);
		mReplyWithProxy = Env::getSingleton<NetworkManager>()->get(request);
		connect(mReplyWithProxy.data(), &QNetworkReply::finished, this, &DiagnosisConnectionTest::onSocketConnectionTestWithProxyDone);
		connect(mReplyWithProxy.data(), &QNetworkReply::sslErrors, this, [this](const QList<QSslError>& pErrors) {
				if (mReplyWithProxy && TlsChecker::containsFatalError(mReplyWithProxy, pErrors))
				{
					mReplyWithProxy->abort();
				}
			});
	}

	mTcpSocketWithoutProxy.reset();
	mTcpSocketWithoutProxy.setProxy(QNetworkProxy::NoProxy);
	connect(&mTcpSocketWithoutProxy, &QAbstractSocket::connected, this, &DiagnosisConnectionTest::onSocketConnectionTestWithoutProxyDone);
	connect(&mTcpSocketWithoutProxy, &QAbstractSocket::errorOccurred, this, &DiagnosisConnectionTest::onSocketConnectionTestWithoutProxyError);
	mTcpSocketWithoutProxy.connectToHost(testUrl.host(), 443);
}
