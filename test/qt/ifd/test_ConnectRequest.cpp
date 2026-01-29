/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ConnectRequest.h"

#include <QSignalSpy>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class MockConnectRequest
	: public ConnectRequest
{
	Q_OBJECT

	private:
		QAbstractSocket::SocketState mState = QAbstractSocket::ConnectedState;

		QAbstractSocket::SocketState getState(const QSharedPointer<QWebSocket>& pSocket) const override;

	public:
		using ConnectRequest::ConnectRequest;

		void setState(QAbstractSocket::SocketState pState)
		{
			mState = pState;
		}


};


QAbstractSocket::SocketState MockConnectRequest::getState(const QSharedPointer<QWebSocket>& pSocket) const
{
	Q_UNUSED(pSocket)
	return mState;
}


class test_ConnectRequest
	: public QObject
{
	Q_OBJECT

	private:
		Discovery mDiscovery = Discovery("Dev1"_L1, QByteArrayLiteral("0123456789ABCDEF"), 1234, {IfdVersion::Version::latest});

	private Q_SLOTS:
		void processResult_success()
		{
			MockConnectRequest connectRequest(mDiscovery, "123456"_ba, 5000);
			QSignalSpy spySucceed(&connectRequest, &MockConnectRequest::fireConnectionCreated);
			QSignalSpy spyError(&connectRequest, &MockConnectRequest::fireConnectionError);

			const auto& socket = QSharedPointer<QWebSocket>::create();
			connectRequest.mSockets << socket;

			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::ConnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection succeeded");
			connectRequest.processResult(socket);
			QCOMPARE(spySucceed.count(), 1);
			QCOMPARE(spyError.count(), 0);
			QVERIFY(connectRequest.mSockets.isEmpty());

			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::ConnectedState");
			QTest::ignoreMessage(QtWarningMsg, "Ignoring result from unexpected socket. ConnectRequest has probably already been completed");
			connectRequest.processResult(socket);
			QCOMPARE(spySucceed.count(), 1);
			QCOMPARE(spyError.count(), 0);
			QVERIFY(connectRequest.mSockets.isEmpty());
		}


		void processResult_fail()
		{
			MockConnectRequest connectRequest(mDiscovery, "123456"_ba, 5000);
			connectRequest.setState(QAbstractSocket::UnconnectedState);
			QSignalSpy spySucceed(&connectRequest, &MockConnectRequest::fireConnectionCreated);
			QSignalSpy spyError(&connectRequest, &MockConnectRequest::fireConnectionError);

			const auto& socket = QSharedPointer<QWebSocket>::create();
			connectRequest.mSockets << socket;

			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection failed. No more pending connections left");
			connectRequest.processResult(socket);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 1);
			QVERIFY(connectRequest.mSockets.isEmpty());

			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtWarningMsg, "Ignoring result from unexpected socket. ConnectRequest has probably already been completed");
			connectRequest.processResult(socket);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 1);
			QVERIFY(connectRequest.mSockets.isEmpty());
		}


		void processResult_timeout()
		{
			MockConnectRequest connectRequest(mDiscovery, "123456"_ba, 5000);
			QSignalSpy spySucceed(&connectRequest, &MockConnectRequest::fireConnectionCreated);
			QSignalSpy spyError(&connectRequest, &MockConnectRequest::fireConnectionError);

			const auto& socket = QSharedPointer<QWebSocket>::create();
			connectRequest.mSockets << socket;

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Connection could not be established after \\d*0 ms"_L1));
			connectRequest.onTimeout();
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 1);
			QVERIFY(connectRequest.mSockets.isEmpty());
		}


		void processResult_successAfterFail()
		{
			MockConnectRequest connectRequest(mDiscovery, "123456"_ba, 5000);
			connectRequest.setState(QAbstractSocket::UnconnectedState);
			QSignalSpy spySucceed(&connectRequest, &MockConnectRequest::fireConnectionCreated);
			QSignalSpy spyError(&connectRequest, &MockConnectRequest::fireConnectionError);

			const auto& socket = QSharedPointer<QWebSocket>::create();
			connectRequest.mSockets << socket;
			connectRequest.mSockets << socket;

			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection failed. Waiting for pending connections");
			connectRequest.processResult(socket);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 0);
			QCOMPARE(connectRequest.mSockets.size(), 1);

			connectRequest.setState(QAbstractSocket::ConnectedState);
			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::ConnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection succeeded");
			connectRequest.processResult(socket);
			QCOMPARE(spySucceed.count(), 1);
			QCOMPARE(spyError.count(), 0);
			QVERIFY(connectRequest.mSockets.isEmpty());
		}


		void onError0()
		{
			MockConnectRequest connectRequest(mDiscovery, "123456"_ba, 5000);
			connectRequest.setState(QAbstractSocket::UnconnectedState);
			QSignalSpy spySucceed(&connectRequest, &MockConnectRequest::fireConnectionCreated);
			QSignalSpy spyError(&connectRequest, &MockConnectRequest::fireConnectionError);

			const auto& socket = QSharedPointer<QWebSocket>::create();
			connectRequest.mSockets << socket;

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Connection error: QAbstractSocket::UnknownSocketError .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection failed. No more pending connections left");
			connectRequest.onError(socket, QAbstractSocket::UnknownSocketError);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 1);
			QCOMPARE(spyError.at(0).at(1).value<IfdErrorCode>(), IfdErrorCode::CONNECTION_ERROR);
			QVERIFY(connectRequest.mSockets.isEmpty());
		}


		void onError1()
		{
			MockConnectRequest connectRequest(mDiscovery, "123456"_ba, 5000);
			connectRequest.setState(QAbstractSocket::UnconnectedState);
			QSignalSpy spySucceed(&connectRequest, &MockConnectRequest::fireConnectionCreated);
			QSignalSpy spyError(&connectRequest, &MockConnectRequest::fireConnectionError);

			const auto& socket = QSharedPointer<QWebSocket>::create();
			connectRequest.mSockets << socket;
			connectRequest.mSockets << socket;

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Connection error: QAbstractSocket::UnknownSocketError .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection failed. Waiting for pending connections");
			connectRequest.onError(socket, QAbstractSocket::UnknownSocketError);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 0);
			QCOMPARE(connectRequest.mSockets.size(), 1);

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Connection error: QAbstractSocket::RemoteHostClosedError .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection failed. No more pending connections left");
			connectRequest.onError(socket, QAbstractSocket::RemoteHostClosedError);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 1);
			QCOMPARE(spyError.at(0).at(1).value<IfdErrorCode>(), IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
			QVERIFY(connectRequest.mSockets.isEmpty());
		}


		void onError2()
		{
			MockConnectRequest connectRequest(mDiscovery, "123456"_ba, 5000);
			connectRequest.setState(QAbstractSocket::UnconnectedState);
			QSignalSpy spySucceed(&connectRequest, &MockConnectRequest::fireConnectionCreated);
			QSignalSpy spyError(&connectRequest, &MockConnectRequest::fireConnectionError);

			const auto& socket = QSharedPointer<QWebSocket>::create();
			connectRequest.mSockets << socket;
			connectRequest.mSockets << socket;

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Connection error: QAbstractSocket::RemoteHostClosedError .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection failed. Waiting for pending connections");
			connectRequest.onError(socket, QAbstractSocket::RemoteHostClosedError);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 0);
			QCOMPARE(connectRequest.mSockets.size(), 1);

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Connection error: QAbstractSocket::UnknownSocketError .*"_L1));
			QTest::ignoreMessage(QtDebugMsg, "    Connection to QUrl(\"\") finished with QAbstractSocket::UnconnectedState");
			QTest::ignoreMessage(QtDebugMsg, "    Connection failed. No more pending connections left");
			connectRequest.onError(socket, QAbstractSocket::UnknownSocketError);
			QCOMPARE(spySucceed.count(), 0);
			QCOMPARE(spyError.count(), 1);
			QCOMPARE(spyError.at(0).at(1).value<IfdErrorCode>(), IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION);
			QVERIFY(connectRequest.mSockets.isEmpty());
		}


};

QTEST_GUILESS_MAIN(test_ConnectRequest)
#include "test_ConnectRequest.moc"
