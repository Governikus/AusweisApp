/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */

#include "RedirectBroadcast.h"

#include "PortFile.h"

#include <QTest>
#include <QUdpSocket>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_RedirectBroadcast
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void broadcast()
		{
#ifdef Q_OS_WIN
			QSKIP("Windows does not use portfiles");
#endif

			QUdpSocket socket;
			QVERIFY(socket.bind(QHostAddress::LocalHost));

			QList<QNetworkDatagram> messages;
			connect(&socket, &QUdpSocket::readyRead, this, [&socket, &messages]{
						while (socket.hasPendingDatagrams())
						{
							messages << socket.receiveDatagram();
						}
					});

			QTest::ignoreMessage(QtMsgType::QtDebugMsg, "Found instances on Ports: QList()");
			Q_UNUSED(new RedirectBroadcast(QNetworkDatagram("test"_ba), 1337));

			PortFile file;
			file.handlePort(socket.localPort());
			QTest::ignoreMessage(QtMsgType::QtDebugMsg, QRegularExpression(R"(Found instances on Ports: QList(\d*))"_L1));
			Q_UNUSED(new RedirectBroadcast(QNetworkDatagram("test"_ba), 1337));
			QTRY_COMPARE(messages.size(), 1);
			QCOMPARE(messages.at(0).data(), "test"_ba);
		}


};

QTEST_GUILESS_MAIN(test_RedirectBroadcast)
#include "test_RedirectBroadcast.moc"
