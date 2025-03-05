/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateCheckApplet.h"

#include "ReaderManager.h"
#include "context/PersonalizationContext.h"

#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>
#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


Q_DECLARE_METATYPE(EidStatus)


class test_StateCheckApplet
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void run_data()
		{
			QTest::addColumn<EidStatus>("status");
			QTest::addColumn<QString>("signal");

			QTest::newRow("internal_error") << EidStatus::INTERNAL_ERROR << SIGNAL(fireAbort(const FailureCode&));
			QTest::newRow("cert_expired") << EidStatus::CERT_EXPIRED << SIGNAL(fireDeletePersonalization());
			QTest::newRow("no_provisioning") << EidStatus::NO_PROVISIONING << SIGNAL(fireInstallApplet());
			QTest::newRow("no_personalization") << EidStatus::NO_PERSONALIZATION << SIGNAL(fireContinue());
			QTest::newRow("unusable") << EidStatus::UNUSABLE << SIGNAL(fireDeleteApplet());
			QTest::newRow("personalized") << EidStatus::PERSONALIZED << SIGNAL(fireDeletePersonalization());
		}


		void run()
		{
			QFETCH(EidStatus, status);
			QFETCH(QString, signal);

			setSmartEidStatus(status);
			auto context = QSharedPointer<PersonalizationContext>::create(QString());

			StateCheckApplet state(context);
			QSignalSpy spy(&state, qPrintable(signal));
			QVERIFY(spy.isValid());

			state.run();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_StateCheckApplet)
#include "test_StateCheckApplet.moc"
