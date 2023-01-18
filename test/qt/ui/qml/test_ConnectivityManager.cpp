/*!
 * \brief Unit tests for \ref ConnectivityManager
 *
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ConnectivityManager.h"

#include "LogHandler.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;


class test_ConnectivityManager
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void test_Active()
		{
			ConnectivityManager manager;
			const QString name = QStringLiteral("name");
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSignalSpy signalSpy(&manager, &ConnectivityManager::fireNetworkInterfaceActiveChanged);

			QVERIFY(!manager.isNetworkInterfaceActive());

			manager.setActive(true, name);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Found active network interface"));
			QCOMPARE(signalSpy.count(), 1);
			QVERIFY(manager.isNetworkInterfaceActive());

			manager.setActive(false, name);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Found no active network interface"));
			QCOMPARE(signalSpy.count(), 2);
			QVERIFY(!manager.isNetworkInterfaceActive());
		}


		void test_Watching()
		{
			ConnectivityManager manager;
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			manager.startWatching();
			QVERIFY(manager.mTimerId != 0);
			QCOMPARE(logSpy.count(), 1);

			manager.startWatching();
			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(1).at(0).toString().contains("Already started, skip"));

			manager.stopWatching();
			QCOMPARE(manager.mTimerId, 0);

			manager.stopWatching();
			QCOMPARE(logSpy.count(), 3);
			QVERIFY(logSpy.at(2).at(0).toString().contains("Already stopped, skip"));
		}


};

QTEST_GUILESS_MAIN(test_ConnectivityManager)
#include "test_ConnectivityManager.moc"
