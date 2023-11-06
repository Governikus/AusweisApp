/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ConnectivityManager
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

			manager.setActive(true);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("A network interface is now available"));
			QCOMPARE(signalSpy.count(), 1);
			QVERIFY(manager.isNetworkInterfaceActive());

			manager.setActive(true);
			QCOMPARE(logSpy.count(), 0);
			QCOMPARE(signalSpy.count(), 1);
			QVERIFY(manager.isNetworkInterfaceActive());

			manager.setActive(false);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("An active network interface is no longer available"));
			QCOMPARE(signalSpy.count(), 2);
			QVERIFY(!manager.isNetworkInterfaceActive());

			manager.setActive(false);
			QCOMPARE(logSpy.count(), 0);
			QCOMPARE(signalSpy.count(), 2);
			QVERIFY(!manager.isNetworkInterfaceActive());
		}


		void test_Watching()
		{
			ConnectivityManager manager;
			QSignalSpy signalSpy(&manager, &ConnectivityManager::fireWatchingChanged);

			manager.setWatching(true);
			QVERIFY(manager.mTimerId != 0);
			QCOMPARE(signalSpy.count(), 1);

			manager.setWatching(true);
			QVERIFY(manager.mTimerId != 0);
			QCOMPARE(signalSpy.count(), 1);

			manager.setWatching(false);
			QCOMPARE(manager.mTimerId, 0);
			QCOMPARE(signalSpy.count(), 2);

			manager.setWatching(false);
			QCOMPARE(manager.mTimerId, 0);
			QCOMPARE(signalSpy.count(), 2);
		}


};

QTEST_GUILESS_MAIN(test_ConnectivityManager)
#include "test_ConnectivityManager.moc"
