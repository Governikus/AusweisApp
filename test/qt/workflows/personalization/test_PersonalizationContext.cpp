/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "context/PersonalizationContext.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_PersonalizationContext
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void SessionIdentifier()
		{
			PersonalizationContext context((QString()));

			QVERIFY(context.getSessionIdentifier().isNull());

			context.setSessionIdentifier(QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"));
			QCOMPARE(context.getSessionIdentifier(), QUuid("135a32d8-ccfa-11eb-b8bc-0242ac130003"));
		}


		void Challenge()
		{
			PersonalizationContext context((QString()));

			QCOMPARE(context.getChallenge(), QString());

			context.setChallenge("Q2hhbGxlbmdl"_L1);
			QCOMPARE(context.getChallenge(), "Q2hhbGxlbmdl"_L1);
		}


		void PreparePersonalizationData()
		{
			PersonalizationContext context((QString()));

			QCOMPARE(context.getPreparePersonalizationData(), QString());

			context.setPreparePersonalizationData("UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="_L1);
			QCOMPARE(context.getPreparePersonalizationData(), "UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="_L1);
		}


		void progress_data()
		{
			QTest::addColumn<int>("initial");
			QTest::addColumn<int>("max");
			QTest::addColumn<int>("progress");
			QTest::addColumn<int>("expected");

			QTest::newRow("full_10") << 0 << 100 << 10 << 10;
			QTest::newRow("full_50") << 0 << 100 << 50 << 50;
			QTest::newRow("full_99") << 0 << 100 << 99 << 99;
			QTest::newRow("full_100") << 0 << 100 << 100 << 100;

			QTest::newRow("first_half_10") << 0 << 50 << 10 << 5;
			QTest::newRow("first_half_50") << 0 << 50 << 50 << 25;
			QTest::newRow("first_half_90") << 0 << 50 << 90 << 45;
			QTest::newRow("first_half_100") << 0 << 50 << 100 << 50;

			QTest::newRow("second_half_10") << 50 << 100 << 10 << 55;
			QTest::newRow("second_half_50") << 50 << 100 << 50 << 75;
			QTest::newRow("second_half_90") << 50 << 100 << 90 << 95;
			QTest::newRow("second_half_100") << 50 << 100 << 100 << 100;
		}


		void progress()
		{
			QFETCH(int, initial);
			QFETCH(int, max);
			QFETCH(int, progress);
			QFETCH(int, expected);

			auto context = QSharedPointer<PersonalizationContext>::create(QString());
			QSignalSpy spy(context.get(), &WorkflowContext::fireProgressChanged);

			context->setProgress(progress, QStringLiteral("dummy"), initial, max);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(context->getProgressValue(), expected);
		}


};

QTEST_GUILESS_MAIN(test_PersonalizationContext)
#include "test_PersonalizationContext.moc"
