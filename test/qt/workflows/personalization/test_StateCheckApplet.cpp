/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateCheckApplet.h"

#include "ReaderManager.h"
#include "SmartManager.h"
#include "context/PersonalizationContext.h"
#include "paos/retrieve/TransmitParser.h"

#include "TestFileHelper.h"
#include "mock/eid_applet_interface_mock.h"

#include <QSharedPointer>

#include <QtTest>


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


class test_StateCheckApplet
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void run_data()
		{
			QTest::addColumn<EidStatus>("status");
			QTest::addColumn<GlobalStatus::Code>("code");

			QTest::newRow("internal_error") << EidStatus::INTERNAL_ERROR << GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed;
			QTest::newRow("unavailable") << EidStatus::UNAVAILABLE << GlobalStatus::Code::Workflow_Smart_eID_Unavailable;
			QTest::newRow("no_provisioning") << EidStatus::NO_PROVISIONING << GlobalStatus::Code::No_Error;
			QTest::newRow("no_personalization") << EidStatus::NO_PERSONALIZATION << GlobalStatus::Code::No_Error;
			QTest::newRow("applet_unusable") << EidStatus::APPLET_UNUSABLE << GlobalStatus::Code::No_Error;
			QTest::newRow("personalized") << EidStatus::PERSONALIZED << GlobalStatus::Code::No_Error;
		}


		void run()
		{
			QFETCH(EidStatus, status);
			QFETCH(GlobalStatus::Code, code);

			setSmartEidStatus(status);
			auto context = QSharedPointer<PersonalizationContext>::create(QString());

			StateCheckApplet state(context);
			QSignalSpy spyAbort(&state, &StateCheckApplet::fireAbort);
			QSignalSpy spyContinue(&state, &StateCheckApplet::fireContinue);
			QSignalSpy spyFurtherStepRequired(&state, &StateCheckApplet::fireFurtherStepRequired);

			state.run();

			switch (code)
			{
				case GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed:
				case GlobalStatus::Code::Workflow_Smart_eID_Unavailable:
					QTRY_COMPARE(spyAbort.size(), 1); // clazy:exclude=qstring-allocations
					QCOMPARE(spyContinue.size(), 0);
					QCOMPARE(spyFurtherStepRequired.size(), 0);
					break;

				default:
					if (status == EidStatus::NO_PERSONALIZATION)
					{
						QCOMPARE(spyAbort.size(), 0);
						QTRY_COMPARE(spyContinue.size(), 1); // clazy:exclude=qstring-allocations
						QCOMPARE(spyFurtherStepRequired.size(), 0);
					}
					else
					{
						QCOMPARE(spyAbort.size(), 0);
						QCOMPARE(spyContinue.size(), 0);
						QTRY_COMPARE(spyFurtherStepRequired.size(), 1); // clazy:exclude=qstring-allocations
					}
			}
			QCOMPARE(context->getStatus(), code);
		}


};

QTEST_GUILESS_MAIN(test_StateCheckApplet)
#include "test_StateCheckApplet.moc"
