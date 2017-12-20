/*!
 * \brief Unit tests for \ref PcscReaderFeature
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "PcscReaderFeature.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>


using namespace governikus;


class test_pcscReaderFeature
	: public QObject
{
	Q_OBJECT

	private:
		int getFeatureCount(const PcscReaderFeature& pPcscReaderFeature)
		{
			int count = 0;
			for (FeatureID feature : Enum<FeatureID>::getList())
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
			LogHandler::getInstance().init();
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void featuresEmpty()
		{
			PcscReaderFeature readerFeature(nullptr);

			QSignalSpy spyLog(&LogHandler::getInstance(), &LogHandler::fireLog);
			qDebug() << readerFeature;
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("()")));

			QCOMPARE(getFeatureCount(readerFeature), 0);
		}


		void featuresCyberjackBasis()
		{
			QByteArray featuresTLV = QByteArray::fromHex("120442330012");
			PcscReaderFeature readerFeature(featuresTLV.constData(), static_cast<PCSC_INT>(featuresTLV.length()));

			QSignalSpy spyLog(&LogHandler::getInstance(), &LogHandler::fireLog);
			qDebug() << readerFeature;
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("(TLV_PROPERTIES)")));

			QCOMPARE(getFeatureCount(readerFeature), 1);
			QVERIFY(readerFeature.contains(FeatureID::TLV_PROPERTIES));
			QCOMPARE(readerFeature.getValue(FeatureID::TLV_PROPERTIES), PCSC_INT(1110638610));
		}


		void featuresCyberjackStandard()
		{
			QByteArray featuresTLV = QByteArray::fromHex("060442000db2070442000db3080442000db4090442000db5200442000dcc");
			PcscReaderFeature readerFeature(featuresTLV.constData(), static_cast<PCSC_INT>(featuresTLV.length()));

			QSignalSpy spyLog(&LogHandler::getInstance(), &LogHandler::fireLog);
			qDebug() << readerFeature;
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("(VERIFY_PIN_DIRECT, MODIFY_PIN_DIRECT, MCT_READERDIRECT, MCT_UNIVERSAL, EXECUTE_PACE)")));

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
