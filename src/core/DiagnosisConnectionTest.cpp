/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisConnectionTest.h"

#include "SecureStorage.h"

using namespace governikus;

DiagnosisConnectionTest::DiagnosisConnectionTest()
	: QObject()
	, mIsProxySet(false)
	, mProxyHostName()
	, mProxyPort()
	, mProxyType()
	, mProxyCapabilities()
	, mConnectionTestWithProxySuccessful(false)
	, mConnectionTestWithoutProxySuccessful(false)
	, mPingTestOnProxySuccessful(false)
	, mProxyPingDone(false)
	, mConnectionTestWithProxyDone(false)
	, mConnectionTestWithoutProxyDone(false)
	, mPingSocketToProxy()
	, mTcpSocketWithProxy()
	, mTcpSocketWithoutProxy()
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
	mConnectionTestWithProxySuccessful = true;
	mTcpSocketWithProxy.disconnectFromHost();
	mConnectionTestWithProxyDone = true;
	checkIfAllProcessesDone();
}


void DiagnosisConnectionTest::onSocketConnectionTestWithProxyError(QAbstractSocket::SocketError pSocketError)
{
	qDebug() << "Could not connect to test server with proxy:" << pSocketError;
	mConnectionTestWithProxySuccessful = false;
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
	return QString();
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

	const auto& proxy = QNetworkProxy::applicationProxy();
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

	const QUrl& testUrl = QUrl(Env::getSingleton<SecureStorage>()->getUpdateServerBaseUrl());
	if (mIsProxySet)
	{
		mPingSocketToProxy.reset();
		mPingSocketToProxy.setProxy(QNetworkProxy::NoProxy);
		connect(&mPingSocketToProxy, &QTcpSocket::connected, this, &DiagnosisConnectionTest::onProxyPingTestDone);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
		connect(&mPingSocketToProxy, &QTcpSocket::errorOccurred, this, &DiagnosisConnectionTest::onProxyPingTestError);
#else
		connect(&mPingSocketToProxy, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &DiagnosisConnectionTest::onProxyPingTestError);
#endif
		mPingSocketToProxy.connectToHost(proxy.hostName(), proxy.port());

		mTcpSocketWithProxy.reset();
		mTcpSocketWithProxy.setProxy(proxy);
		connect(&mTcpSocketWithProxy, &QTcpSocket::connected, this, &DiagnosisConnectionTest::onSocketConnectionTestWithProxyDone);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
		connect(&mTcpSocketWithProxy, &QTcpSocket::errorOccurred, this, &DiagnosisConnectionTest::onSocketConnectionTestWithProxyError);
#else
		connect(&mTcpSocketWithProxy, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &DiagnosisConnectionTest::onSocketConnectionTestWithProxyError);
#endif
		mTcpSocketWithProxy.connectToHost(testUrl.host(), 443);
	}

	mTcpSocketWithoutProxy.reset();
	mTcpSocketWithoutProxy.setProxy(QNetworkProxy::NoProxy);
	connect(&mTcpSocketWithoutProxy, &QTcpSocket::connected, this, &DiagnosisConnectionTest::onSocketConnectionTestWithoutProxyDone);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
	connect(&mTcpSocketWithoutProxy, &QTcpSocket::errorOccurred, this, &DiagnosisConnectionTest::onSocketConnectionTestWithoutProxyError);
#else
	connect(&mTcpSocketWithoutProxy, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &DiagnosisConnectionTest::onSocketConnectionTestWithoutProxyError);
#endif
	mTcpSocketWithoutProxy.connectToHost(testUrl.host(), 443);
}
