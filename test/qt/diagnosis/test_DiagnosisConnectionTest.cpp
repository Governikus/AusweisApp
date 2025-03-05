/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "DiagnosisConnectionTest.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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
			QVERIFY(!mTest->mConnectionTestWithProxySuccessful);
			QVERIFY(mTest->mConnectionTestWithProxyDone);
			QVERIFY(mTest->mReplyWithProxy.isNull());
		}


		void test_OnSocketConnectionTestWithoutProxyDone()
		{
			mTest->onSocketConnectionTestWithoutProxyDone();
			QVERIFY(mTest->mConnectionTestWithoutProxySuccessful);
			QVERIFY(mTest->mConnectionTestWithoutProxyDone);
			QVERIFY(mTest->mReplyWithProxy.isNull());
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
			QCOMPARE(mTest->getProxyCapabilitiesAsQString(caps), "Tunnel, Listen, UDP, Caching, NameLookup, SctpTunnel, SctpListen"_L1);
		}


		void test_StartConnectionTest_NoProxy()
		{
			QSignalSpy spy(mTest.data(), &DiagnosisConnectionTest::fireConnectionTestDone);

			QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
			mTest->startConnectionTest();
			QVERIFY(mTest->mProxyPingDone);
			QVERIFY(!mTest->mConnectionTestWithoutProxyDone);
			QVERIFY(mTest->mConnectionTestWithProxyDone);
			QVERIFY(!mTest->mIsProxySet);
			QCOMPARE(mTest->mTcpSocketWithoutProxy.proxy(), QNetworkProxy::NoProxy);
			QCOMPARE(mTest->mTcpSocketWithoutProxy.state(), QAbstractSocket::SocketState::HostLookupState);

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(mTest->mConnectionTestWithoutProxyDone);
			QCOMPARE(mTest->mTcpSocketWithoutProxy.state(), QAbstractSocket::SocketState::UnconnectedState);
		}


		void test_StartConnectionTest()
		{
			QSignalSpy spy(mTest.data(), &DiagnosisConnectionTest::fireConnectionTestDone);

			QNetworkProxy testProxy(QNetworkProxy::ProxyType::HttpProxy);
			QNetworkProxy::setApplicationProxy(testProxy);

			mTest->startConnectionTest();
			QVERIFY(mTest->mIsProxySet);
			QCOMPARE(mTest->mProxyHostName, QString());
			QCOMPARE(mTest->mProxyPort, "0"_L1);
			QCOMPARE(mTest->mProxyType, "HttpProxy"_L1);
			QCOMPARE(mTest->mProxyType, "HttpProxy"_L1);
			QCOMPARE(mTest->mPingSocketToProxy.proxy(), QNetworkProxy::NoProxy);
			QCOMPARE(mTest->mPingSocketToProxy.state(), QAbstractSocket::SocketState::HostLookupState);
			QVERIFY(!mTest->mReplyWithProxy.isNull());
			QVERIFY(mTest->mReplyWithProxy->isRunning());

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(mTest->mProxyPingDone);
			QVERIFY(mTest->mConnectionTestWithProxyDone);
			QVERIFY(mTest->mReplyWithProxy.isNull());
		}


};

QTEST_GUILESS_MAIN(test_DiagnosisConnectionTest)
#include "test_DiagnosisConnectionTest.moc"
