/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "pace/ec/EcdhGenericMapping.h"

#include "Randomizer.h"
#include "pace/ec/EcUtil.h"

#include <QtTest>


using namespace governikus;


class test_EcdhGenericMapping
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void curve_data()
		{
			QTest::addColumn<int>("nid");

			QTest::newRow("with curve") << NID_brainpoolP256r1;
			QTest::newRow("without curve") << NID_undef;
		}


		void curve()
		{
			QFETCH(int, nid);

			if (nid == NID_undef)
			{
				QTest::ignoreMessage(QtCriticalMsg, "Error on EC_GROUP_new_by_curve_name, curve is unknown: 0");
				QTest::ignoreMessage(QtCriticalMsg, "No curve defined");
			}
			auto curve = EcUtil::createCurve(nid);
			EcdhGenericMapping mapping(curve);
			QCOMPARE(mapping.getNid(), nid);
			QCOMPARE(mapping.generateLocalMappingData().isEmpty(), curve.isNull());
		}


		void equalKeys()
		{
			EcdhGenericMapping mapping(EcUtil::createCurve(NID_brainpoolP256r1));
			const auto& localMapping = mapping.generateLocalMappingData();
			QTest::ignoreMessage(QtCriticalMsg, "The exchanged public keys are equal.");
			QVERIFY(!mapping.generateEphemeralDomainParameters(localMapping, QByteArray("0123456789ABCDEF")));
		}


		void pointGenerator()
		{
			const auto& curve = EcUtil::createCurve(NID_brainpoolP256r1);
			const auto& generator = EcUtil::point2oct(curve, EC_GROUP_get0_generator(curve.data()));
			const auto& nonce = Randomizer::getInstance().createBytes(16);

			EcdhGenericMapping cardMapping(curve);
			const auto& cardMappingData = cardMapping.generateLocalMappingData();
			QVERIFY(!cardMappingData.isEmpty());

			EcdhGenericMapping terminalMapping(curve);
			const auto& terminalMappingData = terminalMapping.generateLocalMappingData();
			QVERIFY(!terminalMappingData.isEmpty());

			QVERIFY(cardMapping.generateEphemeralDomainParameters(terminalMappingData, nonce));
			QVERIFY(!cardMapping.getGenerator().isEmpty());

			QVERIFY(terminalMapping.generateEphemeralDomainParameters(cardMappingData, nonce));
			QVERIFY(!terminalMapping.getGenerator().isEmpty());
			QCOMPARE(cardMapping.getGenerator(), terminalMapping.getGenerator());
			QCOMPARE_NE(generator, cardMapping.getGenerator());
		}


		void explicitCurve()
		{
			auto curve = EcUtil::createCurve(NID_brainpoolP256r1);

			// Change generator by double it to ensure the point is still on the curve
			BN_CTX* ctx = BN_CTX_new();
			EC_POINT* newGenerator = EC_POINT_new(curve.data());
			EC_POINT_dbl(curve.data(), newGenerator, EC_GROUP_get0_generator(curve.data()), ctx);
			EC_GROUP_set_generator(curve.data(), newGenerator, EC_GROUP_get0_order(curve.data()), EC_GROUP_get0_cofactor(curve.data()));
			EC_POINT_free(newGenerator);
			BN_CTX_free(ctx);

			EcdhGenericMapping mapping(curve);
			// Should be NID_undef: https://github.com/openssl/openssl/issues/31616
			QCOMPARE(mapping.getNid(), NID_brainpoolP256r1);
		}


};

QTEST_GUILESS_MAIN(test_EcdhGenericMapping)
#include "test_EcdhGenericMapping.moc"
