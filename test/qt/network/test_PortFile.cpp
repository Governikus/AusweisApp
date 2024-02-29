/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref PortFile
 */

#include "PortFile.h"

#include <QFile>
#include <QTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_PortFile
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testHandlePort_data()
		{
			QTest::addColumn<QString>("usage");
			QTest::addColumn<quint16>("port");
			QTest::addColumn<bool>("fileCreated");

			QTest::newRow("default") << QString() << PortFile::cDefaultPort << false;
			QTest::newRow("default_udp") << QStringLiteral("udp") << PortFile::cDefaultPort << false;
			QTest::newRow("other") << QString() << quint16(1234) << true;
			QTest::newRow("other_udp") << QStringLiteral("udp") << quint16(1234) << true;
		}


		void testHandlePort()
		{
			QFETCH(QString, usage);
			QFETCH(quint16, port);
			QFETCH(bool, fileCreated);

			const auto& filename = PortFile::getPortFilename(usage);
			QVERIFY(filename.contains(QString::number(QCoreApplication::applicationPid())));
			QVERIFY(filename.contains(QCoreApplication::applicationName()));
			QVERIFY(filename.endsWith(QStringLiteral(".port")));

			{
				PortFile portFile(usage);
				QCOMPARE(PortFile::getAllPortFiles().size(), 0);
				QVERIFY(!QFile::exists(filename));
				portFile.handlePort(port);
				QCOMPARE(PortFile::getAllPortFiles().size(), fileCreated ? 1 : 0);
				QCOMPARE(QFile::exists(filename), fileCreated);

				if (!usage.isEmpty())
				{
					QVERIFY(filename.contains('.'_L1 + usage + '.'_L1));
				}
			}

			QVERIFY(!QFile::exists(filename));
		}


};

QTEST_GUILESS_MAIN(test_PortFile)
#include "test_PortFile.moc"
