/*!
 * \brief Tests for the EcdhKeyAgreement
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcdhKeyAgreement.h"

#include "asn1/PaceInfo.h"
#include "MockReader.h"
#include "pace/ec/EcUtil.h"
#include "pace/ec/EllipticCurveFactory.h"
#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_EcdhKeyAgreement
	: public QObject
{
	Q_OBJECT
	QSharedPointer<EFCardAccess> mEfCardAccess;

	private Q_SLOTS:
		void initTestCase()
		{
			auto data = TestFileHelper::readFile(":/card/efCardAccess.hex");
			mEfCardAccess = EFCardAccess::fromHex(data);
		}


		/*
		 * Reported during conformance tests.
		 */
		void perform_failureOnGetNonce()
		{
			QVector<TransmitConfig> transmitConfigs;
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6982")));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(transmitConfigs, mEfCardAccess));
			QSharedPointer<const PaceInfo> paceInfo = mEfCardAccess->getPaceInfos().at(0);
			QScopedPointer<KeyAgreement> keyAgreement(new EcdhKeyAgreement(paceInfo, reader->createCardConnectionWorker()));

			KeyAgreementStatus result = keyAgreement->perform("123456");

			QCOMPARE(result, KeyAgreementStatus::PROTOCOL_ERROR);
		}


		void encodeUncompressedPublicKey()
		{
			QByteArray protocolValue = QByteArray::fromHex("04007F00070202040202");
			QSharedPointer<PaceInfo> paceInfo = PaceInfo::decode(QByteArray::fromHex("300F060A") + protocolValue + QByteArray::fromHex("020102"));
			QSharedPointer<EC_GROUP> ecGroup = EllipticCurveFactory::create(13);
			QByteArray ecPointBytes = QByteArray::fromHex("04a9024a1ce8f02db4463cd359be3a6946fa24fdbed7d19b04bea2f0aa2ff63c6d2a05b17a66edbc15875611a209cb87c972a141263bd0843cc64b8b884c52f725");
			QSharedPointer<EC_POINT> ecPoint = EcUtil::oct2point(ecGroup, ecPointBytes);

			const QByteArray& result = EcdhKeyAgreement::encodeUncompressedPublicKey(paceInfo, ecGroup, ecPoint);
			QCOMPARE(result.toHex(),
					// ISO 7816 Format in TR 3111 - 5.1.2
					QByteArray("7f49")
					+ QByteArray("4f")
					+ QByteArray("06")
					+ QByteArray("0a")
					+ protocolValue.toHex()
					+ QByteArray("86")
					+ QByteArray("41")
					+ ecPointBytes.toHex());
		}


};

QTEST_GUILESS_MAIN(test_EcdhKeyAgreement)
#include "test_EcdhKeyAgreement.moc"
