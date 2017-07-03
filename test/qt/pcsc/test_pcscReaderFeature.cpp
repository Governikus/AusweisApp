/*!
 * \brief Unit tests for \ref PcscReaderFeature
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */
#include "PcscReaderFeature.h"
#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;

class test_pcscReaderFeature
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void featuresEmpty()
		{
			PcscReaderFeature readerFeature(nullptr);
			QCOMPARE(readerFeature.toString(), QString("[]"));
			QCOMPARE(readerFeature.getFeatures().size(), 0);
		}


		void featuresCyberjackBasis()
		{
			QByteArray featuresTLV = QByteArray::fromHex("120442330012");
			PcscReaderFeature readerFeature(featuresTLV.constData(), featuresTLV.length());
			QCOMPARE(readerFeature.toString(), QString("[TLV_PROPERTIES]"));
			QCOMPARE(readerFeature.getFeatures().size(), 1);

			QVERIFY(readerFeature.getFeatures().contains(FeatureID::TLV_PROPERTIES));
			QCOMPARE(readerFeature.getFeatures().value(FeatureID::TLV_PROPERTIES), 1110638610);
		}


		void featuresCyberjackStandard()
		{
			QByteArray featuresTLV = QByteArray::fromHex("060442000db2070442000db3080442000db4090442000db5200442000dcc");
			PcscReaderFeature readerFeature(featuresTLV.constData(), featuresTLV.length());
			QCOMPARE(readerFeature.toString(), QString("[VERIFY_PIN_DIRECT MODIFY_PIN_DIRECT MCT_READERDIRECT MCT_UNIVERSAL EXECUTE_PACE]"));
			QCOMPARE(readerFeature.getFeatures().size(), 5);

			QVERIFY(readerFeature.getFeatures().contains(FeatureID::VERIFY_PIN_DIRECT));
			QVERIFY(readerFeature.getFeatures().contains(FeatureID::MODIFY_PIN_DIRECT));
			QVERIFY(readerFeature.getFeatures().contains(FeatureID::MCT_READERDIRECT));
			QVERIFY(readerFeature.getFeatures().contains(FeatureID::MCT_UNIVERSAL));
			QVERIFY(readerFeature.getFeatures().contains(FeatureID::EXECUTE_PACE));

			QCOMPARE(readerFeature.getFeatures().value(FeatureID::VERIFY_PIN_DIRECT), 1107299762);
			QCOMPARE(readerFeature.getFeatures().value(FeatureID::MODIFY_PIN_DIRECT), 1107299763);
			QCOMPARE(readerFeature.getFeatures().value(FeatureID::MCT_READERDIRECT), 1107299764);
			QCOMPARE(readerFeature.getFeatures().value(FeatureID::MCT_UNIVERSAL), 1107299765);
			QCOMPARE(readerFeature.getFeatures().value(FeatureID::EXECUTE_PACE), 1107299788);
		}


		void capabilitiesEmpty()
		{
			PcscReaderPaceCapability paceCapa(nullptr);
			QCOMPARE(paceCapa.toString(), QString("[]"));
			QVERIFY(paceCapa.getPaceCapabilities().isEmpty());
		}


		void capabilitiesCyberjackStandard()
		{
			QByteArray capabilitiesTLV = QByteArray::fromHex("00000000010060");
			PcscReaderPaceCapability paceCapa(capabilitiesTLV.constData(), capabilitiesTLV.length());
			QCOMPARE(paceCapa.toString(), QString("[EID GENERIC]"));
			QCOMPARE(paceCapa.getPaceCapabilities().size(), 2);
			QVERIFY(paceCapa.getPaceCapabilities().contains(PaceCapabilityId::EID));
			QVERIFY(paceCapa.getPaceCapabilities().contains(PaceCapabilityId::GENERIC));
		}


		void capabilitiesCyberjackKomfort()
		{
			QByteArray capabilitiesTLV = QByteArray::fromHex("00000000010070");
			PcscReaderPaceCapability paceCapa(capabilitiesTLV.constData(), capabilitiesTLV.length());
			QCOMPARE(paceCapa.toString(), QString("[ESIGN EID GENERIC]"));
			QCOMPARE(paceCapa.getPaceCapabilities().size(), 3);
			QVERIFY(paceCapa.getPaceCapabilities().contains(PaceCapabilityId::EID));
			QVERIFY(paceCapa.getPaceCapabilities().contains(PaceCapabilityId::ESIGN));
			QVERIFY(paceCapa.getPaceCapabilities().contains(PaceCapabilityId::GENERIC));
		}


};

QTEST_GUILESS_MAIN(test_pcscReaderFeature)
#include "test_pcscReaderFeature.moc"
