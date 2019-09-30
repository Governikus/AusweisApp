/*!
 * \brief Unit tests for \ref DiagnosisConnectionTest
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisConnectionTest.h"

#include <QtTest>

using namespace governikus;

class test_DiagnosisConnectionTest
	: public QObject
{
	Q_OBJECT

	QSharedPointer<DiagnosisConnectionTest> mTest;

	private Q_SLOTS:
		void init()
		{
			mTest.reset(new DiagnosisConnectionTest());
		}


		void cleanup()
		{
			mTest.clear();
		}


		void test_OnProxyPingTestDone()
		{
			mTest->onProxyPingTestDone();
			QVERIFY(mTest->mPingTestOnProxySuccessful);
			QVERIFY(mTest->mProxyPingDone);
			QCOMPARE(mTest->mPingSocketToProxy.state(), QAbstractSocket::UnconnectedState);
		}


		void test_OnProxyPingTestError()
		{
			QTest::ignoreMessage(QtDebugMsg, "Error occurred while trying to ping proxy: QAbstractSocket::ConnectionRefusedError");
			mTest->onProxyPingTestError(QAbstractSocket::ConnectionRefusedError);
			QVERIFY(!mTest->mPingTestOnProxySuccessful);
			QVERIFY(mTest->mProxyPingDone);
		}


		void test_OnSocketConnectionTestWithProxyDone()
		{
			mTest->onSocketConnectionTestWithProxyDone();
			QVERIFY(mTest->mConnectionTestWithProxySuccessful);
			QVERIFY(mTest->mConnectionTestWithProxyDone);
			QCOMPARE(mTest->mTcpSocketWithProxy.state(), QAbstractSocket::UnconnectedState);
		}


		void test_OnSocketConnectionTestWithProxyError()
		{
			QTest::ignoreMessage(QtDebugMsg, "Could not connect to test server with proxy: QAbstractSocket::NetworkError");
			mTest->onSocketConnectionTestWithProxyError(QAbstractSocket::NetworkError);
			QVERIFY(!mTest->mConnectionTestWithProxySuccessful);
			QVERIFY(mTest->mConnectionTestWithProxyDone);
		}


		void test_OnSocketConnectionTestWithoutProxyDone()
		{
			mTest->onSocketConnectionTestWithoutProxyDone();
			QVERIFY(mTest->mConnectionTestWithoutProxySuccessful);
			QVERIFY(mTest->mConnectionTestWithoutProxyDone);
			QCOMPARE(mTest->mTcpSocketWithProxy.state(), QAbstractSocket::UnconnectedState);
		}


		void test_OnSocketConnectionTestWithoutProxyError()
		{
			QTest::ignoreMessage(QtDebugMsg, "Could not connect to test server without proxy: QAbstractSocket::ProxyProtocolError");
			mTest->onSocketConnectionTestWithoutProxyError(QAbstractSocket::ProxyProtocolError);
			QVERIFY(!mTest->mConnectionTestWithoutProxySuccessful);
			QVERIFY(mTest->mConnectionTestWithoutProxyDone);
		}


		void test_CheckIfAllProcessesDone()
		{
			QSignalSpy spy(mTest.data(), &DiagnosisConnectionTest::fireConnectionTestDone);

			mTest->checkIfAllProcessesDone();
			QCOMPARE(spy.count(), 0);

			mTest->onProxyPingTestDone();
			mTest->onSocketConnectionTestWithoutProxyDone();
			mTest->onSocketConnectionTestWithProxyDone();
			QCOMPARE(spy.count(), 1);
		}


		void test_GetProxyTypeAsString()
		{
			QCOMPARE(mTest->getProxyTypeAsQString(QNetworkProxy::NoProxy), QStringLiteral("NoProxy"));
			QCOMPARE(mTest->getProxyTypeAsQString(QNetworkProxy::DefaultProxy), QStringLiteral("DefaultProxy"));
			QCOMPARE(mTest->getProxyTypeAsQString(QNetworkProxy::Socks5Proxy), QStringLiteral("Socks5Proxy"));
			QCOMPARE(mTest->getProxyTypeAsQString(QNetworkProxy::HttpProxy), QStringLiteral("HttpProxy"));
			QCOMPARE(mTest->getProxyTypeAsQString(QNetworkProxy::HttpCachingProxy), QStringLiteral("HttpCachingProxy"));
			QCOMPARE(mTest->getProxyTypeAsQString(QNetworkProxy::FtpCachingProxy), QStringLiteral("FtpCachingProxy"));
		}


		void test_GetProxyCapabilitiesAsString()
		{
			QNetworkProxy::Capabilities caps = {QNetworkProxy::TunnelingCapability, QNetworkProxy::ListeningCapability, QNetworkProxy::UdpTunnelingCapability,
												QNetworkProxy::CachingCapability, QNetworkProxy::HostNameLookupCapability, QNetworkProxy::SctpTunnelingCapability,
												QNetworkProxy::SctpListeningCapability};
			QCOMPARE(mTest->getProxyCapabilitiesAsQString(caps), QString("Tunnel, Listen, UDP, Caching, NameLookup, SctpTunnel, SctpListen"));
		}


		void test_StartConnectionTest_NoProxy()
		{
			QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
			mTest->startConnectionTest();
			QVERIFY(mTest->mProxyPingDone);
			QVERIFY(!mTest->mConnectionTestWithoutProxyDone);
			QVERIFY(mTest->mConnectionTestWithProxyDone);
			QVERIFY(!mTest->mIsProxySet);
			QCOMPARE(mTest->mTcpSocketWithoutProxy.proxy(), QNetworkProxy::NoProxy);
			QCOMPARE(mTest->mTcpSocketWithoutProxy.state(), QAbstractSocket::SocketState::HostLookupState);
		}


		void test_StartConnectionTest()
		{
			QNetworkProxy testProxy(QNetworkProxy::ProxyType::HttpProxy, QString("localhost"), 25000);
			QNetworkProxy::setApplicationProxy(testProxy);

			mTest->startConnectionTest();
			QVERIFY(mTest->mIsProxySet);
			QCOMPARE(mTest->mProxyHostName, QString("localhost"));
			QCOMPARE(mTest->mProxyPort, QString("25000"));
			QCOMPARE(mTest->mProxyType, QString("HttpProxy"));
			QCOMPARE(mTest->mProxyType, QString("HttpProxy"));
			QCOMPARE(mTest->mPingSocketToProxy.proxy(), QNetworkProxy::NoProxy);
			QCOMPARE(mTest->mPingSocketToProxy.state(), QAbstractSocket::SocketState::HostLookupState);
			QCOMPARE(mTest->mTcpSocketWithProxy.proxy(), testProxy);
			QCOMPARE(mTest->mTcpSocketWithProxy.state(), QAbstractSocket::SocketState::ConnectingState);
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisConnectionTest)
#include "test_DiagnosisConnectionTest.moc"
