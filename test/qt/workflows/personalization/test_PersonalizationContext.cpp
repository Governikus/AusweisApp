/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "context/PersonalizationContext.h"

#include <QtTest>


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

			context.setChallenge(QString("Q2hhbGxlbmdl"));
			QCOMPARE(context.getChallenge(), QString("Q2hhbGxlbmdl"));
		}


		void PreparePersonalizationData()
		{
			PersonalizationContext context((QString()));

			QCOMPARE(context.getPreparePersonalizationData(), QString());

			context.setPreparePersonalizationData(QString("UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="));
			QCOMPARE(context.getPreparePersonalizationData(), QString("UHJlcGFyZVBlcnNvbmFsaXphdGlvbkRhdGE="));
		}


};

QTEST_GUILESS_MAIN(test_PersonalizationContext)
#include "test_PersonalizationContext.moc"
