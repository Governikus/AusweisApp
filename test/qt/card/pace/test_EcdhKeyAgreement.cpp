/*!
 * test_EcdhKeyAgreement.cpp
 *
 * \brief Tests for the EcdhKeyAgreement
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "pace/ec/EcdhKeyAgreement.h"

#include "asn1/PACEInfo.h"
#include "MockReader.h"
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
			transmitConfigs.append(TransmitConfig(ReturnCode::OK, QByteArray::fromHex("6982")));
			QScopedPointer<MockReader> reader(MockReader::createMockReader(transmitConfigs, mEfCardAccess));
			QSharedPointer<PACEInfo> paceInfo = mEfCardAccess->getSecurityInfos<PACEInfo>().at(0);
			QScopedPointer<KeyAgreement> keyAgreement(new EcdhKeyAgreement(paceInfo, reader->createCardConnectionWorker()));

			KeyAgreementStatus result = keyAgreement->perform("123456");

			QCOMPARE(result, KeyAgreementStatus::PROTOCOLL_ERROR);
		}


};

QTEST_GUILESS_MAIN(test_EcdhKeyAgreement)
#include "test_EcdhKeyAgreement.moc"
