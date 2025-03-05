/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcdhKeyAgreement.h"

#include "MockReader.h"
#include "TestFileHelper.h"
#include "asn1/PaceInfo.h"
#include "pace/ec/EcUtil.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_EcdhKeyAgreement
	: public QObject
{
	Q_OBJECT
	QSharedPointer<EFCardAccess> mEfCardAccess;

	private Q_SLOTS:
		void initTestCase()
		{
			auto data = TestFileHelper::readFile(":/card/efCardAccess.hex"_L1);
			mEfCardAccess = EFCardAccess::fromHex(data);
		}


		/*
		 * Reported during conformance tests.
		 */
		void perform_failureOnGetNonce()
		{
			QList<TransmitConfig> transmitConfigs;
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6982")));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(transmitConfigs, mEfCardAccess));
			QSharedPointer<const PaceInfo> paceInfo = mEfCardAccess->getPaceInfos().at(0);
			QScopedPointer<KeyAgreement> keyAgreement(new EcdhKeyAgreement(paceInfo, reader->createCardConnectionWorker(), QSharedPointer<EcdhGenericMapping>()));

			KeyAgreementStatus result = keyAgreement->perform("123456");

			QCOMPARE(result, KeyAgreementStatus::PROTOCOL_ERROR);
		}


		void encodeUncompressedPublicKey()
		{
			const Oid oid(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			const QByteArray ecPointBytes = QByteArray::fromHex("04A9024A1CE8F02DB4463CD359BE3A6946FA24FDBED7D19B04BEA2F0AA2FF63C6D2A05B17A66EDBC15875611A209CB87C972A141263BD0843CC64B8B884C52F725");
			const QByteArray& result = EcdhKeyAgreement::encodeUncompressedPublicKey(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128, ecPointBytes);
			QCOMPARE(result.toHex(),
					// ISO 7816 Format in TR 3111 - 5.1.2
					QByteArray("7f49")
					+ QByteArray("4f")
					+ QByteArray("06")
					+ QByteArray("0a")
					+ QByteArray(oid).toHex()
					+ QByteArray("86")
					+ QByteArray("41")
					+ ecPointBytes.toHex());
		}


};

QTEST_GUILESS_MAIN(test_EcdhKeyAgreement)
#include "test_EcdhKeyAgreement.moc"
