/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderFeature.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_pcscReaderFeature
	: public QObject
{
	Q_OBJECT

	private:
		int getFeatureCount(const PcscReaderFeature& pPcscReaderFeature)
		{
			int count = 0;
			const auto list = Enum<FeatureID>::getList();
			for (FeatureID feature : list)
			{
				if (pPcscReaderFeature.contains(feature))
				{
					count++;
				}
			}
			return count;
		}

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void featuresEmpty()
		{
			PcscReaderFeature readerFeature(nullptr);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			qDebug() << readerFeature;
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("()")));

			QCOMPARE(getFeatureCount(readerFeature), 0);
		}


		void featuresCyberjackBasis()
		{
			const auto featuresTLV = QByteArray::fromHex("120442330012");
			PcscReaderFeature readerFeature(featuresTLV);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			qDebug() << readerFeature;
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("(TLV_PROPERTIES)")));

			QCOMPARE(getFeatureCount(readerFeature), 1);
			QVERIFY(readerFeature.contains(FeatureID::TLV_PROPERTIES));
			QCOMPARE(readerFeature.getValue(FeatureID::TLV_PROPERTIES), PCSC_INT(1110638610));
		}


		void featuresCyberjackStandard()
		{
			const auto featuresTLV = QByteArray::fromHex("060442000DB2070442000DB3080442000DB4090442000DB5200442000DCC");
			PcscReaderFeature readerFeature(featuresTLV);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			qDebug() << readerFeature;
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("(VERIFY_PIN_DIRECT, MODIFY_PIN_DIRECT, MCT_READERDIRECT, MCT_UNIVERSAL, EXECUTE_PACE)")));

			QCOMPARE(getFeatureCount(readerFeature), 5);
			QVERIFY(readerFeature.contains(FeatureID::VERIFY_PIN_DIRECT));
			QVERIFY(readerFeature.contains(FeatureID::MODIFY_PIN_DIRECT));
			QVERIFY(readerFeature.contains(FeatureID::MCT_READERDIRECT));
			QVERIFY(readerFeature.contains(FeatureID::MCT_UNIVERSAL));
			QVERIFY(readerFeature.contains(FeatureID::EXECUTE_PACE));
			QCOMPARE(readerFeature.getValue(FeatureID::VERIFY_PIN_DIRECT), PCSC_INT(1107299762));
			QCOMPARE(readerFeature.getValue(FeatureID::MODIFY_PIN_DIRECT), PCSC_INT(1107299763));
			QCOMPARE(readerFeature.getValue(FeatureID::MCT_READERDIRECT), PCSC_INT(1107299764));
			QCOMPARE(readerFeature.getValue(FeatureID::MCT_UNIVERSAL), PCSC_INT(1107299765));
			QCOMPARE(readerFeature.getValue(FeatureID::EXECUTE_PACE), PCSC_INT(1107299788));
		}


};

QTEST_GUILESS_MAIN(test_pcscReaderFeature)
#include "test_pcscReaderFeature.moc"
