/*!
 * \brief Tests for the EcdhKeyAgreement
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcdhKeyAgreement.h"

#include "MockReader.h"
#include "TestFileHelper.h"
#include "asn1/PaceInfo.h"
#include "pace/ec/EcUtil.h"

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
			QScopedPointer<KeyAgreement> keyAgreement(new EcdhKeyAgreement(paceInfo, reader->createCardConnectionWorker(), QSharedPointer<EcdhGenericMapping>()));

			KeyAgreementStatus result = keyAgreement->perform("123456");

			QCOMPARE(result, KeyAgreementStatus::PROTOCOL_ERROR);
		}


		void encodeUncompressedPublicKey()
		{
			QByteArray protocolValue = QByteArray::fromHex("04007F00070202040202");
			QSharedPointer<const PaceInfo> paceInfo = PaceInfo::decode(QByteArray::fromHex("300F060A") + protocolValue + QByteArray::fromHex("020102"));
			QSharedPointer<EC_GROUP> ecGroup = EcUtil::create(EC_GROUP_new_by_curve_name(NID_brainpoolP256r1));
			QByteArray ecPointBytes = QByteArray::fromHex("04A9024A1CE8F02DB4463CD359BE3A6946FA24FDBED7D19B04BEA2F0AA2FF63C6D2A05B17A66EDBC15875611A209CB87C972A141263BD0843CC64B8B884C52F725");
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
